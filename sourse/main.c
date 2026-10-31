/*
 * Ctext - Simple Text Editor
 * Developer: ExEintel
 * Compilation 32-bit: gcc -m32 main.c -o Ctext32.exe -mwindows -lcomdlg32 -lcomctl32 -lshlwapi -O2 -s
 * Compilation 64-bit: gcc -m64 main.c -o Ctext64.exe -mwindows -lcomdlg32 -lcomctl32 -lshlwapi -O2 -s
 */

#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE

#include <windows.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Control IDs
#define ID_EDIT         100
#define ID_STATUSBAR    101

// Menu IDs
#define IDM_FILE_NEW        1001
#define IDM_FILE_OPEN       1002
#define IDM_FILE_SAVE       1003
#define IDM_FILE_SAVEAS     1004
#define IDM_FILE_EXIT       1005
#define IDM_EDIT_UNDO       2001
#define IDM_EDIT_CUT        2002
#define IDM_EDIT_COPY       2003
#define IDM_EDIT_PASTE      2004
#define IDM_EDIT_DELETE     2005
#define IDM_EDIT_SELECTALL  2006
#define IDM_EDIT_FIND       2007
#define IDM_EDIT_REPLACE    2008
#define IDM_EDIT_GOTO       2009
#define IDM_EDIT_DATETIME   2010
#define IDM_FORMAT_WORDWRAP 3001
#define IDM_FORMAT_FONT     3002
#define IDM_VIEW_STATUSBAR  4001
#define IDM_HELP_ABOUT      5001

// Dialog IDs
#define IDD_ABOUT       100
#define IDD_FIND        101
#define IDD_REPLACE     102
#define IDD_GOTO        103

// Dialog control IDs
#define IDC_FINDTEXT    1001
#define IDC_REPLACETEXT 1002
#define IDC_MATCHCASE   1003
#define IDC_FINDDOWN    1004
#define IDC_FINDUP      1005
#define IDC_FINDNEXT    1006
#define IDC_REPLACE     1007
#define IDC_REPLACEALL  1008
#define IDC_LINENUM     1009

// Buffer sizes
#define MAX_PATH_LEN        512
#define MAX_TITLE_LEN       256
#define MAX_BUFFER_SIZE     (1024 * 1024)

// Global variables
HINSTANCE   g_hInst;
HWND      g_hWndMain;
HWND      g_hEdit;
HWND      g_hStatusBar;
HFONT     g_hFont;
TCHAR     g_szFileName[MAX_PATH_LEN];
TCHAR     g_szTitle[MAX_TITLE_LEN];
BOOL      g_bModified;
BOOL      g_bWordWrap;
BOOL      g_bStatusBar;
LOGFONT   g_lfFont;

// Search variables
TCHAR g_szFindText[256] = {0};
TCHAR g_szReplaceText[256] = {0};
BOOL  g_bFindMatchCase = FALSE;
BOOL  g_bFindDown = TRUE;

// Function prototypes
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK FindDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ReplaceDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK GoToDlgProc(HWND, UINT, WPARAM, LPARAM);

BOOL InitApplication(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
void CreateMainMenu(HWND);
void UpdateTitle(void);
BOOL ConfirmSave(void);
BOOL DoFileNew(void);
BOOL DoFileOpen(void);
BOOL DoFileSave(void);
BOOL DoFileSaveAs(void);
BOOL LoadFile(LPCTSTR);
BOOL SaveFile(LPCTSTR);
void DoEditUndo(void);
void DoEditCut(void);
void DoEditCopy(void);
void DoEditPaste(void);
void DoEditDelete(void);
void DoEditSelectAll(void);
void DoEditFind(void);
void DoEditReplace(void);
void DoEditGoTo(void);
void DoEditDateTime(void);
void DoFormatWordWrap(void);
void DoFormatFont(void);
void DoViewStatusBar(void);
void UpdateStatusBar(void);
void SetFontToEdit(void);
void ShowContextMenu(HWND, int, int);
BOOL DoFindText(BOOL bReplace, BOOL bReplaceAll);

// ==================== ENTRY POINT ====================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    HACCEL hAccel;
    
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_BAR_CLASSES | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icc);
    
    g_hInst = hInstance;
    g_szFileName[0] = 0;
    g_bModified = FALSE;
    g_bWordWrap = FALSE;
    g_bStatusBar = TRUE;
    
    memset(&g_lfFont, 0, sizeof(LOGFONT));
    lstrcpy(g_lfFont.lfFaceName, TEXT("Consolas"));
    g_lfFont.lfHeight = -16;
    g_lfFont.lfWeight = FW_NORMAL;
    
    if (!InitApplication(hInstance))
        return FALSE;
    
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;
    
    // Create accelerator table programmatically
    ACCEL accel[] = {
        { FCONTROL | FVIRTKEY, 'N', IDM_FILE_NEW },
        { FCONTROL | FVIRTKEY, 'O', IDM_FILE_OPEN },
        { FCONTROL | FVIRTKEY, 'S', IDM_FILE_SAVE },
        { FVIRTKEY, VK_F12, IDM_FILE_SAVEAS },
        { FCONTROL | FVIRTKEY, 'Z', IDM_EDIT_UNDO },
        { FCONTROL | FVIRTKEY, 'X', IDM_EDIT_CUT },
        { FCONTROL | FVIRTKEY, 'C', IDM_EDIT_COPY },
        { FCONTROL | FVIRTKEY, 'V', IDM_EDIT_PASTE },
        { FVIRTKEY, VK_DELETE, IDM_EDIT_DELETE },
        { FCONTROL | FVIRTKEY, 'A', IDM_EDIT_SELECTALL },
        { FCONTROL | FVIRTKEY, 'F', IDM_EDIT_FIND },
        { FCONTROL | FVIRTKEY, 'H', IDM_EDIT_REPLACE },
        { FCONTROL | FVIRTKEY, 'G', IDM_EDIT_GOTO },
        { FVIRTKEY, VK_F5, IDM_EDIT_DATETIME },
        { FVIRTKEY, VK_F1, IDM_HELP_ABOUT }
    };
    hAccel = CreateAcceleratorTable(accel, sizeof(accel) / sizeof(ACCEL));
    
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator(g_hWndMain, hAccel, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    DestroyAcceleratorTable(hAccel);
    return (int)msg.wParam;
}

