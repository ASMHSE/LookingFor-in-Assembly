TheTarget struct
    thenext     dq  0
    thestring   dq  0    
    thecreation dq  0
    theuse      dq  0
    thelast     dq  0 
TheTarget ends

.data

    align 8
      chartable \
        db  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        db  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0      ;  31
        db  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15
        db 34,35,36,37,38,39,40,41,42,43,16,17,18,19,20,21      ;  63
        db 22,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58
        db 59,60,61,62,63,64,65,66,67,68,69,23,24,25,26,27      ;  95
        db 28,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58
        db 59,60,61,62,63,64,65,66,67,68,69,29,30,31,32,33      ; 127

       firstcreation dq 0
       lastcreation  dq 0

.code

CleanItems proc ; pbase:qword
    USING rbx, r12
    SaveRegs

    mov rbx, [rcx]
    mov qword ptr [rcx], 0

  loop1i:
        cmp rbx, 0
        je loop1f
        mov r12, [rbx].TheItem.thenext
        mov rcx, [rbx].TheItem.thestring
        mfree rcx
        mfree rbx
        mov rbx, r12
        jmp loop1i
  loop1f:
    RestoreRegs
    ret
CleanItems endp

RemoveTarget proc target:qword
    USING rbx, r12
    SaveRegs

    mov rbx, TheTargets
  loop1i:
        cmp rbx, 0
        je loop1f
        mov rcx, [rbx].TheTarget.thestring
        invoke szCmp, rcx, target
        cmp rax, 0
        jg run
        mov r12, rbx
        mov rbx, [rbx].TheTarget.thenext
        jmp loop1i 

    jmp loop1f

  run:
    mov rax, TheTargets
    .if rbx == rax
        mov rax,[rbx].TheTarget.thenext
        mov TheTargets, rax
    .else
        mov rax,[rbx].TheTarget.thenext
        mov [r12].TheTarget.thenext, rax
    .endif
    mov rcx, [rbx].TheTarget.thestring
    mfree rcx
    mfree rbx
  
  loop1f:

  quit:
    RestoreRegs
    invoke SendMessage, hWnd, WM_COMMAND, 217, 0
    ret
RemoveTarget endp

RetrieveTarget proc target:qword
    USING rbx, rdi, r12
    ANNOTATION prv: r13 ; in RetrieveTargets
    local creation : SQWORD
    local use : QWORD 

    SaveRegs

    mov rdi, target
    mov rcx, len(target)
    mov al, ";"
    REPNE SCASB
    cmp rcx, 0
    jz @initialize
    mov byte ptr [rdi-1], 0
    mov rbx, rdi
    mov rcx, len(rdi)
    mov al, ";"
    REPNE SCASB
    mov byte ptr [rdi-1], 0

    mov creation, uval(rbx)
    .if rax } lastcreation
        mov lastcreation, rax 
    .endif
    mov use, uval(rdi)
    
    jmp @charge

@initialize:

    mov use, 0
    dec firstcreation
    mov rax, firstcreation
    mov creation, rax
 
@charge:
    mov rbx, TheTargets
    .if rbx {} 0
      loop1:

        mov r12, [rbx].TheTarget.thenext
        .if r12 {} 0
            mov rbx, r12
            jmp loop1
        .endif

        mov r12, alloc(sizeof TheTarget)
        mov r13, alloc(len(target))
        cst r13, target
        mov [r12].TheTarget.thestring, r13

        mov [rbx].TheTarget.thenext, r12

    .else
        
        mov r12, alloc(sizeof TheTarget)
        mov r13, alloc(len(target))
        cst r13, target
        mov [r12].TheTarget.thestring, r13
        mov [r12].TheTarget.thenext, 0
        mov TheTargets, r12

    .endif
    mov rax, creation 
    mov [r12].TheTarget.thecreation, rax
    mov rax, use
    mov [r12].TheTarget.theuse, rax


quit:
    RestoreRegs
   ret
RetrieveTarget endp

       
StoreTarget proc target:qword
    USING rbx, r12
    SaveRegs

    mov rbx, TheTargets
    .if rbx {} 0
      loop1:
        mov rcx, [rbx].TheTarget.thestring
        invoke szCmp, rcx, target
        cmp rax, 0
        jne quit
        mov rbx, [rbx].TheTarget.thenext
        cmp rbx, 0
        jne loop1

        mov r12, alloc(sizeof TheTarget)
        
        mov rcx, TheTargets
        mov [r12].TheTarget.thenext, rcx
    .else

        mov r12, alloc(sizeof TheTarget)
        mov [r12].TheTarget.thenext, 0
    .endif
    mov rbx, alloc(len(target))
    cst rbx, target
    mov [r12].TheTarget.thestring, rbx
    inc lastcreation
    mov rax, lastcreation
    mov [r12].TheTarget.thecreation, rax
    mov [r12].TheTarget.theuse, 0
    mov TheTargets, r12
     
    invoke SendMessage,hedit1,CB_INSERTSTRING,0, target

quit:
    RestoreRegs

    mov eax, 1
    ret
StoreTarget endp

szCheckTarget proc target:QWORD

    mov rdx, target
    mov rax, 0
  loop1i:
    mov cl, [rdx]
    .if cl == 0
        jmp loop1f 
    .endif
    .if cl {} " " && cl {} "."
        add rax, 1
    .endif
    add rdx, 1
    jmp loop1i
  loop1f:

    ret
