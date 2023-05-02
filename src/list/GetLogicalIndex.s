global NASMGetLogicalIndex

section .text

NASMGetLogicalIndex:

        test    rdi, rdi                        ;------------------
        je     .not_valid                       ;if test == nullptr           

    
        mov     edx, DWORD [rdi+44]             ;linearize parametr
        movsx   rax, DWORD [rdi+32]             ;head index
        
        test    edx, edx                        ;---------------
        je      .else                           ;check linearize
        
        lea     eax, [rax - 1 + rsi]
        
        ret

.else:
        cmp     esi, 1
        jle     .exit
        mov     rcx, QWORD [rdi+24]
        
        dec     esi
.loop:

        lea     rax, [rax+rax*2]
        
        movsx   rax, DWORD [rcx+4+rax*4]
        
        dec     esi
        test    esi, esi
        jne     .loop

.exit:
        ret

.not_valid:

        mov eax, -1
        ret