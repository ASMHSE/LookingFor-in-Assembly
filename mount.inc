TheItem struct
    thenext     dq  0
    thestring   dq  0    
TheItem ends

.data
    TheExclusion dq 0
    TheInclusion dq 0

.code

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

chargep proc pchain:QWORD, localbuf:QWORD 
    mov byte ptr[rdi], 0
    mov r12, alloc(sizeof TheItem)
    mov rbx, alloc(len(localbuf))
    cst rbx, localbuf
    mov [r12].TheItem.thestring, rbx
    mov rcx, pchain
    mov rax, [rcx]
    mov [r12].TheItem.thenext, rax
    mov [rcx], r12
    ret
chargep endp

SelectionMount proc pchain:QWORD, cadena:QWORD
    USING rbx,rsi,rdi,r12
    LOCAL extension[128]:byte
    SaveRegs

    mov useExcl, 0
    mov rsi, cadena
    lea rdi, extension
    mov rax, 0
    mov ecx, 0

  loop1i:
    mov cl, [rsi]
    .if cl == 0
        jmp loop1f 
    .endif
    .if cl == "|"
        .if rdi } 0
            invoke chargep, pchain, addr extension
            mov ecx, 0
            lea rdi, extension
        .endif
    .elseif cl {} " "
        mov byte ptr[rdi], cl
        add rdi, 1
    .endif
    add rsi, 1
    jmp loop1i

  loop1f:

    lea rax, extension
    add rax, 1
    .if rax {} rdi
        invoke chargep, pchain, addr extension
    .endif

    RestoreRegs
    ret
        
SelectionMount endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい
