00486630 55                       push       ebp
00486631 8bec                     mov        ebp, esp
00486633 51                       push       ecx
00486634 68d4fc5600               push       0x56fcd4
00486639 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0048663f e8ac4fffff               call       0x47b5f0
00486644 8945fc                   mov        dword ptr [ebp - 4], eax
00486647 837dfc00                 cmp        dword ptr [ebp - 4], 0
0048664b 7504                     jne        0x486651
0048664d 33c0                     xor        eax, eax
0048664f eb37                     jmp        0x486688
00486651 8b4508                   mov        eax, dword ptr [ebp + 8]
00486654 50                       push       eax
00486655 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00486658 e843a9ffff               call       0x480fa0
0048665d 85c0                     test       eax, eax
0048665f 7404                     je         0x486665
00486661 eb07                     jmp        0x48666a
00486663 eb05                     jmp        0x48666a
00486665 8b45fc                   mov        eax, dword ptr [ebp - 4]
00486668 eb1e                     jmp        0x486688
0048666a 837dfc00                 cmp        dword ptr [ebp - 4], 0
0048666e 7416                     je         0x486686
00486670 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00486673 51                       push       ecx
00486674 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0048667a e84191f9ff               call       0x41f7c0
0048667f c745fc00000000           mov        dword ptr [ebp - 4], 0
00486686 33c0                     xor        eax, eax
00486688 8be5                     mov        esp, ebp
0048668a 5d                       pop        ebp
0048668b c3                       ret        
0048668c cc                       int3       
0048668d cc                       int3       
0048668e cc                       int3       
0048668f cc                       int3       
