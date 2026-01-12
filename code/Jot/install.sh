#!/bin/bash
# Typewriter - Universal Installer & Updater
# Supports: Linux, macOS, Raspberry Pi

set -e  # Exit on error

# Installation directory
INSTALL_DIR="$HOME/.jot"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Output mode control
COMPACT_OUTPUT=false

# Print output (black and white)
print_header() {
    if [ "$COMPACT_OUTPUT" = true ]; then
        echo ""
        echo "→ $1"
    else
        echo ""
        echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
        echo "  $1"
        echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
        echo ""
    fi
}

print_status() {
    if [ "$COMPACT_OUTPUT" = true ]; then
        printf "  ◦ %s... " "$1"
    else
        echo "[INFO] $1"
    fi
}

print_success() {
    if [ "$COMPACT_OUTPUT" = true ]; then
        echo "✓"
    else
        echo "[✓] $1"
    fi
}

print_error() {
    if [ "$COMPACT_OUTPUT" = true ]; then
        echo "✗ $1"
    else
        echo "[✗] $1"
    fi
}

print_warning() {
    if [ "$COMPACT_OUTPUT" = true ]; then
        echo "⚠ $1"
    else
        echo "[!] $1"
    fi
}

# Detect OS
detect_os() {
    if [[ -f /proc/device-tree/model ]] && grep -q "Raspberry Pi" /proc/device-tree/model; then
        echo "raspberry-pi"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "linux"
    else
        echo "unknown"
    fi
}

# Check if command exists
command_exists() {
    command -v "$1" &> /dev/null
}

# Detect package manager
detect_package_manager() {
    if command_exists apt-get; then
        echo "apt"
    elif command_exists dnf; then
        echo "dnf"
    elif command_exists yum; then
        echo "yum"
    elif command_exists pacman; then
        echo "pacman"
    elif command_exists zypper; then
        echo "zypper"
    elif command_exists apk; then
        echo "apk"
    else
        echo "unknown"
    fi
}

# Install package using detected package manager
install_package() {
    local package=$1
    local pkg_manager=$(detect_package_manager)

    case "$pkg_manager" in
        "apt")
            sudo apt-get install -y "$package"
            ;;
        "dnf")
            sudo dnf install -y "$package"
            ;;
        "yum")
            sudo yum install -y "$package"
            ;;
        "pacman")
            sudo pacman -S --noconfirm "$package"
            ;;
        "zypper")
            sudo zypper install -y "$package"
            ;;
        "apk")
            sudo apk add "$package"
            ;;
        *)
            print_error "Unknown package manager. Please install $package manually."
            return 1
            ;;
    esac
}

# Update package lists
update_packages() {
    local pkg_manager=$(detect_package_manager)

    case "$pkg_manager" in
        "apt")
            sudo apt-get update -qq
            ;;
        "dnf")
            sudo dnf check-update -q || true
            ;;
        "yum")
            sudo yum check-update -q || true
            ;;
        "pacman")
            sudo pacman -Sy
            ;;
        "zypper")
            sudo zypper refresh
            ;;
        "apk")
            sudo apk update
            ;;
    esac
}

