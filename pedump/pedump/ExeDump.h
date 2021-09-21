#pragma once

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "Common.h"
#include "ResourceDump.h"
#include "coffsymboltable.h"

typedef struct _PDB_INFO {
    CHAR    Signature[4];   // "NBxx"
    ULONG   Offset;         // always zero
    ULONG   sig;
    ULONG   age;
    CHAR    PdbName[_MAX_PATH];
} PDB_INFO, * PPDB_INFO;

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
void DumpRuntimeFunctions(DWORD64 base, PIMAGE_NT_HEADERS pNTHeader);
void DumpBaseRelocationsSection(DWORD64 base, PIMAGE_NT_HEADERS pNTHeader);
void DumpMiscDebugInfo(PIMAGE_DEBUG_MISC pMiscDebugInfo);
void DumpCVDebugInfo(PDWORD pCVHeader);
void DumpCOFFHeader(PIMAGE_COFF_SYMBOLS_HEADER pDbgInfo);
BOOL LookupSymbolName(DWORD index, PSTR buffer, UINT length);
void DumpLineNumbers(PIMAGE_LINENUMBER pln, DWORD count);
void GetSectionName(WORD section, PSTR buffer, unsigned cbBuffer);
void DumpSymbolTable(PCOFFSymbolTable pSymTab);
void HexDump(PBYTE ptr, DWORD length);
void DumpRawSectionData(PIMAGE_SECTION_HEADER section, PVOID base, unsigned cSections);
void DumpExeFile(PIMAGE_DOS_HEADER dosHeader);
