===========================================================================================
CTEXT v1.0
Text Editor
Developer: ExEintel
= ... The program's interface is as close as possible to the classic Windows Notepad, ensuring intuitive operation.

FEATURES
-----------
- All code in ONE main.c file
- No resource file (.rc) required
- Dialogs are generated programmatically
- Minimal dependencies
- Support for x86 (32-bit) and x64 (64-bit) architectures

REQUIREMENTS
----------
- Operating system: Windows 7/8/10/11 (32 or 64 bit)
- Compiler: MinGW-w64 (GCC) with target architecture support
- Libraries: kernel32, user32, gdi32, comdlg32, comctl32, shlwapi

PROJECT STRUCTURE
-----------------
Ctext/
├── Ctext32.exe # 32-bit version
├── Ctext64.exe # 64-bit version
├── source/
│ └── main.c # Single source file (all code here)
└── Readme.txt # This file

COMPILATION
-----------

=== 32-bit version (x86) ===
Command to compile a 32-bit executable:

gcc -m32 main.c -o Ctext32.exe -mwindows -lcomdlg32 -lcomctl32 -lshlwapi -O2 -s

=== 64-bit version (x64) ===
Command to compile a 64-bit executable:

gcc -m64 main.c -o Ctext64.exe -mwindows -lcomdlg32 -lcomctl32 -lshlwapi -O2 -s

=== General Compilation Options ===
-m32 / -m64 # Target architecture (32 or 64 bits)
-mwindows # GUI application (no console)
-lcomdlg32 # File open/save dialogs, font selection
-lcomctl32 # Common Controls (status line)
-lshlwapi # Path functions (PathFindFileName)
-O2 # Speed ​​Optimization
-s # Remove debug symbols (reduces size)

=== Compile both versions at once ===

gcc -m32 main.c -o Ctext32.exe -mwindows -lcomdlg32 -lcomctl32 -lshlwapi -O2 -s
gcc -m64 main.c -o Ctext64.exe -mwindows -lcomdlg32 -lcomctl32 -lshlwapi -O2 -s

KEYSTORAGE
--------------
Ctrl+N Create a new file
Ctrl+O Open file
Ctrl+S Save
F12 Save as
Ctrl+Z Undo
Ctrl+X Cut
Ctrl+C Copy
Ctrl+V Paste
Del Delete
Ctrl+A Select all
Ctrl+F Find
Ctrl+H Replace
Ctrl+G Go to line
F5 Insert date and time
F1 About
Alt+F4 Exit

FUNCTIONALITY
----------------
✓ Create, open, and save text files
✓ Supported encodings: ANSI, UTF-8, UTF-16 LE/BE (autodetection)
✓ Save in UTF-8 with BOM
✓ Word Wrap
✓ Font and size selection
✓ Status bar with cursor position information
✓ Find and replace text (case-sensitive/case-insensitive)
✓ Go to a specified line
✓ Insert the current date and time
✓ Context menu (right mouse button)
✓ Confirm save changes

LIMITATIONS
-----------
- Maximum file size: 1 MB
- No drag & drop support (removed for compilation without shellapi.h)
- No syntax highlighting
- No document printing

TECHNICAL DETAILS
-----------------
- Language: C (C99)
- API: Windows API (Win32)
- Interface: Native Windows (Edit Control, Common Controls)
- Dialogs: created programmatically via CreateWindow
- Executable file size: ~30-50 KB (after (optimizations)
- Architectures: x86 (32-bit), x64 (64-bit)

CHANGES IN VERSION 1.0
----------------------
- Completely removed the resource file (.rc)
- The "About," "Find," "Replace," and "Go To" dialogs are created programmatically
- The accelerator table is created in code
- Removed Drag & Drop support (simplification of dependencies)
- All code is now in a single main.c file
- Added support for compilation for x86 and x64

LICENSE
--------
MIT

CONTACTS
--------
Developer: ExEintel

===================================================================================