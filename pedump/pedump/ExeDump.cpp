#include "ExeDump.h"

WORD_FLAG_DESCRIPTIONS ImageFileHeaderCharacteristics[] =
{
{ IMAGE_FILE_RELOCS_STRIPPED, "RELOCS_STRIPPED" },
{ IMAGE_FILE_EXECUTABLE_IMAGE, "EXECUTABLE_IMAGE" },
{ IMAGE_FILE_LINE_NUMS_STRIPPED, "LINE_NUMS_STRIPPED" },
{ IMAGE_FILE_LOCAL_SYMS_STRIPPED, "LOCAL_SYMS_STRIPPED" },
{ IMAGE_FILE_AGGRESIVE_WS_TRIM, "AGGRESIVE_WS_TRIM" },
// { IMAGE_FILE_MINIMAL_OBJECT, "MINIMAL_OBJECT" }, // Removed in NT 3.5
// { IMAGE_FILE_UPDATE_OBJECT, "UPDATE_OBJECT" },   // Removed in NT 3.5
// { IMAGE_FILE_16BIT_MACHINE, "16BIT_MACHINE" },   // Removed in NT 3.5
{ IMAGE_FILE_BYTES_REVERSED_LO, "BYTES_REVERSED_LO" },
{ IMAGE_FILE_32BIT_MACHINE, "32BIT_MACHINE" },
{ IMAGE_FILE_DEBUG_STRIPPED, "DEBUG_STRIPPED" },
// { IMAGE_FILE_PATCH, "PATCH" },
{ IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP, "REMOVABLE_RUN_FROM_SWAP" },
{ IMAGE_FILE_NET_RUN_FROM_SWAP, "NET_RUN_FROM_SWAP" },
{ IMAGE_FILE_SYSTEM, "SYSTEM" },
{ IMAGE_FILE_DLL, "DLL" },
{ IMAGE_FILE_UP_SYSTEM_ONLY, "UP_SYSTEM_ONLY" },
{ IMAGE_FILE_BYTES_REVERSED_HI, "BYTES_REVERSED_HI" }
};

#define NUMBER_IMAGE_HEADER_FLAGS (sizeof(ImageFileHeaderCharacteristics) / sizeof(WORD_FLAG_DESCRIPTIONS))

WORD_FLAG_DESCRIPTIONS DllCharacteristics[] =
{
{ IMAGE_DLLCHARACTERISTICS_WDM_DRIVER, "WDM_DRIVER" },
};
#define NUMBER_DLL_CHARACTERISTICS (sizeof(DllCharacteristics) / sizeof(WORD_FLAG_DESCRIPTIONS))

const char* ImageDirectoryNames[] = {
    "EXPORT", "IMPORT", "RESOURCE", "EXCEPTION", "SECURITY", "BASERELOC",
    "DEBUG", "COPYRIGHT", "GLOBALPTR", "TLS", "LOAD_CONFIG",
    "BOUND_IMPORT", "IAT",  // These two entries added for NT 3.51
    "DELAY_IMPORT" };		// This entry added in NT 5

#define NUMBER_IMAGE_DIRECTORY_ENTRYS (sizeof(ImageDirectoryNames)/sizeof(char *))

#ifndef IMAGE_SCN_TYPE_DSECT
#define IMAGE_SCN_TYPE_DSECT                 0x00000001  // Reserved.
#endif
#ifndef IMAGE_SCN_TYPE_NOLOAD
#define IMAGE_SCN_TYPE_NOLOAD                0x00000002  // Reserved.
#endif
#ifndef IMAGE_SCN_TYPE_GROUP
#define IMAGE_SCN_TYPE_GROUP                 0x00000004  // Reserved.
#endif
#ifndef IMAGE_SCN_TYPE_COPY
#define IMAGE_SCN_TYPE_COPY                  0x00000010  // Reserved.
#endif
#ifndef IMAGE_SCN_TYPE_OVER
#define IMAGE_SCN_TYPE_OVER                  0x00000400  // Reserved.
#endif
#ifndef IMAGE_SCN_MEM_PROTECTED
#define IMAGE_SCN_MEM_PROTECTED              0x00004000
#endif
#ifndef IMAGE_SCN_MEM_SYSHEAP
#define IMAGE_SCN_MEM_SYSHEAP                0x00010000
#endif

// Bitfield values and names for the IMAGE_SECTION_HEADER flags
DWORD_FLAG_DESCRIPTIONS SectionCharacteristics[] =
{

{ IMAGE_SCN_TYPE_DSECT, "DSECT" },
{ IMAGE_SCN_TYPE_NOLOAD, "NOLOAD" },
{ IMAGE_SCN_TYPE_GROUP, "GROUP" },
{ IMAGE_SCN_TYPE_NO_PAD, "NO_PAD" },
{ IMAGE_SCN_TYPE_COPY, "COPY" },
{ IMAGE_SCN_CNT_CODE, "CODE" },
{ IMAGE_SCN_CNT_INITIALIZED_DATA, "INITIALIZED_DATA" },
{ IMAGE_SCN_CNT_UNINITIALIZED_DATA, "UNINITIALIZED_DATA" },
{ IMAGE_SCN_LNK_OTHER, "OTHER" },
{ IMAGE_SCN_LNK_INFO, "INFO" },
{ IMAGE_SCN_TYPE_OVER, "OVER" },
{ IMAGE_SCN_LNK_REMOVE, "REMOVE" },
{ IMAGE_SCN_LNK_COMDAT, "COMDAT" },
{ IMAGE_SCN_MEM_PROTECTED, "PROTECTED" },
{ IMAGE_SCN_MEM_FARDATA, "FARDATA" },
{ IMAGE_SCN_MEM_SYSHEAP, "SYSHEAP" },
{ IMAGE_SCN_MEM_PURGEABLE, "PURGEABLE" },
{ IMAGE_SCN_MEM_LOCKED, "LOCKED" },
{ IMAGE_SCN_MEM_PRELOAD, "PRELOAD" },
{ IMAGE_SCN_LNK_NRELOC_OVFL, "NRELOC_OVFL" },
{ IMAGE_SCN_MEM_DISCARDABLE, "DISCARDABLE" },
{ IMAGE_SCN_MEM_NOT_CACHED, "NOT_CACHED" },
{ IMAGE_SCN_MEM_NOT_PAGED, "NOT_PAGED" },
{ IMAGE_SCN_MEM_SHARED, "SHARED" },
{ IMAGE_SCN_MEM_EXECUTE, "EXECUTE" },
{ IMAGE_SCN_MEM_READ, "READ" },
{ IMAGE_SCN_MEM_WRITE, "WRITE" },
};

