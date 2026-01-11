#!/bin/bash
# Script to fix Google Drive rclone authentication issues

echo "====================================="
echo "Google Drive Authentication Fix"
echo "====================================="
echo ""
echo "Your Google Drive remote has authentication issues."
echo "This usually means the OAuth token has expired or the"
echo "client ID/secret is invalid."
echo ""
echo "OPTION 1: Reconnect (Recommended - Quick Fix)"
echo "This will refresh your authentication."
echo ""
echo "OPTION 2: Delete and Recreate"
echo "Start fresh with a new configuration."
echo ""
read -p "Choose option (1 or 2): " choice

if [ "$choice" == "1" ]; then
    echo ""
    echo "Opening rclone config..."
    echo "Steps:"
    echo "  1. Select your 'Google Drive' remote (usually 'g' or the number shown)"
    echo "  2. Choose 'reconnect' or 'n' for new auth"
    echo "  3. Follow the browser authentication prompts"
    echo "  4. When done, choose 'q' to quit"
    echo ""
    read -p "Press ENTER to continue..."
    rclone config reconnect "Google Drive" || rclone config
elif [ "$choice" == "2" ]; then
    echo ""
    echo "Opening rclone config to delete and recreate..."
    echo "Steps:"
    echo "  1. Select 'd' to delete your 'Google Drive' remote"
    echo "  2. Confirm the deletion"
    echo "  3. Select 'n' to create new remote"
    echo "  4. Name it 'Google Drive' (or whatever you prefer)"
    echo "  5. Choose 'drive' for Google Drive"
    echo "  6. Leave client ID/secret blank (press ENTER) to use defaults"
    echo "  7. Follow all prompts and authenticate via browser"
    echo "  8. When done, choose 'q' to quit"
    echo ""
    read -p "Press ENTER to continue..."
    rclone config
else
    echo "Invalid option"
    exit 1
fi

echo ""
echo "====================================="
echo "Testing connection..."
echo "====================================="
if rclone lsd "Google Drive:" --max-depth 1 2>/dev/null; then
    echo "✓ SUCCESS! Your Google Drive is now connected."
    echo ""
    echo "You can now use the typewriter app with cloud sync enabled."
else
    echo "✗ Connection still failing."
    echo ""
    echo "Try these steps:"
    echo "1. Make sure you're using the default rclone client"
    echo "   (don't set custom client_id/client_secret unless needed)"
    echo "2. Run: rclone config"
    echo "3. Delete the old remote and create a new one"
    echo "4. Make sure to authenticate in the browser when prompted"
    echo ""
    echo "Or temporarily disable cloud sync in the app settings (press 't')"
fi

echo ""
read -p "Press ENTER to exit..."

