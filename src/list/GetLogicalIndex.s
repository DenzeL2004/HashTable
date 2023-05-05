global NASMGetLogicalIndex
global NASMListGetVal

section .text

NASMGetLogicalIndex:

        test    rdi, rdi                        ;------------------
        je     .not_valid                       ;if list ptr == nullptr 

        cmp    rsi, qword [rdi+8]               ;------------------
        jg     .not_valid                       ;if ind > list->size_data

        test    esi, esi                        ;-----------
        jle     .not_valid                      ;if ind <= 0

    
        mov     edx, dword [rdi+44]             ;linearize parametr
        movsx   rax, dword [rdi+32]             ;head index
        
        test    edx, edx                        ;---------------
        je      .else                           ;check linearize
        
        lea     eax, [rax - 1 + rsi]            ;calc index
        
        ret

.else:
        cmp     esi, 1                          ;---------------------
        jle     .exit                           ;check input index = 1
        
        mov     rcx, qword [rdi+24]             ;data addrses
        
        dec     esi                             
.loop:

        lea     rax, [rax+rax*2]                ;---------------
        movsx   rax, dword [rcx+4+rax*4]        ;calc next index
        
        dec     esi
        test    esi, esi
        jne     .loop

.exit:
        ret

.not_valid:

        mov eax, -1
        ret


NASMListGetVal:

        xor     eax, eax

        test    rdi, rdi                        ;check list isn't nullptr
        je      .exit                           ;------------------------

        lea     rdx, [rsi-1]
        cmp     rdx, qword [rdi+16]             ;check ind > list's size || ind == 0
        jnb     .exit                           ;-----------------------------------

        mov     rax, qword [rdi]                ;ans = list.head

        test    rdx, rdx                        ;-------------
        je      .break                          ;check ind = 1

        test    dl, 1
        je      .loop
        sub     rsi, 2
        mov     rax, qword [rax+16]
        mov     rdx, rsi
        je      .break
.loop:
        mov     rax, qword [rax+16]
        mov     rax, qword [rax+16]
        sub     rdx, 2
        jne     .loop
.break:
        mov     rax, qword [rax]                ;return ans.val
.exit:
        ret
