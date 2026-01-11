# File Manager with Automated MLA Formatting

A terminal-based file manager with built-in MLA essay formatting. Create, edit, and manage files with automatic MLA formatting conversion to Word documents.

## Installation

### Required Dependencies

Install all required dependencies:

```bash
# Install ncurses (terminal UI library)
brew install ncurses

# Install micro text editor
brew install micro

# Install Python dependencies for MLA formatting
pip3 install python-docx
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

- **Multi-column view** with pagination (up to 3 columns per page)
- **Template system** for easy document creation
- **Automatic .txt extension** for MLA essays
- **Direct .docx opening** in Word/Pages
- **Folder navigation** with breadcrumb display
- **File operations**: create, rename, delete with confirmation
- **Status messages** for all operations
- **Cancel support** for all input operations (press 'c')

## License

Open source project.

