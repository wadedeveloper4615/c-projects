#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>
#include <locale.h>
#include <shellapi.h>
#include <stdio.h>

char HelpText[] =
"PEDUMP - Win32/COFF EXE/OBJ/LIB file dumper\n\n"
"Syntax: PEDUMP [switches] filename\n\n"
"  /A    include everything in dump\n"
"  /B    show base relocations\n"
"  /H    include hex dump of sections\n"
"  /I    include Import Address Table thunk addresses\n"
"  /L    include line number information\n"
"  /P    include PDATA (runtime functions)\n"
"  /R    include detailed resources (stringtables and dialogs)\n"
"  /S    show symbol table\n";

BOOL fShowRelocations = FALSE;
BOOL fShowRawSectionData = FALSE;
BOOL fShowSymbolTable = FALSE;
BOOL fShowLineNumbers = FALSE;
BOOL fShowIATentries = FALSE;
BOOL fShowPDATA = FALSE;
BOOL fShowResources = FALSE;

TCHAR *ProcessCommandLine(int argc, TCHAR* argv[]) {
    for (int i = 1; i < argc; i++) {
        _tcsupr_s(argv[i],_tcslen(argv[i]));
        // Is it a switch character?
        if ((argv[i][0] == '-') || (argv[i][0] == '/')) {
            if (argv[i][1] == 'A') {
                fShowRelocations = TRUE;
                fShowRawSectionData = TRUE;
                fShowSymbolTable = TRUE;
                fShowLineNumbers = TRUE;
                fShowIATentries = TRUE;
                fShowPDATA = TRUE;
                fShowResources = TRUE;
            }
            else if (argv[i][1] == 'H')
                fShowRawSectionData = TRUE;
            else if (argv[i][1] == 'L')
                fShowLineNumbers = TRUE;
            else if (argv[i][1] == 'P')
                fShowPDATA = TRUE;
            else if (argv[i][1] == 'B')
                fShowRelocations = TRUE;
            else if (argv[i][1] == 'S')
                fShowSymbolTable = TRUE;
            else if (argv[i][1] == 'I')
                fShowIATentries = TRUE;
            else if (argv[i][1] == 'R')
                fShowResources = TRUE;
        }
        else {
            return argv[i];
        }
    }
    return NULL;
}

void DumpFile(TCHAR  *filename) {
    PIMAGE_DOS_HEADER dosHeader;

    HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE) {
        _tprintf(_T("Couldn't open file with CreateFile()\n"));
        return;
    }

    HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hFileMapping == 0) {
        CloseHandle(hFile);
        _tprintf(_T("Couldn't open file mapping with CreateFileMapping()\n"));
        return;
    }

    LPVOID lpFileBase = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
    if (lpFileBase == 0) {
        CloseHandle(hFileMapping);
        CloseHandle(hFile);
        _tprintf(_T("Couldn't map view of file with MapViewOfFile()\n"));
        return;
    }

    _tprintf(_T("Dump of file %s\n\n"), filename);

    UnmapViewOfFile(lpFileBase);
    CloseHandle(hFileMapping);
    CloseHandle(hFile);
}

int main(int argc, TCHAR* argv[])
{
    TCHAR *filename;
    if (argc == 1) {
        printf(HelpText);
        return 1;
    }
    filename = ProcessCommandLine(argc, argv);
    if (filename) {
        DumpFile(filename);
    }
    return 0;
}