// ==================== INITIALIZATION ====================

BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;
    
    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName  = NULL;
    wcex.lpszClassName = TEXT("CtextClass");
    wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
    
    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hWndMain = CreateWindowEx(
        0,
        TEXT("CtextClass"),
        TEXT("Ctext - Untitled"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 
        800, 600,
        NULL, NULL, hInstance, NULL);
    
    if (!g_hWndMain)
        return FALSE;
    
    ShowWindow(g_hWndMain, nCmdShow);
    UpdateWindow(g_hWndMain);
    
    return TRUE;
}

// ==================== WINDOW PROCEDURE ====================

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_CREATE:
            CreateMainMenu(hWnd);
            
            g_hEdit = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                TEXT("EDIT"), NULL,
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | 
                ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | 
                ES_NOHIDESEL | ES_WANTRETURN,
                0, 0, 0, 0,
                hWnd, (HMENU)ID_EDIT, g_hInst, NULL);
            
            SendMessage(g_hEdit, EM_LIMITTEXT, (WPARAM)MAX_BUFFER_SIZE, 0);
            
            g_hStatusBar = CreateWindowEx(
                0, STATUSCLASSNAME, NULL,
                WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
                0, 0, 0, 0,
                hWnd, (HMENU)ID_STATUSBAR, g_hInst, NULL);
            
            SetFontToEdit();
            SetFocus(g_hEdit);
            UpdateStatusBar();
            return 0;
            
        case WM_SIZE: {
            RECT rcClient;
            GetClientRect(hWnd, &rcClient);
            
            int statusHeight = g_bStatusBar ? 20 : 0;
            
            if (g_hStatusBar && g_bStatusBar) {
                SendMessage(g_hStatusBar, WM_SIZE, wParam, lParam);
                RECT rcStatus;
                GetWindowRect(g_hStatusBar, &rcStatus);
                statusHeight = rcStatus.bottom - rcStatus.top;
            }
            
            SetWindowPos(g_hEdit, NULL, 
                0, 0, 
                rcClient.right, 
                rcClient.bottom - statusHeight,
                SWP_NOZORDER);
            
            UpdateStatusBar();
            return 0;
        }
            
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_FILE_NEW:
                    if (ConfirmSave())
                        DoFileNew();
                    break;
                case IDM_FILE_OPEN:
                    if (ConfirmSave())
                        DoFileOpen();
                    break;
                case IDM_FILE_SAVE:
                    DoFileSave();
                    break;
                case IDM_FILE_SAVEAS:
                    DoFileSaveAs();
                    break;
                case IDM_FILE_EXIT:
                    if (ConfirmSave())
                        DestroyWindow(hWnd);
                    break;
                case IDM_EDIT_UNDO:
                    DoEditUndo();
                    break;
                case IDM_EDIT_CUT:
                    DoEditCut();
                    break;
                case IDM_EDIT_COPY:
                    DoEditCopy();
                    break;
                case IDM_EDIT_PASTE:
                    DoEditPaste();
                    break;
                case IDM_EDIT_DELETE:
                    DoEditDelete();
                    break;
                case IDM_EDIT_SELECTALL:
                    DoEditSelectAll();
                    break;
                case IDM_EDIT_FIND:
                    DoEditFind();
                    break;
                case IDM_EDIT_REPLACE:
                    DoEditReplace();
                    break;
                case IDM_EDIT_GOTO:
                    DoEditGoTo();
                    break;
                case IDM_EDIT_DATETIME:
                    DoEditDateTime();
                    break;
                case IDM_FORMAT_WORDWRAP:
                    DoFormatWordWrap();
                    break;
                case IDM_FORMAT_FONT:
                    DoFormatFont();
                    break;
                case IDM_VIEW_STATUSBAR:
                    DoViewStatusBar();
                    break;
                case IDM_HELP_ABOUT:
                    DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutDlgProc);
                    break;
                default:
                    if (HIWORD(wParam) == EN_CHANGE && LOWORD(wParam) == ID_EDIT) {
                        if (!g_bModified) {
                            g_bModified = TRUE;
                            UpdateTitle();
                        }
                        UpdateStatusBar();
                    }
            }
            return 0;
            
        case WM_CONTEXTMENU: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (x == 0xFFFF && y == 0xFFFF) {
                RECT rc;
                GetWindowRect(g_hEdit, &rc);
                x = rc.left + 10;
                y = rc.top + 10;
            }
            ShowContextMenu(hWnd, x, y);
            return 0;
        }
            
        case WM_SETFOCUS:
            SetFocus(g_hEdit);
            return 0;
            
        case WM_CLOSE:
            if (ConfirmSave())
                DestroyWindow(hWnd);
            return 0;
            
        case WM_DESTROY:
            if (g_hFont) DeleteObject(g_hFont);
            PostQuitMessage(0);
            return 0;
            
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ==================== MENU ====================