#define NUMBER_SECTION_CHARACTERISTICS (sizeof(SectionCharacteristics) / sizeof(DWORD_FLAG_DESCRIPTIONS))


PIMAGE_DEBUG_MISC g_pMiscDebugInfo = 0;
PDWORD g_pCVHeader = 0;
PIMAGE_COFF_SYMBOLS_HEADER g_pCOFFHeader = 0;
COFFSymbolTable* g_pCOFFSymbolTable;

const char* GetMachineTypeName(WORD wMachineType) {
    switch (wMachineType) {
    case IMAGE_FILE_MACHINE_I386: 	return "i386";
    case IMAGE_FILE_MACHINE_AMD64:  return "x64 (AMD)";
    case IMAGE_FILE_MACHINE_R3000:  return "R3000";
    case 160:                       return "R3000 big endian";
    case IMAGE_FILE_MACHINE_R4000:  return "R4000";
    case IMAGE_FILE_MACHINE_R10000: return "R10000";
    case IMAGE_FILE_MACHINE_ALPHA:  return "Alpha";
    case IMAGE_FILE_MACHINE_POWERPC:return "PowerPC";
    default:    					return "unknown";
    }
}

void DumpHeader(PIMAGE_FILE_HEADER pImageFileHeader) {
    UINT headerFieldWidth = 30;

    printf("File Header\n");
    printf("  %-*s%04X (%s)\n", headerFieldWidth, "Machine:", pImageFileHeader->Machine, GetMachineTypeName(pImageFileHeader->Machine));
    printf("  %-*s%04X\n", headerFieldWidth, "Number of Sections:", pImageFileHeader->NumberOfSections);
    printf("  %-*s%08X -> %s", headerFieldWidth, "TimeDateStamp:", pImageFileHeader->TimeDateStamp, ctime((time_t*)&pImageFileHeader->TimeDateStamp));
    printf("  %-*s%08X\n", headerFieldWidth, "PointerToSymbolTable:", pImageFileHeader->PointerToSymbolTable);
    printf("  %-*s%08X\n", headerFieldWidth, "NumberOfSymbols:", pImageFileHeader->NumberOfSymbols);
    printf("  %-*s%04X\n", headerFieldWidth, "SizeOfOptionalHeader:", pImageFileHeader->SizeOfOptionalHeader);
    printf("  %-*s%04X\n", headerFieldWidth, "Characteristics:", pImageFileHeader->Characteristics);
    for (int i = 0; i < NUMBER_IMAGE_HEADER_FLAGS; i++) {
        if (pImageFileHeader->Characteristics & ImageFileHeaderCharacteristics[i].flag) {
            printf("    %s\n", ImageFileHeaderCharacteristics[i].name);
        }
    }
}

