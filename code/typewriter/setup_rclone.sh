#!/bin/bash
# Rclone Setup Script for Typewriter App
# This script helps you set up cloud storage integration

echo "============================================"
echo "  Typewriter Cloud Storage Setup (rclone)"
echo "============================================"
echo ""

# Check if rclone is installed
if ! command -v rclone &> /dev/null; then
    echo "❌ rclone is not installed."
    echo ""
    echo "Installing rclone..."
    curl https://rclone.org/install.sh | sudo bash

    if [ $? -ne 0 ]; then
        echo "❌ Failed to install rclone. Please install manually:"
        echo "   Visit: https://rclone.org/install/"
        exit 1
    fi
    echo "✅ rclone installed successfully!"
else
    echo "✅ rclone is already installed (version: $(rclone version | head -n1))"
fi

echo ""
echo "--------------------------------------------"
echo "  Step 1: Configure Cloud Storage Remote"
echo "--------------------------------------------"
echo ""
echo "You can connect to many cloud providers:"
echo "  - Google Drive"
echo "  - Dropbox"
echo "  - OneDrive"
echo "  - Amazon S3"
echo "  - Box"
echo "  - And 40+ more!"
echo ""

read -p "Do you want to configure a new remote now? (y/n): " configure
if [ "$configure" = "y" ] || [ "$configure" = "Y" ]; then
    echo ""
    echo "Starting rclone configuration wizard..."
    echo "Tip: Name your remote something simple like 'mycloud' or 'gdrive'"
    echo ""
    rclone config
else
    echo ""
    echo "Skipping remote configuration."
    echo "You can configure later by running: rclone config"
fi

echo ""
echo "--------------------------------------------"
echo "  Step 2: Test Your Configuration"
echo "--------------------------------------------"
echo ""

# List available remotes
remotes=$(rclone listremotes)
if [ -z "$remotes" ]; then
    echo "⚠️  No remotes configured yet."
    echo "   Run 'rclone config' to set up a cloud storage provider."
else
    echo "Available remotes:"
    echo "$remotes"
    echo ""

    # Ask which remote to use
    read -p "Enter the remote name to test (or press Enter to skip): " remote_name
    if [ ! -z "$remote_name" ]; then
        # Remove trailing colon if present
        remote_name=${remote_name%:}

        echo "Testing connection to ${remote_name}..."
        if rclone lsd ${remote_name}: &> /dev/null; then
            echo "✅ Connection successful!"
        else
            echo "❌ Connection failed. Please check your configuration."
        fi
    fi
fi

echo ""
echo "--------------------------------------------"
echo "  Step 3: Update main.cpp Configuration"
echo "--------------------------------------------"
echo ""
echo "To enable cloud sync, edit main.cpp and update these lines:"
echo ""
echo "  const bool ENABLE_CLOUD_SYNC = true;  // Change from false to true"
echo "  const std::string RCLONE_REMOTE = \"mycloud\";  // Your remote name"
echo "  const std::string RCLONE_PATH = \"typewriter/files\";  // Cloud folder path"
echo ""

if [ ! -z "$remote_name" ]; then
    echo "For your setup, use:"
    echo "  const std::string RCLONE_REMOTE = \"${remote_name}\";"
fi

echo ""
echo "--------------------------------------------"
echo "  Optional: Mount Cloud Storage"
echo "--------------------------------------------"
echo ""
echo "You can mount your cloud storage as a local folder:"
echo ""
echo "  # Create mount point"
echo "  mkdir -p ~/cloud"
echo ""
echo "  # Mount cloud storage (replace 'mycloud' with your remote name)"
echo "  rclone mount mycloud: ~/cloud --daemon --vfs-cache-mode writes"
echo ""
echo "  # Then in main.cpp, use:"
echo "  const std::string BASE_DIR = \"/home/\$USER/cloud/typewriter/files\";"
echo ""
echo "This allows direct read/write to cloud without manual syncing."
echo ""

echo "============================================"
echo "  Setup Complete!"
echo "============================================"
echo ""
echo "Next steps:"
echo "  1. Edit main.cpp and enable cloud sync (see above)"
echo "  2. Recompile: ./build_and_run.sh"
echo "  3. Your files will automatically sync to the cloud!"
echo ""
echo "Useful commands:"
echo "  rclone config        - Configure cloud providers"
echo "  rclone listremotes   - List configured remotes"
echo "  rclone ls mycloud:   - List files on cloud"
echo "  rclone copy files/ mycloud:typewriter/files/  - Manual sync"
echo ""

