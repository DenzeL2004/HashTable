global NASMCRC32

extern CRC32Table

section .text

NASMCRC32:
    
    mov eax, -1         ;hash = 0xFFFFFFFF;

.start:

    test    rsi, rsi    ;---------------------
    je      .exit       ;check counter is zero


    mov     ecx, eax    ;---------------------
    shr     ecx, 8      ;hash >> 8

    xor     al, byte [rdi]                      ;h1 = hash ^ data[it]
    inc     rdi

    movzx   eax, al                             ;(h1) & 0xFF                         
    xor     ecx, dword [CRC32Table + rax*4]    ;hash ^ CRC32Table[h1]

    mov     eax, ecx  

    dec     rsi
    jmp     .start

.exit:

    xor     eax, -1     ;hash ^ 0xFFFFFFFF;
    ret