szCheckTarget endp

checkname proc pchain:QWORD, pname:QWORD
    USING rbx, r12, r13
    SaveRegs

    mov r13, 0
    mov rbx, pchain
  loop1i:
        cmp rbx, 0
        je loop1f
        mov r12, [rbx].TheItem.thenext
        mov rcx, [rbx].TheItem.thestring
        invoke match, rcx, pname
        add r13, rax
        mov rbx, r12
        jmp loop1i
  loop1f:
    mov rax, r13
    RestoreRegs
    ret
checkname endp

findtarget proc pchain:QWORD, pname:QWORD
    USING rbx, r12
    SaveRegs

    mov rbx, pchain
  loop1i:
        cmp rbx, 0
        je loop1f
        mov r12, [rbx].TheItem.thenext
        mov rcx, [rbx].TheItem.thestring
        invoke szCmp, rcx, pname
        test rax, rax
        jnz load 
        mov rbx, r12
        jmp loop1i
  loop1f:
    mov rax, 0
    jmp sale
  load:
    mov rax, rbx
  sale:
    RestoreRegs
    ret
findtarget endp



ShowItems proc items:qword
    USING rbx, r12
    SaveRegs
    conout "+ShowItems",lf

    mov rbx, items
  loop1i:
        cmp rbx, 0
        je loop1f
        mov r12, [rbx].TheItem.thenext
        mov rax, [rbx].TheItem.thestring
        conout rax, lf
        mov rbx, r12
        jmp loop1i
  loop1f:

    conout "-ShowItems",lf

    RestoreRegs
    ret
ShowItems endp

StrCompTableA proc; pStr1:POINTER, pStr2:POINTER, ptable:POINTER
    xor r9, r9
    xor r10, r10
    xor r11, r11
    xor eax, eax
  @ini:
    mov al, [rcx + r10]                                 ;Load char to compare
    cmp al, 0                                           ;Test if end of string
    jz @equal?
    cmp al, 127
    jg @1d
    mov r9b,[r8 + rax]
    jmp @1
  @1d:
    mov r9b, al
  @1:
    mov al, [rdx + r10]                                 ;Compare with the char of the other string
    cmp al, 127
    jg @1d
    mov r11b,[r8 + rax]
    jmp @2
  @2d:
    mov r11b, al
  @2:
    sub r9, r11                                         ;Subtract to see which is smaller
    jnz @dif                                            ;Chars are not equal, check if for capitals
    inc r10                                             ;Goto for next char
    jmp @ini
  @equal?:
    mov al, [rdx + r10]                                 ;Compare with the char of the other string
    mov r11b,[r8 + rax]
    sub r9, r11                                         ;Subtract to see which is smaller
  @dif:
    mov rax, r9
    ret
StrCompTableA endp


SortTargets proc kind:qword, order:qword
    USING rbx, r12, r13, r14, r15
    SaveRegs
 
    jmp sigue
  begin:
    dq offset sorting_0
    dq offset sorting_1
    dq offset sorting_2
  sigue:  
    shl rcx, 3
    lea r15, begin 
    add r15, rcx  
    mov rbx, TheTargets
    
    mov r13, rbx
  loop1i:
        cmp rbx, 0
        je loop1f

        mov r12, [rbx].TheTarget.thenext
        cmp r12, 0
        je loop1f

        mov r14, rbx
      loop2i:
        jmp qword ptr [r15]

       sorting_0:
            mov rax, [rbx].TheTarget.thecreation
            sub rax, [r12].TheTarget.thecreation
            jmp checksort
   
       sorting_1:
            mov rcx, [rbx].TheTarget.thestring
            mov rdx, [r12].TheTarget.thestring
            lea r8, chartable
            call StrCompTableA
            jmp checksort

       sorting_2:
            mov rax, [rbx].TheTarget.theuse
            sub rax, [r12].TheTarget.theuse
            jmp checksort
   
       checksort:
            cmp rax, 0
            jns checkasc 
            .if order == 0 
                jmp change
            .endif
            jmp nochange

        checkasc: 
            .if order == 1 
                jmp change
            .endif
            jmp nochange

        change:
            mov r8, rbx
            mov r9, r12

            mov rax, TheTargets
            .if rbx == rax
                mov TheTargets, r9
            .else
                mov [r13].TheTarget.thenext, r9
            .endif
            mov [r14].TheTarget.thenext, r8

            mov r10, [r9].TheTarget.thenext
            mov r11, [r8].TheTarget.thenext
            .if r9 == r11
                mov [r9].TheTarget.thenext, r8
            .else
                mov [r9].TheTarget.thenext, r11
            .endif
            mov [r8].TheTarget.thenext, r10

            mov rbx, r9
            mov r12, r8
         
         nochange:
            mov r14, r12
            mov r12, [r12].TheTarget.thenext
            cmp r12, 0
            je loop2f
            jmp loop2i 

      loop2f:

        mov r13, rbx   
        mov rbx, [rbx].TheTarget.thenext
        jmp loop1i 

  loop1f:

    RestoreRegs
    invoke SendMessage, hWnd, WM_COMMAND, 217, 0
    ret
SortTargets endp