void DumpOptionalHeader(PIMAGE_OPTIONAL_HEADER optionalHeader) {
    UINT width = 30;
    const char* s;
    UINT i;

    printf("Optional Header\n");
    printf("  %-*s%04X\n", width, "Magic", optionalHeader->Magic);
    printf("  %-*s%u.%02u\n", width, "linker version", optionalHeader->MajorLinkerVersion, optionalHeader->MinorLinkerVersion);
    printf("  %-*s%X\n", width, "size of code", optionalHeader->SizeOfCode);
    printf("  %-*s%X\n", width, "size of initialized data", optionalHeader->SizeOfInitializedData);
    printf("  %-*s%X\n", width, "size of uninitialized data", optionalHeader->SizeOfUninitializedData);
    printf("  %-*s%X\n", width, "entrypoint RVA", optionalHeader->AddressOfEntryPoint);
    printf("  %-*s%X\n", width, "base of code", optionalHeader->BaseOfCode);
#ifndef _WIN64
    printf("  %-*s%X\n", width, "base of data", optionalHeader->BaseOfData);
#endif
    printf("  %-*s%llX\n", width, "image base", (DWORD64)optionalHeader->ImageBase);
    printf("  %-*s%X\n", width, "section align", optionalHeader->SectionAlignment);
    printf("  %-*s%X\n", width, "file align", optionalHeader->FileAlignment);
    printf("  %-*s%u.%02u\n", width, "required OS version", optionalHeader->MajorOperatingSystemVersion, optionalHeader->MinorOperatingSystemVersion);
    printf("  %-*s%u.%02u\n", width, "image version", optionalHeader->MajorImageVersion, optionalHeader->MinorImageVersion);
    printf("  %-*s%u.%02u\n", width, "subsystem version", optionalHeader->MajorSubsystemVersion, optionalHeader->MinorSubsystemVersion);
    printf("  %-*s%X\n", width, "Win32 Version", optionalHeader->Win32VersionValue);
    printf("  %-*s%X\n", width, "size of image", optionalHeader->SizeOfImage);
    printf("  %-*s%X\n", width, "size of headers", optionalHeader->SizeOfHeaders);
    printf("  %-*s%X\n", width, "checksum", optionalHeader->CheckSum);
    switch (optionalHeader->Subsystem) {
    case IMAGE_SUBSYSTEM_NATIVE: s = "Native"; break;
    case IMAGE_SUBSYSTEM_WINDOWS_GUI: s = "Windows GUI"; break;
    case IMAGE_SUBSYSTEM_WINDOWS_CUI: s = "Windows character"; break;
    case IMAGE_SUBSYSTEM_OS2_CUI: s = "OS/2 character"; break;
    case IMAGE_SUBSYSTEM_POSIX_CUI: s = "Posix character"; break;
    default: s = "unknown";
    }
    printf("  %-*s%04X (%s)\n", width, "Subsystem", optionalHeader->Subsystem, s);
    // Marked as obsolete in MSDN CD 9
    printf("  %-*s%04X\n", width, "DLL flags", optionalHeader->DllCharacteristics);
    for (i = 0; i < NUMBER_DLL_CHARACTERISTICS; i++) {
        if (optionalHeader->DllCharacteristics & DllCharacteristics[i].flag)
            printf("  %-*s%s", width, " ", DllCharacteristics[i].name);
    }
    if (optionalHeader->DllCharacteristics)
        printf("\n");

    printf("  %-*s%llX\n", width, "stack reserve size", (DWORD64)optionalHeader->SizeOfStackReserve);
    printf("  %-*s%llX\n", width, "stack commit size", (DWORD64)optionalHeader->SizeOfStackCommit);
    printf("  %-*s%llX\n", width, "heap reserve size", (DWORD64)optionalHeader->SizeOfHeapReserve);
    printf("  %-*s%llX\n", width, "heap commit size", (DWORD64)optionalHeader->SizeOfHeapCommit);
    // Marked as obsolete in MSDN CD 9
    printf("  %-*s%08X\n", width, "loader flags", optionalHeader->LoaderFlags);
    if (optionalHeader->LoaderFlags)
        printf("\n");
    printf("  %-*s%X\n", width, "RVAs & sizes", optionalHeader->NumberOfRvaAndSizes);
    printf("\nData Directory\n");
    for (i = 0; i < optionalHeader->NumberOfRvaAndSizes; i++) {
        printf("  %-12s rva: %08X  size: %08X\n", (i >= NUMBER_IMAGE_DIRECTORY_ENTRYS) ? "unused" : ImageDirectoryNames[i], optionalHeader->DataDirectory[i].VirtualAddress, optionalHeader->DataDirectory[i].Size);
    }
}

void DumpSectionTable(PIMAGE_SECTION_HEADER section, unsigned cSections, BOOL IsEXE) {
    unsigned i, j;
    const char* pszAlign;

    printf("Section Table\n");
    for (i = 1; i <= cSections; i++, section++) {
        printf("  %02X %-8.8s  %s: %08X  VirtAddr:  %08X\n", i, section->Name, IsEXE ? "VirtSize" : "PhysAddr", section->Misc.VirtualSize, section->VirtualAddress);
        printf("    raw data offs:   %08X  raw data size: %08X\n", section->PointerToRawData, section->SizeOfRawData);
        printf("    relocation offs: %08X  relocations:   %08X\n", section->PointerToRelocations, section->NumberOfRelocations);
        printf("    line # offs:     %08X  line #'s:      %08X\n", section->PointerToLinenumbers, section->NumberOfLinenumbers);
        printf("    characteristics: %08X\n", section->Characteristics);
        printf("    ");
        for (j = 0; j < NUMBER_SECTION_CHARACTERISTICS; j++) {
            if (section->Characteristics & SectionCharacteristics[j].flag)
                printf("  %s", SectionCharacteristics[j].name);
        }

        switch (section->Characteristics & IMAGE_SCN_ALIGN_64BYTES) {
        case IMAGE_SCN_ALIGN_1BYTES: pszAlign = "ALIGN_1BYTES"; break;
        case IMAGE_SCN_ALIGN_2BYTES: pszAlign = "ALIGN_2BYTES"; break;
        case IMAGE_SCN_ALIGN_4BYTES: pszAlign = "ALIGN_4BYTES"; break;
        case IMAGE_SCN_ALIGN_8BYTES: pszAlign = "ALIGN_8BYTES"; break;
        case IMAGE_SCN_ALIGN_16BYTES: pszAlign = "ALIGN_16BYTES"; break;
        case IMAGE_SCN_ALIGN_32BYTES: pszAlign = "ALIGN_32BYTES"; break;
        case IMAGE_SCN_ALIGN_64BYTES: pszAlign = "ALIGN_64BYTES"; break;
        default: pszAlign = "ALIGN_DEFAULT(16)"; break;
        }
        printf("  %s", pszAlign);
        printf("\n\n");
    }
}

PIMAGE_SECTION_HEADER GetSectionHeader(PSTR name, PIMAGE_NT_HEADERS pNTHeader) {
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
    unsigned i;

    for (i = 0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++)
    {
        if (0 == strncmp((char*)section->Name, name, IMAGE_SIZEOF_SHORT_NAME))
            return section;
    }

    return 0;
}

PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD64 rva, PIMAGE_NT_HEADERS pNTHeader) {
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
    unsigned i;

    for (i = 0; i < pNTHeader->FileHeader.NumberOfSections; i++, section++) {
        // Is the RVA within this section?
        if ((rva >= section->VirtualAddress) && (rva < (DWORD64)((DWORD64)section->VirtualAddress + (DWORD64)section->Misc.VirtualSize)))
            return section;
    }

    return 0;
}

const char* SzDebugFormats[] = {
"UNKNOWN/BORLAND","COFF","CODEVIEW","FPO","MISC","EXCEPTION","FIXUP",
"OMAP_TO_SRC", "OMAP_FROM_SRC" };

void DumpDebugDirectory(PIMAGE_DEBUG_DIRECTORY debugDir, DWORD size, void* base) {
    DWORD cDebugFormats = size / sizeof(IMAGE_DEBUG_DIRECTORY);
    const char* szDebugFormat;
    unsigned i;

    if (cDebugFormats == 0) return;

    printf(
        "Debug Formats in File\n"
        "  Type            Size     Address  FilePtr  Charactr TimeDate Version\n"
        "  --------------- -------- -------- -------- -------- -------- --------\n"
    );
    for (i = 0; i < cDebugFormats; i++) {
        szDebugFormat = (debugDir->Type <= IMAGE_DEBUG_TYPE_OMAP_FROM_SRC) ? SzDebugFormats[debugDir->Type] : "???";
        printf("  %-15s %08X %08X %08X %08X %08X %u.%02u\n", szDebugFormat, debugDir->SizeOfData, debugDir->AddressOfRawData, debugDir->PointerToRawData, debugDir->Characteristics, debugDir->TimeDateStamp, debugDir->MajorVersion, debugDir->MinorVersion);
        switch (debugDir->Type) {
            case IMAGE_DEBUG_TYPE_COFF:
                g_pCOFFHeader = (PIMAGE_COFF_SYMBOLS_HEADER)((ULONGLONG)base + debugDir->PointerToRawData);
                break;
            case IMAGE_DEBUG_TYPE_MISC:
                g_pMiscDebugInfo = (PIMAGE_DEBUG_MISC)((ULONGLONG)base + debugDir->PointerToRawData);
                break;
            case IMAGE_DEBUG_TYPE_CODEVIEW:
                g_pCVHeader = (PDWORD)((ULONGLONG)base + debugDir->PointerToRawData);
                break;
        }
        debugDir++;
    }
}

void DumpExeDebugDirectory(void* base, PIMAGE_NT_HEADERS pNTHeader){ 
    PIMAGE_DEBUG_DIRECTORY debugDir;
    PIMAGE_SECTION_HEADER header;
    DWORD va_debug_dir;
    DWORD size;

    va_debug_dir = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_DEBUG);
    if (va_debug_dir == 0) return;
    header = GetSectionHeader((PSTR)".debug", pNTHeader);
    if (header && (header->VirtualAddress == va_debug_dir)) {
        debugDir = (PIMAGE_DEBUG_DIRECTORY)(header->PointerToRawData + (DWORD64)base);
        size = GetImgDirEntrySize(pNTHeader, IMAGE_DIRECTORY_ENTRY_DEBUG) * sizeof(IMAGE_DEBUG_DIRECTORY);
    }
    else {
        header = GetEnclosingSectionHeader(va_debug_dir, pNTHeader);
        if (!header) return;
        size = GetImgDirEntrySize(pNTHeader, IMAGE_DIRECTORY_ENTRY_DEBUG);
        debugDir = MakePtr(PIMAGE_DEBUG_DIRECTORY, base, (DWORD64)header->PointerToRawData + ((DWORD64)va_debug_dir - (DWORD64)header->VirtualAddress));
    }
    DumpDebugDirectory(debugDir, size, base);
}

