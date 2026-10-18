004c4740 55                       push       ebp
004c4741 8bec                     mov        ebp, esp
004c4743 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
004c474a 7416                     je         0x4c4762
004c474c 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004c4752 e88913fcff               call       0x485ae0
004c4757 85c0                     test       eax, eax
004c4759 7407                     je         0x4c4762
004c475b b801000000               mov        eax, 1
004c4760 eb0a                     jmp        0x4c476c
004c4762 6a00                     push       0
004c4764 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c4767 e864f1ffff               call       0x4c38d0
004c476c 5d                       pop        ebp
004c476d c3                       ret        
004c476e cc                       int3       
004c476f cc                       int3       
