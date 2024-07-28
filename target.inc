TheTarget struct
    thenext     dq  0
    thestring   dq  0    
    theuse      dq  0
    thelast     dq  0 
TheTarget ends

.data
    TheTargets  dq 0

.code


CleanItems proc pbase:qword
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
    USING rbx, r12
    SaveRegs

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
        mov rbx, alloc(len(target))
        cst rbx, target
        mov [r12].TheTarget.thestring, rbx
        mov [r12].TheTarget.thenext, 0
        mov TheTargets, r12

    .endif
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
        mov rbx, alloc(len(target))
        cst rbx, target
        mov [r12].TheTarget.thestring, rbx
        
        mov rcx, TheTargets
        mov [r12].TheTarget.thenext, rcx
        mov TheTargets, r12

    .else

        mov r12, alloc(sizeof TheTarget)
        mov rbx, alloc(len(target))
        cst rbx, target
        mov [r12].TheTarget.thestring, rbx
        mov [r12].TheTarget.thenext, 0
        mov TheTargets, r12
    .endif
     
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

    ;conout "checkname  ", pchain,"  ",pname," " 

    mov r13, 0
    mov rbx, pchain
  loop1i:
        cmp rbx, 0
        je loop1f
        mov r12,  [rbx].TheItem.thestring
        mov r12, [rbx].TheItem.thenext
        mov rcx, [rbx].TheItem.thestring
        invoke match, rcx, pname
        add r13, rax
        mov rbx, r12
        jmp loop1i
  loop1f:
    ;conout str$(r13),lf
    mov rax, r13
    RestoreRegs
    ret
checkname endp

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

