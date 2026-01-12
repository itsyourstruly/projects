# Jot

A terminal-based file manager with MLA essay formatting, smart notes, and cloud sync support.

## Installation

Run the installer:
```bash
./install.sh
```

The installer will:
- Detect your OS and package manager
- Install all required dependencies (cmake, ncurses, micro, python3, rclone)
- Build the application
- Configure micro editor
- Install to `~/.jot`
- Create a launcher script in the project directory

## Quick Start

After installation, you can run Jot from anywhere:
```bash
./jot              # From the project directory
~/.jot/jot         # From anywhere
jot                # From anywhere (if ~/bin or ~/.local/bin is in PATH)
```

## Controls

### File Manager
| Key | Action |
|-----|--------|
| `↑↓←→` | Navigate files |
| `Enter` | Open file/folder |
| `Backspace` | Go up one folder |
| `n` | Create new note |
| `f` | Create new folder |
| `r` | Rename file/folder |
| `d` | Delete file/folder |
| `c` | Convert to MLA format |
| `s` | Selection mode (multi-select) |
| `u` | Undo last action |
| `t` | Settings menu |
| `q` | Quit |

### Editor (Micro)
| Key | Action |
|-----|--------|
| `Ctrl+S` | Save |
| `Ctrl+Q` | Quit |
| `Ctrl+F` | Find |
| `Ctrl+H` | Find and replace |
| `Ctrl+Z` | Undo |
| `Ctrl+Y` | Redo |


### Settings Menu (Press 't')
1. View Remotes - List configured cloud storage
2. Sync Status - Test connection and view settings
3. Switch Remote - Change cloud storage provider
4. Toggle Cloud Sync - Enable/disable sync
5. Toggle Auto Sync - Auto vs manual sync
6. Sync Now - Manually sync files
7. Setup New Remote - Configure cloud storage
8. Test Connection - Verify remote connection
9. Change Sync Type - Full sync or one-way

## Features

### Smart Notes
Files with `{S}` prefix are automatically formatted on save:
- `1. Title` → Section headers
- `1a. Item` → Bullets under section 1
- `a. Item` → Bullet in current section

Organized numerically and alphabetically on save.

### MLA Formatting
Convert any text file to MLA format:
1. Highlight file
2. Press `c`
3. Opens as formatted .docx

### Cloud Sync
Supports 40+ cloud providers via rclone:
- Google Drive
- Dropbox
- OneDrive
- And more


## File Organization

### Installation Directory
```
~/.typewriter/
├── main.cpp              # Source code
├── CMakeLists.txt        # Build config
├── typewriter            # Launcher script
├── cmake-build-typewriter/
│   └── typewriter       # Compiled binary
├── templates/           # Note templates
│   ├── smart.txt
│   ├── mla_format.txt
│   └── smart_format.py
└── files/               # Your notes
```

### File Types
- Regular files - Plain text
- `{S}` prefix - Smart notes (auto-formatted)
- `[]` prefix - Folders

## Templates

### Blank
Empty note for quick writing.

### MLA
Pre-formatted for essays with proper headers and spacing.

### Smart
Auto-organizing notes with sections and bullets.

## Dependencies

Automatically installed by installer:
- cmake
- ncurses
- micro editor
- Python 3 & python-docx
- aspell (spellcheck)
- rclone (cloud sync)

## Platform Support

- Linux (Debian, Ubuntu, Fedora, Arch, openSUSE, Alpine)
- macOS
- Raspberry Pi (with auto-start option)

## Updating

Check for and install updates:
```bash
./install.sh --update
```

## Uninstall

```bash
rm -rf ~/.typewriter
```

## Tips

- **Multi-select**: Press `s`, use arrows to select multiple files, press `s` again to lock, then `d` to delete all
- **Undo**: Press `u` to undo the last delete, rename, or move
- **Smart notes**: Type anywhere in the file, sections and bullets organize on save
- **Cloud sync**: Enable auto-sync to backup every time you save a file

## Troubleshooting

### Can't run typewriter command
Add to PATH:
```bash
echo 'export PATH="$HOME/.typewriter:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

### Build fails
Check dependencies:
```bash
cmake --version
g++ --version
```

Reinstall:
```bash
./install.sh
```

### Cloud sync not working
Test connection:
1. Press `t` in app
2. Select "Test Connection"
3. Follow fix wizard if issues

## License

MIT

