# File Manager with Automated MLA Formatting

A terminal-based file manager with built-in MLA essay formatting. Create, edit, and manage files with automatic MLA formatting conversion to Word documents.

## ☁️ Cloud Storage Support

Automatically sync your files to Google Drive, Dropbox, OneDrive, and 40+ other cloud providers using rclone.

### Features
- 🔄 Auto-sync after create/edit/delete operations
- 🌐 Works offline (syncs when connection returns)
- 📱 Access files across all your devices
- 🔒 Supports encryption
- 🚀 Background sync (non-blocking)

### Quick Setup

**1. Install rclone:**
```bash
# macOS
brew install rclone

# Linux/Raspberry Pi
curl https://rclone.org/install.sh | sudo bash
```

**2. Configure your cloud provider:**
```bash
rclone config
```
Follow the prompts to connect Google Drive, Dropbox, OneDrive, etc. Give your remote a name (e.g., "gdrive" or "mycloud").

**3. Enable in code:**

Edit `main.cpp` (around line 26) and update:
```cpp
const bool ENABLE_CLOUD_SYNC = true;  // Change from false to true
const std::string RCLONE_REMOTE = "gdrive";  // Your remote name from step 2
const std::string RCLONE_PATH = "typewriter/files";  // Folder path on cloud
```

**4. Rebuild and run:**
```bash
./build_and_run.sh
```

That's it! Your files now automatically sync to the cloud.

### Supported Cloud Providers
- Google Drive (15GB free)
- Dropbox (2GB free)
- Microsoft OneDrive (5GB free)
- Amazon S3
- Box (10GB free)
- pCloud
- Mega (20GB free)
- iCloud Drive
- NextCloud (self-hosted)
- And 40+ more!

### Manual Sync Commands

```bash
# Check configured remotes
rclone listremotes

# Upload to cloud
rclone copy files/ mycloud:typewriter/files/

# Download from cloud
rclone copy mycloud:typewriter/files/ files/

# View files on cloud
rclone ls mycloud:typewriter/files/

# Test connection
rclone lsd mycloud:
```

### Alternative: Mount Cloud Storage

Instead of automatic sync, you can mount cloud storage as a local folder:

```bash
# Create mount point
mkdir ~/cloud

# Mount (replace 'mycloud' with your remote name)
rclone mount mycloud: ~/cloud --daemon --vfs-cache-mode writes

# Update main.cpp to use mounted path
const bool ENABLE_CLOUD_SYNC = false;  // Not needed with mount
const std::string BASE_DIR = "/home/username/cloud/typewriter/files";
```

This provides real-time sync but requires an internet connection.

### Cloud Sync Workflow Examples

**Multi-Device Usage:**
1. Write on Computer 1 → Auto-syncs to cloud
2. Open app on Computer 2 → Downloads from cloud on startup
3. Edit on Computer 2 → Auto-syncs back
4. Access via phone → Open files in Google Drive/Dropbox app
5. Everything stays in sync! ☁️

**Raspberry Pi Cloud Setup:**
```bash
# Install everything
./setup_pi.sh

# Edit main.cpp to enable sync
const bool ENABLE_CLOUD_SYNC = true;
const std::string RCLONE_REMOTE = "gdrive";

# Rebuild
./build_and_run.sh

# Your Pi now syncs to cloud automatically!
```

**Auto-start on Boot (Raspberry Pi):**
```bash
sudo nano /etc/systemd/system/typewriter.service
```

Add:
```ini
[Unit]
Description=Typewriter App
After=network.target

[Service]
Type=simple
User=pi
WorkingDirectory=/home/pi/typewriter
ExecStart=/home/pi/typewriter/typewriter
Restart=always

[Install]
WantedBy=multi-user.target
```

Enable:
```bash
sudo systemctl enable typewriter.service
sudo systemctl start typewriter.service
```

## Installation

### Raspberry Pi Setup

Quick setup with the automated script:

```bash
./setup_pi.sh
```

This installs dependencies, sets up rclone (optional), and configures auto-start on boot.

Or run the rclone setup separately:
```bash
./setup_rclone.sh  # Interactive rclone configuration wizard
```

### Manual Installation

#### Required Dependencies

Install all required dependencies:

```bash
# For macOS:
brew install ncurses
brew install micro
pip3 install python-docx

# For Linux/Raspberry Pi:
sudo apt update
sudo apt install build-essential cmake libncurses5-dev libncursesw5-dev micro
pip3 install python-docx

# Optional: Install rclone for cloud sync
curl https://rclone.org/install.sh | sudo bash
```

## Compilation

Compile the program:

```bash
g++ -std=c++17 main.cpp -o filemanager -lncurses
```