void CreateMainMenu(HWND hWnd)
{
    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreatePopupMenu();
    HMENU hEditMenu = CreatePopupMenu();
    HMENU hFormatMenu = CreatePopupMenu();
    HMENU hViewMenu = CreatePopupMenu();
    HMENU hHelpMenu = CreatePopupMenu();
    
    // File Menu
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_NEW, TEXT("New\tCtrl+N"));
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_OPEN, TEXT("Open...\tCtrl+O"));
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVE, TEXT("Save\tCtrl+S"));
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVEAS, TEXT("Save As...\tF12"));
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_EXIT, TEXT("Exit\tAlt+F4"));
    
    // Edit Menu
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_UNDO, TEXT("Undo\tCtrl+Z"));
    AppendMenu(hEditMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_CUT, TEXT("Cut\tCtrl+X"));
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_COPY, TEXT("Copy\tCtrl+C"));
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_PASTE, TEXT("Paste\tCtrl+V"));
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_DELETE, TEXT("Delete\tDel"));
    AppendMenu(hEditMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_FIND, TEXT("Find...\tCtrl+F"));
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_REPLACE, TEXT("Replace...\tCtrl+H"));
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_GOTO, TEXT("Go To...\tCtrl+G"));
    AppendMenu(hEditMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_SELECTALL, TEXT("Select All\tCtrl+A"));
    AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_DATETIME, TEXT("Time/Date\tF5"));
    
    // Format Menu
    AppendMenu(hFormatMenu, MF_STRING | (g_bWordWrap ? MF_CHECKED : 0), 
               IDM_FORMAT_WORDWRAP, TEXT("Word Wrap"));
    AppendMenu(hFormatMenu, MF_STRING, IDM_FORMAT_FONT, TEXT("Font..."));
    
    // View Menu
    AppendMenu(hViewMenu, MF_STRING | (g_bStatusBar ? MF_CHECKED : 0), 
               IDM_VIEW_STATUSBAR, TEXT("Status Bar"));
    
    // Help Menu
    AppendMenu(hHelpMenu, MF_STRING, IDM_HELP_ABOUT, TEXT("About"));
    
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, TEXT("File"));
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hEditMenu, TEXT("Edit"));
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFormatMenu, TEXT("Format"));
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hViewMenu, TEXT("View"));
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, TEXT("Help"));
    
    SetMenu(hWnd, hMenu);
}

// ==================== FILE OPERATIONS ====================

BOOL ConfirmSave(void)
{
    if (!g_bModified)
        return TRUE;
    
    TCHAR szMsg[MAX_PATH_LEN + 100];
    wsprintf(szMsg, TEXT("File \"%s\" has been modified.\nSave changes?"), 
             g_szFileName[0] ? g_szFileName : TEXT("Untitled"));
    
    int result = MessageBox(g_hWndMain, szMsg, TEXT("Ctext"), 
                           MB_YESNOCANCEL | MB_ICONQUESTION);
    
    if (result == IDYES)
        return DoFileSave();
    else if (result == IDNO)
        return TRUE;
    else
        return FALSE;
}

BOOL DoFileNew(void)
{
    SetWindowText(g_hEdit, TEXT(""));
    g_szFileName[0] = 0;
    g_bModified = FALSE;
    UpdateTitle();
    UpdateStatusBar();
    return TRUE;
}

