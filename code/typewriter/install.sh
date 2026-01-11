#!/bin/bash
# Typewriter - Universal Installer & Updater
# Supports: Linux, macOS, Raspberry Pi

set -e  # Exit on error

# Installation directory
INSTALL_DIR="$HOME/.typewriter"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Print output (black and white)
print_header() {
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "  $1"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
}

print_status() {
    echo "[INFO] $1"
}

print_success() {
    echo "[✓] $1"
}

print_error() {
    echo "[✗] $1"
}

print_warning() {
    echo "[!] $1"
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
    print_header "Installing Dependencies"

    case "$os_type" in
        "macos")
            print_status "Detected: macOS"

            # Check for Homebrew
            if ! command_exists brew; then
                print_error "Homebrew not found. Installing Homebrew..."
                /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
            else
                print_success "Homebrew already installed"
            fi

            # Install dependencies
            local packages=("cmake" "ncurses" "micro" "python3" "git")
            for pkg in "${packages[@]}"; do
                print_status "Checking $pkg..."
                if brew list "$pkg" &>/dev/null; then
                    print_success "$pkg already installed"
                else
                    print_status "Installing $pkg..."
                    brew install "$pkg"
                    print_success "$pkg installed"
                fi
            done
            ;;

        "raspberry-pi"|"linux")
            local pkg_manager=$(detect_package_manager)
            print_status "Detected: Linux/Raspberry Pi (Package manager: $pkg_manager)"

            if [ "$pkg_manager" = "unknown" ]; then
                print_error "Could not detect package manager!"
                print_error "Please install these packages manually:"
                print_error "  - build-essential/base-devel, cmake, ncurses, micro"
                print_error "  - python3, python3-pip, aspell, rclone"
                exit 1
            fi

            print_status "Updating package lists..."
            update_packages

            # Install build tools
            print_status "Checking build tools..."
            if command_exists gcc && command_exists g++; then
                print_success "Build tools already installed"
            else
                print_status "Installing build tools..."
                case "$pkg_manager" in
                    "apt")
                        install_package "build-essential"
                        ;;
                    "dnf"|"yum")
                        install_package "gcc"
                        install_package "gcc-c++"
                        install_package "make"
                        ;;
                    "pacman")
                        install_package "base-devel"
                        ;;
                    "zypper")
                        sudo zypper install -y -t pattern devel_basis
                        ;;
                    "apk")
                        install_package "build-base"
                        ;;
                esac
                print_success "Build tools installed"
            fi

            # Install CMake
            print_status "Checking cmake..."
            if command_exists cmake; then
                print_success "cmake already installed"
            else
                print_status "Installing cmake..."
                install_package "cmake"
                print_success "cmake installed"
            fi

            # Install ncurses
            print_status "Checking ncurses..."
            if [ -f /usr/include/ncurses.h ] || [ -f /usr/include/ncursesw/ncurses.h ]; then
                print_success "ncurses already installed"
            else
                print_status "Installing ncurses..."
                case "$pkg_manager" in
                    "apt")
                        install_package "libncurses5-dev"
                        install_package "libncursesw5-dev"
                        ;;
                    "dnf"|"yum")
                        install_package "ncurses-devel"
                        ;;
                    "pacman")
                        install_package "ncurses"
                        ;;
                    "zypper")
                        install_package "ncurses-devel"
                        ;;
                    "apk")
                        install_package "ncurses-dev"
                        ;;
                esac
                print_success "ncurses installed"
            fi

            # Install micro editor
            print_status "Checking micro..."
            if command_exists micro; then
                print_success "micro already installed"
            else
                print_status "Installing micro editor..."
                cd /tmp
                curl https://getmic.ro | bash
                sudo mv micro /usr/local/bin/
                cd - > /dev/null
                print_success "micro installed"
            fi

            # Install Python3 and pip
            print_status "Checking python3..."
            if command_exists python3; then
                print_success "python3 already installed"
            else
                print_status "Installing python3..."
                case "$pkg_manager" in
                    "apt")
                        install_package "python3"
                        install_package "python3-pip"
                        ;;
                    "dnf"|"yum")
                        install_package "python3"
                        install_package "python3-pip"
                        ;;
                    "pacman")
                        install_package "python"
                        install_package "python-pip"
                        ;;
                    "zypper")
                        install_package "python3"
                        install_package "python3-pip"
                        ;;
                    "apk")
                        install_package "python3"
                        install_package "py3-pip"
                        ;;
                esac
                print_success "python3 installed"
            fi

            # Install git (required for Speltti plugin)
            print_status "Checking git..."
            if command_exists git; then
                print_success "git already installed"
            else
                print_status "Installing git..."
                install_package "git"
                print_success "git installed"
            fi

            ;;

        *)
            print_error "Unsupported operating system"
            exit 1
            ;;
    esac

    # Install Python packages
    print_status "Checking python-docx..."
    if python3 -c "import docx" 2>/dev/null; then
        print_success "python-docx already installed"
    else
        print_status "Installing python-docx..."
        pip3 install --user python-docx
        print_success "python-docx installed"
    fi

    # Install rclone (optional for cloud sync)
    print_status "Checking rclone..."
    if command_exists rclone; then
        print_success "rclone already installed"
    else
        print_status "Installing rclone..."
        curl https://rclone.org/install.sh | sudo bash
        print_success "rclone installed"
    fi

    print_success "All dependencies installed!"
}

