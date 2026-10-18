004c5010 55                       push       ebp
004c5011 8bec                     mov        ebp, esp
004c5013 51                       push       ecx
004c5014 68740d5700               push       0x570d74
004c5019 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004c501f e80cd1ffff               call       0x4c2130
004c5024 8945fc                   mov        dword ptr [ebp - 4], eax
004c5027 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c502b 7504                     jne        0x4c5031
004c502d 33c0                     xor        eax, eax
004c502f eb37                     jmp        0x4c5068
004c5031 8b4508                   mov        eax, dword ptr [ebp + 8]
004c5034 50                       push       eax
004c5035 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c5038 e8c3ebffff               call       0x4c3c00
004c503d 85c0                     test       eax, eax
004c503f 7404                     je         0x4c5045
004c5041 eb07                     jmp        0x4c504a
004c5043 eb05                     jmp        0x4c504a
004c5045 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c5048 eb1e                     jmp        0x4c5068
004c504a 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c504e 7416                     je         0x4c5066
004c5050 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c5053 51                       push       ecx
004c5054 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004c505a e861a7f5ff               call       0x41f7c0
004c505f c745fc00000000           mov        dword ptr [ebp - 4], 0
004c5066 33c0                     xor        eax, eax
004c5068 8be5                     mov        esp, ebp
004c506a 5d                       pop        ebp
004c506b c3                       ret        
004c506c cc                       int3       
004c506d cc                       int3       
004c506e cc                       int3       
004c506f cc                       int3       
