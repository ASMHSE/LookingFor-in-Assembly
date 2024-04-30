    txt_colour equ <00F0F0F0h>
    bak_colour equ <00111111h>

    .data?
        hInstance   dq ?
        hWnd        dq ?
        hIcon       dq ?
        hCursor     dq ?
        sWid        dq ?
        sHgt        dq ?
        hStatus     dq ?
    
        pPath       dq ?
        pBuffer     db 260 dup (?)      ; allocate a buffer for the app path

        hBrush      dq ?
        bBordr      dq ?
        eColor      dq ?
        lColor      dq ?

        lpfnStatusProc  dq ?
        lpListProc      dq ?
        lpfnDecProc     dq ?
        lpfnIntProc     dq ?

        hbutton1     dq ?
        hbutton2     dq ?
        hbutton3     dq ?
        hbutton4     dq ?
        hbutton5     dq ?

        hstatic1     dq ?
        hstatic2     dq ?
        hstatic3     dq ?
        hstatic4     dq ?
        hedit1       dq ?
        hedit2       dq ?
        hedit3       dq ?
        hedit4       dq ?
        hedit5       dq ?
        hedit6       dq ?
        hgbox1       dq ?
        hgbox2       dq ?
        hgbox1t      dq ?
        hgbox2t      dq ?
        hlist        dq ?
        hcb1         dq ?
        hcb2         dq ?
        hcb3         dq ?
        hcb4         dq ?

        aBrush       dq ?

    .data

        glft         dq 0
        gtop         dq 0
        gwid         dq 0
        ghgt         dq 0

        classname   db "lookingfor_class",0
        caption     db "LookingFor",0

        szReg       db "\LookingFor.ini",0
        key1        db "Window",0
            opt1    db "lft",0
            opt2    db "top",0
            opt3    db "wid",0
            opt4    db "hgt",0
        key2        db "Last",0
            opt01   db "target",0
            opt02   db "exp1",0
            opt03   db "exp2",0
            opt04   db "excl",0
            opt05   db "dist",0
            opt06   db "mb",0
            opt07   db "cb1",0
            opt08   db "cb2",0
            opt09   db "cb3",0
            opt10   db "cb4",0
        key3        db "Targets",0

        naranja     db "0",0
        unidad      db "1",0
        empty       db 0

        string1case         dq  1
        string1word         dq  0
        string2case         dq  1
        string2word         dq  0
        Boyer_Moore         dq  0
        Boyer_Moore_Table   dq  0


        CurDir      db  260 dup(0)
        szBuff      db  256 dup(0)
        szBuff1     db   25 dup(0)  
        szBuff2     db   25 dup(0)

        maxMb       dq 0
        maxDist     dd 0
        useExcl     dq 0

        megabyte    sdword 1024*1024
        filecandt   dq 0
        filecount   dq 0

        MasterLock  dq 0
        EscapeLock  dq 0
        AddLock     dq 0


        criterios   dq 0
        psearch1    dq 0
        psearch2    dq 0
        dsize1      dd 0
        dsize2      dd 0

        WM_TARGETSLISTCHANGE dq 0




    IFNDEF MEMORYSTATUSEX
      MEMORYSTATUSEX STRUCT QWORD
        dwLength dd ?
        dwMemoryLoad dd ?
        ullTotalPhys dq ?
        ullAvailPhys dq ?
        ullTotalPageFile dq ?
        ullAvailPageFile dq ?
        ullTotalVirtual dq ?
        ullAvailVirtual dq ?
        ullAvailExtendedVirtual dq ?
      MEMORYSTATUSEX ENDS
    ENDIF

    SYSTEM_INFOx STRUCT QWORD
        UNION
        dwOemId dd ?
          STRUCT
            wProcessorArchitecture dw ?
            wReserved dw ?
          ENDS
        ENDS
      dwPageSize dd ?
      lpMinimumApplicationAddress dq ?
      lpMaximumApplicationAddress dq ?
      dwActiveProcessorMask dq ?
      dwNumberOfProcessors dd ?
      dwProcessorType dd ?
      dwAllocationGranularity dd ?
      wProcessorLevel dw ?
      wProcessorRevision dw ?
    SYSTEM_INFOx ENDS

    q2q macro a1, a2, a3
        ifnb <a3>
            mov a3, a2
            mov a1, a3
        else
            mov rax, a2
            mov a1, rax
        endif
    endm