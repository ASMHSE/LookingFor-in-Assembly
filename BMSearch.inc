.data?
    bad_chars1 dd 256 dup (?)
    bad_chars2 dd 256 dup (?)
.data
    ParseChars db  ' ',',','|','*','"','=','^','+','-',':','/','&','!','\','%',13,10,9,'(',')','{','}','[',']','<','>','.',"_" 

    $numParseChars EQU $ - ParseChars
.code

    check_full_core macro stringword, nosepm
        mov al, [r15+r12]
        lea rdi, ParseChars
        .if &stringword == 1
            mov rcx, $numParseChars-1
            REPNE SCASB
            je &nosepm
            jmp loop1
        .elseif &stringword == 2
            mov rcx, $numParseChars
            REPNE SCASB
            je &nosepm
            jmp loop1
    endm

    check_full macro stringword, dSize
        local nosepi
        local nosepf

        .if &stringword } 0
          ;  mov r13,rax
          ;  conout " check rcx = ", str$(rcx),lf
          ;  conout " check rax = ", str$(r13),lf
          ;  mov rax, r13
    
            mov r12, rax
    
            .if rax == 0
                jmp nosepi
            .endif
            xor eax, eax

            mov al, [r15+r12]
            .if al == 2Eh
                .if point_separator == 0
                    lea rdi, ParseChars
                    mov al, [r15+r12-1]
                    mov rcx, $numParseChars-10
                    REPNE SCASB
                    je nosepi
                    jmp loop1
                .else
                    jmp nosepi
                .endif
            .endif


            dec r12
            check_full_core &stringword, nosepi
           nosepi:
            .endif
    
            add r12d, &dSize
            add r12, 1
            .if r12 } r14
                jmp nosepf
            .endif 
            check_full_core &stringword, nosepf
           nosepf:
            .endif
        .endif
    endm

; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

Boyer_Moore_Search proc pfilename : QWORD

    USING rdi, r12, r14, r15
    ANNOTATION use: r12 rdi ; in check_full
    LOCAL plc            :QWORD		;cmd line pointer
    LOCAL saida          :QWORD		;output handle
    LOCAL arquivo        :QWORD		;file handle
    LOCAL arqmap         :QWORD
    LOCAL pos            :DWORD		;initial position first criterium
    LOCAL return_value   :DWORD		;return result in eax register
    LOCAL pos2           :DWORD		;initial position second criterium
    LOCAL return_value2  :DWORD		;return result in eax register
    LOCAL storedsize1    :DWORD
    	
    SaveRegs
    
    invoke GetStdHandle,STD_OUTPUT_HANDLE
    mov saida, rax
	
    invoke GetCommandLine
    mov plc, rax

    invoke CreateFile, pfilename,GENERIC_READ,0,0,\
                OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,0           ;open source file
    mov arquivo, rax								;handle
	
    mov return_value, 1

    invoke GetFileSize, arquivo, NULL                               ;sizeof source file
    cmp eax, 0
    jle quit
    .if eax { dsize1
        jmp quit
    .endif
    .if criterios == 2
        .if eax { dsize2
            jmp quit
        .endif
    .endif
    mov r14d, eax
    
    invoke CreateFileMapping, arquivo, NULL,PAGE_READONLY,0,0,0	;mapped file handle
    mov arqmap, rax
    invoke MapViewOfFile, rax, FILE_MAP_READ,0,0,0			;map it, file contents pointer
    mov r15, rax
    invoke CloseHandle, arquivo						;don't need this anymore

    mov pos, 0

loop1:

    invoke Boyer_Moore1, addr bad_chars1, pos, psearch1, dsize1
    
    mov return_value, eax
    .if return_value == -1
        jmp done
    .elseif return_value == -2                  ; error in parameters data
        jmp done                    
    .else
           
        inc return_value
        mov r8d, return_value
        mov pos, r8d

        check_full string1word, dsize1

        .if criterios == 2
            
            .if maxDist } 0
                mov storedsize1, r14d
                mov eax, maxDist
                .if eax } r8d
                    mov pos2, 0
                .else
                    sub eax, r8d
                    cmp eax, 0
                    jnbe noesneg
                    mov eax, 0    
                  noesneg: 
                    mov pos2, eax
                .endif
                add eax, r8d
                add eax, dsize1
                add eax, dsize2
                cmp eax, r14d
                jnbe noespos
                sub eax, dsize2
                mov r14d, eax
              noespos:  
            .else
                mov pos2, 0
            .endif
        
            invoke Boyer_Moore2, addr bad_chars2, pos2, psearch2, dsize2
            
            mov return_value2, eax
            .if return_value2 == -1
                jmp done
            .elseif return_value2 == -2                  ; error in parameters data
                jmp done                    
            .else
                check_full string2word, dsize2
                mov return_value, 0
                jmp done
            .endif
            .if maxDist } 0
                mov r14d, storedsize1
            .endif

        .else
            mov return_value, 0
            jmp done
        .endif

        jmp loop1
    .endif