# Setup spellchecker
setup_spellcheck() {
    print_header "Configuring Spell Checker"

    # Check if git is installed (required for cloning Speltti)
    if ! command_exists git; then
        print_error "git is not installed - cannot install Speltti plugin"
        print_status "Speltti plugin requires git to install"
        return 0
    fi

    print_success "git found"

    # Install hunspell (backend for Speltti)
    print_status "Installing hunspell spell checker..."
    local pkg_manager=$(detect_package_manager)
    local os_type=$(detect_os)

    if [[ "$os_type" == "macos" ]]; then
        # macOS
        if brew list hunspell &>/dev/null; then
            print_success "hunspell already installed"
        else
            print_status "Installing hunspell via Homebrew..."
            brew install hunspell || print_warning "hunspell installation failed"
        fi
    else
        # Linux/Raspberry Pi
        case "$pkg_manager" in
            "apt")
                install_package "hunspell" || print_warning "hunspell installation failed"
                install_package "hunspell-en-us" || print_warning "hunspell-en-us installation failed"
                ;;
            "dnf"|"yum")
                install_package "hunspell" || print_warning "hunspell installation failed"
                install_package "hunspell-en-US" || print_warning "hunspell-en-US installation failed"
                ;;
            "pacman")
                install_package "hunspell" || print_warning "hunspell installation failed"
                install_package "hunspell-en_us" || print_warning "hunspell-en_us installation failed"
                ;;
            "zypper")
                install_package "hunspell" || print_warning "hunspell installation failed"
                install_package "hunspell_en_US" || print_warning "hunspell_en_US installation failed"
                ;;
            "apk")
                install_package "hunspell" || print_warning "hunspell installation failed"
                install_package "hunspell-en" || print_warning "hunspell-en installation failed"
                ;;
        esac
    fi

    if command_exists hunspell; then
        print_success "hunspell installed"
    else
        print_warning "hunspell may not be installed correctly"
    fi

    # Install Speltti plugin
    print_status "Installing Speltti plugin for micro..."

    # Create plugin directory
    mkdir -p ~/.config/micro/plug || {
        print_error "Could not create micro plugin directory"
        return 0
    }

    # Check if Speltti is already installed
    if [ -d ~/.config/micro/plug/speltti ]; then
        print_status "Speltti already installed, updating..."
        rm -rf ~/.config/micro/plug/speltti
    fi

    # Clone Speltti
    print_status "Cloning Speltti from GitHub..."
    if git clone https://github.com/kyoto-shift/speltti.git ~/.config/micro/plug/speltti 2>&1; then
        if [ -d ~/.config/micro/plug/speltti ]; then
            print_success "Speltti plugin installed successfully!"
        else
            print_error "Speltti directory not found after clone"
            return 0
        fi
    else
        print_error "Failed to clone Speltti plugin"
        print_status "You can install it manually: git clone https://github.com/kyoto-shift/speltti.git ~/.config/micro/plug/speltti"
        return 0
    fi

    # Configure micro
    print_status "Configuring micro editor..."
    mkdir -p ~/.config/micro || {
        print_error "Could not create micro config directory"
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
        print_success "Micro editor configured!"
        print_success "Spell checker setup complete!"
        echo ""
        print_status "Speltti will automatically check spelling as you type"
        print_status "Press Ctrl+K on a misspelled word for suggestions"
    else
        print_error "Could not create micro settings file"
    fi
}

# Build the application
build_app() {
    print_header "Building Typewriter"

    cd "$INSTALL_DIR"

    # Create build directory
    if [ ! -d "cmake-build-typewriter" ]; then
        mkdir -p cmake-build-typewriter
    fi

    cd cmake-build-typewriter

    print_status "Running CMake..."
    cmake .. -DCMAKE_BUILD_TYPE=Release

    print_status "Compiling..."
    cmake --build . --config Release

    if [ -f "typewriter" ]; then
        print_success "Build successful!"
        return 0
    else
        print_error "Build failed!"
        return 1
    fi
}

