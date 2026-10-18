004235c0 55                       push       ebp
004235c1 8bec                     mov        ebp, esp
004235c3 83ec08                   sub        esp, 8
004235c6 894df8                   mov        dword ptr [ebp - 8], ecx
004235c9 8b45f8                   mov        eax, dword ptr [ebp - 8]
004235cc 8b4804                   mov        ecx, dword ptr [eax + 4]
004235cf 3b4d08                   cmp        ecx, dword ptr [ebp + 8]
004235d2 7c09                     jl         0x4235dd
004235d4 c745fc01000000           mov        dword ptr [ebp - 4], 1
004235db eb07                     jmp        0x4235e4
004235dd c745fc00000000           mov        dword ptr [ebp - 4], 0
004235e4 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
004235e8 8be5                     mov        esp, ebp
004235ea 5d                       pop        ebp
004235eb c20400                   ret        4
004235ee cc                       int3       
004235ef cc                       int3       
