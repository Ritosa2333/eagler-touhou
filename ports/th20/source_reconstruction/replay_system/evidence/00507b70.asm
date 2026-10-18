00507b70 55                       push       ebp
00507b71 8bec                     mov        ebp, esp
00507b73 83ec44                   sub        esp, 0x44
00507b76 56                       push       esi
00507b77 894dfc                   mov        dword ptr [ebp - 4], ecx
00507b7a 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
00507b81 750a                     jne        0x507b8d
00507b83 b801000000               mov        eax, 1
00507b88 e9d9030000               jmp        0x507f66
00507b8d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507b90 e8fbebfdff               call       0x4e6790
00507b95 85c0                     test       eax, eax
00507b97 740a                     je         0x507ba3
00507b99 b801000000               mov        eax, 1
00507b9e e9c3030000               jmp        0x507f66
00507ba3 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507ba6 05e8000000               add        eax, 0xe8
00507bab 8945ec                   mov        dword ptr [ebp - 0x14], eax
00507bae 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507bb1 8b9158020000             mov        edx, dword ptr [ecx + 0x258]
00507bb7 52                       push       edx
00507bb8 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00507bbb e870f2f7ff               call       0x486e30
00507bc0 83781400                 cmp        dword ptr [eax + 0x14], 0
00507bc4 7d40                     jge        0x507c06
00507bc6 b8c0020000               mov        eax, 0x2c0
00507bcb 6bc800                   imul       ecx, eax, 0
00507bce c7814c8b5b0000000000     mov        dword ptr [ecx + 0x5b8b4c], 0
00507bd8 bac0020000               mov        edx, 0x2c0
00507bdd 6bc200                   imul       eax, edx, 0
00507be0 c780588b5b0000000000     mov        dword ptr [eax + 0x5b8b58], 0
00507bea b9c0020000               mov        ecx, 0x2c0
00507bef 6bd100                   imul       edx, ecx, 0
00507bf2 c7825c8b5b0000000000     mov        dword ptr [edx + 0x5b8b5c], 0
00507bfc b801000000               mov        eax, 1
00507c01 e960030000               jmp        0x507f66
00507c06 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507c09 83b85802000000           cmp        dword ptr [eax + 0x258], 0
00507c10 0f8c00030000             jl         0x507f16
00507c16 b9c0020000               mov        ecx, 0x2c0
00507c1b 6bd100                   imul       edx, ecx, 0
00507c1e b8c0020000               mov        eax, 0x2c0
00507c23 6bc800                   imul       ecx, eax, 0
00507c26 8b924c8b5b00             mov        edx, dword ptr [edx + 0x5b8b4c]
00507c2c 8991508b5b00             mov        dword ptr [ecx + 0x5b8b50], edx
00507c32 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507c35 05e8000000               add        eax, 0xe8
00507c3a 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00507c3d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507c40 81c1e8000000             add        ecx, 0xe8
00507c46 894de8                   mov        dword ptr [ebp - 0x18], ecx
00507c49 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507c4c 8b8258020000             mov        eax, dword ptr [edx + 0x258]
00507c52 50                       push       eax
00507c53 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00507c56 e8d5f1f7ff               call       0x486e30
00507c5b 8b7010                   mov        esi, dword ptr [eax + 0x10]
00507c5e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507c61 8b9158020000             mov        edx, dword ptr [ecx + 0x258]
00507c67 52                       push       edx
00507c68 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00507c6b e8c0f1f7ff               call       0x486e30
00507c70 8b4014                   mov        eax, dword ptr [eax + 0x14]
00507c73 3b4608                   cmp        eax, dword ptr [esi + 8]
00507c76 0f8d31020000             jge        0x507ead
00507c7c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507c7f 81c1e8000000             add        ecx, 0xe8
00507c85 894de0                   mov        dword ptr [ebp - 0x20], ecx
00507c88 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507c8b 8b8258020000             mov        eax, dword ptr [edx + 0x258]
00507c91 50                       push       eax
00507c92 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00507c95 e896f1f7ff               call       0x486e30
00507c9a 8b4804                   mov        ecx, dword ptr [eax + 4]
00507c9d 0fb711                   movzx      edx, word ptr [ecx]
00507ca0 81faffff0000             cmp        edx, 0xffff
00507ca6 0f85b4000000             jne        0x507d60
00507cac 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507caf 05e8000000               add        eax, 0xe8
00507cb4 8945dc                   mov        dword ptr [ebp - 0x24], eax
00507cb7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507cba 8b9158020000             mov        edx, dword ptr [ecx + 0x258]
00507cc0 52                       push       edx
00507cc1 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00507cc4 e867f1f7ff               call       0x486e30
00507cc9 8b4004                   mov        eax, dword ptr [eax + 4]
00507ccc 0fb74802                 movzx      ecx, word ptr [eax + 2]
00507cd0 81f9ffff0000             cmp        ecx, 0xffff
00507cd6 0f8584000000             jne        0x507d60
00507cdc 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507cdf 81c2e8000000             add        edx, 0xe8
00507ce5 8955d8                   mov        dword ptr [ebp - 0x28], edx
00507ce8 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507ceb 8b8858020000             mov        ecx, dword ptr [eax + 0x258]
00507cf1 51                       push       ecx
00507cf2 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00507cf5 e836f1f7ff               call       0x486e30
00507cfa 8b5004                   mov        edx, dword ptr [eax + 4]
00507cfd 0fb74204                 movzx      eax, word ptr [edx + 4]
00507d01 3dffff0000               cmp        eax, 0xffff
00507d06 7558                     jne        0x507d60
00507d08 b9c0020000               mov        ecx, 0x2c0
00507d0d 6bd100                   imul       edx, ecx, 0
00507d10 c7824c8b5b0000000000     mov        dword ptr [edx + 0x5b8b4c], 0
00507d1a b8c0020000               mov        eax, 0x2c0
00507d1f 6bc800                   imul       ecx, eax, 0
00507d22 c781588b5b0000000000     mov        dword ptr [ecx + 0x5b8b58], 0
00507d2c bac0020000               mov        edx, 0x2c0
00507d31 6bc200                   imul       eax, edx, 0
00507d34 c7805c8b5b0000000000     mov        dword ptr [eax + 0x5b8b5c], 0
00507d3e 8b0dbc605c00             mov        ecx, dword ptr [0x5c60bc]
00507d44 e8e7e1fdff               call       0x4e5f30
00507d49 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507d4c c78158020000ffffffff     mov        dword ptr [ecx + 0x258], 0xffffffff
00507d56 b801000000               mov        eax, 1
00507d5b e906020000               jmp        0x507f66
00507d60 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507d63 81c2e8000000             add        edx, 0xe8
00507d69 8955d4                   mov        dword ptr [ebp - 0x2c], edx
00507d6c 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507d6f 8b8858020000             mov        ecx, dword ptr [eax + 0x258]
00507d75 51                       push       ecx
00507d76 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00507d79 e8b2f0f7ff               call       0x486e30
00507d7e 8b5004                   mov        edx, dword ptr [eax + 4]
00507d81 0fb702                   movzx      eax, word ptr [edx]
00507d84 b9c0020000               mov        ecx, 0x2c0
00507d89 6bd100                   imul       edx, ecx, 0
00507d8c 89824c8b5b00             mov        dword ptr [edx + 0x5b8b4c], eax
00507d92 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507d95 05e8000000               add        eax, 0xe8
00507d9a 8945d0                   mov        dword ptr [ebp - 0x30], eax
00507d9d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507da0 8b9158020000             mov        edx, dword ptr [ecx + 0x258]
00507da6 52                       push       edx
00507da7 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00507daa e881f0f7ff               call       0x486e30
00507daf 8b4004                   mov        eax, dword ptr [eax + 4]
00507db2 0fb74802                 movzx      ecx, word ptr [eax + 2]
00507db6 bac0020000               mov        edx, 0x2c0
00507dbb 6bc200                   imul       eax, edx, 0
00507dbe 8988588b5b00             mov        dword ptr [eax + 0x5b8b58], ecx
00507dc4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507dc7 81c1e8000000             add        ecx, 0xe8
00507dcd 894dcc                   mov        dword ptr [ebp - 0x34], ecx
00507dd0 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507dd3 8b8258020000             mov        eax, dword ptr [edx + 0x258]
00507dd9 50                       push       eax
00507dda 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00507ddd e84ef0f7ff               call       0x486e30
00507de2 8b4804                   mov        ecx, dword ptr [eax + 4]
00507de5 0fb75104                 movzx      edx, word ptr [ecx + 4]
00507de9 b8c0020000               mov        eax, 0x2c0
00507dee 6bc800                   imul       ecx, eax, 0
00507df1 89915c8b5b00             mov        dword ptr [ecx + 0x5b8b5c], edx
00507df7 bac0020000               mov        edx, 0x2c0
00507dfc 6bca00                   imul       ecx, edx, 0
00507dff 81c1b0885b00             add        ecx, 0x5b88b0
00507e05 e8c6260000               call       0x50a4d0
00507e0a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507e0d 05e8000000               add        eax, 0xe8
00507e12 8945c8                   mov        dword ptr [ebp - 0x38], eax
00507e15 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507e18 8b9158020000             mov        edx, dword ptr [ecx + 0x258]
00507e1e 52                       push       edx
00507e1f 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00507e22 e809f0f7ff               call       0x486e30
00507e27 8b400c                   mov        eax, dword ptr [eax + 0xc]
00507e2a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507e2d 8a10                     mov        dl, byte ptr [eax]
00507e2f 88914c020000             mov        byte ptr [ecx + 0x24c], dl
00507e35 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507e38 05e8000000               add        eax, 0xe8
00507e3d 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00507e40 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507e43 8b9158020000             mov        edx, dword ptr [ecx + 0x258]
00507e49 52                       push       edx
00507e4a 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00507e4d e8deeff7ff               call       0x486e30
00507e52 83c004                   add        eax, 4
00507e55 8945f8                   mov        dword ptr [ebp - 8], eax
00507e58 8b45f8                   mov        eax, dword ptr [ebp - 8]
00507e5b 8b08                     mov        ecx, dword ptr [eax]
00507e5d 83c106                   add        ecx, 6
00507e60 8b55f8                   mov        edx, dword ptr [ebp - 8]
00507e63 890a                     mov        dword ptr [edx], ecx
00507e65 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507e68 8b8050020000             mov        eax, dword ptr [eax + 0x250]
00507e6e 99                       cdq        
00507e6f b91e000000               mov        ecx, 0x1e
00507e74 f7f9                     idiv       ecx
00507e76 85d2                     test       edx, edx
00507e78 7531                     jne        0x507eab
00507e7a 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507e7d 81c2e8000000             add        edx, 0xe8
00507e83 8955c0                   mov        dword ptr [ebp - 0x40], edx
00507e86 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507e89 8b8858020000             mov        ecx, dword ptr [eax + 0x258]
00507e8f 51                       push       ecx
00507e90 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00507e93 e898eff7ff               call       0x486e30
00507e98 83c00c                   add        eax, 0xc
00507e9b 8945f4                   mov        dword ptr [ebp - 0xc], eax
00507e9e 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00507ea1 8b02                     mov        eax, dword ptr [edx]
00507ea3 83c001                   add        eax, 1
00507ea6 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00507ea9 8901                     mov        dword ptr [ecx], eax
00507eab eb36                     jmp        0x507ee3
00507ead bac0020000               mov        edx, 0x2c0
00507eb2 6bc200                   imul       eax, edx, 0
00507eb5 c7804c8b5b0000000000     mov        dword ptr [eax + 0x5b8b4c], 0
00507ebf b9c0020000               mov        ecx, 0x2c0
00507ec4 6bd100                   imul       edx, ecx, 0
00507ec7 c782588b5b0000000000     mov        dword ptr [edx + 0x5b8b58], 0
00507ed1 b8c0020000               mov        eax, 0x2c0
00507ed6 6bc800                   imul       ecx, eax, 0
00507ed9 c7815c8b5b0000000000     mov        dword ptr [ecx + 0x5b8b5c], 0
00507ee3 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507ee6 81c2e8000000             add        edx, 0xe8
00507eec 8955bc                   mov        dword ptr [ebp - 0x44], edx
00507eef 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507ef2 8b8858020000             mov        ecx, dword ptr [eax + 0x258]
00507ef8 51                       push       ecx
00507ef9 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00507efc e82feff7ff               call       0x486e30
00507f01 83c014                   add        eax, 0x14
00507f04 8945f0                   mov        dword ptr [ebp - 0x10], eax
00507f07 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00507f0a 8b02                     mov        eax, dword ptr [edx]
00507f0c 83c001                   add        eax, 1
00507f0f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00507f12 8901                     mov        dword ptr [ecx], eax
00507f14 eb36                     jmp        0x507f4c
00507f16 bac0020000               mov        edx, 0x2c0
00507f1b 6bc200                   imul       eax, edx, 0
00507f1e c7804c8b5b0000000000     mov        dword ptr [eax + 0x5b8b4c], 0
00507f28 b9c0020000               mov        ecx, 0x2c0
00507f2d 6bd100                   imul       edx, ecx, 0
00507f30 c782588b5b0000000000     mov        dword ptr [edx + 0x5b8b58], 0
00507f3a b8c0020000               mov        eax, 0x2c0
00507f3f 6bc800                   imul       ecx, eax, 0
00507f42 c7815c8b5b0000000000     mov        dword ptr [ecx + 0x5b8b5c], 0
00507f4c 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507f4f 8b8250020000             mov        eax, dword ptr [edx + 0x250]
00507f55 83c001                   add        eax, 1
00507f58 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507f5b 898150020000             mov        dword ptr [ecx + 0x250], eax
00507f61 b801000000               mov        eax, 1
00507f66 5e                       pop        esi
00507f67 8be5                     mov        esp, ebp
00507f69 5d                       pop        ebp
00507f6a c3                       ret        
00507f6b cc                       int3       
00507f6c cc                       int3       
00507f6d cc                       int3       
00507f6e cc                       int3       
00507f6f cc                       int3       
00507f70 cc                       int3       
00507f71 cc                       int3       
00507f72 cc                       int3       
00507f73 cc                       int3       
00507f74 cc                       int3       
00507f75 cc                       int3       
00507f76 cc                       int3       
00507f77 cc                       int3       
00507f78 cc                       int3       
00507f79 cc                       int3       
00507f7a cc                       int3       
00507f7b cc                       int3       
00507f7c cc                       int3       
00507f7d cc                       int3       
00507f7e cc                       int3       
00507f7f cc                       int3       