# Install dependencies based on OS
install_dependencies() {
    local os_type=$1

    if [ "$COMPACT_OUTPUT" = false ]; then
        print_header "Installing Dependencies"
    else
        print_header "Installing Dependencies"
    fi

    case "$os_type" in
        "macos")
            if [ "$COMPACT_OUTPUT" = false ]; then
                print_status "Detected: macOS"
            fi

            # Check for Homebrew
            if ! command_exists brew; then
                print_status "Installing Homebrew"
                /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
                print_success "Homebrew installed"
            else
                print_status "Homebrew"
                print_success ""
            fi

            # Install dependencies
            local packages=("cmake" "ncurses" "micro" "python3" "git")
            for pkg in "${packages[@]}"; do
                print_status "$pkg"
                if brew list "$pkg" &>/dev/null; then
                    print_success ""
                else
                    brew install "$pkg" > /dev/null 2>&1
                    print_success ""
                fi
            done
            ;;

        "raspberry-pi"|"linux")
            local pkg_manager=$(detect_package_manager)

            if [ "$pkg_manager" = "unknown" ]; then
                print_error "Could not detect package manager!"
                COMPACT_OUTPUT=false
                print_error "Please install these packages manually:"
                print_error "  - build-essential/base-devel, cmake, ncurses, micro"
                print_error "  - python3, python3-pip, rclone"
                exit 1
            fi

            print_status "Updating packages"
            update_packages > /dev/null 2>&1
            print_success ""

            # Install build tools
            print_status "Build tools"
            if command_exists gcc && command_exists g++; then
                print_success ""
            else
                case "$pkg_manager" in
                    "apt")
                        install_package "build-essential" > /dev/null 2>&1
                        ;;
                    "dnf"|"yum")
                        install_package "gcc" > /dev/null 2>&1
                        install_package "gcc-c++" > /dev/null 2>&1
                        install_package "make" > /dev/null 2>&1
                        ;;
                    "pacman")
                        install_package "base-devel" > /dev/null 2>&1
                        ;;
                    "zypper")
                        sudo zypper install -y -t pattern devel_basis > /dev/null 2>&1
                        ;;
                    "apk")
                        install_package "build-base" > /dev/null 2>&1
                        ;;
                esac
                print_success ""
            fi

            # Install CMake
            print_status "cmake"
            if command_exists cmake; then
                print_success ""
            else
                install_package "cmake" > /dev/null 2>&1
                print_success ""
            fi

            # Install ncurses
            print_status "ncurses"
            if [ -f /usr/include/ncurses.h ] || [ -f /usr/include/ncursesw/ncurses.h ]; then
                print_success ""
            else
                case "$pkg_manager" in
                    "apt")
                        install_package "libncurses5-dev" > /dev/null 2>&1
                        install_package "libncursesw5-dev" > /dev/null 2>&1
                        ;;
                    "dnf"|"yum")
                        install_package "ncurses-devel" > /dev/null 2>&1
                        ;;
                    "pacman")
                        install_package "ncurses" > /dev/null 2>&1
                        ;;
                    "zypper")
                        install_package "ncurses-devel" > /dev/null 2>&1
                        ;;
                    "apk")
                        install_package "ncurses-dev" > /dev/null 2>&1
                        ;;
                esac
                print_success ""
            fi

            # Install micro editor
            print_status "micro editor"
            if command_exists micro; then
                print_success ""
            else
                cd /tmp
                curl https://getmic.ro 2>/dev/null | bash > /dev/null 2>&1
                sudo mv micro /usr/local/bin/
                cd - > /dev/null
                print_success ""
            fi

            # Install Python3 and pip
            print_status "python3"
            if command_exists python3; then
                print_success ""
            else
                case "$pkg_manager" in
                    "apt")
                        install_package "python3" > /dev/null 2>&1
                        install_package "python3-pip" > /dev/null 2>&1
                        ;;
                    "dnf"|"yum")
                        install_package "python3" > /dev/null 2>&1
                        install_package "python3-pip" > /dev/null 2>&1
                        ;;
                    "pacman")
                        install_package "python" > /dev/null 2>&1
                        install_package "python-pip" > /dev/null 2>&1
                        ;;
                    "zypper")
                        install_package "python3" > /dev/null 2>&1
                        install_package "python3-pip" > /dev/null 2>&1
                        ;;
                    "apk")
                        install_package "python3" > /dev/null 2>&1
                        install_package "py3-pip" > /dev/null 2>&1
                        ;;
                esac
                print_success ""
            fi

            # Install git
            print_status "git"
            if command_exists git; then
                print_success ""
            else
                install_package "git" > /dev/null 2>&1
                print_success ""
            fi

            ;;

        *)
            print_error "Unsupported operating system"
            exit 1
            ;;
    esac

    # Install Python packages
    print_status "python-docx"
    if python3 -c "import docx" 2>/dev/null; then
        print_success ""
    else
        pip3 install --user python-docx > /dev/null 2>&1
        print_success ""
    fi

    # Install rclone
    print_status "rclone"
    if command_exists rclone; then
        print_success ""
    else
        curl https://rclone.org/install.sh 2>/dev/null | sudo bash > /dev/null 2>&1
        print_success ""
    fi

    if [ "$COMPACT_OUTPUT" = true ]; then
        echo ""
    fi
}

