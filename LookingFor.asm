; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

    include \masm64\include64\masm64rt.inc

 ;   include \masm64\include64\advapi32.inc
 ;   includelib \masm64\lib64\advapi32.lib


;    AllowPromotions equ 1
;    include macros64G3.inc

    include LookingFor.inc

    ;   For cleanning disk and possible bugs test use 1 
    TRACKING_CRASH equ 0
;    TRACKING_CRASH equ 1       ; Also need to be linked like CONSOLE

test2 struct
    .left qword 0
test2 ends

    test3 test2 {2}
    .code

    include BMSearch.inc        
    include about.inc
    include assoc.inc
    include match.inc
    include filetree.inc
    include target.inc
    include app_run.inc
    include more.inc

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

entry_point proc

conout str$(test3..left),lf

mov test3..left, 1

conout str$(test3..left),lf

    

    GdiPlusBegin                    ; initialise GDIPlus

    mov hInstance, rv(GetModuleHandle,0)
    mov hIcon,     rv(LoadIcon,hInstance,10)
    mov hCursor,   rv(LoadCursor,0,IDC_ARROW)
    mov sWid,      rv(GetSystemMetrics,SM_CXSCREEN)
    mov sHgt,      rv(GetSystemMetrics,SM_CYSCREEN)
    mov hBrush,    rv(CreateSolidBrush,00C4C4C4h)

    call app_init                           ; initialise app settings
    call main                               ; call the main app procedure

    GdiPlusEnd                      ; GdiPlus cleanup

    invoke ExitProcess,0

    ret

entry_point endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

main proc

    LOCAL wc      :WNDCLASSEX

    mov wc.cbSize,         SIZEOF WNDCLASSEX
    mov wc.style,          CS_BYTEALIGNCLIENT or CS_BYTEALIGNWINDOW
    mov wc.lpfnWndProc,    ptr$(WndProc)
    mov wc.cbClsExtra,     0
    mov wc.cbWndExtra,     0
    mrm wc.hInstance,      hInstance
    mrm wc.hIcon,          hIcon
    mrm wc.hCursor,        hCursor
    mrm wc.hbrBackground,  hBrush
    mov wc.lpszMenuName,   0
    mov wc.lpszClassName,  ptr$(classname)
    mrm wc.hIconSm,        hIcon

    invoke RegisterClassEx,ADDR wc

    .if gwid == 0
        mov gwid, 450
        mov ghgt, function(getpercent,sHgt,65)   ; 65% screen height
    
        mov rax, sWid                           ; calculate offset from left side
        sub rax, gwid
        shr rax, 1
        mov glft, rax
    
        mov rax, sHgt                           ; calculate offset from top edge
        sub rax, ghgt
        shr rax, 1
        mov gtop, rax
    .endif
  ; ---------------------------------
  ; centre window at calculated sizes
  ; ---------------------------------
    invoke CreateWindowEx,WS_EX_LEFT or WS_EX_ACCEPTFILES, \
                          ADDR classname, ADDR caption, \
                          WS_OVERLAPPEDWINDOW or WS_VISIBLE,\
                          glft,gtop,gwid,ghgt,0,0,hInstance,0
    mov hWnd, rax
    call msgloop

    ret

main endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

app_init proc

    mov hInstance, rv(GetModuleHandle,0)
    mov hCursor,   rv(LoadCursor,0,IDC_ARROW)
    mov sWid,      rv(GetSystemMetrics,SM_CXSCREEN)
    mov sHgt,      rv(GetSystemMetrics,SM_CYSCREEN)
    mov hIcon,     rv(LoadImage,hInstance,10,IMAGE_ICON,48,48,LR_LOADTRANSPARENT)
    mov pPath,     rv(GetAppPath,ADDR pBuffer)              ; get the editor path for later use
    
    ; --------------------
    ;   brush colours
    ; --------------------
    mov hBrush, rvcall(CreateSolidBrush,00222222h)      ; create the dialog background brush
    mov bBordr, rvcall(CreateSolidBrush,00222222h)      ; create the button border colour
    mov eColor, rvcall(CreateSolidBrush,00111111h)      ; create the Edit window back colour
    mov lColor, rvcall(CreateSolidBrush,00333333h)      ; create the list and combo back colour
    mov aBrush, rvcall(CreateSolidBrush,00F300F3h)      ; create the list and combo back colour

    ; Ini file 
    invoke GetAppPath, addr CurDir
    lea rcx, HelpDir
    mov pHelp, rcx
    mov byte ptr[rcx], 0  
    invoke szappend, rcx, addr CurDir, 0
    invoke szappend, pHelp,"\LookingFor.pdf", rax

    invoke szCatStr, addr CurDir, addr szReg
    
    RRegSize key1, opt1, naranja, glft
    RRegSize key1, opt2, naranja, gtop
    RRegSize key1, opt3, naranja, gwid
    RRegSize key1, opt4, naranja, ghgt

    mov WM_TARGETSLISTCHANGE, rv(RegisterWindowMessage,"targets_list_change")
    mov WM_EXTASSOCIATIONCHANGE, rv(RegisterWindowMessage,"ext_association_change")

    ret
