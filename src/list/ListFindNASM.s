global ListFindValNASM

section .text

ListFindValNASM:
        
        test    rdi, rdi                        ;--------------------
        je      .not_find                       ;check list size != 0
        
        mov     rcx, qword [rdi+8]              ;list size

        mov     rax, qword [rdi+32]             ;list head_ptr
                       
        test    rcx, rcx
        je      .not_find
        
        mov     r8, qword [rsi+8]               ;input value len           

        mov     r9, qword [rdi+24]              ;data address

        dec     rcx
        jmp     .begin

.next
        mov     rax, qword [rdx+8]              ;node->next
        sub     rcx, 1
        jb      .not_find
.begin
        lea     rdx, [rax+rax*2]                ;---------------
        lea     rdx, [r9+rdx*8]                 ;calc index list

        mov     rdi, qword [rdx]                ;address list vallue

        cmp     qword [rdi+8], r8               ;------------------------------------------------
        jne     .next                           ;check input val len and current list element len

        mov             r10, qword [rdi]        ;first string address
        mov             rdi, qword [rsi]        ;second string address
        
        vmovdqa         ymm0, yword [r10]       
        vpcmpeqb        ymm0, ymm0, yword [rdi] ;comparee string 

        vpmovmskb       edi, ymm0               ;get mask        
        
        cmp     edi, -1                 
        jne     .next                           ;check mask

        ret

.not_find:
        xor eax, eax
        ret