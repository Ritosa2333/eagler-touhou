0046a090  55                                 push       ebp
0046a091  8b ec                              mov        ebp, esp
0046a093  83 ec 0c                           sub        esp, 0xc
0046a096  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0046a09b  33 c5                              xor        eax, ebp
0046a09d  89 45 fc                           mov        dword ptr [ebp - 4], eax
0046a0a0  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
0046a0a3  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046a0a6  83 38 00                           cmp        dword ptr [eax], 0
0046a0a9  75 02                              jne        0x46a0ad
0046a0ab  eb 3a                              jmp        0x46a0e7
0046a0ad  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046a0b0  8b 11                              mov        edx, dword ptr [ecx]
0046a0b2  52                                 push       edx
0046a0b3  e8 58 01 00 00                     call       0x46a210
0046a0b8  83 c4 04                           add        esp, 4
0046a0bb  6a 01                              push       1
0046a0bd  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0046a0c2  e8 49 22 fa ff                     call       0x40c310
0046a0c7  50                                 push       eax
0046a0c8  8d 4d f8                           lea        ecx, [ebp - 8]
0046a0cb  e8 60 16 fa ff                     call       0x40b730
0046a0d0  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046a0d3  8b 08                              mov        ecx, dword ptr [eax]
0046a0d5  51                                 push       ecx
0046a0d6  e8 21 89 0d 00                     call       0x5429fc
0046a0db  83 c4 04                           add        esp, 4
0046a0de  8d 4d f8                           lea        ecx, [ebp - 8]
0046a0e1  e8 1a 18 fa ff                     call       0x40b900
0046a0e6  90                                 nop        
0046a0e7  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046a0ea  33 cd                              xor        ecx, ebp
0046a0ec  e8 fd 88 0d 00                     call       0x5429ee
0046a0f1  8b e5                              mov        esp, ebp
0046a0f3  5d                                 pop        ebp
0046a0f4  c2 04 00                           ret        4