void DumpImportsSection(void* base, PIMAGE_NT_HEADERS pNTHeader)
{
    PIMAGE_IMPORT_DESCRIPTOR importDesc;
    PIMAGE_SECTION_HEADER pSection;
    PIMAGE_THUNK_DATA thunk, thunkIAT = 0;
    PIMAGE_IMPORT_BY_NAME pOrdinalName;
    DWORD importsStartRVA;
    PSTR pszTimeDate;

    // Look up where the imports section is (normally in the .idata section)
    // but not necessarily so.  Therefore, grab the RVA from the data dir.
    importsStartRVA = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_IMPORT);
    if (!importsStartRVA)
        return;

    // Get the IMAGE_SECTION_HEADER that contains the imports.  This is
    // usually the .idata section, but doesn't have to be.
    pSection = GetEnclosingSectionHeader(importsStartRVA, pNTHeader);
    if (!pSection)
        return;

    importDesc = (PIMAGE_IMPORT_DESCRIPTOR)
        GetPtrFromRVA(importsStartRVA, pNTHeader, (DWORD64)base);
    if (!importDesc)
        return;

    printf("Imports Table:\n");

    while (1)
    {
        // See if we've reached an empty IMAGE_IMPORT_DESCRIPTOR
        if ((importDesc->TimeDateStamp == 0) && (importDesc->Name == 0))
            break;

        printf("  %llx\n", (DWORD64)GetPtrFromRVA(importDesc->Name, pNTHeader, (DWORD64)base));

        printf("  OrigFirstThunk:  %08X (Unbound IAT)\n",
            importDesc->Characteristics);

        pszTimeDate = ctime((time_t*)&importDesc->TimeDateStamp);
        printf("  TimeDateStamp:   %08X", importDesc->TimeDateStamp);
        printf(pszTimeDate ? " -> %s" : "\n", pszTimeDate);

        printf("  ForwarderChain:  %08X\n", importDesc->ForwarderChain);
        printf("  First thunk RVA: %08X\n", importDesc->FirstThunk);

        thunk = (PIMAGE_THUNK_DATA)(DWORD64)importDesc->Characteristics;
        thunkIAT = (PIMAGE_THUNK_DATA)(DWORD64)importDesc->FirstThunk;

        if (thunk == 0)   // No Characteristics field?
        {
            // Yes! Gotta have a non-zero FirstThunk field then.
            thunk = thunkIAT;

            if (thunk == 0)   // No FirstThunk field?  Ooops!!!
                return;
        }

        // Adjust the pointer to point where the tables are in the
        // mem mapped file.
        thunk = (PIMAGE_THUNK_DATA)GetPtrFromRVA((DWORD64)thunk, pNTHeader, (DWORD64)base);
        if (!thunk)
            return;

        thunkIAT = (PIMAGE_THUNK_DATA)
            GetPtrFromRVA((DWORD64)thunkIAT, pNTHeader, (DWORD64)base);

        printf("  Ordn  Name\n");

        while (1) // Loop forever (or until we break out)
        {
            if (thunk->u1.AddressOfData == 0)
                break;

            if (thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
            {
                printf("  %4llu", (DWORD64)IMAGE_ORDINAL(thunk->u1.Ordinal));
            }
            else
            {
                pOrdinalName = (PIMAGE_IMPORT_BY_NAME)thunk->u1.AddressOfData;
                pOrdinalName = (PIMAGE_IMPORT_BY_NAME)GetPtrFromRVA((DWORD64)pOrdinalName, pNTHeader, (DWORD64)base);

                printf("  %4llu  %s", (DWORD64)pOrdinalName->Hint, pOrdinalName->Name);
            }

            // If the user explicitly asked to see the IAT entries, or
            // if it looks like the image has been bound, append the address
            if (fShowIATentries || importDesc->TimeDateStamp)
                printf(" (Bound to: %08llX)", (DWORD64)thunkIAT->u1.Function);

            printf("\n");

            thunk++;            // Advance to next thunk
            thunkIAT++;         // advance to next thunk
        }

        importDesc++;   // advance to next IMAGE_IMPORT_DESCRIPTOR
        printf("\n");
    }
}

void DumpExportsSection(void* base, PIMAGE_NT_HEADERS pNTHeader) {
    PIMAGE_EXPORT_DIRECTORY exportDir;
    PIMAGE_SECTION_HEADER header;
    INT delta;
    PSTR filename;
    DWORD i;
    PDWORD functions;
    PWORD ordinals;
    PSTR* name;
    DWORD exportsStartRVA, exportsEndRVA;

    exportsStartRVA = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_EXPORT);
    exportsEndRVA = exportsStartRVA +
        GetImgDirEntrySize(pNTHeader, IMAGE_DIRECTORY_ENTRY_EXPORT);

    // Get the IMAGE_SECTION_HEADER that contains the exports.  This is
    // usually the .edata section, but doesn't have to be.
    header = GetEnclosingSectionHeader(exportsStartRVA, pNTHeader);
    if (!header)
        return;

    delta = (INT)(header->VirtualAddress - header->PointerToRawData);

    exportDir = MakePtr(PIMAGE_EXPORT_DIRECTORY, base, (DWORD64)exportsStartRVA - (DWORD64)delta);

    filename = (PSTR)((DWORD64)exportDir->Name - (DWORD64)delta + (DWORD64)base);

    printf("exports table:\n\n");
    printf("  Name:            %s\n", filename);
    printf("  Characteristics: %08X\n", exportDir->Characteristics);
    printf("  TimeDateStamp:   %08X -> %s",
        exportDir->TimeDateStamp,
        ctime((time_t*)&exportDir->TimeDateStamp));
    printf("  Version:         %u.%02u\n", exportDir->MajorVersion,
        exportDir->MinorVersion);
    printf("  Ordinal base:    %08X\n", exportDir->Base);
    printf("  # of functions:  %08X\n", exportDir->NumberOfFunctions);
    printf("  # of Names:      %08X\n", exportDir->NumberOfNames);

    functions = (PDWORD)((DWORD64)exportDir->AddressOfFunctions - delta + (DWORD64)base);
    ordinals = (PWORD)((DWORD64)exportDir->AddressOfNameOrdinals - delta + (DWORD64)base);
    name = (PSTR*)((DWORD64)exportDir->AddressOfNames - delta + (DWORD64)base);

    printf("\n  Entry Pt  Ordn  Name\n");
    for (i = 0; i < exportDir->NumberOfFunctions; i++)
    {
        DWORD entryPointRVA = functions[i];
        DWORD j;

        if (entryPointRVA == 0)   // Skip over gaps in exported function
            continue;               // ordinals (the entrypoint is 0 for
                                    // these functions).

        printf("  %08X  %4u", entryPointRVA, i + exportDir->Base);

        // See if this function has an associated name exported for it.
        for (j = 0; j < exportDir->NumberOfNames; j++)
            if (ordinals[j] == i)
                printf("  %s", name[j] - delta + (DWORD64)base);

        // Is it a forwarder?  If so, the entry point RVA is inside the
        // .edata section, and is an RVA to the DllName.EntryPointName
        if ((entryPointRVA >= exportsStartRVA)
            && (entryPointRVA <= exportsEndRVA))
        {
            printf(" (forwarder -> %lX)", (DWORD)((DWORD)entryPointRVA - (DWORD)delta + (DWORD64)base));
        }

        printf("\n");
    }
}