BOOL DoFileOpen(void)
{
    TCHAR szFile[MAX_PATH_LEN] = {0};
    OPENFILENAME ofn;
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_hWndMain;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH_LEN;
    ofn.lpstrFilter = TEXT("Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileName(&ofn))
        return LoadFile(szFile);
    
    return FALSE;
}

BOOL DoFileSave(void)
{
    if (g_szFileName[0])
        return SaveFile(g_szFileName);
    else
        return DoFileSaveAs();
}

BOOL DoFileSaveAs(void)
{
    TCHAR szFile[MAX_PATH_LEN] = {0};
    OPENFILENAME ofn;
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_hWndMain;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH_LEN;
    ofn.lpstrFilter = TEXT("Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrDefExt = TEXT("txt");
    ofn.Flags = OFN_OVERWRITEPROMPT;
    
    if (GetSaveFileName(&ofn))
        return SaveFile(szFile);
    
    return FALSE;
}

BOOL LoadFile(LPCTSTR lpFileName)
{
    HANDLE hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        MessageBox(g_hWndMain, TEXT("Failed to open file."), 
                   TEXT("Error"), MB_OK | MB_ICONERROR);
        return FALSE;
    }
    
    DWORD dwSize = GetFileSize(hFile, NULL);
    if (dwSize == INVALID_FILE_SIZE || dwSize > MAX_BUFFER_SIZE) {
        CloseHandle(hFile);
        MessageBox(g_hWndMain, TEXT("File is too large."), 
                   TEXT("Error"), MB_OK | MB_ICONERROR);
        return FALSE;
    }
    
    char* pBuffer = (char*)GlobalAlloc(GPTR, dwSize + 1);
    if (!pBuffer) {
        CloseHandle(hFile);
        return FALSE;
    }
    
    DWORD dwRead;
    BOOL bResult = ReadFile(hFile, pBuffer, dwSize, &dwRead, NULL);
    CloseHandle(hFile);
    
    if (!bResult) {
        GlobalFree(pBuffer);
        return FALSE;
    }
    
    pBuffer[dwRead] = 0;
    
    int nLen;
    LPWSTR lpwBuffer;
    
    if (dwRead >= 2 && (BYTE)pBuffer[0] == 0xFF && (BYTE)pBuffer[1] == 0xFE) {
        // UTF-16 LE
        lpwBuffer = (LPWSTR)(pBuffer + 2);
        nLen = (dwRead - 2) / sizeof(WCHAR);
    } else if (dwRead >= 2 && (BYTE)pBuffer[0] == 0xFE && (BYTE)pBuffer[1] == 0xFF) {
        // UTF-16 BE
        lpwBuffer = (LPWSTR)GlobalAlloc(GPTR, (dwRead + 2) * sizeof(WCHAR));
        nLen = (dwRead - 2) / sizeof(WCHAR);
        for (int i = 0; i < nLen; i++) {
            lpwBuffer[i] = ((BYTE)pBuffer[2 + i*2] << 8) | (BYTE)pBuffer[3 + i*2];
        }
        GlobalFree(pBuffer);
        pBuffer = (char*)lpwBuffer;
    } else if (dwRead >= 3 && (BYTE)pBuffer[0] == 0xEF && 
               (BYTE)pBuffer[1] == 0xBB && (BYTE)pBuffer[2] == 0xBF) {
        // UTF-8
        nLen = MultiByteToWideChar(CP_UTF8, 0, pBuffer + 3, dwRead - 3, NULL, 0);
        lpwBuffer = (LPWSTR)GlobalAlloc(GPTR, (nLen + 1) * sizeof(WCHAR));
        MultiByteToWideChar(CP_UTF8, 0, pBuffer + 3, dwRead - 3, lpwBuffer, nLen);
        GlobalFree(pBuffer);
        pBuffer = (char*)lpwBuffer;
    } else {
        // ANSI
        nLen = MultiByteToWideChar(CP_ACP, 0, pBuffer, dwRead, NULL, 0);
        lpwBuffer = (LPWSTR)GlobalAlloc(GPTR, (nLen + 1) * sizeof(WCHAR));
        MultiByteToWideChar(CP_ACP, 0, pBuffer, dwRead, lpwBuffer, nLen);
        GlobalFree(pBuffer);
        pBuffer = (char*)lpwBuffer;
    }
    
    lpwBuffer[nLen] = 0;
    SetWindowText(g_hEdit, lpwBuffer);
    GlobalFree(lpwBuffer);
    
    lstrcpy(g_szFileName, lpFileName);
    g_bModified = FALSE;
    UpdateTitle();
    UpdateStatusBar();
    
    return TRUE;
}

BOOL SaveFile(LPCTSTR lpFileName)
{
    int nLen = GetWindowTextLength(g_hEdit);
    LPWSTR lpwBuffer = (LPWSTR)GlobalAlloc(GPTR, (nLen + 1) * sizeof(WCHAR));
    if (!lpwBuffer) return FALSE;
    
    GetWindowText(g_hEdit, lpwBuffer, nLen + 1);
    
    int nUTF8Len = WideCharToMultiByte(CP_UTF8, 0, lpwBuffer, nLen, NULL, 0, NULL, NULL);
    char* pBuffer = (char*)GlobalAlloc(GPTR, nUTF8Len + 3);
    
    // UTF-8 BOM
    pBuffer[0] = (char)0xEF;
    pBuffer[1] = (char)0xBB;
    pBuffer[2] = (char)0xBF;
    
    WideCharToMultiByte(CP_UTF8, 0, lpwBuffer, nLen, pBuffer + 3, nUTF8Len, NULL, NULL);
    GlobalFree(lpwBuffer);
    
    HANDLE hFile = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        GlobalFree(pBuffer);
        MessageBox(g_hWndMain, TEXT("Failed to save file."), 
                   TEXT("Error"), MB_OK | MB_ICONERROR);
        return FALSE;
    }
    
    DWORD dwWritten;
    WriteFile(hFile, pBuffer, nUTF8Len + 3, &dwWritten, NULL);
    CloseHandle(hFile);
    GlobalFree(pBuffer);
    
    lstrcpy(g_szFileName, lpFileName);
    g_bModified = FALSE;
    UpdateTitle();
    UpdateStatusBar();
    
    return TRUE;
}

