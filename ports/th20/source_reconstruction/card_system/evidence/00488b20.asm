00488b20 55                       push       ebp
00488b21 8bec                     mov        ebp, esp
00488b23 51                       push       ecx
00488b24 68bcfd5600               push       0x56fdbc
00488b29 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00488b2f e84cdfffff               call       0x486a80
00488b34 8945fc                   mov        dword ptr [ebp - 4], eax
00488b37 837dfc00                 cmp        dword ptr [ebp - 4], 0
00488b3b 7504                     jne        0x488b41
00488b3d 33c0                     xor        eax, eax
00488b3f eb37                     jmp        0x488b78
00488b41 8b4508                   mov        eax, dword ptr [ebp + 8]
00488b44 50                       push       eax
00488b45 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00488b48 e8c3efffff               call       0x487b10
00488b4d 85c0                     test       eax, eax
00488b4f 7404                     je         0x488b55
00488b51 eb07                     jmp        0x488b5a
00488b53 eb05                     jmp        0x488b5a
00488b55 8b45fc                   mov        eax, dword ptr [ebp - 4]
00488b58 eb1e                     jmp        0x488b78
00488b5a 837dfc00                 cmp        dword ptr [ebp - 4], 0
00488b5e 7416                     je         0x488b76
00488b60 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00488b63 51                       push       ecx
00488b64 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00488b6a e8516cf9ff               call       0x41f7c0
00488b6f c745fc00000000           mov        dword ptr [ebp - 4], 0
00488b76 33c0                     xor        eax, eax
00488b78 8be5                     mov        esp, ebp
00488b7a 5d                       pop        ebp
00488b7b c3                       ret        
00488b7c cc                       int3       
00488b7d cc                       int3       
00488b7e cc                       int3       
00488b7f cc                       int3       
