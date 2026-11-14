#include <windows.h>


//
// ImageRvcToVa does not exist on NT 3.1
//
//


#if defined(_WIN64)
    typedef unsigned __int64 ULONG_PTR;
#else
    typedef  unsigned long ULONG_PTR;
#endif


PIMAGE_SECTION_HEADER CustomImageRvaToSection(
    PIMAGE_NT_HEADERS NtHeaders,
    ULONG Rva)
{
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(NtHeaders);
    WORD numSections = NtHeaders->FileHeader.NumberOfSections;

    for (WORD i = 0; i < numSections; i++, section++) {
        ULONG size = section->Misc.VirtualSize;
        if (size == 0) {
            size = section->SizeOfRawData;
        }
        if (Rva >= section->VirtualAddress &&
            Rva < section->VirtualAddress + size) {
            return section;
        }
    }
    return NULL;
}

PVOID ImageRvaToVa(
    PIMAGE_NT_HEADERS NtHeaders,
    PVOID Base,
    ULONG Rva,
    PIMAGE_SECTION_HEADER *LastRvaSection)
{
    PIMAGE_SECTION_HEADER section = NULL;

    if (LastRvaSection != NULL && *LastRvaSection != NULL) {
        section = *LastRvaSection;
        ULONG size = section->Misc.VirtualSize ? section->Misc.VirtualSize : section->SizeOfRawData;
        if (Rva < section->VirtualAddress || Rva >= section->VirtualAddress + size) {
            section = NULL;
        }
    }

    if (section == NULL) {
        section = CustomImageRvaToSection(NtHeaders, Rva);
        if (section == NULL) {
            return NULL;
        }
        if (LastRvaSection != NULL) {
            *LastRvaSection = section;
        }
    }

    return (PVOID)((ULONG_PTR)Base + Rva - section->VirtualAddress + section->PointerToRawData);
}