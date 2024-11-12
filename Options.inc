; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

OrderOptionsDlg proc hWin:QWORD,uMsg:QWORD,wParam:QWORD,lParam:QWORD

    LOCAL hStatic :QWORD
    LOCAL hCheck[6] :QWORD
    LOCAL hEdit   :QWORD
    LOCAL hText   :QWORD
    LOCAL dfbuff[260]:BYTE


    LOCAL buffer[1024]:BYTE
    LOCAL pbuff   :QWORD

    .switch uMsg
      .case WM_INITDIALOG
        mov hStatic, rv(GetDlgItem,hWin,101)
        mov hText,   rv(GetDlgItem,hWin,103)
        mov hCheck[0*8],  rv(GetDlgItem,hWin,1002)
        mov hCheck[1*8],  rv(GetDlgItem,hWin,1003)
        mov hCheck[2*8],  rv(GetDlgItem,hWin,1004)
        mov hCheck[3*8],  rv(GetDlgItem,hWin,1005)
        mov hCheck[4*8],  rv(GetDlgItem,hWin,1006)
        mov hCheck[5*8],  rv(GetDlgItem,hWin,1007)

        mov rax, sorting
        lea rdx, hCheck
        mov rcx, [rdx+rax*8] 
        invoke SendMessage, rcx, BM_SETCHECK, BST_CHECKED, 0

      ; -------------------------------------
      ; load the icon into the static control
      ; -------------------------------------
        invoke SendMessage,hStatic,STM_SETIMAGE,IMAGE_ICON,hIcon

      ; ---------------------------
      ; set the icon for the dialog
      ; ---------------------------
        invoke SendMessage,hWin,WM_SETICON,1,hIcon

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

      .case WM_COMMAND
        .switch wParam

          .case 1002
            mov sorting, 0
            mov sort_kind, 0
            mov sort_asc, 0
            mov sorted, 0

          .case 1003
            mov sorting, 1
            mov sort_kind, 0
            mov sort_asc, 1
            mov sorted, 0

          .case 1004
            mov sorting, 2
            mov sort_kind, 1
            mov sort_asc, 1
            mov sorted, 0

          .case 1005
            mov sorting, 3
            mov sort_kind, 1
            mov sort_asc, 0
            mov sorted, 0

          .case 1006
            mov sorting, 4
            mov sort_kind, 2
            mov sort_asc, 0
            mov sorted, 0

          .case 1007
            mov sorting, 5
            mov sort_kind, 2
            mov sort_asc, 1
            mov sorted, 0

          .case 1001                     ; the Cancel button
            jmp exit_dialog
          .case 102                      ; the OK button
     
            .if sorted == 0        
              invoke SendMessage,hedit1,WM_GETTEXT,259, ADDR dfbuff
              invoke SendMessage,hedit1,CB_RESETCONTENT,0,0
              invoke SortTargets, sort_kind, sort_asc
              invoke SendMessage,hedit1,WM_SETTEXT,0, ADDR dfbuff
              invoke ShowItems, TheTargets
              invoke LoadTargets 
              mov sorted, 1
;            mov AddLock, 1
;            invoke PostMessage,HWND_BROADCAST,WM_EXTASSOCIATIONCHANGE,0,0
            .endif
            jmp exit_dialog
        .endsw

      .case WM_CLOSE
        exit_dialog:
        invoke EndDialog,hWin,0         ; exit from system menu
    .endsw

    xor rax, rax
    ret

OrderOptionsDlg endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい
