#include <windows.h>
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


char *ProcessCommandLine(int argc, char* argv[]) {
    return NULL;
}

void DumpFile(char *filename) {
}

int main(int argc, char* argv[])
{
    char *filename;

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
