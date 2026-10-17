00539ed0 55                       push       ebp
00539ed1 8bec                     mov        ebp, esp
00539ed3 83ec48                   sub        esp, 0x48
00539ed6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00539edb 33c5                     xor        eax, ebp
00539edd 8945fc                   mov        dword ptr [ebp - 4], eax
00539ee0 894de8                   mov        dword ptr [ebp - 0x18], ecx
00539ee3 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
00539eea c745e000000000           mov        dword ptr [ebp - 0x20], 0
00539ef1 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00539ef4 83780c00                 cmp        dword ptr [eax + 0xc], 0
00539ef8 7507                     jne        0x539f01
00539efa 32c0                     xor        al, al
00539efc e95c020000               jmp        0x53a15d
00539f01 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00539f04 8b510c                   mov        edx, dword ptr [ecx + 0xc]
00539f07 8955d4                   mov        dword ptr [ebp - 0x2c], edx
00539f0a 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00539f0d 8b480c                   mov        ecx, dword ptr [eax + 0xc]
00539f10 8b11                     mov        edx, dword ptr [ecx]
00539f12 8b02                     mov        eax, dword ptr [edx]
00539f14 8945d0                   mov        dword ptr [ebp - 0x30], eax
00539f17 8b0df80a5b00             mov        ecx, dword ptr [0x5b0af8]
00539f1d 51                       push       ecx
00539f1e 8b5508                   mov        edx, dword ptr [ebp + 8]
00539f21 52                       push       edx
00539f22 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00539f25 ff55d0                   call       dword ptr [ebp - 0x30]
00539f28 0fb6c0                   movzx      eax, al
00539f2b 85c0                     test       eax, eax
00539f2d 750a                     jne        0x539f39
00539f2f e9d3010000               jmp        0x53a107
00539f34 e9ce010000               jmp        0x53a107
00539f39 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00539f3c 8b510c                   mov        edx, dword ptr [ecx + 0xc]
00539f3f 8955cc                   mov        dword ptr [ebp - 0x34], edx
00539f42 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00539f45 8b480c                   mov        ecx, dword ptr [eax + 0xc]
00539f48 8b11                     mov        edx, dword ptr [ecx]
00539f4a 8b4208                   mov        eax, dword ptr [edx + 8]
00539f4d 8945c8                   mov        dword ptr [ebp - 0x38], eax
00539f50 6a10                     push       0x10
00539f52 8d4dec                   lea        ecx, [ebp - 0x14]
00539f55 51                       push       ecx
00539f56 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00539f59 ff55c8                   call       dword ptr [ebp - 0x38]
00539f5c 85c0                     test       eax, eax
00539f5e 750a                     jne        0x539f6a
00539f60 e9a2010000               jmp        0x53a107
00539f65 e99d010000               jmp        0x53a107
00539f6a 6a10                     push       0x10
00539f6c 6a10                     push       0x10
00539f6e 6a37                     push       0x37
00539f70 6a1b                     push       0x1b
00539f72 6a10                     push       0x10
00539f74 8d55ec                   lea        edx, [ebp - 0x14]
00539f77 52                       push       edx
00539f78 e86361edff               call       0x4100e0
00539f7d 83c418                   add        esp, 0x18
00539f80 817dec54484131           cmp        dword ptr [ebp - 0x14], 0x31414854
00539f87 740a                     je         0x539f93
00539f89 e979010000               jmp        0x53a107
00539f8e e974010000               jmp        0x53a107
00539f93 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00539f96 2d15cd5b07               sub        eax, 0x75bcd15
00539f9b 8945f0                   mov        dword ptr [ebp - 0x10], eax
00539f9e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00539fa1 81e9b168de3a             sub        ecx, 0x3ade68b1
00539fa7 894df4                   mov        dword ptr [ebp - 0xc], ecx
00539faa 8b55f8                   mov        edx, dword ptr [ebp - 8]
00539fad 81ea54071808             sub        edx, 0x8180754
00539fb3 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00539fb6 895004                   mov        dword ptr [eax + 4], edx
00539fb9 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00539fbc 8b510c                   mov        edx, dword ptr [ecx + 0xc]
00539fbf 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00539fc2 8b12                     mov        edx, dword ptr [edx]
00539fc4 8b480c                   mov        ecx, dword ptr [eax + 0xc]
00539fc7 8b4214                   mov        eax, dword ptr [edx + 0x14]
00539fca ffd0                     call       eax
00539fcc 8945dc                   mov        dword ptr [ebp - 0x24], eax
00539fcf 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00539fd2 2b4df4                   sub        ecx, dword ptr [ebp - 0xc]
00539fd5 894dd8                   mov        dword ptr [ebp - 0x28], ecx
00539fd8 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00539fdb 8b420c                   mov        eax, dword ptr [edx + 0xc]
00539fde 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00539fe1 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00539fe4 8b510c                   mov        edx, dword ptr [ecx + 0xc]
00539fe7 8b02                     mov        eax, dword ptr [edx]
00539fe9 8b4818                   mov        ecx, dword ptr [eax + 0x18]
00539fec 894dc0                   mov        dword ptr [ebp - 0x40], ecx
00539fef 8b15d4675700             mov        edx, dword ptr [0x5767d4]
00539ff5 52                       push       edx
00539ff6 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00539ff9 50                       push       eax
00539ffa 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00539ffd ff55c0                   call       dword ptr [ebp - 0x40]
0053a000 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0053a003 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0053a006 6860695700               push       0x576960
0053a00b 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0053a00e 52                       push       edx
0053a00f 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0053a015 e8f655eeff               call       0x41f610
0053a01a 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0053a01d 837de400                 cmp        dword ptr [ebp - 0x1c], 0
0053a021 750a                     jne        0x53a02d
0053a023 e9df000000               jmp        0x53a107
0053a028 e9da000000               jmp        0x53a107
0053a02d 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0053a030 8b480c                   mov        ecx, dword ptr [eax + 0xc]
0053a033 894dbc                   mov        dword ptr [ebp - 0x44], ecx
0053a036 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0053a039 8b420c                   mov        eax, dword ptr [edx + 0xc]
0053a03c 8b08                     mov        ecx, dword ptr [eax]
0053a03e 8b5108                   mov        edx, dword ptr [ecx + 8]
0053a041 8955b8                   mov        dword ptr [ebp - 0x48], edx
0053a044 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0053a047 50                       push       eax
0053a048 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0053a04b 51                       push       ecx
0053a04c 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
0053a04f ff55b8                   call       dword ptr [ebp - 0x48]
0053a052 85c0                     test       eax, eax
0053a054 750a                     jne        0x53a060
0053a056 e9ac000000               jmp        0x53a107
0053a05b e9a7000000               jmp        0x53a107
0053a060 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0053a063 52                       push       edx
0053a064 6880000000               push       0x80
0053a069 689b000000               push       0x9b
0053a06e 6a3e                     push       0x3e
0053a070 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0053a073 50                       push       eax
0053a074 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0053a077 51                       push       ecx
0053a078 e86360edff               call       0x4100e0
0053a07d 83c418                   add        esp, 0x18
0053a080 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0053a083 52                       push       edx
0053a084 6a00                     push       0
0053a086 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0053a089 50                       push       eax
0053a08a 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0053a08d 51                       push       ecx
0053a08e e85df1ffff               call       0x5391f0
0053a093 83c410                   add        esp, 0x10
0053a096 8945e0                   mov        dword ptr [ebp - 0x20], eax
0053a099 837de000                 cmp        dword ptr [ebp - 0x20], 0
0053a09d 7504                     jne        0x53a0a3
0053a09f eb66                     jmp        0x53a107
0053a0a1 eb64                     jmp        0x53a107
0053a0a3 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
0053a0a6 52                       push       edx
0053a0a7 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0053a0aa 8b4804                   mov        ecx, dword ptr [eax + 4]
0053a0ad 51                       push       ecx
0053a0ae 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0053a0b1 52                       push       edx
0053a0b2 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0053a0b5 e856010000               call       0x53a210
0053a0ba 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0053a0bd 8901                     mov        dword ptr [ecx], eax
0053a0bf 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0053a0c2 833a00                   cmp        dword ptr [edx], 0
0053a0c5 7504                     jne        0x53a0cb
0053a0c7 eb3e                     jmp        0x53a107
0053a0c9 eb3c                     jmp        0x53a107
0053a0cb 837de400                 cmp        dword ptr [ebp - 0x1c], 0
0053a0cf 7416                     je         0x53a0e7
0053a0d1 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0053a0d4 50                       push       eax
0053a0d5 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0053a0db e89055eeff               call       0x41f670
0053a0e0 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
0053a0e7 837de000                 cmp        dword ptr [ebp - 0x20], 0
0053a0eb 7416                     je         0x53a103
0053a0ed 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0053a0f0 51                       push       ecx
0053a0f1 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0053a0f7 e87455eeff               call       0x41f670
0053a0fc c745e000000000           mov        dword ptr [ebp - 0x20], 0
0053a103 b001                     mov        al, 1
0053a105 eb56                     jmp        0x53a15d
0053a107 837de400                 cmp        dword ptr [ebp - 0x1c], 0
0053a10b 7416                     je         0x53a123
0053a10d 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0053a110 52                       push       edx
0053a111 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0053a117 e85455eeff               call       0x41f670
0053a11c c745e400000000           mov        dword ptr [ebp - 0x1c], 0
0053a123 837de000                 cmp        dword ptr [ebp - 0x20], 0
0053a127 7416                     je         0x53a13f
0053a129 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0053a12c 50                       push       eax
0053a12d 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0053a133 e83855eeff               call       0x41f670
0053a138 c745e000000000           mov        dword ptr [ebp - 0x20], 0
0053a13f 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0053a142 8b510c                   mov        edx, dword ptr [ecx + 0xc]
0053a145 52                       push       edx
0053a146 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0053a14c e8cff9ffff               call       0x539b20
0053a151 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0053a154 c7400c00000000           mov        dword ptr [eax + 0xc], 0
0053a15b 32c0                     xor        al, al
0053a15d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053a160 33cd                     xor        ecx, ebp
0053a162 e887880000               call       0x5429ee
0053a167 8be5                     mov        esp, ebp
0053a169 5d                       pop        ebp
0053a16a c20400                   ret        4
0053a16d cc                       int3       
0053a16e cc                       int3       
0053a16f cc                       int3       
