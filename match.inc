    .code

;   https:\\www.geeksforgeeks.org/wildcard-character-matching/

; The main function that checks if two given strings 
; match. The first string may contain wildcard characters 

match proc first:QWORD, second:QWORD
    USING r13, r14
    SaveRegs

    mov r13, rcx
    mov r14, rdx

    ;  If we reach at the end of both strings, we are done 

    .if byte ptr [r13] == 0 && byte ptr [r14] == 0 
        mov rax, 1
        jmp quit
    .endif

    ;  Make sure to eliminate consecutive '*' 
    .if byte ptr [r13] == '*'  
      rpt1:
        mov rax, r13
        inc rax
        .if byte ptr [rax] == '*' 
            inc r13
            jmp rpt1
        .endif 
    .endif 

    ;  Make sure that the characters after '*' are present 
    ;  in second string. This function assumes that the 
    ;  first string will not contain two consecutive '*' 

    mov rax, r13
    add rax, 1
    .if byte ptr [r13] == '*' && byte ptr [rax] {} 0 && byte ptr [r14] == 0
        mov eax, 0
        jmp quit
    .endif       

    ;  If the first string contains '?', or current 
    ;  characters of both strings match 

    mov al, byte ptr [r14]
    .if byte ptr [r13] == '?' || byte ptr [r13] == al
        mov rcx, r13
        add rcx, 1
        mov rdx, r14
        add rdx, 1 
        call match
        jmp quit
    .endif 

    ;  If there is *, then there are two possibilities 
    ;  a) We consider current character of second string 
    ;  b) We ignore current character of second string. 

    .if byte ptr [r13] == '*'
        mov rcx, r13
        add rcx, 1
        mov rdx, r14
        call match
        .if rax == 1
            jmp quit
        .endif
        mov rcx, r13
        mov rdx, r14
        add rdx, 1 
        call match
        jmp quit
    .endif

    xor eax, eax
quit:
    RestoreRegs
    ret
match endp 

