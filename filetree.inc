; ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
    file_core macro

        .if wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
            chdir ADDR wfd.cFileName
            .if rax == 0
                mov pbuf, ptr$(buffer)
              IF TRACKING_CRASH 
                ;lea rax, wfd.cFileName
                ;conout "dir = ", rax, lf
              ENDIF
                    invoke MessageBox,0, "Fail to set directory", addr caption,MB_OK
                jmp close_file
            .endif
            invoke file_tree,lpcb,upatn,uexcl,flag    ; recurse to next directory level
        .endif

          IF TRACKING_CRASH 
            lea rax, wfd.cFileName
            conout rax, lf
          ENDIF

        invoke szLen, addr wfd.cFileName
        .if rax { MAX_PATH
            invoke lpcb, addr wfd.cFileName, addr wfd, upatn, uexcl

        .else
          IF TRACKING_CRASH 
            conout "to deep",lf
          ENDIF
            invoke MessageBox,0, "Path to deep to open filey", addr caption,MB_OK
        .endif
    endm

file_tree proc lpcb:QWORD, upatn:QWORD, uexcl:QWORD, flag:QWORD
    LOCAL hSrch :QWORD
    LOCAL wfd   :WIN32_FIND_DATA
    LOCAL pbuf  :QWORD
    LOCAL buffer[1024]:BYTE
    
    add flag, 1
    mov hSrch, rv(FindFirstFile,"*.*",ADDR wfd)
    .if hSrch {} INVALID_HANDLE_VALUE
        invoke szCmp, addr wfd.cFileName, "."       ; bypass current directory character
        test rax, rax
        jnz @F

        file_core
      
    @@:
        test rv(FindNextFile,hSrch,ADDR wfd), rax
        jz close_file
        invoke szCmp, addr wfd.cFileName,".."       ; bypass previous directory characters
        test eax, eax
        jnz @F

        file_core

    @@:                                             ; loop through the rest
        test rv(FindNextFile,hSrch,ADDR wfd), rax
        jz close_file

        file_core    

        jmp @B

    close_file:
        invoke FindClose,hSrch
    .endif

    .if flag } 0                            ; flag controlled tail recursion
        chdir ".."                          ; drop back to next lower directory
    .endif

    ret

file_tree endp

; ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл

cb_file_tree proc pfilename:QWORD, pwfd:QWORD, upatn:QWORD, uexcl:QWORD

    LOCAL pbuf  : QWORD
    LOCAL pdir  : QWORD
    LOCAL pfile : QWORD
    LOCAL buffer1[1024]:BYTE
    LOCAL buffer2[1024]:BYTE
    LOCAL buffer3[1024]:BYTE


          IF TRACKING_CRASH 
            conout "cb_file  -  " , pfilename, lf
          ENDIF


  ; ----------------------
  ; don't list a directory
  ; ----------------------
    cmp rv(GetFileAttributes,pfilename), FILE_ATTRIBUTE_DIRECTORY
    jne @F
    ret
  @@:

    mov pbuf,  ptr$(buffer1)
    mov pdir,  ptr$(buffer2)
    mov pfile, ptr$(buffer3)

    mov pfilename, lcase$(pfilename)
    .if point1 } 0
        invoke szLen, pfilename
        mov rcx, rax
        mov rdi, pfilename
        mov al, "."
        repne scasb
        je jump1
        mcat pfilename, pfilename, "."
      jump1: 
    .endif
  ; --------------------------------
  ; user file extension is specified
  ; --------------------------------
    fn checkname, upatn, pfilename
    test rax, rax
    jz @F

    fn checkname, uexcl, pfilename
    test rax, rax
    jnz @F

    .if maxMb } 0
        mov rax, maxMb
        mov rbx, pwfd
        .if [rbx].WIN32_FIND_DATA.nFileSizeLow } eax || [rbx].WIN32_FIND_DATA.nFileSizeHigh } 0
            jmp @F
	 .endif
    .endif

    inc filecandt 
    invoke SendMessage, hStatus, SB_SETTEXT, 0, str$(filecandt)

    invoke GetCurrentDirectory, 1024, pdir
    mcat pfile, pdir, "\", pfilename

    mov rax, 0
    .if criterios } 0
        invoke Boyer_Moore_Search, pfile
    .endif
    .if rax == 0
        invoke SendMessage, hlist, LB_ADDSTRING, 0, pfile  ; display the file name
        inc filecount 
    .endif

  @@:
    ret

cb_file_tree endp

; ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл

get_pattern proc src:QWORD
    USING rsi

    SaveRegs

    mov rsi, src

  stlp:
    cmp BYTE PTR [rsi], "."
    jne @F
    mov rdx, rsi                ; store period position in RDX
  @@:
    cmp BYTE PTR [rsi], 0
    je nxt
    add rsi, 1
    jmp stlp

  nxt:
    test rdx, rdx
    jz quit
    add rdx, 1                  ; step past period
    mov rsi, src
    mov rcx, -1

  cpy:
    add rcx, 1
    mov al, [rdx+rcx]           ; write bare pattern back to SRC
    mov [rsi+rcx], al
    test al, al
    jnz cpy

    mov rax, src
  quit:

    RestoreRegs

    ret

get_pattern endp
