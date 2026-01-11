#!/bin/bash
# Quick setup script for Raspberry Pi

echo "=========================================="
echo "  Typewriter - Raspberry Pi Quick Setup"
echo "=========================================="
echo ""

# Check if we're on a Raspberry Pi
if [ -f /proc/device-tree/model ]; then
    model=$(cat /proc/device-tree/model)
    echo "Detected: $model"
else
    echo "Note: Not detected as Raspberry Pi, but continuing..."
fi

echo ""
echo "This script will:"
echo "  1. Install dependencies"
echo "  2. Set up rclone for cloud storage"
echo "  3. Build the application"
echo "  4. (Optional) Configure auto-start on boot"
echo ""

read -p "Continue? (y/n): " continue
if [ "$continue" != "y" ] && [ "$continue" != "Y" ]; then
    echo "Setup cancelled."
    exit 0
fi

echo ""
echo "--------------------------------------------"
echo "  Installing Dependencies"
echo "--------------------------------------------"
echo ""

# Update package list
echo "Updating package list..."
sudo apt update

# Install required packages
echo "Installing ncurses, cmake, and other dependencies..."
sudo apt install -y \
    build-essential \
    cmake \
    libncurses5-dev \
    libncursesw5-dev \
    git \
    micro

if [ $? -eq 0 ]; then
    echo "✅ Dependencies installed successfully!"
else
    echo "❌ Failed to install some dependencies."
    exit 1
fi

echo ""
echo "--------------------------------------------"
echo "  Setting up Cloud Storage (rclone)"
echo "--------------------------------------------"
echo ""

./setup_rclone.sh

echo ""
echo "--------------------------------------------"
echo "  Building Application"
echo "--------------------------------------------"
echo ""

if [ -f "build_and_run.sh" ]; then
    chmod +x build_and_run.sh
    ./build_and_run.sh
else
    echo "Building with CMake..."
    mkdir -p build
    cd build
    cmake ..
    cmake --build .
    cd ..
    echo "✅ Build complete! Executable: ./build/typewriter"
fi

echo ""
echo "--------------------------------------------"
echo "  Optional: Auto-start on Boot"
echo "--------------------------------------------"
echo ""

read -p "Do you want the app to start automatically on boot? (y/n): " autostart
if [ "$autostart" = "y" ] || [ "$autostart" = "Y" ]; then
    echo ""
    echo "Creating systemd service..."

    # Get current directory
    INSTALL_DIR=$(pwd)

    # Create service file
    cat > /tmp/typewriter.service << EOF
[Unit]
Description=Typewriter Note-Taking App
After=network.target

[Service]
Type=simple
User=$USER
WorkingDirectory=$INSTALL_DIR
ExecStart=$INSTALL_DIR/typewriter
Restart=always
RestartSec=10
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
EOF

    # Install service
    sudo mv /tmp/typewriter.service /etc/systemd/system/
    sudo systemctl daemon-reload
    sudo systemctl enable typewriter.service

    echo "✅ Auto-start configured!"
    echo ""
    echo "Useful commands:"
    echo "  sudo systemctl start typewriter    - Start the service"
    echo "  sudo systemctl stop typewriter     - Stop the service"
    echo "  sudo systemctl status typewriter   - Check status"
    echo "  sudo journalctl -u typewriter -f   - View logs"
fi

echo ""
echo "=========================================="
echo "  Setup Complete!"
echo "=========================================="
echo ""
echo "Next steps:"
echo ""
echo "  1. To enable cloud sync, edit main.cpp:"
echo "     nano main.cpp"
echo "     Change: const bool ENABLE_CLOUD_SYNC = true;"
echo ""
echo "  2. Set your rclone remote name:"
echo "     const std::string RCLONE_REMOTE = \"mycloud\";"
echo ""
echo "  3. Rebuild:"
echo "     ./build_and_run.sh"
echo ""
echo "  4. Read the cloud setup guide:"
echo "     cat CLOUD_SETUP.md"
echo ""
echo "  5. Run the app:"
echo "     ./typewriter"
echo ""
echo "Happy writing! ✍️"
echo ""

