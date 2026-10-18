00559a70 8bff                     mov        edi, edi
00559a72 55                       push       ebp
00559a73 8bec                     mov        ebp, esp
00559a75 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00559a78 83c8ff                   or         eax, 0xffffffff
00559a7b 8bd1                     mov        edx, ecx
00559a7d c1e916                   shr        ecx, 0x16
00559a80 c1ea0e                   shr        edx, 0xe
00559a83 81e100030000             and        ecx, 0x300
00559a89 81e200030000             and        edx, 0x300
00559a8f 3bd1                     cmp        edx, ecx
