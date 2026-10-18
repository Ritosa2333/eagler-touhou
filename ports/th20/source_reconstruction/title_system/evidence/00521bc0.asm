00521bc0 55                       push       ebp
00521bc1 8bec                     mov        ebp, esp
00521bc3 81ecac000000             sub        esp, 0xac
00521bc9 894dfc                   mov        dword ptr [ebp - 4], ecx
00521bcc b968a55b00               mov        ecx, 0x5ba568
00521bd1 e8ca24f4ff               call       0x4640a0
00521bd6 83f804                   cmp        eax, 4
00521bd9 7d09                     jge        0x521be4
00521bdb c745f43a000000           mov        dword ptr [ebp - 0xc], 0x3a
00521be2 eb07                     jmp        0x521beb
00521be4 c745f43b000000           mov        dword ptr [ebp - 0xc], 0x3b
00521beb 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00521bee 8945f8                   mov        dword ptr [ebp - 8], eax
00521bf1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521bf4 8b5120                   mov        edx, dword ptr [ecx + 0x20]
00521bf7 8955f0                   mov        dword ptr [ebp - 0x10], edx
00521bfa 837df004                 cmp        dword ptr [ebp - 0x10], 4
00521bfe 0f87b2070000             ja         0x5223b6
00521c04 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00521c07 ff2485c0235200           jmp        dword ptr [eax*4 + 0x5223c0]
00521c0e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521c11 81c190030000             add        ecx, 0x390
00521c17 894dd0                   mov        dword ptr [ebp - 0x30], ecx
00521c1a 6a00                     push       0
00521c1c 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00521c1f e80c78f2ff               call       0x449430
00521c24 0fb6d0                   movzx      edx, al
00521c27 85d2                     test       edx, edx
00521c29 7430                     je         0x521c5b
00521c2b 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00521c31 e81a5df1ff               call       0x437950
00521c36 8945cc                   mov        dword ptr [ebp - 0x34], eax
00521c39 6a00                     push       0
00521c3b 6aff                     push       -1
00521c3d 6a13                     push       0x13
00521c3f 6a00                     push       0
00521c41 8d8574ffffff             lea        eax, [ebp - 0x8c]
00521c47 50                       push       eax
00521c48 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00521c4b e820f0f2ff               call       0x450c70
00521c50 8b08                     mov        ecx, dword ptr [eax]
00521c52 8b55fc                   mov        edx, dword ptr [ebp - 4]
00521c55 898a90030000             mov        dword ptr [edx + 0x390], ecx
00521c5b 8b45fc                   mov        eax, dword ptr [ebp - 4]
00521c5e 83c024                   add        eax, 0x24
00521c61 8945c8                   mov        dword ptr [ebp - 0x38], eax
00521c64 b968a55b00               mov        ecx, 0x5ba568
00521c69 e83224f4ff               call       0x4640a0
00521c6e 83f804                   cmp        eax, 4
00521c71 7d09                     jge        0x521c7c
00521c73 c745ec04000000           mov        dword ptr [ebp - 0x14], 4
00521c7a eb07                     jmp        0x521c83
00521c7c c745ec01000000           mov        dword ptr [ebp - 0x14], 1
00521c83 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00521c86 51                       push       ecx
00521c87 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00521c8a e851a8f7ff               call       0x49c4e0
00521c8f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00521c92 83c224                   add        edx, 0x24
00521c95 8955c4                   mov        dword ptr [ebp - 0x3c], edx
00521c98 6a00                     push       0
00521c9a 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00521c9d e83edff9ff               call       0x4bfbe0
00521ca2 8b45f8                   mov        eax, dword ptr [ebp - 8]
00521ca5 50                       push       eax
00521ca6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521ca9 e882af0000               call       0x52cc30
00521cae 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00521cb1 51                       push       ecx
00521cb2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521cb5 e8c6b20000               call       0x52cf80
00521cba 8b55f8                   mov        edx, dword ptr [ebp - 8]
00521cbd 52                       push       edx
00521cbe 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521cc1 e8caaf0000               call       0x52cc90
00521cc6 8b45fc                   mov        eax, dword ptr [ebp - 4]
00521cc9 83c024                   add        eax, 0x24
00521ccc 8945c0                   mov        dword ptr [ebp - 0x40], eax
00521ccf 6a07                     push       7
00521cd1 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00521cd4 e8a7c3ffff               call       0x51e080
00521cd9 83c006                   add        eax, 6
00521cdc 0fbfc8                   movsx      ecx, ax
00521cdf 51                       push       ecx
00521ce0 8b55f8                   mov        edx, dword ptr [ebp - 8]
00521ce3 52                       push       edx
00521ce4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521ce7 e8e4af0000               call       0x52ccd0
00521cec 90                       nop        
00521ced 833d28615c0004           cmp        dword ptr [0x5c6128], 4
00521cf4 0f858e000000             jne        0x521d88
00521cfa b968a55b00               mov        ecx, 0x5ba568
00521cff e89c23f4ff               call       0x4640a0
00521d04 50                       push       eax
00521d05 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521d08 83c124                   add        ecx, 0x24
00521d0b e840d0f9ff               call       0x4bed50
00521d10 8b45f8                   mov        eax, dword ptr [ebp - 8]
00521d13 50                       push       eax
00521d14 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521d17 e874af0000               call       0x52cc90
00521d1c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521d1f 83c124                   add        ecx, 0x24
00521d22 894dbc                   mov        dword ptr [ebp - 0x44], ecx
00521d25 6a07                     push       7
00521d27 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00521d2a e851c3ffff               call       0x51e080
00521d2f 0fbfd0                   movsx      edx, ax
00521d32 52                       push       edx
00521d33 8b45f8                   mov        eax, dword ptr [ebp - 8]
00521d36 50                       push       eax
00521d37 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521d3a e8d1af0000               call       0x52cd10
00521d3f 6a06                     push       6
00521d41 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00521d44 51                       push       ecx
00521d45 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521d48 e8c3af0000               call       0x52cd10
00521d4d 90                       nop        
00521d4e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521d51 83c124                   add        ecx, 0x24
00521d54 e8a7a5eeff               call       0x40c300
00521d59 83c030                   add        eax, 0x30
00521d5c 50                       push       eax
00521d5d 8b55f8                   mov        edx, dword ptr [ebp - 8]
00521d60 52                       push       edx
00521d61 8d8570ffffff             lea        eax, [ebp - 0x90]
00521d67 50                       push       eax
00521d68 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521d6b e8d0a40000               call       0x52c240
00521d70 8945b8                   mov        dword ptr [ebp - 0x48], eax
00521d73 6a02                     push       2
00521d75 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00521d78 e813d2f2ff               call       0x44ef90
00521d7d 90                       nop        
00521d7e e969040000               jmp        0x5221ec
00521d83 e964040000               jmp        0x5221ec
00521d88 6a22                     push       0x22
00521d8a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521d8d e8eeb10000               call       0x52cf80
00521d92 6a01                     push       1
00521d94 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521d97 e8b4b00000               call       0x52ce50
00521d9c 90                       nop        
00521d9d b968a55b00               mov        ecx, 0x5ba568
00521da2 e8f922f4ff               call       0x4640a0
00521da7 83f804                   cmp        eax, 4
00521daa 0f8dc6000000             jge        0x521e76
00521db0 6a00                     push       0
00521db2 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00521db8 e883a80000               call       0x52c640
00521dbd 0fb6c8                   movzx      ecx, al
00521dc0 85c9                     test       ecx, ecx
00521dc2 751d                     jne        0x521de1
00521dc4 6a11                     push       0x11
00521dc6 8b55f8                   mov        edx, dword ptr [ebp - 8]
00521dc9 52                       push       edx
00521dca 8d856cffffff             lea        eax, [ebp - 0x94]
00521dd0 50                       push       eax
00521dd1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521dd4 e867a40000               call       0x52c240
00521dd9 8bc8                     mov        ecx, eax
00521ddb e840e3f2ff               call       0x450120
00521de0 90                       nop        
00521de1 6a01                     push       1
00521de3 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00521de9 e852a80000               call       0x52c640
00521dee 0fb6c8                   movzx      ecx, al
00521df1 85c9                     test       ecx, ecx
00521df3 751d                     jne        0x521e12
00521df5 6a12                     push       0x12
00521df7 8b55f8                   mov        edx, dword ptr [ebp - 8]
00521dfa 52                       push       edx
00521dfb 8d8568ffffff             lea        eax, [ebp - 0x98]
00521e01 50                       push       eax
00521e02 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521e05 e836a40000               call       0x52c240
00521e0a 8bc8                     mov        ecx, eax
00521e0c e80fe3f2ff               call       0x450120
00521e11 90                       nop        
00521e12 6a02                     push       2
00521e14 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00521e1a e821a80000               call       0x52c640
00521e1f 0fb6c8                   movzx      ecx, al
00521e22 85c9                     test       ecx, ecx
00521e24 751d                     jne        0x521e43
00521e26 6a13                     push       0x13
00521e28 8b55f8                   mov        edx, dword ptr [ebp - 8]
00521e2b 52                       push       edx
00521e2c 8d8564ffffff             lea        eax, [ebp - 0x9c]
00521e32 50                       push       eax
00521e33 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521e36 e805a40000               call       0x52c240
00521e3b 8bc8                     mov        ecx, eax
00521e3d e8dee2f2ff               call       0x450120
00521e42 90                       nop        
00521e43 6a03                     push       3
00521e45 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00521e4b e8f0a70000               call       0x52c640
00521e50 0fb6c8                   movzx      ecx, al
00521e53 85c9                     test       ecx, ecx
00521e55 751d                     jne        0x521e74
00521e57 6a14                     push       0x14
00521e59 8b55f8                   mov        edx, dword ptr [ebp - 8]
00521e5c 52                       push       edx
00521e5d 8d8560ffffff             lea        eax, [ebp - 0xa0]
00521e63 50                       push       eax
00521e64 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521e67 e8d4a30000               call       0x52c240
00521e6c 8bc8                     mov        ecx, eax
00521e6e e8ade2f2ff               call       0x450120
00521e73 90                       nop        
00521e74 eb31                     jmp        0x521ea7
00521e76 6a04                     push       4
00521e78 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00521e7e e8bda70000               call       0x52c640
00521e83 0fb6c8                   movzx      ecx, al
00521e86 85c9                     test       ecx, ecx
00521e88 751d                     jne        0x521ea7
00521e8a 6a15                     push       0x15
00521e8c 8b55f8                   mov        edx, dword ptr [ebp - 8]
00521e8f 52                       push       edx
00521e90 8d855cffffff             lea        eax, [ebp - 0xa4]
00521e96 50                       push       eax
00521e97 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521e9a e8a1a30000               call       0x52c240
00521e9f 8bc8                     mov        ecx, eax
00521ea1 e87ae2f2ff               call       0x450120
00521ea6 90                       nop        
00521ea7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521eaa 81c154010000             add        ecx, 0x154
00521eb0 894db4                   mov        dword ptr [ebp - 0x4c], ecx
00521eb3 6a06                     push       6
00521eb5 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00521eb8 e8b3f1f3ff               call       0x461070
00521ebd 0fb6d0                   movzx      edx, al
00521ec0 85d2                     test       edx, edx
00521ec2 740b                     je         0x521ecf
00521ec4 6a02                     push       2
00521ec6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521ec9 e882af0000               call       0x52ce50
00521ece 90                       nop        
00521ecf e9e2040000               jmp        0x5223b6
00521ed4 b968a55b00               mov        ecx, 0x5ba568
00521ed9 e8c221f4ff               call       0x4640a0
00521ede 83f804                   cmp        eax, 4
00521ee1 0f8d8c010000             jge        0x522073
00521ee7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521eea 83c124                   add        ecx, 0x24
00521eed e80eddf9ff               call       0x4bfc00
00521ef2 90                       nop        
00521ef3 33c0                     xor        eax, eax
00521ef5 7531                     jne        0x521f28
00521ef7 b904000000               mov        ecx, 4
00521efc 6bd100                   imul       edx, ecx, 0
00521eff 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00521f06 7420                     je         0x521f28
00521f08 b804000000               mov        eax, 4
00521f0d 6bc800                   imul       ecx, eax, 0
00521f10 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00521f16 8955b0                   mov        dword ptr [ebp - 0x50], edx
00521f19 6a10                     push       0x10
00521f1b 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00521f1e e85d83efff               call       0x41a280
00521f23 8945e8                   mov        dword ptr [ebp - 0x18], eax
00521f26 eb07                     jmp        0x521f2f
00521f28 c745e800000000           mov        dword ptr [ebp - 0x18], 0
00521f2f 837de800                 cmp        dword ptr [ebp - 0x18], 0
00521f33 7542                     jne        0x521f77
00521f35 33c0                     xor        eax, eax
00521f37 7531                     jne        0x521f6a
00521f39 b904000000               mov        ecx, 4
00521f3e 6bd100                   imul       edx, ecx, 0
00521f41 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00521f48 7420                     je         0x521f6a
00521f4a b804000000               mov        eax, 4
00521f4f 6bc800                   imul       ecx, eax, 0
00521f52 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00521f58 8955ac                   mov        dword ptr [ebp - 0x54], edx
00521f5b 6a40                     push       0x40
00521f5d 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00521f60 e81b83efff               call       0x41a280
00521f65 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00521f68 eb07                     jmp        0x521f71
00521f6a c745e400000000           mov        dword ptr [ebp - 0x1c], 0
00521f71 837de400                 cmp        dword ptr [ebp - 0x1c], 0
00521f75 7414                     je         0x521f8b
00521f77 8b45fc                   mov        eax, dword ptr [ebp - 4]
00521f7a 83c024                   add        eax, 0x24
00521f7d 8945a8                   mov        dword ptr [ebp - 0x58], eax
00521f80 6a00                     push       0
00521f82 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00521f85 e8b6d0f9ff               call       0x4bf040
00521f8a 90                       nop        
00521f8b 33c9                     xor        ecx, ecx
00521f8d 7531                     jne        0x521fc0
00521f8f ba04000000               mov        edx, 4
00521f94 6bc200                   imul       eax, edx, 0
00521f97 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
00521f9e 7420                     je         0x521fc0
00521fa0 b904000000               mov        ecx, 4
00521fa5 6bd100                   imul       edx, ecx, 0
00521fa8 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
00521fae 8945a4                   mov        dword ptr [ebp - 0x5c], eax
00521fb1 6a20                     push       0x20
00521fb3 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
00521fb6 e8c582efff               call       0x41a280
00521fbb 8945e0                   mov        dword ptr [ebp - 0x20], eax
00521fbe eb07                     jmp        0x521fc7
00521fc0 c745e000000000           mov        dword ptr [ebp - 0x20], 0
00521fc7 837de000                 cmp        dword ptr [ebp - 0x20], 0
00521fcb 7545                     jne        0x522012
00521fcd 33c9                     xor        ecx, ecx
00521fcf 7534                     jne        0x522005
00521fd1 ba04000000               mov        edx, 4
00521fd6 6bc200                   imul       eax, edx, 0
00521fd9 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
00521fe0 7423                     je         0x522005
00521fe2 b904000000               mov        ecx, 4
00521fe7 6bd100                   imul       edx, ecx, 0
00521fea 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
00521ff0 8945a0                   mov        dword ptr [ebp - 0x60], eax
00521ff3 6880000000               push       0x80
00521ff8 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00521ffb e88082efff               call       0x41a280
00522000 8945dc                   mov        dword ptr [ebp - 0x24], eax
00522003 eb07                     jmp        0x52200c
00522005 c745dc00000000           mov        dword ptr [ebp - 0x24], 0
0052200c 837ddc00                 cmp        dword ptr [ebp - 0x24], 0
00522010 7414                     je         0x522026
00522012 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522015 83c124                   add        ecx, 0x24
00522018 894d9c                   mov        dword ptr [ebp - 0x64], ecx
0052201b 6a00                     push       0
0052201d 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
00522020 e8fbcff9ff               call       0x4bf020
00522025 90                       nop        
00522026 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522029 83c124                   add        ecx, 0x24
0052202c e8ffaff3ff               call       0x45d030
00522031 85c0                     test       eax, eax
00522033 743e                     je         0x522073
00522035 6a00                     push       0
00522037 6a0a                     push       0xa
00522039 b930a85b00               mov        ecx, 0x5ba830
0052203e e82d4df0ff               call       0x426d70
00522043 8b55f8                   mov        edx, dword ptr [ebp - 8]
00522046 52                       push       edx
00522047 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052204a e841ac0000               call       0x52cc90
0052204f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00522052 83c024                   add        eax, 0x24
00522055 894598                   mov        dword ptr [ebp - 0x68], eax
00522058 6a07                     push       7
0052205a 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
0052205d e81ec0ffff               call       0x51e080
00522062 0fbfc8                   movsx      ecx, ax
00522065 51                       push       ecx
00522066 8b55f8                   mov        edx, dword ptr [ebp - 8]
00522069 52                       push       edx
0052206a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052206d e85eac0000               call       0x52ccd0
00522072 90                       nop        
00522073 33c0                     xor        eax, eax
00522075 7534                     jne        0x5220ab
00522077 b904000000               mov        ecx, 4
0052207c 6bd100                   imul       edx, ecx, 0
0052207f 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00522086 7423                     je         0x5220ab
00522088 b804000000               mov        eax, 4
0052208d 6bc800                   imul       ecx, eax, 0
00522090 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00522096 895594                   mov        dword ptr [ebp - 0x6c], edx
00522099 6806010000               push       0x106
0052209e 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
005220a1 e85a7befff               call       0x419c00
005220a6 8945d8                   mov        dword ptr [ebp - 0x28], eax
005220a9 eb07                     jmp        0x5220b2
005220ab c745d800000000           mov        dword ptr [ebp - 0x28], 0
005220b2 837dd800                 cmp        dword ptr [ebp - 0x28], 0
005220b6 742a                     je         0x5220e2
005220b8 6a04                     push       4
005220ba 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005220bd e88ead0000               call       0x52ce50
005220c2 6a00                     push       0
005220c4 6a09                     push       9
005220c6 b930a85b00               mov        ecx, 0x5ba830
005220cb e8a04cf0ff               call       0x426d70
005220d0 8b45f8                   mov        eax, dword ptr [ebp - 8]
005220d3 50                       push       eax
005220d4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005220d7 e854ab0000               call       0x52cc30
005220dc 90                       nop        
005220dd e9d4020000               jmp        0x5223b6
005220e2 33c9                     xor        ecx, ecx
005220e4 7534                     jne        0x52211a
005220e6 ba04000000               mov        edx, 4
005220eb 6bc200                   imul       eax, edx, 0
005220ee 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
005220f5 7423                     je         0x52211a
005220f7 b904000000               mov        ecx, 4
005220fc 6bd100                   imul       edx, ecx, 0
005220ff 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
00522105 894590                   mov        dword ptr [ebp - 0x70], eax
00522108 6801000800               push       0x80001
0052210d 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00522110 e8eb7aefff               call       0x419c00
00522115 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00522118 eb07                     jmp        0x522121
0052211a c745d400000000           mov        dword ptr [ebp - 0x2c], 0
00522121 837dd400                 cmp        dword ptr [ebp - 0x2c], 0
00522125 0f8491000000             je         0x5221bc
0052212b 6a06                     push       6
0052212d 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00522130 51                       push       ecx
00522131 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522134 e897ab0000               call       0x52ccd0
00522139 90                       nop        
0052213a b968a55b00               mov        ecx, 0x5ba568
0052213f e85c1ff4ff               call       0x4640a0
00522144 83f804                   cmp        eax, 4
00522147 7d32                     jge        0x52217b
00522149 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052214c 83c124                   add        ecx, 0x24
0052214f e8aca1eeff               call       0x40c300
00522154 83c030                   add        eax, 0x30
00522157 50                       push       eax
00522158 8b55f8                   mov        edx, dword ptr [ebp - 8]
0052215b 52                       push       edx
0052215c 8d8558ffffff             lea        eax, [ebp - 0xa8]
00522162 50                       push       eax
00522163 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522166 e8d5a00000               call       0x52c240
0052216b 89458c                   mov        dword ptr [ebp - 0x74], eax
0052216e 6a02                     push       2
00522170 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00522173 e818cef2ff               call       0x44ef90
00522178 90                       nop        
00522179 eb23                     jmp        0x52219e
0052217b 6a34                     push       0x34
0052217d 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00522180 51                       push       ecx
00522181 8d9554ffffff             lea        edx, [ebp - 0xac]
00522187 52                       push       edx
00522188 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052218b e8b0a00000               call       0x52c240
00522190 894588                   mov        dword ptr [ebp - 0x78], eax
00522193 6a02                     push       2
00522195 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00522198 e8f3cdf2ff               call       0x44ef90
0052219d 90                       nop        
0052219e 6a03                     push       3
005221a0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005221a3 e8a8ac0000               call       0x52ce50
005221a8 6a00                     push       0
005221aa 6a07                     push       7
005221ac b930a85b00               mov        ecx, 0x5ba830
005221b1 e8ba4bf0ff               call       0x426d70
005221b6 90                       nop        
005221b7 e9fa010000               jmp        0x5223b6
005221bc e9f5010000               jmp        0x5223b6
005221c1 8b45fc                   mov        eax, dword ptr [ebp - 4]
005221c4 0554010000               add        eax, 0x154
005221c9 894584                   mov        dword ptr [ebp - 0x7c], eax
005221cc 6a0e                     push       0xe
005221ce 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
005221d1 e8ea13f0ff               call       0x4235c0
005221d6 0fb6c8                   movzx      ecx, al
005221d9 85c9                     test       ecx, ecx
005221db 0f84a5000000             je         0x522286
005221e1 6a22                     push       0x22
005221e3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005221e6 e845aa0000               call       0x52cc30
005221eb 90                       nop        
005221ec 6a06                     push       6
005221ee 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005221f1 e8daac0000               call       0x52ced0
005221f6 90                       nop        
005221f7 b968a55b00               mov        ecx, 0x5ba568
005221fc e89f1ef4ff               call       0x4640a0
00522201 83f804                   cmp        eax, 4
00522204 7d25                     jge        0x52222b
00522206 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522209 83c124                   add        ecx, 0x24
0052220c e8efa0eeff               call       0x40c300
00522211 50                       push       eax
00522212 b968a55b00               mov        ecx, 0x5ba568
00522217 e894bff9ff               call       0x4be1b0
0052221c b968a55b00               mov        ecx, 0x5ba568
00522221 e87a1ef4ff               call       0x4640a0
00522226 a3600a5b00               mov        dword ptr [0x5b0a60], eax
0052222b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052222e 83c124                   add        ecx, 0x24
00522231 e87a3efaff               call       0x4c60b0
00522236 8b55fc                   mov        edx, dword ptr [ebp - 4]
00522239 83c224                   add        edx, 0x24
0052223c 895580                   mov        dword ptr [ebp - 0x80], edx
0052223f 6a01                     push       1
00522241 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00522244 e897d9f9ff               call       0x4bfbe0
00522249 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052224c 83c024                   add        eax, 0x24
0052224f 89857cffffff             mov        dword ptr [ebp - 0x84], eax
00522255 6a02                     push       2
00522257 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
0052225d e87ea2f7ff               call       0x49c4e0
00522262 8b0d40615c00             mov        ecx, dword ptr [0x5c6140]
00522268 51                       push       ecx
00522269 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052226c 83c124                   add        ecx, 0x24
0052226f e8dccaf9ff               call       0x4bed50
00522274 8b1540615c00             mov        edx, dword ptr [0x5c6140]
0052227a 52                       push       edx
0052227b b968a55b00               mov        ecx, 0x5ba568
00522280 e85b81feff               call       0x50a3e0
00522285 90                       nop        
00522286 e92b010000               jmp        0x5223b6
0052228b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052228e 0554010000               add        eax, 0x154
00522293 898578ffffff             mov        dword ptr [ebp - 0x88], eax
00522299 6a06                     push       6
0052229b 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005222a1 e81a13f0ff               call       0x4235c0
005222a6 0fb6c8                   movzx      ecx, al
005222a9 85c9                     test       ecx, ecx
005222ab 0f8405010000             je         0x5223b6
005222b1 6a22                     push       0x22
005222b3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005222b6 e875a90000               call       0x52cc30
005222bb 90                       nop        
005222bc b968a55b00               mov        ecx, 0x5ba568
005222c1 e85ab5f9ff               call       0x4bd820
005222c6 0fb6d0                   movzx      edx, al
005222c9 85d2                     test       edx, edx
005222cb 7458                     je         0x522325
005222cd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005222d0 83c124                   add        ecx, 0x24
005222d3 e828a0eeff               call       0x40c300
005222d8 a3600a5b00               mov        dword ptr [0x5b0a60], eax
005222dd a1600a5b00               mov        eax, dword ptr [0x5b0a60]
005222e2 50                       push       eax
005222e3 b968a55b00               mov        ecx, 0x5ba568
005222e8 e8c3bef9ff               call       0x4be1b0
005222ed 6a01                     push       1
005222ef 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005222f2 e8d9ab0000               call       0x52ced0
005222f7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005222fa 81c190030000             add        ecx, 0x390
00522300 e83b6df5ff               call       0x479040
00522305 6a00                     push       0
00522307 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052230a 81c190030000             add        ecx, 0x390
00522310 e88bf4eeff               call       0x4117a0
00522315 6a01                     push       1
00522317 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052231a e8b1ab0000               call       0x52ced0
0052231f 90                       nop        
00522320 e985000000               jmp        0x5223aa
00522325 b968a55b00               mov        ecx, 0x5ba568
0052232a e8711df4ff               call       0x4640a0
0052232f 83f804                   cmp        eax, 4
00522332 7d32                     jge        0x522366
00522334 6a01                     push       1
00522336 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522339 e892ab0000               call       0x52ced0
0052233e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522341 83c124                   add        ecx, 0x24
00522344 e8b79feeff               call       0x40c300
00522349 50                       push       eax
0052234a b968a55b00               mov        ecx, 0x5ba568
0052234f e85cbef9ff               call       0x4be1b0
00522354 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522357 83c124                   add        ecx, 0x24
0052235a e8a19feeff               call       0x40c300
0052235f a3600a5b00               mov        dword ptr [0x5b0a60], eax
00522364 eb44                     jmp        0x5223aa
00522366 8b0d600a5b00             mov        ecx, dword ptr [0x5b0a60]
0052236c 51                       push       ecx
0052236d b968a55b00               mov        ecx, 0x5ba568
00522372 e839bef9ff               call       0x4be1b0
00522377 6a01                     push       1
00522379 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052237c e84fab0000               call       0x52ced0
00522381 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522384 81c190030000             add        ecx, 0x390
0052238a e8b16cf5ff               call       0x479040
0052238f 6a00                     push       0
00522391 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00522394 81c190030000             add        ecx, 0x390
0052239a e801f4eeff               call       0x4117a0
0052239f 6a01                     push       1
005223a1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005223a4 e827ab0000               call       0x52ced0
005223a9 90                       nop        
005223aa 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005223ad 83c124                   add        ecx, 0x24
005223b0 e85b3cfaff               call       0x4c6010
005223b5 90                       nop        
005223b6 b801000000               mov        eax, 1
005223bb 8be5                     mov        esp, ebp
005223bd 5d                       pop        ebp
005223be c3                       ret        
005223bf 90                       nop        
005223c0 0e                       push       cs
005223c1 1c52                     sbb        al, 0x52
005223c3 00a71e5200d4             add        byte ptr [edi - 0x2bffade2], ah
005223c9 1e                       push       ds
005223ca 52                       push       edx
005223cb 00c1                     add        cl, al
005223cd 215200                   and        dword ptr [edx], edx
005223d0 8b22                     mov        esp, dword ptr [edx]
005223d2 52                       push       edx
005223d3 00cc                     add        ah, cl
005223d5 cc                       int3       
005223d6 cc                       int3       
005223d7 cc                       int3       
005223d8 cc                       int3       
005223d9 cc                       int3       
005223da cc                       int3       
005223db cc                       int3       
005223dc cc                       int3       
005223dd cc                       int3       
005223de cc                       int3       
005223df cc                       int3       