# Configure micro editor
configure_micro() {
    if [ "$COMPACT_OUTPUT" = false ]; then
        print_header "Configuring Editor"
    fi

    print_status "Configuring micro"
    mkdir -p ~/.config/micro || {
        print_warning "Could not create config directory"
        return 0
    }

    cat > ~/.config/micro/settings.json << 'EOF'
{
    "softwrap": true,
    "scrollbar": true,
    "tabsize": 4,
    "tabstospaces": true,
    "autoindent": true,
    "savecursor": true,
    "colorscheme": "simple"
}
EOF

    if [ -f ~/.config/micro/settings.json ]; then
        print_success ""
    else
        print_warning "Could not create settings file"
    fi

    if [ "$COMPACT_OUTPUT" = true ]; then
        echo ""
    fi
}

# Build the application
build_app() {
    if [ "$COMPACT_OUTPUT" = false ]; then
        print_header "Building Jot"
    fi

    cd "$INSTALL_DIR"

    # Create build directory
    if [ ! -d "cmake-build-jot" ]; then
        mkdir -p cmake-build-jot
    fi

    cd cmake-build-jot

    print_status "Building application"

    # Run cmake and build with error checking
    if ! cmake .. -DCMAKE_BUILD_TYPE=Release > /tmp/jot_cmake.log 2>&1; then
        print_error "CMake configuration failed. Check /tmp/jot_cmake.log"
        return 1
    fi

    if ! cmake --build . --config Release > /tmp/jot_build.log 2>&1; then
        print_error "Build failed. Check /tmp/jot_build.log"
        return 1
    fi

    # The executable is created in INSTALL_DIR (parent directory) due to CMakeLists.txt settings
    if [ -f "$INSTALL_DIR/jot" ]; then
        chmod +x "$INSTALL_DIR/jot" 2>/dev/null || true
        print_success ""
        if [ "$COMPACT_OUTPUT" = true ]; then
            echo ""
        fi
        return 0
    else
        print_error "Build failed - executable not found at $INSTALL_DIR/jot"
        echo "Checking build directory contents:"
        ls -la "$INSTALL_DIR" || true
        ls -la "$INSTALL_DIR/cmake-build-jot" || true
        return 1
    fi
}

# Create launcher script
create_launcher() {
    if [ "$COMPACT_OUTPUT" = false ]; then
        print_header "Creating Launcher"
    fi

    print_status "Creating launcher"

    # The jot executable should already be in $INSTALL_DIR/jot from the build step
    # Just ensure it's executable
    chmod +x "$INSTALL_DIR/jot" 2>/dev/null || true

    # Create launcher script in the original script directory
    cat > "$SCRIPT_DIR/jot" << EOF
#!/bin/bash
# Jot launcher script
exec "$INSTALL_DIR/jot" "\$@"
EOF
    chmod +x "$SCRIPT_DIR/jot"

    # Create symlink in PATH
    if [ -d "$HOME/bin" ]; then
        ln -sf "$INSTALL_DIR/jot" "$HOME/bin/jot" 2>/dev/null || true
    elif [ -d "$HOME/.local/bin" ]; then
        mkdir -p "$HOME/.local/bin"
        ln -sf "$INSTALL_DIR/jot" "$HOME/.local/bin/jot" 2>/dev/null || true
    fi

    print_success ""

    if [ "$COMPACT_OUTPUT" = true ]; then
        echo ""
    fi
}

