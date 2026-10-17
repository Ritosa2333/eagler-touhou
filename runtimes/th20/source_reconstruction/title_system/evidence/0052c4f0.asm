0052c4f0 55                       push       ebp
0052c4f1 8bec                     mov        ebp, esp
0052c4f3 6aff                     push       -1
0052c4f5 688d805600               push       0x56808d
0052c4fa 64a100000000             mov        eax, dword ptr fs:[0]
0052c500 50                       push       eax
0052c501 83ec14                   sub        esp, 0x14
0052c504 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052c509 33c5                     xor        eax, ebp
0052c50b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0052c50e 50                       push       eax
0052c50f 8d45f4                   lea        eax, [ebp - 0xc]
0052c512 64a300000000             mov        dword ptr fs:[0], eax
0052c518 894de8                   mov        dword ptr [ebp - 0x18], ecx
0052c51b 6a14                     push       0x14
0052c51d b940025c00               mov        ecx, 0x5c0240
0052c522 e8e9fdedff               call       0x40c310
0052c527 50                       push       eax
0052c528 8d4dec                   lea        ecx, [ebp - 0x14]
0052c52b e800f2edff               call       0x40b730
0052c530 c745fc00000000           mov        dword ptr [ebp - 4], 0
0052c537 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c53a e8717af3ff               call       0x463fb0
0052c53f 90                       nop        
0052c540 6a00                     push       0
0052c542 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0052c545 50                       push       eax
0052c546 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052c549 51                       push       ecx
0052c54a 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c54d e8eefeffff               call       0x52c440
0052c552 85c0                     test       eax, eax
0052c554 0f84a7000000             je         0x52c601
0052c55a 6a01                     push       1
0052c55c 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0052c55f 52                       push       edx
0052c560 8b4508                   mov        eax, dword ptr [ebp + 8]
0052c563 50                       push       eax
0052c564 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c567 e8d4feffff               call       0x52c440
0052c56c 85c0                     test       eax, eax
0052c56e 0f848d000000             je         0x52c601
0052c574 6a02                     push       2
0052c576 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0052c579 51                       push       ecx
0052c57a 8b5508                   mov        edx, dword ptr [ebp + 8]
0052c57d 52                       push       edx
0052c57e 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c581 e8bafeffff               call       0x52c440
0052c586 85c0                     test       eax, eax
0052c588 7477                     je         0x52c601
0052c58a 6a03                     push       3
0052c58c 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0052c58f 50                       push       eax
0052c590 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052c593 51                       push       ecx
0052c594 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c597 e8a4feffff               call       0x52c440
0052c59c 85c0                     test       eax, eax
0052c59e 7461                     je         0x52c601
0052c5a0 6a04                     push       4
0052c5a2 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0052c5a5 52                       push       edx
0052c5a6 8b4508                   mov        eax, dword ptr [ebp + 8]
0052c5a9 50                       push       eax
0052c5aa 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c5ad e88efeffff               call       0x52c440
0052c5b2 85c0                     test       eax, eax
0052c5b4 744b                     je         0x52c601
0052c5b6 6a05                     push       5
0052c5b8 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0052c5bb 51                       push       ecx
0052c5bc 8b5508                   mov        edx, dword ptr [ebp + 8]
0052c5bf 52                       push       edx
0052c5c0 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c5c3 e878feffff               call       0x52c440
0052c5c8 85c0                     test       eax, eax
0052c5ca 7435                     je         0x52c601
0052c5cc 6a06                     push       6
0052c5ce 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0052c5d1 50                       push       eax
0052c5d2 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052c5d5 51                       push       ecx
0052c5d6 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c5d9 e862feffff               call       0x52c440
0052c5de 85c0                     test       eax, eax
0052c5e0 741f                     je         0x52c601
0052c5e2 6a07                     push       7
0052c5e4 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0052c5e7 52                       push       edx
0052c5e8 8b4508                   mov        eax, dword ptr [ebp + 8]
0052c5eb 50                       push       eax
0052c5ec 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c5ef e84cfeffff               call       0x52c440
0052c5f4 85c0                     test       eax, eax
0052c5f6 7409                     je         0x52c601
0052c5f8 c745e401000000           mov        dword ptr [ebp - 0x1c], 1
0052c5ff eb07                     jmp        0x52c608
0052c601 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
0052c608 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0052c60b 894de0                   mov        dword ptr [ebp - 0x20], ecx
0052c60e c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0052c615 8d4dec                   lea        ecx, [ebp - 0x14]
0052c618 e8e3f2edff               call       0x40b900
0052c61d 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0052c620 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052c623 64890d00000000           mov        dword ptr fs:[0], ecx
0052c62a 59                       pop        ecx
0052c62b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052c62e 33cd                     xor        ecx, ebp
0052c630 e8b9630100               call       0x5429ee
0052c635 8be5                     mov        esp, ebp
0052c637 5d                       pop        ebp
0052c638 c20800                   ret        8
0052c63b cc                       int3       
0052c63c cc                       int3       
0052c63d cc                       int3       
0052c63e cc                       int3       
0052c63f cc                       int3       
