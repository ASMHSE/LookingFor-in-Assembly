szText macro name, value
    .data
        &name db &value, 0
    .code
endm
szText$ macro name,value
    .data
        &name db &value, 0
    .code
    exitm <offset &name>
endm

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

ListProc proc hWin:QWORD,uMsg:QWORD,wParam:QWORD,lParam:QWORD
    USING r13
  ; -----------------------------------
  ; subclass procedure for the list box
  ; -----------------------------------
    LOCAL indx        :QWORD                                ; list box index variable
    LOCAL pbuf        :QWORD                                ; buffer pointer
    LOCAL buffer0[248]:BYTE                                 ; allocate space for text
    LOCAL buffer1[248]:BYTE                                 ; allocate space for command

    .switch uMsg
        .case WM_KEYUP
            ; -----------------
            ; single press keys
            ; -----------------
            .switch wParam
                .case VK_RETURN
                    jmp beginrun
            .endsw

        .case WM_LBUTTONDBLCLK
          beginrun:
            mov EscapeLock, 1
            mov pbuf, ptr$(buffer0)                             ; get buffer address
            mov indx, rv(SendMessage,hWin,LB_GETCURSEL,0,0)     ; get index of current selection
            invoke SendMessage, hWin, LB_GETTEXT, indx, pbuf    ; load selected text into "pbuf"
            mov indx, ptr$(buffer1)
            cst indx, pbuf 
            mov indx, rv(get_pattern, indx)
            fn szCmp, indx, "exe"
            test eax, eax
            jz @F
            invoke MessageBox, hWin, "Binaries will not run from here", addr caption, MB_OK
            ;invoke MsgboxI, hWin, "Binaries will not run from here", "LookingFor:", MB_OK, 10
            .return 0
@@:
            fn szCmp, indx, "bat"
            
            test eax, eax
            jz @F
            mov prevented, 1
            invoke ShellExecute, hWin, 0, addr EditorBat, pbuf, 0, SW_RESTORE
            jmp result
@@:
            fn szCmp, indx, "cmd"
            test eax, eax
            jz @F
            mov prevented, 2
            invoke ShellExecute, hWin, 0, addr EditorCmd, pbuf, 0, SW_RESTORE
            jmp result
@@:
            fn szCmp, indx, "js"
            test eax, eax
            jz @F
            mov prevented, 3
            invoke ShellExecute, hWin, 0, addr EditorJs, pbuf, 0, SW_RESTORE
            jmp result
@@:
            mov prevented, 0

            invoke ShellExecute, hWin, "open", pbuf, 0, 0, SW_RESTORE
result:
            .if eax {= 32
                .if eax == SE_ERR_NOASSOC	
                    lea rdx, err1
                .elseif eax == ERROR_FILE_NOT_FOUND
                    .if prevented == 0
                        lea rdx, err2
                    .else
                        if 0
                        SaveRegs
                        lea r13, buffer0	
                        mcat r13, addr err3, addr default_editor
                        mov rdx, r13
                        RestoreRegs
                        endif
                        
                        invoke DialogBoxParam, hInstance, 1000, hWin, ADDR AssocDlg, 0
                        .return 0
                        .endif
                .else
                    lea rdx, err0
                .endif
        	  invoke  MessageBox, hWin, rdx, addr caption, MB_OK
            .endif
            .return 0
    .endsw

    invoke CallWindowProc,lpListProc,hWin,uMsg,wParam,lParam

    ret

ListProc endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

IntProc proc hWin:QWORD,uMsg:QWORD,wParam:QWORD,lParam:QWORD

    .if uMsg == WM_CHAR
        .if wParam == 8                   ; backspace
           jmp accept
        .endif
    
        .if wParam { "0"
           .return 0
        .endif
    
        .if wParam } "9"
           .return 0
        .endif
    .endif

accept:
    
    invoke CallWindowProc,lpfnIntProc,hWin,uMsg,wParam,lParam

    ret

IntProc endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

DecProc proc hWin:QWORD,uMsg:QWORD,wParam:QWORD,lParam:QWORD
     LOCAL cBuffer[32]:BYTE
     USING rsi
                                          ; With DednDave and Biterider help
                                          ; https://masm32.com/board/index.php?topic=4453.0

    .if uMsg == WM_CHAR

        .if wParam == 8                     ; backspace
           jmp accept
        .endif

        .if wParam == "."                   ; only allow one decimal point
            SaveRegs
            invoke CallWindowProc, lpfnDecProc, hWin, WM_GETTEXT, lengthof cBuffer, addr cBuffer
   
            mov ecx, lengthof cBuffer       ; byte count in rcx
		   
            lea rsi, cBuffer                ; address in rsi
          @xxx:
                lodsb                       ; load byte into al
                cmp al, "."                 ; if decimal point already in Buffer
                jne @xx1
                .return 0                   ; throw it away
          @xx1:
                dec rcx
                cmp rcx, 0
                jne @xxx
            RestoreRegs
            .if eax } 0 
                .return 0
            .endif
            jmp accept
        .endif

        .if wParam { "0"
           .return 0
        .endif

        .if wParam } "9"
           .return 0
        .endif
    .endif

