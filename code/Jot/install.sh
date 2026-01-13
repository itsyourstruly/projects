#!/bin/bash

# Jot Installation Script

# Temp file for dependency check
DEP_CHECK_FILE="/tmp/jot_dep_check_$$"

# Detect package manager
detect_package_manager() {
    if command -v dnf &> /dev/null; then
        echo "dnf"
    elif command -v yum &> /dev/null; then
        echo "yum"
    elif command -v apt-get &> /dev/null; then
        echo "apt-get"
    elif command -v pacman &> /dev/null; then
        echo "pacman"
    elif command -v zypper &> /dev/null; then
        echo "zypper"
    else
        echo "unknown"
    fi
}

PKG_MANAGER=$(detect_package_manager)

# Map dependencies to package manager
get_package_name() {
    local dep=$1

    case "$PKG_MANAGER" in
        "dnf"|"yum")
            case "$dep" in
                "g++") echo "gcc-c++" ;;
                "libncurses5-dev"|"libncursesw5-dev") echo "ncurses-devel" ;;
                *) echo "$dep" ;;
            esac
            ;;
        "pacman")
            case "$dep" in
                "g++") echo "gcc" ;;
                "libncurses5-dev"|"libncursesw5-dev") echo "ncurses" ;;
                *) echo "$dep" ;;
            esac
            ;;
        "apt-get")
            case "$dep" in
                "g++") echo "g++" ;;
                "libncurses5-dev"|"libncursesw5-dev") echo "$dep" ;;
                *) echo "$dep" ;;
            esac
            ;;
        *)
            echo "$dep"
            ;;
    esac
}

# Required dependencies (generic names)
REQUIRED_DEPS=("cmake" "g++" "make" "libncurses5-dev" "micro" "rclone")