# Copy files to installation directory
copy_files() {
    if [ "$COMPACT_OUTPUT" = false ]; then
        print_header "Installing Files"
    fi

    # Create installation directory
    mkdir -p "$INSTALL_DIR"

    print_status "Copying files"

    # Copy assets directory (source files)
    mkdir -p "$INSTALL_DIR/assets"
    cp -r "$SCRIPT_DIR/assets/"* "$INSTALL_DIR/assets/" 2>/dev/null || true

    # Copy CMakeLists.txt from root
    cp "$SCRIPT_DIR/CMakeLists.txt" "$INSTALL_DIR/"

    # Copy templates
    mkdir -p "$INSTALL_DIR/templates"
    cp -r "$SCRIPT_DIR/templates/"* "$INSTALL_DIR/templates/" 2>/dev/null || true

    # Copy files directory if exists
    if [ -d "$SCRIPT_DIR/files" ]; then
        mkdir -p "$INSTALL_DIR/files"
        cp -r "$SCRIPT_DIR/files/"* "$INSTALL_DIR/files/" 2>/dev/null || true
    else
        mkdir -p "$INSTALL_DIR/files"
        mkdir -p "$INSTALL_DIR/files/templates"
    fi

    print_success ""

    if [ "$COMPACT_OUTPUT" = true ]; then
        echo ""
    fi
}

# Check if updates are available (silent check)
check_for_updates() {
    if [ ! -d "$INSTALL_DIR" ]; then
        return 1  # Not installed
    fi

    # Check CMakeLists.txt
    if ! cmp -s "$SCRIPT_DIR/CMakeLists.txt" "$INSTALL_DIR/CMakeLists.txt" 2>/dev/null; then
        return 0  # Updates available
    fi

    # Check all source files in assets directory
    if [ -d "$SCRIPT_DIR/assets" ]; then
        for source_file in "$SCRIPT_DIR/assets/"*.cpp "$SCRIPT_DIR/assets/"*.h; do
            if [ -f "$source_file" ]; then
                filename=$(basename "$source_file")
                if ! cmp -s "$source_file" "$INSTALL_DIR/assets/$filename" 2>/dev/null; then
                    return 0  # Updates available
                fi
            fi
        done
    fi

    # Check templates directory
    if [ -d "$SCRIPT_DIR/templates" ]; then
        for template in "$SCRIPT_DIR/templates/"*; do
            template_name=$(basename "$template")
            if [ -f "$template" ] && ! cmp -s "$template" "$INSTALL_DIR/templates/$template_name" 2>/dev/null; then
                return 0  # Updates available
            fi
        done
    fi

    return 1  # No updates
}

# Update installation immediately without prompting
immediate_update() {
    print_header "Installing Update"

    if [ ! -d "$INSTALL_DIR" ]; then
        print_error "Jot not installed. Please run installation first."
        return 1
    fi

    print_status "Updating files..."

    # Enable compact mode for cleaner output
    COMPACT_OUTPUT=true

    copy_files

    if build_app; then
        create_launcher
        COMPACT_OUTPUT=false
        echo ""
        print_success "Jot updated successfully!"
        return 0
    else
        COMPACT_OUTPUT=false
        print_error "Update failed during build"
        return 1
    fi
}

# Update installation
update_installation() {
    print_header "Checking for Updates"

    if [ ! -d "$INSTALL_DIR" ]; then
        print_error "Jot not installed. Please run installation first."
        return 1
    fi

    print_status "Comparing files..."

    local needs_update=false

    # Check if source files have changed
    if ! cmp -s "$SCRIPT_DIR/assets/main.cpp" "$INSTALL_DIR/assets/main.cpp" 2>/dev/null; then
        print_warning "main.cpp has changed"
        needs_update=true
    fi

    if ! cmp -s "$SCRIPT_DIR/CMakeLists.txt" "$INSTALL_DIR/CMakeLists.txt" 2>/dev/null; then
        print_warning "CMakeLists.txt has changed"
        needs_update=true
    fi

    if [ "$needs_update" = false ]; then
        print_success "Jot is up to date!"
        return 0
    fi

    echo ""
    read -p "Updates available. Install updates? (y/n): " update_choice

    if [[ "$update_choice" =~ ^[Yy]$ ]]; then
        print_status "Updating..."
        copy_files
        build_app
        create_launcher
        print_success "Jot updated successfully!"
        return 0
    else
        print_status "Update cancelled"
        return 0
    fi
}