// ==================== EDITING ====================

void DoEditUndo(void) { SendMessage(g_hEdit, EM_UNDO, 0, 0); }
void DoEditCut(void) { SendMessage(g_hEdit, WM_CUT, 0, 0); }
void DoEditCopy(void) { SendMessage(g_hEdit, WM_COPY, 0, 0); }
void DoEditPaste(void) { SendMessage(g_hEdit, WM_PASTE, 0, 0); }
void DoEditDelete(void) { SendMessage(g_hEdit, WM_CLEAR, 0, 0); }
void DoEditSelectAll(void) { SendMessage(g_hEdit, EM_SETSEL, 0, -1); }

void DoEditDateTime(void)
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    TCHAR szDateTime[64];
    wsprintf(szDateTime, TEXT("%02d.%02d.%04d %02d:%02d"), 
             st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute);
    SendMessage(g_hEdit, EM_REPLACESEL, TRUE, (LPARAM)szDateTime);
}

// ==================== FIND AND REPLACE ====================

void DoEditFind(void)
{
    DialogBox(g_hInst, MAKEINTRESOURCE(IDD_FIND), g_hWndMain, FindDlgProc);
}

void DoEditReplace(void)
{
    DialogBox(g_hInst, MAKEINTRESOURCE(IDD_REPLACE), g_hWndMain, ReplaceDlgProc);
}

void DoEditGoTo(void)
{
    DialogBox(g_hInst, MAKEINTRESOURCE(IDD_GOTO), g_hWndMain, GoToDlgProc);
}

// About Dialog
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            CreateWindow(TEXT("STATIC"), TEXT("Ctext v1.0"),
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                20, 10, 160, 15, hDlg, NULL, g_hInst, NULL);
            CreateWindow(TEXT("STATIC"), TEXT("Simple Text Editor"),
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                20, 30, 160, 15, hDlg, NULL, g_hInst, NULL);
            CreateWindow(TEXT("STATIC"), TEXT("Developer: ExEintel"),
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                20, 50, 160, 15, hDlg, NULL, g_hInst, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("OK"),
                WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                70, 75, 60, 20, hDlg, (HMENU)IDOK, g_hInst, NULL);
            return TRUE;
        }
        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                EndDialog(hDlg, LOWORD(wParam));
                return TRUE;
            }
            break;
    }
    return FALSE;
}

// Find Dialog
INT_PTR CALLBACK FindDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            CreateWindow(TEXT("STATIC"), TEXT("Find what:"),
                WS_CHILD | WS_VISIBLE,
                10, 10, 50, 15, hDlg, NULL, g_hInst, NULL);
            CreateWindow(TEXT("EDIT"), g_szFindText,
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                65, 8, 150, 18, hDlg, (HMENU)IDC_FINDTEXT, g_hInst, NULL);
            
            CreateWindow(TEXT("BUTTON"), TEXT("Match case"),
                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                10, 35, 120, 15, hDlg, (HMENU)IDC_MATCHCASE, g_hInst, NULL);
            CheckDlgButton(hDlg, IDC_MATCHCASE, g_bFindMatchCase ? BST_CHECKED : BST_UNCHECKED);
            
            CreateWindow(TEXT("BUTTON"), TEXT("Down"),
                WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
                10, 55, 50, 15, hDlg, (HMENU)IDC_FINDDOWN, g_hInst, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Up"),
                WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                70, 55, 50, 15, hDlg, (HMENU)IDC_FINDUP, g_hInst, NULL);
            CheckDlgButton(hDlg, IDC_FINDDOWN, g_bFindDown ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hDlg, IDC_FINDUP, !g_bFindDown ? BST_CHECKED : BST_UNCHECKED);
            
            CreateWindow(TEXT("BUTTON"), TEXT("Find Next"),
                WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                230, 10, 80, 22, hDlg, (HMENU)IDC_FINDNEXT, g_hInst, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Cancel"),
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                230, 40, 80, 22, hDlg, (HMENU)IDCANCEL, g_hInst, NULL);
            
            return TRUE;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_FINDNEXT:
                case IDOK: {
                    GetDlgItemText(hDlg, IDC_FINDTEXT, g_szFindText, 256);
                    g_bFindMatchCase = IsDlgButtonChecked(hDlg, IDC_MATCHCASE) == BST_CHECKED;
                    g_bFindDown = IsDlgButtonChecked(hDlg, IDC_FINDDOWN) == BST_CHECKED;
                    DoFindText(FALSE, FALSE);
                    if (LOWORD(wParam) == IDOK)
                        EndDialog(hDlg, 0);
                    return TRUE;
                }
                case IDCANCEL:
                    EndDialog(hDlg, 0);
                    return TRUE;
            }
            break;
    }
    return FALSE;
}

