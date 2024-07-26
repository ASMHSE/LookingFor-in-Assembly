; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤

AssocDlg proc hWin:QWORD,uMsg:QWORD,wParam:QWORD,lParam:QWORD

    LOCAL hStatic :QWORD
    ;LOCAL hButn1  :QWORD
    ;LOCAL hButn2  :QWORD
    LOCAL hEdit   :QWORD
    LOCAL hText   :QWORD


    ;LOCAL rct     :RECT
    LOCAL buffer[1024]:BYTE
    LOCAL pbuff   :QWORD
    ;LOCAL vbuffr[64]:BYTE
    ;LOCAL vendor  :QWORD
    ;LOCAL cpustr  :QWORD
    ;LOCAL mstat   :QWORD
    ;LOCAL astat   :QWORD

    .switch uMsg
        .case WM_INITDIALOG
            mov hStatic, rv(GetDlgItem,hWin,101)
            mov hText,   rv(GetDlgItem,hWin,103)
            ;mov hButn1,  rv(GetDlgItem,hWin,102)
            ;mov hButn2,  rv(GetDlgItem,hWin,1001)
            mov hEdit,   rv(GetDlgItem,hWin,1002)
            ; «»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»

            invoke SetWindowText, hWin," File extension association"

            ; «»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»«»
            mov pbuff, ptr$(buffer)
            
            mcat pbuff, addr err3a
            .if prevented == 1
                mcat pbuff, pbuff, ".bat"
            .elseif prevented == 2
                mcat pbuff, pbuff, ".cmd"
            .elseif prevented == 3
                mcat pbuff, pbuff, ".js"
            .endif
            mcat pbuff, pbuff, addr err3b
    
            invoke SetWindowText, hText, pbuff 
          ; -------------------------------------
          ; load the icon into the static control
          ; -------------------------------------
            invoke SendMessage,hStatic,STM_SETIMAGE,IMAGE_ICON,hIcon
    
          ; ---------------------------
          ; set the icon for the dialog
          ; ---------------------------
            invoke SendMessage,hWin,WM_SETICON,1,hIcon
            mov rax, TRUE
            .if prevented == 1
                lea r9, EditorBat
            .elseif prevented == 2
                lea r9, EditorCmd
            .elseif prevented == 3
                lea r9, EditorJs
            .endif
            invoke SendMessage, hEdit, WM_SETTEXT, 0, r9
        
            ret

      .case WM_CTLCOLORDLG
        mov rax, hBrush
        ret

      .case WM_CTLCOLORBTN
        mov rax, bBordr
        ret

      .case WM_CTLCOLORSTATIC                                       ; static controls
        rcall SetBkColor,wParam,00222222h
        rcall SetTextColor,wParam,00FFFFFFh
        mov rax, hBrush
        ret

      .case WM_CTLCOLOREDIT                                         ; edit controls
        rcall SetBkColor,wParam,00111111h
        rcall SetTextColor,wParam,00FFFFFFh
        mov rax, eColor
        ret

      .case WM_COMMAND
        .switch wParam
          .case 1001                     ; the Cancel button
            jmp exit_dialog
          .case 102                      ; the OK button
            mov hEdit, rv(GetDlgItem,hWin,1002)
            .if prevented == 1
                lea r9, EditorBat
            .elseif prevented == 2
                lea r9, EditorCmd
            .elseif prevented == 3
                lea r9, EditorJs
            .endif
            invoke SendMessage, hEdit, WM_GETTEXT, 259, r9
            conout addr EditorBat, lf
            call SaveAssoc
            mov AddLock, 1
            invoke PostMessage,HWND_BROADCAST,WM_EXTASSOCIATIONCHANGE,0,0
            jmp exit_dialog
        .endsw

      .case WM_CLOSE
        exit_dialog:
        invoke EndDialog,hWin,0         ; exit from system menu
    .endsw

    xor rax, rax
    ret

AssocDlg endp

; ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