void DumpBoundImportDescriptors(void* base, PIMAGE_NT_HEADERS pNTHeader) {
    DWORD bidRVA;   // Bound import descriptors RVA
    PIMAGE_BOUND_IMPORT_DESCRIPTOR pibid;

    bidRVA = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);
    if (!bidRVA)
        return;

    pibid = MakePtr(PIMAGE_BOUND_IMPORT_DESCRIPTOR, base, bidRVA);

    printf("Bound import descriptors:\n\n");
    printf("  Module        TimeDate\n");
    printf("  ------------  --------\n");

    while (pibid->TimeDateStamp)
    {
        unsigned i;
        PIMAGE_BOUND_FORWARDER_REF pibfr;

        printf("  %-12llx  %08X -> %s",
            (DWORD64)base + (DWORD64)bidRVA + (DWORD64)pibid->OffsetModuleName,
            pibid->TimeDateStamp,
            ctime((time_t*)&pibid->TimeDateStamp));

        pibfr = MakePtr(PIMAGE_BOUND_FORWARDER_REF, pibid,
            sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR));

        for (i = 0; i < pibid->NumberOfModuleForwarderRefs; i++)
        {
            printf("    forwarder:  %-12llx  %08X -> %s",
                (DWORD64)base + (DWORD64)bidRVA + (DWORD64)pibfr->OffsetModuleName,
                pibfr->TimeDateStamp,
                ctime((time_t*)&pibfr->TimeDateStamp));
            pibfr++;    // advance to next forwarder ref

            // Keep the outer loop pointer up to date too!
            pibid = MakePtr(PIMAGE_BOUND_IMPORT_DESCRIPTOR, pibid,
                sizeof(IMAGE_BOUND_FORWARDER_REF));
        }

        pibid++;    // Advance to next pibid;
    }
}

void DumpRuntimeFunctions(DWORD64 base, PIMAGE_NT_HEADERS pNTHeader) {
    DWORD rtFnRVA;

    rtFnRVA = GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_EXCEPTION);
    if (!rtFnRVA)
        return;

    DWORD cEntries =
        GetImgDirEntrySize(pNTHeader, IMAGE_DIRECTORY_ENTRY_EXCEPTION)
        / sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY);
    if (0 == cEntries)
        return;

    PIMAGE_RUNTIME_FUNCTION_ENTRY pRTFn = (PIMAGE_RUNTIME_FUNCTION_ENTRY)
        GetPtrFromRVA(rtFnRVA, pNTHeader, base);

    if (!pRTFn)
        return;

    printf("Runtime Function Table (Exception handling)\n");
    printf("  Begin     End\n");
    printf("  --------  --------  --------\n");

    for (unsigned i = 0; i < cEntries; i++, pRTFn++) {
        printf("  %08X  %08X", pRTFn->BeginAddress, pRTFn->EndAddress);
        if (g_pCOFFSymbolTable) {
            PCOFFSymbol pSymbol = g_pCOFFSymbolTable->GetNearestSymbolFromRVA((DWORD64)pRTFn->BeginAddress - (DWORD64)pNTHeader->OptionalHeader.ImageBase, TRUE);
            if (pSymbol) printf("  %s", pSymbol->GetName());
            delete pSymbol;
        }
        printf("\n");
    }
}

const char* SzRelocTypes[] = {"ABSOLUTE","HIGH","LOW","HIGHLOW","HIGHADJ","MIPS_JMPADDR","SECTION","REL32" };

void DumpBaseRelocationsSection(DWORD64 base, PIMAGE_NT_HEADERS pNTHeader)
{
    DWORD dwBaseRelocRVA;
    PIMAGE_BASE_RELOCATION baseReloc;

    dwBaseRelocRVA =
        GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_BASERELOC);
    if (!dwBaseRelocRVA)
        return;

    baseReloc = (PIMAGE_BASE_RELOCATION)
        GetPtrFromRVA(dwBaseRelocRVA, pNTHeader, base);
    if (!baseReloc)
        return;

    printf("base relocations:\n\n");

    while (baseReloc->SizeOfBlock != 0)
    {
        unsigned i, cEntries;
        PWORD pEntry;
        char* szRelocType;
        WORD relocType;

        // Sanity check to make sure the data looks OK.
        if (0 == baseReloc->VirtualAddress)
            break;
        if (baseReloc->SizeOfBlock < sizeof(*baseReloc))
            break;

        cEntries = (baseReloc->SizeOfBlock - sizeof(*baseReloc)) / sizeof(WORD);
        pEntry = MakePtr(PWORD, baseReloc, sizeof(*baseReloc));

        printf("Virtual Address: %08X  size: %08X\n",
            baseReloc->VirtualAddress, baseReloc->SizeOfBlock);

        for (i = 0; i < cEntries; i++)
        {
            // Extract the top 4 bits of the relocation entry.  Turn those 4
            // bits into an appropriate descriptive string (szRelocType)
            relocType = (*pEntry & 0xF000) >> 12;
            szRelocType = (char*)(relocType < 8 ? SzRelocTypes[relocType] : "unknown");

            printf("  %08X %s",
                (*pEntry & 0x0FFF) + baseReloc->VirtualAddress,
                szRelocType);

            if (IMAGE_REL_BASED_HIGHADJ == relocType)
            {
                pEntry++;
                cEntries--;
                printf(" (%X)", *pEntry);
            }

            printf("\n");
            pEntry++;   // Advance to next relocation entry
        }

        baseReloc = MakePtr(PIMAGE_BASE_RELOCATION, baseReloc,
            baseReloc->SizeOfBlock);
    }
}

