004de5a0 55                       push       ebp
004de5a1 8bec                     mov        ebp, esp
004de5a3 51                       push       ecx
004de5a4 894dfc                   mov        dword ptr [ebp - 4], ecx
004de5a7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004de5aa e8f1d7f2ff               call       0x40bda0
004de5af 8b45fc                   mov        eax, dword ptr [ebp - 4]
004de5b2 c70050205700             mov        dword ptr [eax], 0x572050
004de5b8 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004de5bb 51                       push       ecx
004de5bc e8bfccf2ff               call       0x40b280
004de5c1 83c404                   add        esp, 4
004de5c4 8b45fc                   mov        eax, dword ptr [ebp - 4]
004de5c7 8be5                     mov        esp, ebp
004de5c9 5d                       pop        ebp
004de5ca c20400                   ret        4
004de5cd cc                       int3       
004de5ce cc                       int3       
004de5cf cc                       int3       