accept:
    
    invoke CallWindowProc,lpfnDecProc,hWin,uMsg,wParam,lParam

    ret

DecProc endp


; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

list proc instance:QWORD,parent:QWORD,topx:QWORD, \
                 topy:QWORD,wid:QWORD,hgt:QWORD,idnum:QWORD

    LOCAL hklist :QWORD

    invoke CreateWindowEx,WS_EX_LEFT,"LISTBOX",0, \
                  WS_CHILD or WS_VISIBLE or WS_EX_RIGHTSCROLLBAR or WS_VSCROLL or \
                  LBS_HASSTRINGS or LBS_NOTIFY or WS_TABSTOP or WS_BORDER or LBS_NOINTEGRALHEIGHT,\
                  topx,topy,wid,hgt,parent,idnum,instance,0
    mov hklist, rax
    invoke SendMessage, hklist, WM_SETFONT, rv(GetStockObject, ANSI_VAR_FONT), 0

    mov rax, hklist
    ret

list endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

target_combo proc instance:QWORD,parent:QWORD,topx:QWORD, \
                 topy:QWORD,wid:QWORD,hgt:QWORD,idnum:QWORD

    invoke CreateWindowEx,WS_EX_LEFT,"COMBOBOX",0, WS_CHILD or WS_VISIBLE or \
                          CBS_HASSTRINGS or WS_VSCROLL or CBS_DROPDOWN or \
                          CBS_NOINTEGRALHEIGHT  or WS_TABSTOP, \
                          topx,topy,wid,hgt,parent,idnum,instance,0

    ret

target_combo endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

select_combo proc instance:QWORD,parent:QWORD,topx:QWORD, \
                 topy:QWORD,wid:QWORD,hgt:QWORD,idnum:QWORD

    LOCAL hComb :QWORD

    invoke CreateWindowEx,WS_EX_LEFT,"COMBOBOX",0, WS_CHILD or WS_VISIBLE or \
                          CBS_HASSTRINGS or WS_VSCROLL or CBS_DROPDOWNLIST or \
                          CBS_NOINTEGRALHEIGHT or CBS_DISABLENOSCROLL, \
                          topx,topy,wid,hgt,parent,idnum,instance,0
    ret

select_combo endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

staticr proc instance:QWORD,hparent:QWORD,text:QWORD,topx:QWORD,topy:QWORD,wid:QWORD,hgt:QWORD

    invoke CreateWindowEx,WS_EX_RIGHT,"STATIC",text, \
                          WS_CHILD or WS_VISIBLE,\
                          topx,topy,wid,hgt,hparent,-1,instance,0
    ret

staticr endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

static proc instance:QWORD,hparent:QWORD,text:QWORD,topx:QWORD,topy:QWORD,wid:QWORD,hgt:QWORD

    invoke CreateWindowEx,WS_EX_LEFT,"STATIC",text, \
                          WS_CHILD or WS_VISIBLE,\
                          topx,topy,wid,hgt,hparent,-1,instance,0
    ret

static endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

buttonp proc instance:QWORD,hparent:QWORD,text:QWORD,topx:QWORD,topy:QWORD,wid:QWORD,hgt:QWORD,idnum:QWORD

    LOCAL hbtt :QWORD

    invoke CreateWindowEx,WS_EX_LEFT,"BUTTON",text, \
                          WS_CHILD or WS_VISIBLE or WS_TABSTOP,\
                          topx,topy,wid,hgt,hparent,idnum,instance,0
    
    ret

buttonp endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

editboxl proc hparent:QWORD,instance:QWORD,text:QWORD,topx:QWORD,topy:QWORD,wid:QWORD,hgt:QWORD,idnum:QWORD
    LOCAL hedit : QWORD

    invoke CreateWindowEx,WS_EX_LEFT,"EDIT",text, \
                          WS_CHILD or WS_VISIBLE or WS_BORDER or ES_LEFT or WS_TABSTOP  or ES_AUTOHSCROLL,\
                          topx,topy,wid,hgt,hparent,idnum,instance,0
    mov hedit, rax
    invoke SendMessage, hedit, EM_SETLIMITTEXT, 256, 0
    mov rax, hedit
    ret

editboxl endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

editboxc proc hparent:QWORD,instance:QWORD,text:QWORD,topx:QWORD,topy:QWORD,wid:QWORD,hgt:QWORD,idnum:QWORD

    invoke CreateWindowEx,WS_EX_LEFT,"EDIT",text, \
                          WS_CHILD or WS_VISIBLE or WS_BORDER or ES_CENTER or WS_TABSTOP,\
                          topx,topy,wid,hgt,hparent,idnum,instance,0
    ret

editboxc endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

m2m macro a1, a2
    mov rax, a2
    mov a1, rax
endm

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

RRegistroOption macro clave, opcion, defecto
    mov szBuff, 0
    invoke GetPrivateProfileString, addr clave, addr opcion, addr defecto,\
                 addr szBuff, sizeof szBuff, addr CurDir