void DumpMiscDebugInfo(PIMAGE_DEBUG_MISC pMiscDebugInfo){
    if (IMAGE_DEBUG_MISC_EXENAME != pMiscDebugInfo->DataType) {
        printf("Unknown Miscellaneous Debug Information type: %u\n", pMiscDebugInfo->DataType);
        return;
    }
    printf("Miscellaneous Debug Information\n");
    printf("  %s\n", pMiscDebugInfo->Data);
}

void DumpCVDebugInfo(PDWORD pCVHeader)
{
    PPDB_INFO pPDBInfo;

    printf("CodeView Signature: %08X\n", *pCVHeader);

    if ('01BN' != *pCVHeader)
    {
        printf("Unhandled CodeView Information format %llX\n", (DWORD64)pCVHeader);
        return;
    }

    pPDBInfo = (PPDB_INFO)pCVHeader;

    printf("  Offset: %08X  Signature: %08X  Age: %08X\n", pPDBInfo->Offset, pPDBInfo->sig, pPDBInfo->age);
    printf("  File: %s\n", pPDBInfo->PdbName);
}

void DumpCOFFHeader(PIMAGE_COFF_SYMBOLS_HEADER pDbgInfo)
{
    printf("COFF Debug Info Header\n");
    printf("  NumberOfSymbols:      %08X\n", pDbgInfo->NumberOfSymbols);
    printf("  LvaToFirstSymbol:     %08X\n", pDbgInfo->LvaToFirstSymbol);
    printf("  NumberOfLinenumbers:  %08X\n", pDbgInfo->NumberOfLinenumbers);
    printf("  LvaToFirstLinenumber: %08X\n", pDbgInfo->LvaToFirstLinenumber);
    printf("  RvaToFirstByteOfCode: %08X\n", pDbgInfo->RvaToFirstByteOfCode);
    printf("  RvaToLastByteOfCode:  %08X\n", pDbgInfo->RvaToLastByteOfCode);
    printf("  RvaToFirstByteOfData: %08X\n", pDbgInfo->RvaToFirstByteOfData);
    printf("  RvaToLastByteOfData:  %08X\n", pDbgInfo->RvaToLastByteOfData);
}

BOOL LookupSymbolName(DWORD index, PSTR buffer, UINT length)
{
    if (!g_pCOFFSymbolTable)
        return FALSE;

    PCOFFSymbol pSymbol = g_pCOFFSymbolTable->GetSymbolFromIndex(index);

    if (!pSymbol)
        return FALSE;

    strncpy(buffer, pSymbol->GetName(), length);

    delete pSymbol;

    return TRUE;
}

void DumpLineNumbers(PIMAGE_LINENUMBER pln, DWORD count)
{
    char buffer[64];
    DWORD i;

    printf("Line Numbers\n");

    for (i = 0; i < count; i++)
    {
        if (pln->Linenumber == 0) // A symbol table index
        {
            buffer[0] = 0;
            LookupSymbolName(pln->Type.SymbolTableIndex, buffer,
                sizeof(buffer));
            printf("SymIndex: %X (%s)\n", pln->Type.SymbolTableIndex,
                buffer);
        }
        else        // A regular line number
            printf(" Addr: %05X  Line: %04u\n",
                pln->Type.VirtualAddress, pln->Linenumber);
        pln++;
    }
}

void GetSectionName(WORD section, PSTR buffer, unsigned cbBuffer)
{
    char tempbuffer[10];

    switch ((SHORT)section)
    {
    case IMAGE_SYM_UNDEFINED: strcpy(tempbuffer, "UNDEF"); break;
    case IMAGE_SYM_ABSOLUTE:  strcpy(tempbuffer, "ABS"); break;
    case IMAGE_SYM_DEBUG:     strcpy(tempbuffer, "DEBUG"); break;
    default: sprintf(tempbuffer, "%X", section);
    }

    strncpy(buffer, tempbuffer, cbBuffer - 1);
}

void DumpSymbolTable(PCOFFSymbolTable pSymTab)
{
    printf("Symbol Table - %X entries  (* = auxillary symbol)\n",
        pSymTab->GetNumberOfSymbols());


    printf(
        "Indx Sectn Value    Type  Storage  Name\n"
        "---- ----- -------- ----- -------  --------\n");

    PCOFFSymbol pSymbol = pSymTab->GetNextSymbol(0);

    while (pSymbol)
    {
        char szSection[10];
        GetSectionName(pSymbol->GetSectionNumber(), szSection, sizeof(szSection));

        printf("%04X %5s %08X  %s %-8s %s\n",
            pSymbol->GetIndex(), szSection, pSymbol->GetValue(),
            pSymbol->GetTypeName(), pSymbol->GetStorageClassName(),
            pSymbol->GetName());

        if (pSymbol->GetNumberOfAuxSymbols())
        {
            char szAuxSymbol[1024];
            if (pSymbol->GetAuxSymbolAsString(szAuxSymbol, sizeof(szAuxSymbol)))
                printf("     * %s\n", szAuxSymbol);
        }

        pSymbol = pSymTab->GetNextSymbol(pSymbol);

    }
}

#define HEX_DUMP_WIDTH 16

