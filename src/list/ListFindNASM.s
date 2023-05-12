global ListFindValNASM

section .text

ListFindValNASM:
        
        test    rdi, rdi                        ;--------------------
        je      .not_find                       ;check list size != 0
        
        mov     rcx, qword [rdi+8]              ;list size

        test    rcx, rcx                        ;--------------------
        je      .not_find                       ;check list size != 0

        mov     rax, qword [rdi+32]             ;list head ptr
        
        dec     rcx
        
        mov     rdi, qword [rdi+24]             ;list data address
        vmovdqa ymm1, yword [rsi]               ;set to ymm input str

        jmp     .check_str

.next_ind:

        mov     rax, qword [rdx+8]              ;ans_ind = list->data[ind].next
        
        sub     rcx, 1
        jb      .not_find

.check_str:

        lea     rdx, [rax+rax*2]                ;calc index val
        lea     rdx, [rdi+rdx*8]                ;--------------

        mov     rbx, qword [rdx]                ;current strinng address
        
        vpcmpeqb        ymm0, ymm1, yword [rbx]
        vpmovmskb       ebx, ymm0
        
        cmp     ebx, -1                         ;compare strings
        jne     .next_ind

        ret

.not_find:
        xor eax, eax
        ret