endm

RRegSize macro clave, opcion, defecto, variable
    RRegistroOption clave, opcion, defecto
    invoke atou_ex, addr szBuff
    mov &variable, rax
endm

RRegEdit macro clave, opcion, defecto, edit
    RRegistroOption clave, opcion, defecto
    invoke SendMessage, edit,  WM_SETTEXT, 0, addr szBuff
endm

RRegMode macro clave, opcion, defecto, combo
    RRegistroOption clave, opcion, defecto
    invoke atou_ex, addr szBuff
    invoke SendMessage, combo, CB_SETCURSEL, rax, 0
endm

WRegistroOption macro clave, opcion
    invoke WritePrivateProfileString, addr clave, addr opcion, r14, r15
endm

WRegSize macro clave, opcion, dato
    invoke WritePrivateProfileString, addr clave, addr opcion, str$(dato), r15
endm

WRegEdit macro clave, opcion, edit
    invoke SendMessage, edit, WM_GETTEXT, 256, r14
    WRegistroOption clave, opcion
endm

WRegMode macro clave, opcion, combo
    invoke SendMessage, combo, CB_GETCURSEL, 0, 0
    WRegSize clave, opcion, rax
endm

RRegAssoc macro clave, opcion, defecto, namebuf
    mov namebuf, 0
    invoke GetPrivateProfileString, ADDR clave, addr opcion, addr defecto,\
                 addr namebuf, sizeof namebuf, addr CurDir
endm

WRegAssoc macro clave, opcion, namebuf
    invoke WritePrivateProfileString, addr clave, addr opcion, addr namebuf, addr CurDir
endm




fill_cb macro hcombo:req, sel1:req, args:VARARG
    count = 0
    FOR item, <args>
        invoke SendMessage, &hcombo, CB_ADDSTRING, count, <item>
        count = count + 1
    ENDM
    invoke SendMessage, &hcombo, CB_SETCURSEL, &sel1, 0
endm

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

SaveTargets proc
    USING rbx, r12, r13
    LOCAL opcion[8]:byte

    SaveRegs
    mov r12, 0

    mov rbx, TheTargets
  loop1i:
   
        mov opcion, 0
        invoke szappend, addr opcion, "tgt", 0
        mov r13, rax
        invoke szappend, addr opcion, str$(r12), r13
 
        .if rbx == 0
            mov r8, 0
        .else
            mov r8, [rbx].TheTarget.thestring
        .endif

        invoke WritePrivateProfileString, addr key3, addr opcion, r8, r15
        .if rbx == 0
            jmp loop1f
        .endif

        mov rbx, [rbx].TheTarget.thenext
        add r12, 1
        jmp loop1i 
  loop1f:
    ret
SaveTargets endp 

; --------------------------------------------------------------------------------------------------

RetrieveTargets proc

    USING rbx, r12, r13
    LOCAL opcion[8]:byte

    SaveRegs

    mov r12, 0
    mov TheTargets, 0
  loop1i:
        mov opcion, 0
        invoke szappend, addr opcion, "tgt", 0
        mov r13, rax
        invoke szappend, addr opcion, str$(r12), r13
        RRegistroOption key3, opcion, empty
        
        invoke szCheckTarget, addr szBuff
        .if rax == 0
            jmp loop1f
        .endif
        invoke RetrieveTarget, addr szBuff
        invoke SendMessage, hedit1, CB_INSERTSTRING, r12, addr szBuff
        add r12, 1
        jmp loop1i 
  loop1f:

    RestoreRegs
    ret
RetrieveTargets endp

; --------------------------------------------------------------------------------------------------

LoadTargets proc
    USING rbx
    SaveRegs
    mov rbx, TheTargets
  loop1i:
        .if rbx == 0
            jmp loop1f
        .endif
        mov r9, [rbx].TheTarget.thestring
        invoke SendMessage, hedit1, CB_ADDSTRING, 0, r9
        mov rbx, [rbx].TheTarget.thenext
        jmp loop1i 
  loop1f:
    RestoreRegs
    ret
LoadTargets endp 

; --------------------------------------------------------------------------------------------------

ShowTargets proc
    USING rbx
    SaveRegs
    mov rbx, TheTargets
  loop1i:
        .if rbx == 0
            jmp loop1f
        .endif
        mov r9, [rbx].TheTarget.thestring
        conout r9, lf
        mov rbx, [rbx].TheTarget.thenext
        jmp loop1i 
  loop1f:
    RestoreRegs
    ret
ShowTargets endp 

;***************************************************************************************************

RetrieveAssoc proc
    RRegAssoc key4, opt001, default_editor, EditorBat
    RRegAssoc key4, opt002, default_editor, EditorCmd
    RRegAssoc key4, opt003, default_editor, EditorJs
    ret
RetrieveAssoc endp

SaveAssoc proc
    WRegAssoc key4, opt001, EditorBat
    WRegAssoc key4, opt002, EditorCmd
    WRegAssoc key4, opt003, EditorJs
    ret
SaveAssoc endp


