#!/bin/bash
# Quick test script for rclone configuration

echo "════════════════════════════════════════════════"
echo "  Rclone Configuration Check"
echo "════════════════════════════════════════════════"
echo ""

# Check if rclone is installed
if command -v rclone &> /dev/null; then
    echo "✅ rclone is installed"
    rclone version | head -1
    echo ""
else
    echo "❌ rclone is NOT installed"
    echo ""
    echo "Install it with:"
    echo "  macOS:  brew install rclone"
    echo "  Linux:  curl https://rclone.org/install.sh | sudo bash"
    echo ""
    exit 1
fi

# Check for configured remotes
echo "Checking configured remotes..."
echo ""

remotes=$(rclone listremotes 2>/dev/null)
if [ -z "$remotes" ]; then
    echo "❌ No remotes configured yet"
    echo ""
    echo "To configure a remote, run:"
    echo "  rclone config"
    echo ""
    echo "Then:"
    echo "  1. Press 'n' for new remote"
    echo "  2. Name it (e.g., 'gdrive' or 'dropbox')"
    echo "  3. Choose your cloud provider"
    echo "  4. Follow the authorization prompts"
    echo ""
else
    echo "✅ Found remotes:"
    echo "$remotes"
    echo ""

    # Test each remote
    echo "Testing connections..."
    echo ""
    while IFS= read -r remote; do
        remote_name=${remote%:}
        echo -n "  Testing $remote_name... "
        if timeout 5 rclone lsd "$remote" &> /dev/null; then
            echo "✅ Connected"
        else
            echo "❌ Failed (check internet or reauthorize)"
        fi
    done <<< "$remotes"
    echo ""

    # Show what to put in main.cpp
    first_remote=$(echo "$remotes" | head -1)
    remote_name=${first_remote%:}

    echo "════════════════════════════════════════════════"
    echo "  Configuration for main.cpp"
    echo "════════════════════════════════════════════════"
    echo ""
    echo "Update line ~26 in main.cpp:"
    echo ""
    echo "  const bool ENABLE_CLOUD_SYNC = true;"
    echo "  const std::string RCLONE_REMOTE = \"$remote_name\";"
    echo "  const std::string RCLONE_PATH = \"typewriter/files\";"
    echo ""
fi

echo "════════════════════════════════════════════════"
echo "  Next Steps"
echo "════════════════════════════════════════════════"
echo ""

if [ ! -z "$remotes" ]; then
    echo "1. Update RCLONE_REMOTE in main.cpp (see above)"
    echo "2. Rebuild: ./build_and_run.sh"
    echo "3. Your files will auto-sync to cloud!"
    echo ""
    echo "Test manual sync:"
    first_remote=$(echo "$remotes" | head -1)
    remote_name=${first_remote%:}
    echo "  rclone copy files/ $remote_name:typewriter/files/"
else
    echo "1. Run: rclone config"
    echo "2. Configure your cloud provider"
    echo "3. Run this script again: ./test_rclone.sh"
fi

echo ""

