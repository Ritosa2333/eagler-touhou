004863c0 55                       push       ebp
004863c1 8bec                     mov        ebp, esp
004863c3 51                       push       ecx
004863c4 894dfc                   mov        dword ptr [ebp - 4], ecx
004863c7 0fb64508                 movzx      eax, byte ptr [ebp + 8]
004863cb 2407                     and        al, 7
004863cd c0e002                   shl        al, 2
004863d0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004863d3 8a91a1040000             mov        dl, byte ptr [ecx + 0x4a1]
004863d9 80e2e3                   and        dl, 0xe3
004863dc 0ad0                     or         dl, al
004863de 8b45fc                   mov        eax, dword ptr [ebp - 4]
004863e1 8890a1040000             mov        byte ptr [eax + 0x4a1], dl
004863e7 8be5                     mov        esp, ebp
004863e9 5d                       pop        ebp
004863ea c20400                   ret        4
004863ed cc                       int3       
004863ee cc                       int3       
004863ef cc                       int3       
