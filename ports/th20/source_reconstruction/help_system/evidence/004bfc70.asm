004bfc70 55                       push       ebp
004bfc71 8bec                     mov        ebp, esp
004bfc73 51                       push       ecx
004bfc74 682c0c5700               push       0x570c2c
004bfc79 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004bfc7f e86cf1ffff               call       0x4bedf0
004bfc84 8945fc                   mov        dword ptr [ebp - 4], eax
004bfc87 837dfc00                 cmp        dword ptr [ebp - 4], 0
004bfc8b 7504                     jne        0x4bfc91
004bfc8d 33c0                     xor        eax, eax
004bfc8f eb33                     jmp        0x4bfcc4
004bfc91 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfc94 e8f7fcffff               call       0x4bf990
004bfc99 85c0                     test       eax, eax
004bfc9b 7404                     je         0x4bfca1
004bfc9d eb07                     jmp        0x4bfca6
004bfc9f eb05                     jmp        0x4bfca6
004bfca1 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bfca4 eb1e                     jmp        0x4bfcc4
004bfca6 837dfc00                 cmp        dword ptr [ebp - 4], 0
004bfcaa 7416                     je         0x4bfcc2
004bfcac 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bfcaf 50                       push       eax
004bfcb0 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004bfcb6 e805fbf5ff               call       0x41f7c0
004bfcbb c745fc00000000           mov        dword ptr [ebp - 4], 0
004bfcc2 33c0                     xor        eax, eax
004bfcc4 8be5                     mov        esp, ebp
004bfcc6 5d                       pop        ebp
004bfcc7 c3                       ret        
004bfcc8 cc                       int3       
004bfcc9 cc                       int3       
004bfcca cc                       int3       
004bfccb cc                       int3       
004bfccc cc                       int3       
004bfccd cc                       int3       
004bfcce cc                       int3       
004bfccf cc                       int3       