done:

    invoke UnmapViewOfFile, r15
    invoke CloseHandle, arqmap

quit:    

    RestoreRegs

    mov eax, return_value
    ret
Boyer_Moore_Search endp

; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

Boyer_Moore_TableSens proc lptable:qword, lpSubStr:qword, subLngth:dword

    USING rbx, rsi, rdi 

    SaveRegs

    mov rdi, lptable      ;create a lookup table filled with substring size
    mov ecx, 256
    mov eax, subLngth
    rep stosd

    mov rdi, lptable
    mov esi, subLngth
    mov ecx, subLngth
    add rsi, lpSubStr
    dec rsi                 ;point to last char of substring, zero index based

    ;-------------------------
    ;eg: string = "batata", a=1, t=1, b=5, any other chars will be = 6 (sizeof string)
    ;eg: string = "done", e=1, n=1, o=2, d=3, any other chars will be = 4
    ;-------------------------
    xor eax, eax
    mov al, byte ptr [rsi]	             ;read last char
    mov dword ptr [rdi+rax*4], 1        ;its size will be one displacement, insert in bad_chars table
    mov ebx, 1			      ;second char will be one too, third char will be 2, 
                                        ;      fourth char will be 3, ...
    dec rsi
    dec ecx

    mov edx, subLngth
    jmp while1c
  while1i:
    movzx eax, byte ptr [rsi]	              ;read that char
		
    .if dword ptr [rdi+rax*4] == edx		;actual char have a sizeof subLngth?
        mov dword ptr [rdi+rax*4], ebx		;yes, so overwrite this char displacement
                                                ; substring in lookup table
        inc ebx                                 ;increase displacement
                            			;<--fault if moved to below
    .endif
    dec rsi
    dec ecx
        
  while1c:
    cmp rcx, 0                   ;while exist chars in substring
    jnz while1i

    RestoreRegs
    ret
Boyer_Moore_TableSens endp

; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

