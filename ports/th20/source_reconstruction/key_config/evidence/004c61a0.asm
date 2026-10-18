004c61a0 55                       push       ebp
004c61a1 8bec                     mov        ebp, esp
004c61a3 51                       push       ecx
004c61a4 894dfc                   mov        dword ptr [ebp - 4], ecx
004c61a7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c61aa 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c61ad 898800010000             mov        dword ptr [eax + 0x100], ecx
004c61b3 6a00                     push       0
004c61b5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c61b8 81c1c0000000             add        ecx, 0xc0
004c61be e85dd3f5ff               call       0x423520
004c61c3 90                       nop        
004c61c4 8be5                     mov        esp, ebp
004c61c6 5d                       pop        ebp
004c61c7 c20400                   ret        4
004c61ca cc                       int3       
004c61cb cc                       int3       
004c61cc cc                       int3       
004c61cd cc                       int3       
004c61ce cc                       int3       
004c61cf cc                       int3       
