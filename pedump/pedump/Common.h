#pragma once

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#define MakePtr(cast, ptr, addValue) (cast)((DWORD64)((DWORD64)(ptr) + (DWORD64)(addValue)))
#define GetImgDirEntryRVA(pNTHdr, IDE) (pNTHdr->OptionalHeader.DataDirectory[IDE].VirtualAddress)
#define GetImgDirEntrySize(pNTHdr, IDE) (pNTHdr->OptionalHeader.DataDirectory[IDE].Size)

typedef struct
{
    WORD    flag;
    const char* name;
} WORD_FLAG_DESCRIPTIONS;

typedef struct
{
    DWORD   flag;
    const char* name;
} DWORD_FLAG_DESCRIPTIONS;

extern BOOL fShowRelocations;
extern BOOL fShowRawSectionData;
extern BOOL fShowSymbolTable;
extern BOOL fShowLineNumbers;
extern BOOL fShowIATentries;
extern BOOL fShowPDATA;
extern BOOL fShowResources;

LPVOID GetPtrFromRVA(DWORD64 rva, PIMAGE_NT_HEADERS pNTHeader, DWORD64 imageBase);
void GetResourceNameFromId(DWORD id, DWORD64 resourceBase, PSTR buffer, UINT cBytes);
void GetResourceTypeName(DWORD type, PSTR buffer, UINT cBytes);
