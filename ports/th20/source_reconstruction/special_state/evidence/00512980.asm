00512980 55                       push       ebp
00512981 8bec                     mov        ebp, esp
00512983 83ec50                   sub        esp, 0x50
00512986 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0051298b 33c5                     xor        eax, ebp
0051298d 8945fc                   mov        dword ptr [ebp - 4], eax
00512990 894dc0                   mov        dword ptr [ebp - 0x40], ecx
00512993 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00512996 83c114                   add        ecx, 0x14
00512999 e82281f9ff               call       0x4aaac0
0051299e 85c0                     test       eax, eax
005129a0 750a                     jne        0x5129ac
005129a2 b801000000               mov        eax, 1
005129a7 e9e3000000               jmp        0x512a8f
005129ac 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
005129af 83c01c                   add        eax, 0x1c
005129b2 8945bc                   mov        dword ptr [ebp - 0x44], eax
005129b5 6a3c                     push       0x3c
005129b7 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
005129ba e8b1e6f4ff               call       0x461070
005129bf 0fb6c8                   movzx      ecx, al
005129c2 85c9                     test       ecx, ecx
005129c4 0f84b0000000             je         0x512a7a
005129ca 8d4dc4                   lea        ecx, [ebp - 0x3c]
005129cd e85e90f6ff               call       0x47ba30
005129d2 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
005129d5 8b422c                   mov        eax, dword ptr [edx + 0x2c]
005129d8 8b0c85380a5b00           mov        ecx, dword ptr [eax*4 + 0x5b0a38]
005129df 51                       push       ecx
005129e0 8d4de4                   lea        ecx, [ebp - 0x1c]
005129e3 e868acf4ff               call       0x45d650
005129e8 b9a8a45b00               mov        ecx, 0x5ba4a8
005129ed e8ee14f1ff               call       0x423ee0
005129f2 33d2                     xor        edx, edx
005129f4 b940000000               mov        ecx, 0x40
005129f9 f7f1                     div        ecx
005129fb 0fb645e6                 movzx      eax, byte ptr [ebp - 0x1a]
005129ff 2bc2                     sub        eax, edx
00512a01 8845e6                   mov        byte ptr [ebp - 0x1a], al
00512a04 b9a8a45b00               mov        ecx, 0x5ba4a8
00512a09 e8d214f1ff               call       0x423ee0
00512a0e 33d2                     xor        edx, edx
00512a10 b940000000               mov        ecx, 0x40
00512a15 f7f1                     div        ecx
00512a17 0fb645e5                 movzx      eax, byte ptr [ebp - 0x1b]
00512a1b 2bc2                     sub        eax, edx
00512a1d 8845e5                   mov        byte ptr [ebp - 0x1b], al
00512a20 b9a8a45b00               mov        ecx, 0x5ba4a8
00512a25 e8b614f1ff               call       0x423ee0
00512a2a 33d2                     xor        edx, edx
00512a2c b940000000               mov        ecx, 0x40
00512a31 f7f1                     div        ecx
00512a33 0fb645e4                 movzx      eax, byte ptr [ebp - 0x1c]
00512a37 2bc2                     sub        eax, edx
00512a39 8845e4                   mov        byte ptr [ebp - 0x1c], al
00512a3c 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00512a3f 83c114                   add        ecx, 0x14
00512a42 e86983f9ff               call       0x4aadb0
00512a47 8b08                     mov        ecx, dword ptr [eax]
00512a49 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
00512a4c 8b5004                   mov        edx, dword ptr [eax + 4]
00512a4f 8955c8                   mov        dword ptr [ebp - 0x38], edx
00512a52 8b4008                   mov        eax, dword ptr [eax + 8]
00512a55 8945cc                   mov        dword ptr [ebp - 0x34], eax
00512a58 6a00                     push       0
00512a5a e8c14af2ff               call       0x437520
00512a5f 83c404                   add        esp, 4
00512a62 8945b8                   mov        dword ptr [ebp - 0x48], eax
00512a65 6a00                     push       0
00512a67 8d4dc4                   lea        ecx, [ebp - 0x3c]
00512a6a 51                       push       ecx
00512a6b 6a0a                     push       0xa
00512a6d 8d55b0                   lea        edx, [ebp - 0x50]
00512a70 52                       push       edx
00512a71 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00512a74 e8f7b0f8ff               call       0x49db70
00512a79 90                       nop        
00512a7a 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
00512a7d 83c01c                   add        eax, 0x1c
00512a80 8945b4                   mov        dword ptr [ebp - 0x4c], eax
00512a83 6a00                     push       0
00512a85 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00512a88 e8b30af1ff               call       0x423540
00512a8d 33c0                     xor        eax, eax
00512a8f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00512a92 33cd                     xor        ecx, ebp
00512a94 e855ff0200               call       0x5429ee
00512a99 8be5                     mov        esp, ebp
00512a9b 5d                       pop        ebp
00512a9c c3                       ret        
00512a9d cc                       int3       
00512a9e cc                       int3       
00512a9f cc                       int3       
