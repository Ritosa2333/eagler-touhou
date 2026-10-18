005108d0 55                       push       ebp
005108d1 8bec                     mov        ebp, esp
005108d3 51                       push       ecx
005108d4 6898395700               push       0x573998
005108d9 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
005108df e81cf4ffff               call       0x50fd00
005108e4 8945fc                   mov        dword ptr [ebp - 4], eax
005108e7 837dfc00                 cmp        dword ptr [ebp - 4], 0
005108eb 7504                     jne        0x5108f1
005108ed 33c0                     xor        eax, eax
005108ef eb37                     jmp        0x510928
005108f1 8b4508                   mov        eax, dword ptr [ebp + 8]
005108f4 50                       push       eax
005108f5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005108f8 e843fdffff               call       0x510640
005108fd 85c0                     test       eax, eax
005108ff 7404                     je         0x510905
00510901 eb07                     jmp        0x51090a
00510903 eb05                     jmp        0x51090a
00510905 8b45fc                   mov        eax, dword ptr [ebp - 4]
00510908 eb1e                     jmp        0x510928
0051090a 837dfc00                 cmp        dword ptr [ebp - 4], 0
0051090e 7416                     je         0x510926
00510910 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00510913 51                       push       ecx
00510914 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0051091a e8a1eef0ff               call       0x41f7c0
0051091f c745fc00000000           mov        dword ptr [ebp - 4], 0
00510926 33c0                     xor        eax, eax
00510928 8be5                     mov        esp, ebp
0051092a 5d                       pop        ebp
0051092b c3                       ret        
0051092c cc                       int3       
0051092d cc                       int3       
0051092e cc                       int3       
0051092f cc                       int3       