# Check if all dependencies are installed
check_dependencies() {
    local missing_deps=()

    for dep in "${REQUIRED_DEPS[@]}"; do
        local pkg_name=$(get_package_name "$dep")

        case "$PKG_MANAGER" in
            "dnf"|"yum")
                if [[ "$dep" == "libncurses5-dev"* ]]; then
                    if ! rpm -qa | grep -q "ncurses-devel"; then
                        missing_deps+=("$pkg_name")
                    fi
                elif [[ "$dep" == "g++" ]]; then
                    if ! command -v g++ &> /dev/null; then
                        missing_deps+=("$pkg_name")
                    fi
                else
                    if ! command -v "$dep" &> /dev/null; then
                        missing_deps+=("$pkg_name")
                    fi
                fi
                ;;
            "apt-get")
                case "$dep" in
                    "libncurses5-dev")
                        if ! dpkg -l | grep -q "^ii.*$dep"; then
                            missing_deps+=("$dep")
                        fi
                        ;;
                    *)
                        if ! command -v "$dep" &> /dev/null; then
                            missing_deps+=("$pkg_name")
                        fi
                        ;;
                esac
                ;;
            "pacman")
                if [[ "$dep" == "libncurses5-dev"* ]]; then
                    if ! pacman -Q ncurses &> /dev/null; then
                        missing_deps+=("$pkg_name")
                    fi
                elif [[ "$dep" == "g++" ]]; then
                    if ! command -v g++ &> /dev/null; then
                        missing_deps+=("$pkg_name")
                    fi
                else
                    if ! command -v "$dep" &> /dev/null; then
                        missing_deps+=("$pkg_name")
                    fi
                fi
                ;;
            *)
                if ! command -v "$dep" &> /dev/null; then
                    missing_deps+=("$dep")
                fi
                ;;
        esac
    done

    if [ ${#missing_deps[@]} -eq 0 ]; then
        echo "INSTALLED" > "$DEP_CHECK_FILE"
    else
        # Remove duplicates
        local unique_deps=($(echo "${missing_deps[@]}" | tr ' ' '\n' | sort -u | tr '\n' ' '))
        echo "MISSING:${unique_deps[*]}" > "$DEP_CHECK_FILE"
    fi
}

# Check if executable exists and if it's up to date
check_executable_status() {
    if [ ! -f "./jot" ]; then
        echo "NOT_INSTALLED"
        return
    fi

    # Check if any source file is newer than the executable
    local needs_update=0
    for src in assets/*.cpp assets/*.h CMakeLists.txt; do
        if [ -f "$src" ] && [ "$src" -nt "./jot" ]; then
            needs_update=1
            break
        fi
    done

    if [ $needs_update -eq 1 ]; then
        echo "UPDATE_AVAILABLE"
    else
        echo "UP_TO_DATE"
    fi
}

# Install dependencies
install_dependencies() {
    local deps_to_install=$1

    clear
    echo "Installing dependencies: $deps_to_install"
    echo ""

    case "$PKG_MANAGER" in
        "dnf")
            sudo dnf install -y $deps_to_install
            ;;
        "yum")
            sudo yum install -y $deps_to_install
            ;;
        "apt-get")
            sudo apt-get update
            sudo apt-get install -y $deps_to_install
            ;;
        "pacman")
            sudo pacman -S --noconfirm $deps_to_install
            ;;
        "zypper")
            sudo zypper install -y $deps_to_install
            ;;
        *)
            echo "Unknown package manager. Please install dependencies manually:"
            echo "$deps_to_install"
            return 1
            ;;
    esac

    if [ $? -eq 0 ]; then
        echo ""
        echo "Dependencies installed successfully."
        sleep 2
        return 0
    else
        echo ""
        echo "Failed to install dependencies."
        sleep 2
        return 1
    fi
}

# Build the project
build_project() {
    local build_type=$1
    local build_dir="cmake-build-jot"

    clear
    echo "Building Jot..."
    echo ""

    # Create build directory
    mkdir -p "$build_dir"
    cd "$build_dir" || return 1

    # Configure cmake
    if [ "$build_type" == "raspberry_pi" ]; then
        cmake -DRASPBERRY_PI=ON .. || { cd ..; return 1; }
    else
        cmake .. || { cd ..; return 1; }
    fi

    # Build
    make || { cd ..; return 1; }

    cd ..

    if [ -f "./jot" ]; then
        echo ""
        echo "Build successful! Executable created: ./jot"
        sleep 2
        return 0
    else
        echo ""
        echo "Build failed."
        sleep 2
        return 1
    fi
}

# Main installation function
perform_installation() {
    local install_type=$1
    local skip_prompt=$2

    # Read dependency status
    if [ ! -f "$DEP_CHECK_FILE" ]; then
        check_dependencies
    fi

    local dep_status=$(cat "$DEP_CHECK_FILE")

    if [[ $dep_status == "MISSING:"* ]]; then
        local missing="${dep_status#MISSING:}"

        # Prompt user about missing dependencies
        echo ""
        echo "Required dependencies not installed."
        echo "Missing: $missing"
        echo ""
        read -p "y to proceed with installation, n to withdraw and quit: " response

        if [[ "$response" == "y" || "$response" == "Y" ]]; then
            install_dependencies "$missing"
            if [ $? -ne 0 ]; then
                echo "Dependency installation failed. Cannot proceed."
                sleep 2
                return 1
            fi
        else
            echo "Installation cancelled."
            sleep 1
            return 1
        fi
    else
        # Dependencies already installed
        if [ "$skip_prompt" != "true" ]; then
            echo ""
            echo "All dependencies installed."
            echo ""
            read -p "y to compile and build new executable, n to withdraw and quit: " response

            if [[ "$response" != "y" && "$response" != "Y" ]]; then
                return 0
            fi
        fi
    fi

    # Build the project
    build_project "$install_type"

    if [ $? -eq 0 ]; then
        echo ""
        if [ "$skip_prompt" == "true" ]; then
            echo "Update complete!"
        else
            echo "Installation complete!"
        fi
        echo ""
        read -p "Open Jot now? (y/n): " run_response

        if [[ "$run_response" == "y" || "$run_response" == "Y" ]]; then
            clear
            # Use internal launcher that will reopen installer after jot exits
            exec ./.run_jot_from_installer.sh
        else
            return 0
        fi
    else
        echo ""
        if [ "$skip_prompt" == "true" ]; then
            echo "Update failed."
        else
            echo "Installation failed."
        fi
        echo ""
        read -p "Press Enter to continue..."
        return 1
    fi
}

# Main menu function
show_main_menu() {
    local exe_status=$1

    clear
    echo ""

    # Show status message
    if [ "$exe_status" == "UP_TO_DATE" ]; then
        echo "Already installed. Use './jot' to run."
        echo ""
    elif [ "$exe_status" == "UPDATE_AVAILABLE" ]; then
        echo "Update found."
        echo ""
    fi

    # Show menu options
    echo "1. Install normally"
    echo "2. Install to Raspberry Pi"

    if [ "$exe_status" == "UPDATE_AVAILABLE" ]; then
        echo "3. Update to latest version"
    fi

    # Add run option if Jot is installed
    if [ "$exe_status" == "UP_TO_DATE" ] || [ "$exe_status" == "UPDATE_AVAILABLE" ]; then
        if [ "$exe_status" == "UPDATE_AVAILABLE" ]; then
            echo "4. Run Jot"
        else
            echo "3. Run Jot"
        fi
    fi

    echo "0. Exit"
    echo ""

    read -p "Select option: " choice
}

# Main script execution
main() {
    # Perform background checks
    check_dependencies &

    # Check executable status
    exe_status=$(check_executable_status)

    # Wait for dependency check to complete
    wait

    # Main loop
    while true; do
        show_main_menu "$exe_status"

        case $choice in
            1)
                perform_installation "normal"
                # Recheck status after installation
                exe_status=$(check_executable_status)
                ;;
            2)
                perform_installation "raspberry_pi"
                # Recheck status after installation
                exe_status=$(check_executable_status)
                ;;
            3)
                if [ "$exe_status" == "UPDATE_AVAILABLE" ]; then
                    perform_installation "normal" "true"
                    exe_status=$(check_executable_status)
                elif [ "$exe_status" == "UP_TO_DATE" ]; then
                    # Run Jot using internal launcher that reopens installer
                    clear
                    exec ./.run_jot_from_installer.sh
                fi
                ;;
            4)
                if [ "$exe_status" == "UPDATE_AVAILABLE" ]; then
                    # Run Jot using internal launcher that reopens installer
                    clear
                    exec ./.run_jot_from_installer.sh
                fi
                ;;
            0|"")
                clear
                echo "Exiting installer."
                rm -f "$DEP_CHECK_FILE"
                exit 0
                ;;
            *)
                echo "Invalid option."
                sleep 1
                continue
                ;;
        esac
    done
}

# Cleanup on exit
trap "rm -f $DEP_CHECK_FILE" EXIT

# Run main
main