# Create launcher script
create_launcher() {
    print_header "Creating Launcher"

    cat > "$INSTALL_DIR/typewriter" << 'EOF'
#!/bin/bash
# Typewriter Launcher
cd "$HOME/.typewriter"
./cmake-build-typewriter/typewriter "$@"
EOF

    chmod +x "$INSTALL_DIR/typewriter"

    # Create symlink in PATH
    if [ -d "$HOME/bin" ]; then
        ln -sf "$INSTALL_DIR/typewriter" "$HOME/bin/typewriter" 2>/dev/null || true
    elif [ -d "$HOME/.local/bin" ]; then
        mkdir -p "$HOME/.local/bin"
        ln -sf "$INSTALL_DIR/typewriter" "$HOME/.local/bin/typewriter" 2>/dev/null || true
    fi

    print_success "Launcher created!"
}

# Copy files to installation directory
copy_files() {
    print_header "Installing Files"

    # Create installation directory
    mkdir -p "$INSTALL_DIR"

    print_status "Copying files..."

    # Copy source files
    cp "$SCRIPT_DIR/main.cpp" "$INSTALL_DIR/"
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

    print_success "Files installed to $INSTALL_DIR"
}

# Update installation
update_installation() {
    print_header "Checking for Updates"

    if [ ! -d "$INSTALL_DIR" ]; then
        print_error "Typewriter not installed. Please run installation first."
        return 1
    fi

    print_status "Comparing files..."

    local needs_update=false

    # Check if source files have changed
    if ! cmp -s "$SCRIPT_DIR/main.cpp" "$INSTALL_DIR/main.cpp" 2>/dev/null; then
        print_warning "main.cpp has changed"
        needs_update=true
    fi

    if ! cmp -s "$SCRIPT_DIR/CMakeLists.txt" "$INSTALL_DIR/CMakeLists.txt" 2>/dev/null; then
        print_warning "CMakeLists.txt has changed"
        needs_update=true
    fi

    if [ "$needs_update" = false ]; then
        print_success "Typewriter is up to date!"
        return 0
    fi

    echo ""
    read -p "Updates available. Install updates? (y/n): " update_choice

    if [[ "$update_choice" =~ ^[Yy]$ ]]; then
        print_status "Updating..."
        copy_files
        build_app
        print_success "Typewriter updated successfully!"
        return 0
    else
        print_status "Update cancelled"
        return 0
    fi
}

# Full installation
full_installation() {
    local os_type=$1

    print_header "Typewriter Installation"
    echo -e "Installation directory: ${GREEN}$INSTALL_DIR${NC}\n"

    # Install dependencies
    install_dependencies "$os_type"

    # Copy files
    copy_files

    # Build application
    if ! build_app; then
        print_error "Installation failed during build"
        exit 1
    fi

    # Setup spellcheck
    setup_spellcheck

    # Create launcher
    create_launcher

    print_header "Installation Complete!"
    print_success "Typewriter installed successfully!"
    echo ""
    print_status "You can run Typewriter using:"
    echo "  $INSTALL_DIR/typewriter"
    echo "  or simply: typewriter (if PATH is configured)"
    echo ""
}

# Main menu with arrow key navigation and ESC to exit
show_menu() {
    local selected=0
    local options=("Standard Installation (Recommended)" "Raspberry Pi Setup" "Update Existing Installation" "Exit")

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
┃              📝  Typewriter Installer  📝            ┃
┃                                                      ┃
┃                simple note organization              ┃
┃                                                      ┃
┃                                                      ┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

EOF
    echo "Select Installation Mode:"
    echo ""

    # Function to redraw just the options
    draw_options() {
        for i in "${!options[@]}"; do
            # Move cursor to line (starting at line 11)
            tput cup $((11 + i)) 0
            # Clear the line and redraw
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
    tput cup $((11 + ${#options[@]} + 1)) 0
    echo ""
    echo "Use ↑/↓ arrows or 1-4 to select, Enter to confirm, ESC to exit"

    while true; do

        # Read input
        read -rsn1 key

        # Check for escape sequences
        if [[ $key == $'\x1b' ]]; then
            # Read the rest of the sequence
            read -rsn2 -t 0.01 rest
            key+="$rest"
        fi

        # Handle the input
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
            [1-4])  # Number keys
                selected=$((key - 1))
                break
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
                sudo tee /etc/systemd/system/typewriter.service > /dev/null << EOF
[Unit]
Description=Typewriter App
After=network.target

[Service]
Type=simple
User=$USER
WorkingDirectory=$INSTALL_DIR
ExecStart=$INSTALL_DIR/typewriter
Restart=always

[Install]
WantedBy=multi-user.target
EOF
                sudo systemctl enable typewriter.service
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
}

# Ask to start typewriter
ask_to_start() {
    echo ""
    read -p "Would you like to start Typewriter now? (y/n): " start_choice

    if [[ "$start_choice" =~ ^[Yy]$ ]]; then
        print_status "Starting Typewriter..."
        cd "$INSTALL_DIR"
        ./typewriter
    else
        echo ""
        print_success "You can access Typewriter at any moment by using:"
        echo "  $INSTALL_DIR/typewriter"
        echo "  or: typewriter"
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

    # Show menu
    show_menu

    # Ask to start
    ask_to_start
}

# Run main
main "$@"