// Replace Dialog
INT_PTR CALLBACK ReplaceDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            CreateWindow(TEXT("STATIC"), TEXT("Find what:"),
                WS_CHILD | WS_VISIBLE,
                10, 10, 60, 15, hDlg, NULL, g_hInst, NULL);
            CreateWindow(TEXT("EDIT"), g_szFindText,
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                75, 8, 150, 18, hDlg, (HMENU)IDC_FINDTEXT, g_hInst, NULL);
            
            CreateWindow(TEXT("STATIC"), TEXT("Replace with:"),
                WS_CHILD | WS_VISIBLE,
                10, 35, 65, 15, hDlg, NULL, g_hInst, NULL);
            CreateWindow(TEXT("EDIT"), g_szReplaceText,
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                75, 33, 150, 18, hDlg, (HMENU)IDC_REPLACETEXT, g_hInst, NULL);
            
            CreateWindow(TEXT("BUTTON"), TEXT("Match case"),
                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                10, 60, 120, 15, hDlg, (HMENU)IDC_MATCHCASE, g_hInst, NULL);
            CheckDlgButton(hDlg, IDC_MATCHCASE, g_bFindMatchCase ? BST_CHECKED : BST_UNCHECKED);
            
            CreateWindow(TEXT("BUTTON"), TEXT("Find Next"),
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                240, 10, 80, 22, hDlg, (HMENU)IDC_FINDNEXT, g_hInst, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Replace"),
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                240, 35, 80, 22, hDlg, (HMENU)IDC_REPLACE, g_hInst, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Replace All"),
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                240, 60, 80, 22, hDlg, (HMENU)IDC_REPLACEALL, g_hInst, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Cancel"),
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                240, 90, 80, 22, hDlg, (HMENU)IDCANCEL, g_hInst, NULL);
            
            return TRUE;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_FINDNEXT:
                    GetDlgItemText(hDlg, IDC_FINDTEXT, g_szFindText, 256);
                    g_bFindMatchCase = IsDlgButtonChecked(hDlg, IDC_MATCHCASE) == BST_CHECKED;
                    DoFindText(FALSE, FALSE);
                    return TRUE;
                case IDC_REPLACE:
                    GetDlgItemText(hDlg, IDC_FINDTEXT, g_szFindText, 256);
                    GetDlgItemText(hDlg, IDC_REPLACETEXT, g_szReplaceText, 256);
                    g_bFindMatchCase = IsDlgButtonChecked(hDlg, IDC_MATCHCASE) == BST_CHECKED;
                    DoFindText(TRUE, FALSE);
                    return TRUE;
                case IDC_REPLACEALL:
                    GetDlgItemText(hDlg, IDC_FINDTEXT, g_szFindText, 256);
                    GetDlgItemText(hDlg, IDC_REPLACETEXT, g_szReplaceText, 256);
                    g_bFindMatchCase = IsDlgButtonChecked(hDlg, IDC_MATCHCASE) == BST_CHECKED;
                    DoFindText(TRUE, TRUE);
                    return TRUE;
                case IDCANCEL:
                    EndDialog(hDlg, 0);
                    return TRUE;
            }
            break;
    }
    return FALSE;
}

// Go To Dialog
INT_PTR CALLBACK GoToDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_INITDIALOG: {
            CreateWindow(TEXT("STATIC"), TEXT("Line number:"),
                WS_CHILD | WS_VISIBLE,
                10, 15, 70, 15, hDlg, NULL, g_hInst, NULL);
            CreateWindow(TEXT("EDIT"), TEXT(""),
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
                85, 13, 60, 18, hDlg, (HMENU)IDC_LINENUM, g_hInst, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Go To"),
                WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                20, 50, 60, 22, hDlg, (HMENU)IDOK, g_hInst, NULL);
            CreateWindow(TEXT("BUTTON"), TEXT("Cancel"),
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                90, 50, 60, 22, hDlg, (HMENU)IDCANCEL, g_hInst, NULL);
            return TRUE;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK: {
                    TCHAR szLine[32];
                    GetDlgItemText(hDlg, IDC_LINENUM, szLine, 32);
                    int nLine = _wtoi(szLine);
                    if (nLine > 0) {
                        int nCharIndex = (int)SendMessage(g_hEdit, EM_LINEINDEX, nLine - 1, 0);
                        if (nCharIndex >= 0) {
                            SendMessage(g_hEdit, EM_SETSEL, nCharIndex, nCharIndex);
                            SetFocus(g_hEdit);
                        }
                    }
                    EndDialog(hDlg, 0);
                    return TRUE;
                }
                case IDCANCEL:
                    EndDialog(hDlg, 0);
                    return TRUE;
            }
            break;
    }
    return FALSE;
}

