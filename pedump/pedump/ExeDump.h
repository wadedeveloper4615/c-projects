#pragma once

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "Common.h"
#include "ResourceDump.h"
#include "coffsymboltable.h"

const char* GetMachineTypeName(WORD wMachineType);
void DumpHeader(PIMAGE_FILE_HEADER pImageFileHeader);
void DumpOptionalHeader(PIMAGE_OPTIONAL_HEADER optionalHeader);
void DumpSectionTable(PIMAGE_SECTION_HEADER section, unsigned cSections, BOOL IsEXE);
PIMAGE_SECTION_HEADER GetSectionHeader(PSTR name, PIMAGE_NT_HEADERS pNTHeader);
PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD64 rva, PIMAGE_NT_HEADERS pNTHeader);
void DumpDebugDirectory(PIMAGE_DEBUG_DIRECTORY debugDir, DWORD size, void* base);
void DumpExeDebugDirectory(void* base, PIMAGE_NT_HEADERS pNTHeader);
void DumpImportsSection(void* base, PIMAGE_NT_HEADERS pNTHeader);
void DumpExportsSection(void* base, PIMAGE_NT_HEADERS pNTHeader);
void DumpBoundImportDescriptors(void* base, PIMAGE_NT_HEADERS pNTHeader);
void DumpExeFile(PIMAGE_DOS_HEADER dosHeader);
