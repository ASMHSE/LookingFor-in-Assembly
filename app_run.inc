TheItem struct
    thenext     dq  0
    thestring   dq  0    
TheItem ends

.data
    TheExclusion dq 0
    TheInclusion dq 0
    point1       dq 0

.code

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

chargep proc pchain:QWORD, localbuf:QWORD 
    ANNOTATION prv: rbx rdi r12 ; in SelectionMount
    mov byte ptr[rdi], 0
    mov r12, alloc(sizeof TheItem)
    mov rbx, alloc(len(localbuf))
    cst rbx, localbuf
    mov [r12].TheItem.thestring, rbx
    mov rcx, pchain
    mov rax, [rcx]
    mov [r12].TheItem.thenext, rax
    mov [rcx], r12
    dec rdi
    cmp byte ptr[rdi], "."
    jne local1
    mov point1, 1
  local1:
    ret
chargep endp


SelectionMount proc pchain:QWORD, cadena:QWORD
    USING rbx,rsi,rdi,r12
    LOCAL extension[128]:byte
    ANNOTATION use: rbx r12 ; in chargep
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
    .if rdi } 0
        invoke chargep, pchain, addr extension
    .endif

    RestoreRegs
    ret
        
SelectionMount endp


; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

app_run proc
    LOCAL pbuf  :QWORD, pbuf4 :QWORD
    LOCAL pcmd  :QWORD, lgral1:REAL8

    LOCAL buffer0[260]:BYTE
    LOCAL buffer1[260]:BYTE
    LOCAL buffer2[260]:BYTE
    LOCAL buffer3[260]:BYTE
    LOCAL buffer4[248]:BYTE

    LOCAL searchcall[260]:BYTE
    LOCAL searchpath[260]:BYTE
    LOCAL searchname[260]:BYTE

    invoke SendMessage,hStatus,SB_SETTEXT,0,"loading parameters..."

    mov point1, 0
    
    invoke SendMessage, hlist, LB_RESETCONTENT, 0, 0

    invoke SendMessage, hedit1, WM_GETTEXT, 260, addr buffer0 
    invoke GetPathOnly, addr buffer0, addr searchcall
    lea rcx, searchcall
    mov al, [rcx]
    .if al == 0
        mcat addr buffer3, "Path target problem"
        invoke MessageBox,0, addr buffer3, addr caption,MB_OK
        invoke SendMessage, hStatus, SB_SETTEXT, 0, addr sb_msg_1
        jmp app_out
        ret
    IF TRACKING_CRASH 
    .else
        conout "searchcall = ", addr searchcall, lf
    ENDIF
    .endif    

    invoke NameFromPath, addr buffer0, addr searchname
    lea rcx, searchname
    mov al, [rcx]
    .if al == 0
        mcat addr buffer3, "Name or extension target problem:", \
            lf, lf, tab, "Target Path is:",lf, tab, addr searchcall, \
            lf, lf, tab, "Target Mask is empty"
        invoke MessageBox,0,addr buffer3, addr caption,MB_OK
        invoke SendMessage, hStatus, SB_SETTEXT, 0, "Failed!"
        jmp app_out
        ret
    .endif    
    invoke SelectionMount, addr TheInclusion, addr searchname     ; ????

    mov criterios, 0    
    invoke SendMessage, hedit2, WM_GETTEXT, 259, addr buffer1
    invoke szLen, addr buffer1
    .if rax } 0 
        mov criterios, 1
        mov dsize1, eax
        mov psearch1, ptr$(buffer1)
    .endif

    invoke SendMessage, hedit3, WM_GETTEXT, 259, addr buffer2 
    .if rax } 0
        .if criterios } 0 
            mov criterios, 2
            mov dsize2, eax
            mov psearch2, ptr$(buffer2)
        .else
            mov criterios, 1
            mov dsize1, eax
            mov psearch1, ptr$(buffer2)
        .endif
    .endif

    invoke SendMessage, hcb1, CB_GETCURSEL, 0, 0
    mov string1case, rax
    invoke SendMessage, hcb3, CB_GETCURSEL, 0, 0
    mov string2case, rax
    invoke SendMessage, hcb2, CB_GETCURSEL, 0, 0
    mov string1word, rax
    invoke SendMessage, hcb4, CB_GETCURSEL, 0, 0
    mov string2word, rax

    .if criterios } 0
        .if string1case == 0
            q2q Boyer_Moore1, Boyer_MooreSens
            q2q Boyer_Moore_Table, Boyer_Moore_TableSens
        .elseif string1case == 1
            q2q Boyer_Moore1, Boyer_MooreIns
            invoke szUpper, psearch1
            q2q Boyer_Moore_Table, Boyer_Moore_TableIns
        .else
            q2q Boyer_Moore1,Boyer_MooreIgn
            q2q Boyer_Moore_Table, Boyer_Moore_TableIgn
        .endif
        invoke Boyer_Moore_Table, addr bad_chars1, psearch1, dsize1

        .if criterios == 2
            .if string2case == 0
                q2q Boyer_Moore2, Boyer_MooreSens
                q2q Boyer_Moore_Table, Boyer_Moore_TableSens
            .elseif string2case == 1
                q2q Boyer_Moore2,Boyer_MooreIns
                invoke szUpper, psearch2
                q2q Boyer_Moore_Table, Boyer_Moore_TableIns
            .else
                q2q Boyer_Moore2,Boyer_MooreIgn
                q2q Boyer_Moore_Table, Boyer_Moore_TableIgn
            .endif
            invoke Boyer_Moore_Table, addr bad_chars2, psearch2, dsize2
 
        .endif
    .endif

    ;-------------------------------
    ;   Exclusion mount
    ;-------------------------------
    invoke SendMessage, hedit4, WM_GETTEXT, 128, addr buffer3 
 
    invoke SelectionMount, addr TheExclusion, addr buffer3

    ;-------------------------------
    ;   Distance mount
    ;-------------------------------
    invoke SendMessage, hedit5, WM_GETTEXT, 128, addr buffer4 
    invoke vc_sscanf, ADDR buffer4,"%lu", addr maxDist
    
    ;-------------------------------
    ;   Size mount
    ;-------------------------------
    invoke SendMessage, hedit6, WM_GETTEXT, 128, addr buffer4 
    invoke vc_sscanf, ADDR buffer4,"%lf", addr lgral1
    fld  lgral1
    fild megabyte
    fmul
    fistp maxMb

    lea rcx, searchcall
    mov al, [rcx+1]
    .if al == ":"
        invoke SetCurrentDirectory, addr searchcall
        mov rcx, 0
    .else
        invoke GetCurrentDirectory, 259, addr searchpath
        invoke szappend, addr searchpath, addr searchcall, 2
        invoke SetCurrentDirectory, addr searchpath
        mov rcx, 1
    .endif
    .if rax == 0
        .if rcx == 0
            mcat addr buffer3, "Fail to set Target Path:",lf,lf, tab, addr searchcall
        .else
            mcat addr buffer3, "Fail to set Target Path:",lf,lf, tab, addr searchpath
        .endif
        invoke MessageBox, 0, addr buffer3, addr caption,MB_OK
        invoke SendMessage,hStatus,SB_SETTEXT,0,"looking for..."
        jmp app_out
    .endif

    invoke SendMessage,hStatus,SB_SETTEXT,0,"looking for..."
     
    mov filecount, 0
    mov filecandt, 0
    fn file_tree, addr cb_file_tree, TheInclusion, TheExclusion, -1

    mov pbuf, ptr$(buffer4)
    mov buffer4, 0
    mcat pbuf, str$(filecount), " files match current definitions from ",str$(filecandt)," candidates"

    invoke SendMessage, hStatus, SB_SETTEXT, 0, pbuf

    invoke CleanItems, addr TheInclusion
    invoke CleanItems, addr TheExclusion
    .if filecount } 0
        invoke SendMessage, hlist, LB_SETCURSEL, 0, 0
        invoke SetFocus, hlist
    .endif

app_out:
    invoke SendMessage, hbutton1, WM_SETTEXT, 0,addr but1_a
    mov MasterLock, 0
    ret
app_run endp

