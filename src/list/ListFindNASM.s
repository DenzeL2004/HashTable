global ListFindValNASM

section .text

ListFindValNASM:
        
        test    rdi, rdi                        ;--------------------
        je      .not_find                       ;check list size != 0
        
        mov     rcx, qword [rdi+8]
        mov     rax, qword [rdi+32]
        lea     rdx, [rcx-1]
        test    rcx, rcx                        ;check size_list
        je      .not_find

        mov     rdi, qword [rdi+24]
        vmovdqa ymm1, yword [rsi]

        jmp     .start

.next_id:
        mov     rax, qword [rcx+8]
        sub     rdx, 1                          ;next id
        jb      .not_find

.start:
        lea     rcx, [rax+rax*2]
        lea     rcx, [rdi+rcx*8]
        mov     rsi, qword  [rcx]

        vpcmpeqb        ymm0, ymm1, yword [rsi]
        vpmovmskb       esi, ymm0

        cmp     esi, -1                         ;cmp string
        jne     .next_id
        
        ret

.not_find:
        xor     eax, eax
        ret