#pragma once

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "Common.h"
#include "ExeDump.h"

void DumpResourceEntry(PIMAGE_RESOURCE_DIRECTORY_ENTRY resDirEntry, DWORD64 resourceBase, DWORD level);
void DumpResourceDirectory(PIMAGE_RESOURCE_DIRECTORY resDir, DWORD64 resourceBase, DWORD level, DWORD resourceType);
void DumpStringTable(DWORD64 base, PIMAGE_NT_HEADERS pNTHeader, DWORD64 resourceBase, PIMAGE_RESOURCE_DIRECTORY_ENTRY pStrResEntry, DWORD64 cStrResEntries);
void DumpResourceSection(void* base, PIMAGE_NT_HEADERS pNTHeader);
DWORD GetOffsetToDataFromResEntry(DWORD64 base, DWORD64 resourceBase, PIMAGE_RESOURCE_DIRECTORY_ENTRY pResEntry);
void DumpDialogs(DWORD64 base, PIMAGE_NT_HEADERS pNTHeader, DWORD resourceBase, PIMAGE_RESOURCE_DIRECTORY_ENTRY pDlgResEntry, DWORD cDlgResEntries);
void DumpResourceSection(void* base, PIMAGE_NT_HEADERS pNTHeader);