# Full installation
full_installation() {
    local os_type=$1

    print_header "Jot Installation"
    echo "Installation directory: $INSTALL_DIR"
    echo ""

    # Enable compact mode for cleaner output during installation
    COMPACT_OUTPUT=true

    # Install dependencies
    install_dependencies "$os_type"

    # Copy files
    copy_files

    # Build application
    if ! build_app; then
        COMPACT_OUTPUT=false
        print_error "Installation failed during build"
        exit 1
    fi

    # Configure micro editor
    configure_micro

    # Create launcher
    create_launcher

    # Restore verbose output for final messages
    COMPACT_OUTPUT=false

    print_header "Installation Successful!"
}

# Main menu with arrow key navigation and ESC to exit
show_menu() {
    local selected=0
    local has_update=false

    # Check for updates first
    if check_for_updates; then
        has_update=true
        local options=("Update Available - Install Now " "Standard Installation (Recommended)" "Raspberry Pi Setup" "Update Existing Installation" "Exit")
    else
        local options=("Standard Installation (Recommended)" "Raspberry Pi Setup" "Update Existing Installation" "Exit")
    fi

    # Ensure cursor is shown on exit (trap for any exit)
    trap 'tput cnorm; printf "\033[?25h"' EXIT INT TERM

    # Hide cursor
    tput civis
    printf "\033[?25l"

    # Clear screen and draw static menu once
    clear
    cat << "EOF"
┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃                                                      ┃
┃                 📝  Jot Installer  📝                ┃
┃                                                      ┃
┃                simple note organization              ┃
┃                                                      ┃
┃                                                      ┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

EOF

    if [ "$has_update" = true ]; then
        echo "⚠️  New version detected! Update recommended."
        echo ""
    fi

    echo "Select Installation Mode:"
    echo ""

    # Function to redraw just the options
    draw_options() {
        for i in "${!options[@]}"; do
            # Move cursor to line (adjust based on update notice)
            local line_offset=11
            if [ "$has_update" = true ]; then
                line_offset=13
            fi
            tput cup $((line_offset + i)) 0
            tput el
            if [ $i -eq $selected ]; then
                echo "  ▶ $(($i + 1)). ${options[$i]}"
            else
                echo "    $(($i + 1)). ${options[$i]}"
            fi
        done
    }

    # Draw initial menu
    draw_options

    # Move cursor below menu and print instructions
    local line_offset=11
    if [ "$has_update" = true ]; then
        line_offset=13
    fi
    tput cup $((line_offset + ${#options[@]} + 1)) 0
    echo ""
    echo "Use ↑/↓ arrows or 1-${#options[@]} to select, Enter to confirm, ESC to exit"

    while true; do
        read -rsn1 key

        if [[ $key == $'\x1b' ]]; then
            read -rsn2 -t 0.01 rest
            key+="$rest"
        fi

        case "$key" in
            $'\x1b[A')  # Up arrow
                selected=$((selected - 1))
                if [ $selected -lt 0 ]; then
                    selected=$((${#options[@]} - 1))
                fi
                draw_options
                ;;
            $'\x1b[B')  # Down arrow
                selected=$((selected + 1))
                if [ $selected -ge ${#options[@]} ]; then
                    selected=0
                fi
                draw_options
                ;;
            $'\x1b')  # ESC key only
                tput cnorm  # Show cursor
                printf "\033[?25h"
                clear
                echo ""
                print_status "Installation cancelled. Goodbye!"
                echo ""
                exit 0
                ;;
            [1-9])  # Number keys (changed from [1-4])
                if [ $key -le ${#options[@]} ]; then
                    selected=$((key - 1))
                    break
                fi
                ;;
            '')  # Enter key
                break
                ;;
        esac
    done

    # Show cursor and clear screen before proceeding
    tput cnorm
    printf "\033[?25h"
    clear
    echo ""

    # Handle selection based on whether update option was shown
    if [ "$has_update" = true ]; then
        case $selected in
            0)
                # Update now option (new first option) - immediate update without prompt
                immediate_update
                if [ $? -eq 0 ]; then
                    ask_to_start
                fi
                exit 0
                ;;
            1)
                # Standard Installation
                print_header "Standard Installation"
                os_type=$(detect_os)
                full_installation "$os_type"
                ;;
            2)
                # Raspberry Pi Setup
                print_header "Raspberry Pi Setup"
                os_type=$(detect_os)
                if [[ "$os_type" == "raspberry-pi" ]]; then
                    print_success "Raspberry Pi detected"
                else
                    print_warning "Not a Raspberry Pi, but continuing with Linux setup"
                fi

                full_installation "raspberry-pi"

                # Ask about auto-start
                echo ""
                read -p "Enable auto-start on boot? (y/n): " autostart_choice
                if [[ "$autostart_choice" =~ ^[Yy]$ ]]; then
                    print_status "Configuring auto-start..."
                    sudo tee /etc/systemd/system/jot.service > /dev/null << EOF
[Unit]
Description=Jot App
After=network.target

[Service]
Type=simple
User=$USER
WorkingDirectory=$INSTALL_DIR
ExecStart=$INSTALL_DIR/jot
Restart=always

[Install]
WantedBy=multi-user.target
EOF
                    sudo systemctl enable jot.service
                    sudo systemctl daemon-reload
                    print_success "Auto-start enabled!"
                fi
                ;;
            3)
                # Update Installation
                print_header "Update Installation"
                update_installation
                ;;
            4)
                # Exit
                clear
                print_status "Goodbye!"
                exit 0
                ;;
        esac
    else
        # No update available - original menu
        case $selected in
            0)
                print_header "Standard Installation"
                os_type=$(detect_os)
                full_installation "$os_type"
                ;;
            1)
                print_header "Raspberry Pi Setup"
                os_type=$(detect_os)
                if [[ "$os_type" == "raspberry-pi" ]]; then
                    print_success "Raspberry Pi detected"
                else
                    print_warning "Not a Raspberry Pi, but continuing with Linux setup"
                fi

                full_installation "raspberry-pi"

                # Ask about auto-start
                echo ""
                read -p "Enable auto-start on boot? (y/n): " autostart_choice
                if [[ "$autostart_choice" =~ ^[Yy]$ ]]; then
                    print_status "Configuring auto-start..."
                    sudo tee /etc/systemd/system/jot.service > /dev/null << EOF
