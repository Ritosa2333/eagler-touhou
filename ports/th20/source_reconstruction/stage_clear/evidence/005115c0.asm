005115c0 55                       push       ebp
005115c1 8bec                     mov        ebp, esp
005115c3 51                       push       ecx
005115c4 833d14615c0000           cmp        dword ptr [0x5c6114], 0
005115cb 740e                     je         0x5115db
005115cd a114615c00               mov        eax, dword ptr [0x5c6114]
005115d2 50                       push       eax
005115d3 e8e801f1ff               call       0x4217c0
005115d8 83c404                   add        esp, 4
005115db 68083f5700               push       0x573f08
005115e0 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
005115e6 e845f3ffff               call       0x510930
005115eb 8945fc                   mov        dword ptr [ebp - 4], eax
005115ee 837dfc00                 cmp        dword ptr [ebp - 4], 0
005115f2 7504                     jne        0x5115f8
005115f4 33c0                     xor        eax, eax
005115f6 eb37                     jmp        0x51162f
005115f8 8b4d08                   mov        ecx, dword ptr [ebp + 8]
005115fb 51                       push       ecx
005115fc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005115ff e86cfcffff               call       0x511270
00511604 85c0                     test       eax, eax
00511606 7404                     je         0x51160c
00511608 eb07                     jmp        0x511611
0051160a eb05                     jmp        0x511611
0051160c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051160f eb1e                     jmp        0x51162f
00511611 837dfc00                 cmp        dword ptr [ebp - 4], 0
00511615 7416                     je         0x51162d
00511617 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051161a 52                       push       edx
0051161b 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00511621 e89ae1f0ff               call       0x41f7c0
00511626 c745fc00000000           mov        dword ptr [ebp - 4], 0
0051162d 33c0                     xor        eax, eax
0051162f 8be5                     mov        esp, ebp
00511631 5d                       pop        ebp
00511632 c3                       ret        
00511633 cc                       int3       
00511634 cc                       int3       
00511635 cc                       int3       
00511636 cc                       int3       
00511637 cc                       int3       
00511638 cc                       int3       
00511639 cc                       int3       
0051163a cc                       int3       
0051163b cc                       int3       
0051163c cc                       int3       
0051163d cc                       int3       
0051163e cc                       int3       
0051163f cc                       int3       
