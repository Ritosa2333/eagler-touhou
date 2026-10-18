004a0590 55                       push       ebp
004a0591 8bec                     mov        ebp, esp
004a0593 83ec08                   sub        esp, 8
004a0596 894dfc                   mov        dword ptr [ebp - 4], ecx
004a0599 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a059c e82feeffff               call       0x49f3d0
004a05a1 85c0                     test       eax, eax
004a05a3 7407                     je         0x4a05ac
004a05a5 b801000000               mov        eax, 1
004a05aa eb15                     jmp        0x4a05c1
004a05ac 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a05af 83c004                   add        eax, 4
004a05b2 8945f8                   mov        dword ptr [ebp - 8], eax
004a05b5 6a00                     push       0
004a05b7 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004a05ba e8812ff8ff               call       0x423540
004a05bf 33c0                     xor        eax, eax
004a05c1 8be5                     mov        esp, ebp
004a05c3 5d                       pop        ebp
004a05c4 c3                       ret        
004a05c5 cc                       int3       
004a05c6 cc                       int3       
004a05c7 cc                       int3       
004a05c8 cc                       int3       
004a05c9 cc                       int3       
004a05ca cc                       int3       
004a05cb cc                       int3       
004a05cc cc                       int3       
004a05cd cc                       int3       
004a05ce cc                       int3       
004a05cf cc                       int3       
