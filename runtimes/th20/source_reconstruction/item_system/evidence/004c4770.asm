004c4770 55                       push       ebp
004c4771 8bec                     mov        ebp, esp
004c4773 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
004c477a 7416                     je         0x4c4792
004c477c 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004c4782 e85913fcff               call       0x485ae0
004c4787 85c0                     test       eax, eax
004c4789 7407                     je         0x4c4792
004c478b b801000000               mov        eax, 1
004c4790 eb0a                     jmp        0x4c479c
004c4792 6a01                     push       1
004c4794 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c4797 e834f1ffff               call       0x4c38d0
004c479c 5d                       pop        ebp
004c479d c3                       ret        
004c479e cc                       int3       
004c479f cc                       int3       
