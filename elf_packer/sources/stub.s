[BITS 64]

global _start

_start:
    ; 1. 레지스터 저장
    push rdi
    push rsi
    push rdx
    push rcx
    push rax
    push r8
    push r9
    push r10
    push r11

    ; 2. 내 이름 "inskim" 출력
    mov rax, 1                  ; sys_write
    mov rdi, 1                  ; fd = 1
    lea rsi, [rel msg]          ; RIP-relative address
    mov rdx, 7                  ; length
    syscall
    
    ; 3. 복호화
    xor rax, rax
    mov rcx, 0xFFFFFFFFFFFFFFFF ; decrypt size
    mov rsi, 0x1111111111111111 ; start addr
    mov rdi, 0x2222222222222222 ; key
loop:
    cmp rax, rcx
    jge outer_loop
    xor [rsi], dil
    add rsi, 1
    add rax, 1
    jmp loop
outer_loop:

    ; 4. 레지스터 복구
    pop r11
    pop r10
    pop r9
    pop r8
    pop rax
    pop rcx
    pop rdx
    pop rsi
    pop rdi

    ; 5. 원본 OEP로 점프
    mov rax, 0x1122334455667788 ; Placeholder 매직 넘버
    jmp rax

msg: db "inskim", 0x0a