Or use the build script (compiles and runs):

```bash
chmod +x build_and_run.sh
./build_and_run.sh
```

## Running

```bash
./filemanager
```

## File Manager Controls

| Key | Action |
|-----|--------|
| `↑` / `↓` | Navigate files |
| `←` / `→` | Navigate columns/pages |
| `ENTER` | Open file/folder |
| `n` | Create new file |
| `f` | Create new folder |
| `r` | Rename file/folder |
| `DEL` or `d` | Delete file/folder |
| `c` | Convert to Word (MLA format) |
| `b` | Go back to parent folder |
| `q` | Quit |

## Micro Editor Controls

When editing files in micro:

| Key | Action |
|-----|--------|
| `Ctrl-S` | Save file |
| `Ctrl-Q` | Quit micro |
| `Ctrl-G` | Open help menu |
| `Ctrl-F` | Find text |
| `Ctrl-C` | Copy |
| `Ctrl-X` | Cut |
| `Ctrl-V` | Paste |
| `Ctrl-Z` | Undo |
| `Ctrl-Y` | Redo |
| `Ctrl-A` | Select all |
| `Ctrl-E` | Command mode |

## Creating an MLA Essay

1. **Create file**: Press `n`, enter filename (extension `.txt` added automatically for MLA)
2. **Select template**: Choose "Mla Format"
3. **Edit in micro**:
   - Line 1: Your full name (e.g., "John Doe")
   - Line 3: Instructor's name
   - Line 5: Course title
   - Line 7: Date (format: Day Month Year, e.g., "10 January 2026")
   - Line 9: Your essay title
   - Line 11+: Write your essay content
4. **Save & quit**: `Ctrl-S` then `Ctrl-Q`
5. **Convert**: Press `c` on your `.txt` file
6. **Open**: Press `ENTER` on the `.docx` file - fully formatted and ready to submit!

## Automatic MLA Formatting

When you convert a file with `c`, the system automatically applies:

- ✅ Header with last name + page number (upper right on every page)
- ✅ Student info block (name, instructor, course, date) - upper left
- ✅ Title centered
- ✅ Times New Roman 12pt font throughout
- ✅ Double spacing (2.0)
- ✅ 1-inch margins on all sides
- ✅ Body paragraphs indented 0.5 inches
- ✅ Works Cited centered and bold
- ✅ Citations with hanging indent

**No manual formatting required in Word!**

## Project Structure

```
typewriter/
├── main.cpp              # Main program source
├── filemanager           # Compiled executable
├── build_and_run.sh      # Build and run script
├── README.md             # This file
├── templates/
│   ├── mla_format.txt    # MLA essay template
│   └── convert_to_mla.py # Python conversion script
└── files/                # Your documents (auto-created)
```

## Features

- **Cloud sync support** with rclone (Google Drive, Dropbox, OneDrive, etc.)
- **Multi-column view** with pagination (up to 3 columns per page)
- **Template system** for easy document creation
- **Automatic .txt extension** for MLA essays
- **Direct .docx opening** in Word/Pages
- **Folder navigation** with breadcrumb display
- **File operations**: create, rename, delete with confirmation
- **Selection mode**: select and move multiple files
- **Undo support**: undo last file operation
- **Status messages** for all operations
- **Cancel support** for all input operations (press 'c')
- **Raspberry Pi ready**: lightweight and efficient

## Troubleshooting

### Cloud Sync Issues

**Files not syncing?**
```bash
# Verify cloud sync is enabled
grep ENABLE_CLOUD_SYNC main.cpp  # Should show: true

# Check your remote name
rclone listremotes

# Test connection
rclone lsd "your-remote-name:"

# Test manual sync
rclone copy files/ "your-remote-name:typewriter/files/" -v
```

**Remote name has spaces?**

If your remote is named "Google Drive" (with space), make sure you're using quotes in commands:
```bash
rclone ls "Google Drive:typewriter/files/"
```

The code already handles this automatically.

**Connection errors?**
```bash
# Reauthorize your remote
rclone config reconnect "your-remote-name"

# Or reconfigure from scratch
rclone config
```

**Need verbose output?**

Edit the sync functions in `main.cpp` and remove `--quiet` flag to see sync activity.

### General Issues

**Build errors?**
- Make sure ncurses is installed: `brew install ncurses` (macOS) or `sudo apt install libncurses5-dev` (Linux)
- Use C++17: `g++ -std=c++17 main.cpp -o typewriter -lncurses`

**Micro editor not found?**
```bash
# macOS
brew install micro

# Linux/Raspberry Pi
sudo apt install micro
```

**Python conversion not working?**
```bash
pip3 install python-docx
```

## License

Open source project.

