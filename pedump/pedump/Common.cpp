#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "Common.h"
#include "ExeDump.h"

BOOL fShowRelocations = FALSE;
BOOL fShowRawSectionData = FALSE;
BOOL fShowSymbolTable = FALSE;
BOOL fShowLineNumbers = FALSE;
BOOL fShowIATentries = FALSE;
BOOL fShowPDATA = FALSE;
BOOL fShowResources = FALSE;

const char* SzResourceTypes[] = {
"???_0",
"CURSOR",
"BITMAP",
"ICON",
"MENU",
"DIALOG",
"STRING",
"FONTDIR",
"FONT",
"ACCELERATORS",
"RCDATA",
"MESSAGETABLE",
"GROUP_CURSOR",
"???_13",
"GROUP_ICON",
"???_15",
"VERSION",
"DLGINCLUDE",
"???_18",
"PLUGPLAY",
"VXD",
"ANICURSOR",
"ANIICON"
};

void GetResourceTypeName(DWORD type, PSTR buffer, UINT cBytes)
{
    if ((DWORD64)type <= (DWORD64)RT_ANIICON)
        strncpy(buffer, SzResourceTypes[type], cBytes);
    else
        sprintf(buffer, "%X", type);
}

void GetResourceNameFromId(DWORD id, DWORD64 resourceBase, PSTR buffer, UINT cBytes) {
    PIMAGE_RESOURCE_DIR_STRING_U prdsu;

    // If it's a regular ID, just format it.
    if (!(id & IMAGE_RESOURCE_NAME_IS_STRING))
    {
        sprintf(buffer, "%X", id);
        return;
    }

    id &= 0x7FFFFFFF;
    prdsu = (PIMAGE_RESOURCE_DIR_STRING_U)((DWORD64)resourceBase + (DWORD64)id);

    // prdsu->Length is the number of unicode characters
    WideCharToMultiByte(CP_ACP, 0, prdsu->NameString, prdsu->Length,
        buffer, cBytes, 0, 0);
    buffer[min(cBytes - 1, prdsu->Length)] = 0;  // Null terminate it!!!
}

LPVOID GetPtrFromRVA(DWORD64 rva, PIMAGE_NT_HEADERS pNTHeader, DWORD64 imageBase)
{
    PIMAGE_SECTION_HEADER pSectionHdr;
    INT delta;

    pSectionHdr = GetEnclosingSectionHeader(rva, pNTHeader);
    if (!pSectionHdr) return 0;
    delta = (INT)(pSectionHdr->VirtualAddress - pSectionHdr->PointerToRawData);
    return (PVOID)((DWORD64)imageBase + (DWORD64)rva - (DWORD64)delta);
}