BOOL DoFindText(BOOL bReplace, BOOL bReplaceAll)
{
    if (lstrlen(g_szFindText) == 0) return FALSE;
    
    int nLen = GetWindowTextLength(g_hEdit);
    LPWSTR lpText = (LPWSTR)GlobalAlloc(GPTR, (nLen + 1) * sizeof(WCHAR));
    GetWindowText(g_hEdit, lpText, nLen + 1);
    
    DWORD dwStart, dwEnd;
    SendMessage(g_hEdit, EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);
    
    if (bReplaceAll) {
        int nFindLen = lstrlen(g_szFindText);
        int nReplaceLen = lstrlen(g_szReplaceText);
        
        int nCount = 0;
        for (int i = 0; i < nLen; ) {
            BOOL bMatch = g_bFindMatchCase ? 
                (wcsncmp(lpText + i, g_szFindText, nFindLen) == 0) :
                (_wcsnicmp(lpText + i, g_szFindText, nFindLen) == 0);
            
            if (bMatch && nFindLen > 0) {
                nCount++;
                i += nFindLen;
            } else {
                i++;
            }
        }
        
        if (nCount == 0) {
            GlobalFree(lpText);
            MessageBox(g_hWndMain, TEXT("Text not found."), 
                      TEXT("Ctext"), MB_OK | MB_ICONINFORMATION);
            return FALSE;
        }
        
        int nNewLen = nLen + nCount * (nReplaceLen - nFindLen);
        LPWSTR lpResult = (LPWSTR)GlobalAlloc(GPTR, (nNewLen + 1) * sizeof(WCHAR));
        int nResultLen = 0;
        
        for (int i = 0; i < nLen; ) {
            BOOL bMatch = g_bFindMatchCase ? 
                (wcsncmp(lpText + i, g_szFindText, nFindLen) == 0) :
                (_wcsnicmp(lpText + i, g_szFindText, nFindLen) == 0);
            
            if (bMatch && nFindLen > 0) {
                memcpy(lpResult + nResultLen, g_szReplaceText, 
                       nReplaceLen * sizeof(WCHAR));
                nResultLen += nReplaceLen;
                i += nFindLen;
            } else {
                lpResult[nResultLen++] = lpText[i++];
            }
        }
        lpResult[nResultLen] = 0;
        
        SetWindowText(g_hEdit, lpResult);
        GlobalFree(lpResult);
        
        g_bModified = TRUE;
        UpdateTitle();
        UpdateStatusBar();
        
        TCHAR szMsg[64];
        wsprintf(szMsg, TEXT("Replaced %d occurrences."), nCount);
        MessageBox(g_hWndMain, szMsg, TEXT("Ctext"), MB_OK | MB_ICONINFORMATION);
    } else {
        int nStart = g_bFindDown ? dwEnd : dwStart;
        int nPos = -1;
        
        if (g_bFindDown) {
            for (int i = nStart; i < nLen; i++) {
                BOOL bMatch = g_bFindMatchCase ? 
                    (wcsncmp(lpText + i, g_szFindText, lstrlen(g_szFindText)) == 0) :
                    (_wcsnicmp(lpText + i, g_szFindText, lstrlen(g_szFindText)) == 0);
                
                if (bMatch) {
                    nPos = i;
                    break;
                }
            }
        } else {
            for (int i = nStart - 1; i >= 0; i--) {
                BOOL bMatch = g_bFindMatchCase ? 
                    (wcsncmp(lpText + i, g_szFindText, lstrlen(g_szFindText)) == 0) :
                    (_wcsnicmp(lpText + i, g_szFindText, lstrlen(g_szFindText)) == 0);
                
                if (bMatch) {
                    nPos = i;
                    break;
                }
            }
        }
        
        if (nPos >= 0) {
            if (bReplace) {
                SendMessage(g_hEdit, EM_SETSEL, nPos, nPos + lstrlen(g_szFindText));
                SendMessage(g_hEdit, EM_REPLACESEL, TRUE, (LPARAM)g_szReplaceText);
            } else {
                SendMessage(g_hEdit, EM_SETSEL, nPos, nPos + lstrlen(g_szFindText));
            }
            SetFocus(g_hEdit);
        } else {
            MessageBox(g_hWndMain, TEXT("Text not found."), 
                      TEXT("Ctext"), MB_OK | MB_ICONINFORMATION);
        }
    }
    
    GlobalFree(lpText);
    return TRUE;
}

// ==================== FORMATTING ====================

void DoFormatWordWrap(void)
{
    g_bWordWrap = !g_bWordWrap;
    
    RECT rc;
    GetWindowRect(g_hEdit, &rc);
    ScreenToClient(g_hWndMain, (LPPOINT)&rc);
    ScreenToClient(g_hWndMain, ((LPPOINT)&rc) + 1);
    
    TCHAR* pText = NULL;
    int nLen = GetWindowTextLength(g_hEdit);
    if (nLen > 0) {
        pText = (TCHAR*)GlobalAlloc(GPTR, (nLen + 1) * sizeof(TCHAR));
        GetWindowText(g_hEdit, pText, nLen + 1);
    }
    
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | 
                    ES_AUTOVSCROLL | ES_NOHIDESEL | ES_WANTRETURN;
    
    if (!g_bWordWrap) {
        dwStyle |= WS_HSCROLL | ES_AUTOHSCROLL;
    }
    
    DestroyWindow(g_hEdit);
    
    g_hEdit = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        TEXT("EDIT"), NULL,
        dwStyle,
        rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
        g_hWndMain, (HMENU)ID_EDIT, g_hInst, NULL);
    
    SendMessage(g_hEdit, EM_LIMITTEXT, (WPARAM)MAX_BUFFER_SIZE, 0);
    SetFontToEdit();
    
    if (pText) {
        SetWindowText(g_hEdit, pText);
        GlobalFree(pText);
    }
    
    HMENU hMenu = GetMenu(g_hWndMain);
    HMENU hFormatMenu = GetSubMenu(hMenu, 2);
    CheckMenuItem(hFormatMenu, IDM_FORMAT_WORDWRAP, 
                  MF_BYCOMMAND | (g_bWordWrap ? MF_CHECKED : MF_UNCHECKED));
    
    SetFocus(g_hEdit);
}