[Unit]
Description=Jot App
After=network.target

[Service]
Type=simple
User=$USER
WorkingDirectory=$INSTALL_DIR
ExecStart=$INSTALL_DIR/jot
Restart=always

[Install]
WantedBy=multi-user.target
EOF
                    sudo systemctl enable jot.service
                    sudo systemctl daemon-reload
                    print_success "Auto-start enabled!"
                fi
                ;;
            2)
                print_header "Update Installation"
                update_installation
                ;;
            3)
                clear
                print_status "Goodbye!"
                exit 0
                ;;
        esac
    fi
}

# Ask to start jot
ask_to_start() {
    echo ""
    read -p "Would you like to start Jot now? (y/n): " start_choice

    if [[ "$start_choice" =~ ^[Yy]$ ]]; then
        print_status "Starting Jot..."
        "$INSTALL_DIR/jot"
    else
        echo ""
        print_success "You can access Jot at any moment by running:"
        echo "  ./jot         (from $SCRIPT_DIR)"
        echo "  ~/.jot/jot    (from anywhere)"
        if [ -f "$HOME/bin/jot" ] || [ -f "$HOME/.local/bin/jot" ]; then
            echo "  jot           (from anywhere if ~/bin or ~/.local/bin is in your PATH)"
        fi
        echo ""
    fi
}

# Main execution
main() {
    # Check if running as updater
    if [ "$1" = "--update" ] || [ "$1" = "-u" ]; then
        update_installation
        exit 0
    fi

    # Check if already installed
    if [ -d "$INSTALL_DIR" ] && [ -f "$INSTALL_DIR/jot" ]; then
        # Already installed, check for updates
        if check_for_updates; then
            # Updates available - show menu with update option at top
            show_menu
            ask_to_start
        else
            # Already up to date
            clear
            echo ""
            echo "Jot is already installed and up to date!"
            echo ""
            echo "You can run Jot by typing ./jot in this directory."
            echo ""
            read -p "Would you like to start Jot now? (y/n): " start_choice
            if [[ "$start_choice" =~ ^[Yy]$ ]]; then
                "$INSTALL_DIR/jot"
            fi
        fi
    else
        # Not installed - show regular menu
        show_menu
        ask_to_start
    fi
}

# Run main
main "$@"

