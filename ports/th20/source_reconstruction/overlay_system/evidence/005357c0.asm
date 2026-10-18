005357c0 55                       push       ebp
005357c1 8bec                     mov        ebp, esp
005357c3 83ec08                   sub        esp, 8
005357c6 894dfc                   mov        dword ptr [ebp - 4], ecx
005357c9 8b45fc                   mov        eax, dword ptr [ebp - 4]
005357cc 0584060000               add        eax, 0x684
005357d1 8945f8                   mov        dword ptr [ebp - 8], eax
005357d4 8b4d08                   mov        ecx, dword ptr [ebp + 8]
005357d7 51                       push       ecx
005357d8 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005357db e8004cf8ff               call       0x4ba3e0
005357e0 8be5                     mov        esp, ebp
005357e2 5d                       pop        ebp
005357e3 c20400                   ret        4
005357e6 cc                       int3       
005357e7 cc                       int3       
005357e8 cc                       int3       
005357e9 cc                       int3       
005357ea cc                       int3       
005357eb cc                       int3       
005357ec cc                       int3       
005357ed cc                       int3       
005357ee cc                       int3       
005357ef cc                       int3       