Boyer_Moore_TableIns proc lptable:qword, lpSubStr:qword, subLngth:dword

    USING rbx, rsi, rdi

    SaveRegs

    mov rdi, lptable      ;create a lookup table filled with substring size
    mov ecx, 256
    mov eax, subLngth
    rep stosd

    mov rdi, lptable
    mov esi, subLngth
    mov ecx, subLngth
    add rsi, lpSubStr
    dec rsi                 ;point to last char of substring, zero index based

    ;-------------------------
    ;eg: string = "batata", a=1, t=1, b=5, any other chars will be = 6 (sizeof string)
    ;eg: string = "done", e=1, n=1, o=2, d=3, any other chars will be = 4
    ;-------------------------
    xor eax, eax
    mov al, byte ptr [rsi]	             ;read last char
    mov dword ptr [rdi+rax*4], 1        ;its size will be one displacement, insert in bad_chars table
    mov ebx, 1			      ;second char will be one too, third char will be 2, 
                                        ;      fourth char will be 3, ...
    .if rax } 64 && rax { 91
        add rax, 32
        mov dword ptr [rdi+rax*4], ebx
    .endif

    dec rsi
    dec rcx
    mov edx, subLngth
    jmp while1c
  while1i:
    movzx eax, byte ptr [rsi]	              ;read that char
		
    .if dword ptr [rdi+rax*4] == edx		;actual char have a sizeof subLngth?
        mov dword ptr [rdi+rax*4], ebx		;yes, so overwrite this char displacement
                                                ; substring in lookup table
        .if rax } 64 && rax { 91
            add rax, 32
            mov dword ptr [rdi+rax*4], ebx
        .endif
        inc ebx                                 ;increase displacement
                            			;<--fault if moved to below
    .endif
    dec rsi
    dec rcx
        
  while1c:
    cmp rcx, 0                   ;while exist chars in substring
    jnz while1i

    RestoreRegs
    ret
Boyer_Moore_TableIns endp

; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

Boyer_Moore_TableIgn proc lptable:qword, lpSubStr:qword, subLngth:dword

    USING rbx, rsi, rdi

    SaveRegs

    mov rdi, lptable      ;create a lookup table filled with substring size
    mov ecx, 256
    mov eax, subLngth
    rep stosd

    mov rdi, lptable
    mov esi, subLngth
    mov ecx, subLngth
    add rsi, lpSubStr
    dec rsi                 ;point to last char of substring, zero index based

    ;-------------------------
    ;eg: string = "batata", a=1, t=1, b=5, any other chars will be = 6 (sizeof string)
    ;eg: string = "done", e=1, n=1, o=2, d=3, any other chars will be = 4
    ;-------------------------
    xor eax, eax
    mov al, byte ptr [rsi]	             ;read last char
    mov dword ptr [rdi+rax*4], 1        ;its size will be one displacement, insert in bad_chars table
    mov ebx, 1			      ;second char will be one too, third char will be 2, 
                                        ;      fourth char will be 3, ...
    dec rsi
    dec rcx
    mov edx, subLngth
    jmp while1c
  while1i:
    movzx eax, byte ptr [rsi]	              ;read that char
		
    .if dword ptr [rdi+rax*4] == edx		;actual char have a sizeof subLngth?
        mov dword ptr [rdi+rax*4], ebx		;yes, so overwrite this char displacement
        inc ebx                                 ;increase displacement
                            			;<--fault if moved to below
    .endif
    dec rsi
    dec rcx
        
  while1c:
    cmp rcx, 0                   ;while exist chars in substring
    jnz while1i

    inc rsi
    inc rcx

    .if rax } 64 && rax { 91
            add rax, 32
        .if dword ptr [rdi+rax*4] == edx		;actual char have a sizeof subLngth?
            dec ebx
            mov dword ptr [rdi+rax*4], ebx		;yes, so overwrite this char displacement
        .endif
    .elseif rax } 96 && rax { 123
            sub rax, 32
        .if dword ptr [rdi+rax*4] == edx		;actual char have a sizeof subLngth?
            dec ebx
            mov dword ptr [rdi+rax*4], ebx		;yes, so overwrite this char displacement
        .endif
    .endif

    RestoreRegs
    ret
Boyer_Moore_TableIgn endp

; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

Boyer_MooreSens proc lptable:qword, startpos:dword, lpSubStr:qword, subLngth:dword

    USING rbx, rsi, rdi 
    ANNOTATION prv: r14 r15 ; in Boyer_Moore_Search

    SaveRegs
    
    mov eax, -2                 ;assume error in parameters data
    mov rsi, lpSubStr
    mov rdi, r15
    mov ebx, r14d
    mov ecx, subLngth
	
    test ebx,ebx
    jz done
    test ecx,ecx
    jz done
    cmp ecx,ebx
    ja done

    add rbx, rdi                ;ebx=lpSource+srcLngth
    mov r8d, startpos           ;edi=lpSource+startpos        
    add rdi, r8

    mov edx, subLngth
align 8
try_again:

    mov eax, -1
    mov rcx, rdi
    mov r11d, edx
    add r11, rdi
    cmp rbx, r11;rdi
    jbe done
    mov ecx, edx
@@:
    xor eax, eax 
    mov al, byte ptr [rdi+rcx-1]		;read one char from source
    cmp byte ptr [rsi+rcx-1], al
    jne @F
    dec ecx
    jnz @B
    sub rdi, r15
    mov rax, rdi
    jmp done
@@:
    mov r10, lptable        
    mov eax, dword ptr [r10+rax*4]
    cmp eax,edx			;test eax,edx		;<-first fault
    cmove eax,ecx
    add rdi, rax
    jmp try_again

align 8
done:
    RestoreRegs        
    ret

Boyer_MooreSens endp


; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

Boyer_MooreIns proc lptable:qword, startpos:dword, lpSubStr:qword, subLngth:dword

    USING rbx, rsi, rdi 
    ANNOTATION prv: r14 r15 ; in Boyer_Moore_Search

    SaveRegs

    mov eax, -2                 ;assume error in parameters data
    mov rsi, lpSubStr
    mov rdi, r15
    mov ebx, r14d
    mov ecx, subLngth
	
    test ebx,ebx
    jz done
    test ecx,ecx
    jz done
    cmp ecx,ebx
    ja done

    add rbx, rdi                ;ebx=lpSource+srcLngth
    mov r8d, startpos           ;edi=lpSource+startpos        
    add rdi, r8

    mov edx, subLngth
align 8
try_again:
    mov eax, -1
    mov r11d, edx
    add r11, rdi
    cmp rbx, r11;rdi
    jbe done
    mov ecx, edx
@1:
    xor eax, eax 
    mov al, byte ptr [rdi+rcx-1]		;read one char from source
    cmp byte ptr [rsi+rcx-1], al
    je @2
    .if rax } 96 && rax { 123
        sub rax, 32
    .endif
    cmp byte ptr [rsi+rcx-1], al
    jne @3
@2:
    dec ecx
    jnz @1
    sub rdi, r15
    mov rax, rdi
    jmp done
@3:
    mov r10, lptable        
    mov eax, dword ptr [r10+rax*4]
    cmp eax, edx			;test eax,edx		;<-first fault
    cmove eax, ecx
    add rdi, rax
    jmp try_again

align 8
done:
    RestoreRegs        
    ret

Boyer_MooreIns endp

; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

Boyer_MooreIgn proc lptable:qword, startpos:dword, lpSubStr:qword, subLngth:dword

    USING rbx, rsi, rdi 
    ANNOTATION prv: r14 r15 ; in Boyer_Moore_Search

    SaveRegs
    
    mov eax, -2                 ;assume error in parameters data
    mov rsi, lpSubStr
    mov rdi, r15
    mov ebx, r14d
    mov ecx, subLngth
	
    test ebx,ebx
    jz done
    test ecx,ecx
    jz done
    cmp ecx,ebx
    ja done

    add rbx, rdi                ;ebx=lpSource+srcLngth
    mov r8d, startpos           ;edi=lpSource+startpos        
    add rdi, r8

    mov edx, subLngth
align 8
try_again:
    mov eax, -1
    mov r11d, edx
    add r11, rdi
    cmp rbx, r11;rdi
    jbe done
    mov ecx, edx
@1:
    xor eax, eax 
    mov al, byte ptr [rdi+rcx-1]		;read one char from source
    cmp byte ptr [rsi+rcx-1], al
    je @2
    .if rcx == 1
        .if rax } 96 && rax { 123
            sub rax, 32
        .elseif rax } 64 && rax { 91
            add rax, 32
        .endif
        cmp byte ptr [rsi+rcx-1], al
        je @2
    .endif
    jmp @3
@2:
    dec ecx
    jnz @1
    sub rdi, r15
    mov rax, rdi
    jmp done
@3:
    mov r10, lptable        
    mov eax, dword ptr [r10+rax*4]
    cmp eax, edx			;test eax,edx		;<-first fault
    cmove eax, ecx
    add rdi, rax
    jmp try_again

align 8
done:
    RestoreRegs        
    ret

Boyer_MooreIgn endp

