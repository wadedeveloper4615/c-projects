#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "Common.h"
#include "ExeDump.h"

char HelpText[] =
"PEDUMP - Win32/COFF EXE/OBJ/LIB file dumper - 1998 Matt Pietrek\n\n"
"Syntax: PEDUMP [switches] filename\n\n"
"  /A    include everything in dump\n"
"  /B    show base relocations\n"
"  /H    include hex dump of sections\n"
"  /I    include Import Address Table thunk addresses\n"
"  /L    include line number information\n"
"  /P    include PDATA (runtime functions)\n"
"  /R    include detailed resources (stringtables and dialogs)\n"
"  /S    show symbol table\n";

void DumpFile(LPSTR filename) {
    HANDLE hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Couldn't open file with CreateFile()\n");
        return;
    }

    HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hFileMapping == 0) {
        CloseHandle(hFile);
        printf("Couldn't open file mapping with CreateFileMapping()\n");
        return;
    }

    LPVOID lpFileBase = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
    if (lpFileBase == 0) {
        CloseHandle(hFileMapping);
        CloseHandle(hFile);
        printf("Couldn't map view of file with MapViewOfFile()\n");
        return;
    }

    printf("Dump of file %s\n\n", filename);

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)lpFileBase;
    PIMAGE_FILE_HEADER pImgFileHdr = (PIMAGE_FILE_HEADER)lpFileBase;
    if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE) {
        DumpExeFile(dosHeader);
    }

    UnmapViewOfFile(lpFileBase);
    CloseHandle(hFileMapping);
    CloseHandle(hFile);
}

PSTR ProcessCommandLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        _strupr(argv[i]);
        if ((argv[i][0] == '-') || (argv[i][0] == '/')) {
            if (argv[i][1] == 'A'){
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

int main(int argc, char* argv[]){
    if (argc == 1) {
        printf(HelpText);
        return 1;
    }

    PSTR filename = ProcessCommandLine(argc, argv);
    if (filename) {
        DumpFile(filename);
    }
    return 0;
}