app_init endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

msgloop proc

    LOCAL msg    :MSG
    LOCAL pmsg   :QWORD

    mov pmsg, ptr$(msg)                     ; get the msg structure address
    jmp gmsg                                ; jump directly to GetMessage()

  mloop:
    .switch msg.message
        .case WM_KEYUP
            ; -----------------
            ; single press keys
            ; -----------------
            .switch msg.wParam
              .case VK_ESCAPE
                .if EscapeLock } 0
                    mov EscapeLock, 0
                .else
                    rcall SendMessage,hWnd,WM_CLOSE,0,0
                .endif
              .case VK_RETURN
                .if EscapeLock } 0
                    mov EscapeLock, 0
                .endif
            .endsw

          .case WM_KEYDOWN
            .switch msg.wParam
              .case VK_SPACE
                .if EscapeLock } 0
                    mov EscapeLock, 0
                .endif
            .endsw
    .endsw
    invoke IsDialogMessage, hWnd, pmsg    
    .if rax == 0
        invoke TranslateMessage,pmsg
        invoke DispatchMessage,pmsg
    .endif    
  gmsg:
    test rax, rv(GetMessage,pmsg,0,0,0)     ; loop until GetMessage returns zero
    jnz mloop

    ret
msgloop endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

app_help proc hWin:QWORD
    invoke ShellExecute, hWin, "open", pHelp, 0, 0, SW_RESTORE
    .if eax {= 32
        mov EscapeLock, 1 
        invoke  MessageBox, hWin, "Could not open LookingFor.pdf", "LookingFor:", MB_OK
    .endif
    ret
app_help endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

show_edit proc hwnd:HWND, lparam:LPARAM
    mov lpfnEditProc, rv(SetWindowLongPtr,hwnd,GWL_WNDPROC,addr EditProc)
    ret
show_edit endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

WndProc proc hWin:QWORD,uMsg:QWORD,wParam:QWORD,lParam:QWORD

    USING rbx, r12, r13, r14, r15
    LOCAL dfbuff[260]:BYTE
    LOCAL Rct: RECT, Rct1: RECT
    LOCAL sbh: DWORD, sbParts[2] :DWORD
    LOCAL rct:RECT, units[2]:word, pt:POINT

    .switch uMsg
        .case WM_COMMAND
            .switch wParam
                .case 201
                    .if MasterLock == 0
                        jmp run_now
                    .endif    
                    .return 0
                .case 202
                    .if MasterLock == 0
                      run_now:
                        invoke SendMessage, hbutton1, WM_SETTEXT, 0, ADDR but1_b
                        mov MasterLock, 1
                        invoke CreateThread, NULL, 0, addr app_run, 0, 0, NULL
                        mov Thread1, rax
                        invoke SetFocus, hlist
                    .else
                        invoke CleanItems, addr TheInclusion                ; after greenozon  07 July 2024, 18:38:28
                        invoke CleanItems, addr TheExclusion

                        invoke TerminateThread, Thread1,0
                        mov MasterLock, 0
                        invoke SendMessage, hbutton1, WM_SETTEXT, 0, ADDR but1_a
                        invoke SendMessage, hStatus, SB_SETTEXT, 0, "Canceled!"
                    .endif    
                    .return 0
                .case 203
                     invoke CreateThread, NULL, 0, addr app_help, hWin, 0, NULL
                    .return 0

                .case 204
                     mov DialogLock, 1
	              invoke DialogBoxParam, hInstance, 100, hWin, ADDR AboutDlg, 0
                     mov DialogLock, 0
                    .return 0

                .case 206
                    invoke SendMessage,hedit1,WM_GETTEXT,256,addr dfbuff
                    invoke RemoveTarget,addr dfbuff
                    invoke SendMessage,hedit1,CB_FINDSTRINGEXACT,-1,addr dfbuff
                    .if rax == CB_ERR
                        mov EscapeLock, 1
                        invoke MessageBox,hWnd,"Target is not listed","LookingFor", MB_OK
                    .else
                        invoke SendMessage,hedit1, CB_DELETESTRING,rax,0
                        SaveRegs
                        mov r15, ptr$(CurDir)
                        ;lea r14, dfbuff
                        call SaveTargets
                        RestoreRegs
                        invoke PostMessage,HWND_BROADCAST,WM_TARGETSLISTCHANGE,0,0
                        invoke SleepEx,100,0
                    .endif
                    .return 0

                .case 207

                    invoke SendMessage,hedit1,WM_GETTEXT,256,addr dfbuff
                    invoke StoreTarget, addr dfbuff
                    SaveRegs
                    mov r15, ptr$(CurDir)
                    ;lea r14, dfbuff
                    call SaveTargets
                    RestoreRegs
                    mov AddLock, 1
                    invoke PostMessage,HWND_BROADCAST,WM_TARGETSLISTCHANGE,0,0
                    invoke SleepEx,100,0
                    .return 0
            .endsw
    
        .case WM_CREATE
            invoke CreateStatusWindow,WS_CHILD or WS_VISIBLE, NULL, hWin, 201
            mov hStatus, rax
    
            mov hbutton1, rv(buttonp,hInstance,hWin, addr but1_a,0,0,0,0,202)
            mov hbutton2, rv(buttonp,hInstance,hWin,"Help",0,0,0,0,203)
            mov hbutton3, rv(buttonp,hInstance,hWin,"About",0,0,0,0,204)
            mov hbutton4, rv(button,hInstance,hWin,"-",0,0,0,0,206)
            mov hbutton5, rv(button,hInstance,hWin,"+",0,0,0,0,207)

            mov hstatic1, rv(static,hInstance,hWin,"Base directory and files:",0,0,0,0)
            mov hstatic2, rv(staticr,hInstance,hWin,"excluded:  ",0,0,0,0)
            
            mov hstatic3, rv(staticr,hInstance,hWin,"maxDist:  ",0,0,0,0)
            mov hstatic4, rv(staticr,hInstance,hWin,"maxMb:  ",0,0,0,0)

            mov hedit1, rv(target_combo,hInstance,hWin, 0,20,20,200,205)
            mov hedit2, rv(editboxl,hWin,hInstance,0,0,0,0,0,206)
            mov hedit3, rv(editboxl,hWin,hInstance,0,0,0,0,0,207)
            mov hedit4, rv(editboxl,hWin,hInstance,0,0,0,0,0,208)
            mov hedit5, rv(editboxc,hWin,hInstance,0,0,0,0,0,209)
            mov hedit6, rv(editboxc,hWin,hInstance,0,0,0,0,0,210)
          
            mov hgbox1,  rv(group_box,hInstance,hWin, 0,0,0,0,0)
            mov hgbox2,  rv(group_box,hInstance,hWin,0,0,0,0,0)
            mov hgbox1t, rv(static,hInstance,hWin,"First search string",0,0,0,0)
            mov hgbox2t, rv(static,hInstance,hWin,"Second search string",0,0,0,0)

            mov hcb1, rv(select_combo,hInstance,hWin, 0,0,0,00,211)
            fill_cb hcb1, string1case, 0, "sens.", "insens.", "ignore"
            mov hcb2, rv(select_combo,hInstance,hWin, 0,0,0,0,212)
            fill_cb hcb2, string1word, 0, "partial", "full", "+underscore","+point"
            mov hcb3, rv(select_combo,hInstance,hWin, 0,0,0,0,213)
            fill_cb hcb3, string2case, 0, "sens.", "insens.", "ignore"
            mov hcb4, rv(select_combo,hInstance,hWin, 0,0,0,0,214)
            fill_cb hcb4, string2word, 0, "partial", "full", "+underscore","+point"
          
            mov hlist, rv(list,hInstance,hWin, 0,0,0,0,700)

            mov lpListProc, rv(SetWindowLongPtr,hlist,GWL_WNDPROC,ADDR ListProc)
            mov lpfnIntProc, rv(SetWindowLongPtr,hedit5,GWL_WNDPROC,addr IntProc)
            mov lpfnDecProc, rv(SetWindowLongPtr,hedit6,GWL_WNDPROC,addr DecProc)
            invoke SetWindowLongPtr,hedit2,GWL_WNDPROC,addr EditProc
            invoke SetWindowLongPtr,hedit3,GWL_WNDPROC,addr EditProc
            invoke SetWindowLongPtr,hedit4,GWL_WNDPROC,addr EditProc
            invoke EnumChildWindows,hedit1,addr show_edit, 0

            invoke RetrieveTargets
            RRegEdit key2,opt01,empty,hedit1    
            RRegEdit key2,opt02,empty,hedit2    
            RRegEdit key2,opt03,empty,hedit3    
            RRegEdit key2,opt04,empty,hedit4    
            RRegEdit key2,opt05,empty,hedit5    
            RRegEdit key2,opt06,empty,hedit6    
            RRegMode key2,opt07,unidad,hcb1    
            RRegMode key2,opt08,naranja,hcb2    
            RRegMode key2,opt09,unidad,hcb3    
            RRegMode key2,opt10,naranja,hcb4    
            invoke RetrieveAssoc
            invoke SetFocus, hbutton1		

            .return 0

        .case WM_TARGETSLISTCHANGE
            .if AddLock == 0
                invoke SendMessage,hedit1,WM_GETTEXT,259, ADDR dfbuff
                invoke SendMessage,hedit1,CB_RESETCONTENT,0,0
                invoke RetrieveTargets
                invoke SendMessage,hedit1,WM_SETTEXT,0, ADDR dfbuff
            .else
                mov AddLock, 0
            .endif
            .return 0
        .case WM_EXTASSOCIATIONCHANGE
            .if AddLock == 0
                invoke RetrieveAssoc
            .else
                mov AddLock, 0
            .endif
            .return 0

        .case WM_SIZE
            SaveRegs
            invoke GetClientRect,hWin,ADDR Rct
            mov ebx, Rct.right
            shr rbx, 1
            mov sbParts, ebx

            invoke MoveWindow,hStatus,0,0,0,0,TRUE
            ;invoke SendMessage, hStatus, SB_SETPARTS, 2, ADDR sbParts

            invoke GetClientRect,hStatus,ADDR Rct1
            mov eax, Rct1.bottom
            mov sbh, eax      ; status bar height

            mov rax, rbx
            sub rax, 6
            invoke MoveWindow, hstatic1, 3, 3, rax, 22,TRUE
            invoke SendMessage,hstatic1, BN_PAINT,0,0
            mov rax, rbx
            mov rcx, 3
            div rcx
            mov r12, rax
            sub r12, 3

            invoke MoveWindow, hbutton1,rbx,3,r12,22,TRUE
            add rbx, r12
            add rbx, 3
            invoke MoveWindow, hbutton2,rbx,3,r12,22,TRUE
            add rbx, r12
            add rbx, 3
            invoke MoveWindow, hbutton3,rbx,3,r12,22,TRUE

            mov r15d, Rct.right
            sub r15, 6
            mov r14, r15
            sub r14, 57 
            mov eax, Rct.bottom
            invoke MoveWindow, hedit1,3,33,r14,rax,TRUE
            add r14, 6
            invoke MoveWindow, hbutton4,r14,33,24,24,TRUE
            add r14, 27
            invoke MoveWindow, hbutton5,r14,33,24,24,TRUE

            invoke MoveWindow, hgbox1,3,63,r15,57,TRUE
            invoke MoveWindow, hgbox2,3,123,r15,57,TRUE
            invoke MoveWindow, hgbox1t,13,62,127,15,TRUE
            invoke MoveWindow, hgbox2t,13,122,140,15,TRUE
            invoke SendMessage,hgbox1t, BN_PAINT,0,0
            invoke SendMessage,hgbox2t, BN_PAINT,0,0

            sub r15, 176
            invoke MoveWindow, hedit2,9,85,r15,24,TRUE
            invoke MoveWindow, hedit3,9,145,r15,24,TRUE
            add r15, 13
            invoke MoveWindow, hcb1,r15,85,77,95,TRUE
            invoke MoveWindow, hcb3,r15,145,77,95,TRUE
            add r15, 80
            invoke MoveWindow, hcb2,r15,85,77,95,TRUE
            invoke MoveWindow, hcb4,r15,145,77,95,TRUE

            mov eax, Rct.right
            mov rcx, 6
            div rcx
            mov r12, rax
            mov r13, rax
            mov rbx, rax
            sub r12, 3

            invoke MoveWindow, hstatic2,3,183,r12,22,TRUE
            invoke SendMessage,hstatic2, BN_PAINT,0,0
            invoke MoveWindow, hedit4,rbx,183,r12,22,TRUE
            add rbx, r13
            invoke MoveWindow, hstatic3,rbx,183,r12,22,TRUE
            invoke SendMessage,hstatic3, BN_PAINT,0,0
            add rbx, r13
            invoke MoveWindow, hedit5,rbx,183,r12,22,TRUE
            add rbx, r13
            invoke MoveWindow, hstatic4,rbx,183,r12,22,TRUE
            invoke SendMessage,hstatic4, BN_PAINT,0,0
            add rbx, r13
            invoke MoveWindow, hedit6,rbx,183,r12,22,TRUE
            mov eax, Rct.bottom
            sub eax, sbh
            sub rax, 213
            add r15, 176-93
            invoke MoveWindow, hlist, 3, 210, r15,rax,TRUE
            RestoreRegs
 
        .case WM_CLOSE
            
            invoke GetWindowRect, hWin, addr rct
            mov eax, rct.left
            mov glft, rax
            mov ecx, rct.right
            sub ecx, eax
            mov gwid, rcx

            mov eax, rct.top
            mov gtop, rax
            mov ecx, rct.bottom
            sub ecx, eax
            mov ghgt, rcx
            mov sbh, 0
            SaveRegs
            mov r15, ptr$(CurDir)
            lea r14, dfbuff
            WRegSize key1,opt1,glft
            WRegSize key1,opt2,gtop
            WRegSize key1,opt3,gwid
            WRegSize key1,opt4,ghgt
            WRegEdit key2,opt01,hedit1    
            WRegEdit key2,opt02,hedit2    
            WRegEdit key2,opt03,hedit3    
            WRegEdit key2,opt04,hedit4    
            WRegEdit key2,opt05,hedit5    
            WRegEdit key2,opt06,hedit6    
            WRegMode key2,opt07,hcb1    
            WRegMode key2,opt08,hcb2    
            WRegMode key2,opt09,hcb3    
            WRegMode key2,opt10,hcb4    
            call SaveTargets
            RestoreRegs
            invoke SendMessage,hWin,WM_DESTROY,0,0

        .case WM_DESTROY
            invoke PostQuitMessage,NULL

        .case WM_CTLCOLORDLG
            mov rax, hBrush
            ret

        .case WM_CTLCOLORBTN
            mov rax, bBordr
            ret

        .case WM_CTLCOLORSTATIC                                       ; static controls
            rcall SetBkColor,wParam,00222222h
            rcall SetTextColor,wParam,00DDDDDDh
            mov rax, hBrush
            ret

        .case WM_CTLCOLOREDIT                                         ; edit controls
            rcall SetBkColor,wParam,00111111h
            rcall SetTextColor,wParam,00DDDDDDh
            mov rax, eColor
            ret

        .case WM_CTLCOLORLISTBOX
            rcall SetBkColor,wParam,00111111h
            rcall SetTextColor,wParam,00DDDDDDh
            mov rax, eColor
            ret
        .case WM_CTLCOLORMSGBOX
            rcall SetBkColor,wParam,00111111h
            rcall SetTextColor,wParam,00DDDDDDh
            mov rax, eColor
            ret

    .endsw

    invoke DefWindowProc,hWin,uMsg,wParam,lParam

    ret

WndProc endp

; いいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいいい

    end
