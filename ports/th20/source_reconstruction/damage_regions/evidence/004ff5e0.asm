004ff5e0 55                       push       ebp
004ff5e1 8bec                     mov        ebp, esp
004ff5e3 83ec08                   sub        esp, 8
004ff5e6 894df8                   mov        dword ptr [ebp - 8], ecx
004ff5e9 8b45f8                   mov        eax, dword ptr [ebp - 8]
004ff5ec 0fb6884c200000           movzx      ecx, byte ptr [eax + 0x204c]
004ff5f3 85c9                     test       ecx, ecx
004ff5f5 7409                     je         0x4ff600
004ff5f7 c745fc01000000           mov        dword ptr [ebp - 4], 1
004ff5fe eb07                     jmp        0x4ff607
004ff600 c745fc00000000           mov        dword ptr [ebp - 4], 0
004ff607 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
004ff60b 8be5                     mov        esp, ebp
004ff60d 5d                       pop        ebp
004ff60e c3                       ret        
004ff60f cc                       int3       
