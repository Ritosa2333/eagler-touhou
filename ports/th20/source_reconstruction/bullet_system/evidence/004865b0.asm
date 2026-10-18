004865b0 55                       push       ebp
004865b1 8bec                     mov        ebp, esp
004865b3 51                       push       ecx
004865b4 894dfc                   mov        dword ptr [ebp - 4], ecx
004865b7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004865ba 8a4d08                   mov        cl, byte ptr [ebp + 8]
004865bd 884835                   mov        byte ptr [eax + 0x35], cl
004865c0 8be5                     mov        esp, ebp
004865c2 5d                       pop        ebp
004865c3 c20400                   ret        4
004865c6 cc                       int3       
004865c7 cc                       int3       
004865c8 cc                       int3       
004865c9 cc                       int3       
004865ca cc                       int3       
004865cb cc                       int3       
004865cc cc                       int3       
004865cd cc                       int3       
004865ce cc                       int3       
004865cf cc                       int3       