//
// Dump a region of memory in a hexadecimal format
//
void HexDump(PBYTE ptr, DWORD length)
{
    char buffer[256];
    PSTR buffPtr, buffPtr2;
    unsigned cOutput, i;
    DWORD bytesToGo = length;

    while (bytesToGo)
    {
        cOutput = bytesToGo >= HEX_DUMP_WIDTH ? HEX_DUMP_WIDTH : bytesToGo;

        buffPtr = buffer;
        buffPtr += sprintf(buffPtr, "%08X:  ", length - bytesToGo);
        buffPtr2 = buffPtr + (HEX_DUMP_WIDTH * 3) + 1;

        for (i = 0; i < HEX_DUMP_WIDTH; i++)
        {
            BYTE value = *(ptr + i);

            if (i >= cOutput)
            {
                // On last line.  Pad with spaces
                *buffPtr++ = ' ';
                *buffPtr++ = ' ';
                *buffPtr++ = ' ';
            }
            else
            {
                if (value < 0x10)
                {
                    *buffPtr++ = '0';
                    _itoa(value, buffPtr++, 16);
                }
                else
                {
                    _itoa(value, buffPtr, 16);
                    buffPtr += 2;
                }

                *buffPtr++ = ' ';
                *buffPtr2++ = isprint(value) ? value : '.';
            }

            // Put an extra space between the 1st and 2nd half of the bytes
            // on each line.
            if (i == (HEX_DUMP_WIDTH / 2) - 1)
                *buffPtr++ = ' ';
        }

        *buffPtr2 = 0;  // Null terminate it.
        puts(buffer);   // Can't use printf(), since there may be a '%'
                        // in the string.
        bytesToGo -= cOutput;
        ptr += HEX_DUMP_WIDTH;
    }
}

void DumpRawSectionData(PIMAGE_SECTION_HEADER section, PVOID base, unsigned cSections) {
    unsigned i;
    char name[IMAGE_SIZEOF_SHORT_NAME + 1];

    printf("Section Hex Dumps\n");

    for (i = 1; i <= cSections; i++, section++)
    {
        // Make a copy of the section name so that we can ensure that
        // it's null-terminated
        memcpy(name, section->Name, IMAGE_SIZEOF_SHORT_NAME);
        name[IMAGE_SIZEOF_SHORT_NAME] = 0;

        // Don't dump sections that don't exist in the file!
        if (section->PointerToRawData == 0)
            continue;

        printf("section %02X (%s)  size: %08X  file offs: %08X\n",
            i, name, section->SizeOfRawData, section->PointerToRawData);

        HexDump(MakePtr(PBYTE, base, section->PointerToRawData),
            section->SizeOfRawData);
        printf("\n");
    }
}

void DumpExeFile(PIMAGE_DOS_HEADER dosHeader) {
    PIMAGE_NT_HEADERS pNTHeader;
    void* base = (void *)dosHeader;

    pNTHeader = MakePtr(PIMAGE_NT_HEADERS, base, dosHeader->e_lfanew);
    if (pNTHeader->Signature != IMAGE_NT_SIGNATURE) {
        printf("Not a Portable Executable (PE) EXE\n");
        return;
    }
    DumpHeader((PIMAGE_FILE_HEADER)&pNTHeader->FileHeader);
    printf("\n");
    DumpOptionalHeader((PIMAGE_OPTIONAL_HEADER)&pNTHeader->OptionalHeader);
    printf("\n");
    DumpSectionTable(IMAGE_FIRST_SECTION(pNTHeader), pNTHeader->FileHeader.NumberOfSections, TRUE);
    printf("\n");
    DumpExeDebugDirectory(base, pNTHeader);
    printf("\n");
    DumpResourceSection(base, pNTHeader);
    printf("\n");
    DumpImportsSection(base, pNTHeader);
    printf("\n");
    if (GetImgDirEntryRVA(pNTHeader, IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT)){
        DumpBoundImportDescriptors(base, pNTHeader);
        printf("\n");
    }
    DumpExportsSection(base, pNTHeader);
    printf("\n");
    if (g_pCOFFHeader) {	
        g_pCOFFSymbolTable = new COFFSymbolTable((PVOID)((DWORD64)base + pNTHeader->FileHeader.PointerToSymbolTable),pNTHeader->FileHeader.NumberOfSymbols);
    }
    if (fShowPDATA) {
        DumpRuntimeFunctions((DWORD64)base, pNTHeader);
        printf("\n");
    }
    if (fShowRelocations){
        DumpBaseRelocationsSection((DWORD64)base, pNTHeader);
        printf("\n");
    }
    if (fShowSymbolTable && g_pMiscDebugInfo) {
        DumpMiscDebugInfo(g_pMiscDebugInfo);
        printf("\n");
    }
    if (fShowSymbolTable && g_pCVHeader) {
        DumpCVDebugInfo(g_pCVHeader);
        printf("\n");
    }
    if (fShowSymbolTable && g_pCOFFHeader) {
        DumpCOFFHeader(g_pCOFFHeader);
        printf("\n");
    }
    if (fShowLineNumbers && g_pCOFFHeader) {
        DumpLineNumbers(MakePtr(PIMAGE_LINENUMBER, g_pCOFFHeader, g_pCOFFHeader->LvaToFirstLinenumber), g_pCOFFHeader->NumberOfLinenumbers);
        printf("\n");
    }
    if (fShowSymbolTable) {
        if (pNTHeader->FileHeader.NumberOfSymbols && pNTHeader->FileHeader.PointerToSymbolTable && g_pCOFFSymbolTable) {
            DumpSymbolTable(g_pCOFFSymbolTable);
            printf("\n");
        }
    }
    if (fShowRawSectionData) {
        DumpRawSectionData((PIMAGE_SECTION_HEADER)(pNTHeader + 1), dosHeader, pNTHeader->FileHeader.NumberOfSections);
    }
    if (g_pCOFFSymbolTable) delete g_pCOFFSymbolTable;
}