void DoFormatFont(void)
{
    CHOOSEFONT cf;
    LOGFONT lf = g_lfFont;
    
    ZeroMemory(&cf, sizeof(cf));
    cf.lStructSize = sizeof(cf);
    cf.hwndOwner = g_hWndMain;
    cf.lpLogFont = &lf;
    cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT | CF_EFFECTS;
    cf.rgbColors = RGB(0, 0, 0);
    
    if (ChooseFont(&cf)) {
        g_lfFont = lf;
        SetFontToEdit();
    }
}

void SetFontToEdit(void)
{
    if (g_hFont) DeleteObject(g_hFont);
    g_hFont = CreateFontIndirect(&g_lfFont);
    SendMessage(g_hEdit, WM_SETFONT, (WPARAM)g_hFont, TRUE);
}

// ==================== VIEW ====================

void DoViewStatusBar(void)
{
    g_bStatusBar = !g_bStatusBar;
    ShowWindow(g_hStatusBar, g_bStatusBar ? SW_SHOW : SW_HIDE);
    
    HMENU hMenu = GetMenu(g_hWndMain);
    HMENU hViewMenu = GetSubMenu(hMenu, 3);
    CheckMenuItem(hViewMenu, IDM_VIEW_STATUSBAR, 
                  MF_BYCOMMAND | (g_bStatusBar ? MF_CHECKED : MF_UNCHECKED));
    
    RECT rc;
    GetClientRect(g_hWndMain, &rc);
    SendMessage(g_hWndMain, WM_SIZE, SIZE_RESTORED, 
                MAKELPARAM(rc.right, rc.bottom));
}

// ==================== STATUS BAR ====================

void UpdateStatusBar(void)
{
    if (!g_bStatusBar) return;
    
    TCHAR szStatus[256];
    int nLen = GetWindowTextLength(g_hEdit);
    DWORD dwStart, dwEnd;
    SendMessage(g_hEdit, EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);
    
    int nLine = (int)SendMessage(g_hEdit, EM_LINEFROMCHAR, dwEnd, 0) + 1;
    int nCol = dwEnd - (int)SendMessage(g_hEdit, EM_LINEINDEX, nLine - 1, 0) + 1;
    
    wsprintf(szStatus, TEXT("Ln %d, Col %d | %s | %s"), 
             nLine, nCol,
             g_bModified ? TEXT("Modified") : TEXT(""),
             g_szFileName[0] ? PathFindFileName(g_szFileName) : TEXT("Untitled"));
    
    SendMessage(g_hStatusBar, SB_SETTEXT, 0, (LPARAM)szStatus);
}

void UpdateTitle(void)
{
    TCHAR szTitle[MAX_TITLE_LEN];
    wsprintf(szTitle, TEXT("%s%s - Ctext"), 
             g_bModified ? TEXT("*") : TEXT(""),
             g_szFileName[0] ? PathFindFileName(g_szFileName) : TEXT("Untitled"));
    SetWindowText(g_hWndMain, szTitle);
}

// ==================== CONTEXT MENU ====================

void ShowContextMenu(HWND hWnd, int x, int y)
{
    HMENU hMenu = CreatePopupMenu();
    
    BOOL bCanUndo = (BOOL)SendMessage(g_hEdit, EM_CANUNDO, 0, 0);
    DWORD dwStart, dwEnd;
    SendMessage(g_hEdit, EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);
    BOOL bHasSel = (dwStart != dwEnd);
    
    AppendMenu(hMenu, MF_STRING | (bCanUndo ? 0 : MF_GRAYED), IDM_EDIT_UNDO, TEXT("Undo"));
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING | (bHasSel ? 0 : MF_GRAYED), IDM_EDIT_CUT, TEXT("Cut"));
    AppendMenu(hMenu, MF_STRING | (bHasSel ? 0 : MF_GRAYED), IDM_EDIT_COPY, TEXT("Copy"));
    AppendMenu(hMenu, MF_STRING, IDM_EDIT_PASTE, TEXT("Paste"));
    AppendMenu(hMenu, MF_STRING | (bHasSel ? 0 : MF_GRAYED), IDM_EDIT_DELETE, TEXT("Delete"));
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, IDM_EDIT_SELECTALL, TEXT("Select All"));
    
    TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, 0, hWnd, NULL);
    DestroyMenu(hMenu);
}