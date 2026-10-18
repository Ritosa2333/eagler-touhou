00529e80 55                       push       ebp
00529e81 8bec                     mov        ebp, esp
00529e83 83ec5c                   sub        esp, 0x5c
00529e86 894dfc                   mov        dword ptr [ebp - 4], ecx
00529e89 8b45fc                   mov        eax, dword ptr [ebp - 4]
00529e8c 8b4820                   mov        ecx, dword ptr [eax + 0x20]
00529e8f 894df8                   mov        dword ptr [ebp - 8], ecx
00529e92 837df804                 cmp        dword ptr [ebp - 8], 4
00529e96 0f87e8070000             ja         0x52a684
00529e9c 8b55f8                   mov        edx, dword ptr [ebp - 8]
00529e9f ff249590a65200           jmp        dword ptr [edx*4 + 0x52a690]
00529ea6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529ea9 81c190030000             add        ecx, 0x390
00529eaf e88cf1f4ff               call       0x479040
00529eb4 6a00                     push       0
00529eb6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529eb9 81c190030000             add        ecx, 0x390
00529ebf e8dc78eeff               call       0x4117a0
00529ec4 8b45fc                   mov        eax, dword ptr [ebp - 4]
00529ec7 83c024                   add        eax, 0x24
00529eca 8945dc                   mov        dword ptr [ebp - 0x24], eax
00529ecd 6a0a                     push       0xa
00529ecf 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00529ed2 e80926f7ff               call       0x49c4e0
00529ed7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529eda 83c124                   add        ecx, 0x24
00529edd 894dd8                   mov        dword ptr [ebp - 0x28], ecx
00529ee0 6a01                     push       1
00529ee2 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00529ee5 e8f65cf9ff               call       0x4bfbe0
00529eea 90                       nop        
00529eeb 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00529ef1 e8ba280000               call       0x52c7b0
00529ef6 0fb6d0                   movzx      edx, al
00529ef9 85d2                     test       edx, edx
00529efb 7514                     jne        0x529f11
00529efd 8b45fc                   mov        eax, dword ptr [ebp - 4]
00529f00 83c024                   add        eax, 0x24
00529f03 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00529f06 6a01                     push       1
00529f08 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00529f0b e80039ffff               call       0x51d810
00529f10 90                       nop        
00529f11 b968a55b00               mov        ecx, 0x5ba568
00529f16 e835e9f5ff               call       0x488850
00529f1b 0fb6c8                   movzx      ecx, al
00529f1e 85c9                     test       ecx, ecx
00529f20 741c                     je         0x529f3e
00529f22 6a03                     push       3
00529f24 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529f27 83c124                   add        ecx, 0x24
00529f2a e8214ef9ff               call       0x4bed50
00529f2f 6a00                     push       0
00529f31 b968a55b00               mov        ecx, 0x5ba568
00529f36 e86505feff               call       0x50a4a0
00529f3b 90                       nop        
00529f3c eb2b                     jmp        0x529f69
00529f3e b968a55b00               mov        ecx, 0x5ba568
00529f43 e8d838f9ff               call       0x4bd820
00529f48 0fb6d0                   movzx      edx, al
00529f4b 85d2                     test       edx, edx
00529f4d 741a                     je         0x529f69
00529f4f 6a02                     push       2
00529f51 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529f54 83c124                   add        ecx, 0x24
00529f57 e8f44df9ff               call       0x4bed50
00529f5c 6a00                     push       0
00529f5e b968a55b00               mov        ecx, 0x5ba568
00529f63 e83805feff               call       0x50a4a0
00529f68 90                       nop        
00529f69 6a01                     push       1
00529f6b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529f6e e8dd2e0000               call       0x52ce50
00529f73 90                       nop        
00529f74 8b45fc                   mov        eax, dword ptr [ebp - 4]
00529f77 8b88d4580000             mov        ecx, dword ptr [eax + 0x58d4]
00529f7d d1e9                     shr        ecx, 1
00529f7f 83e101                   and        ecx, 1
00529f82 742b                     je         0x529faf
00529f84 6a00                     push       0
00529f86 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529f89 e8f22f0000               call       0x52cf80
00529f8e 6a1f                     push       0x1f
00529f90 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529f93 e8e82f0000               call       0x52cf80
00529f98 8b55fc                   mov        edx, dword ptr [ebp - 4]
00529f9b 8b82d4580000             mov        eax, dword ptr [edx + 0x58d4]
00529fa1 83e0fd                   and        eax, 0xfffffffd
00529fa4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529fa7 8981d4580000             mov        dword ptr [ecx + 0x58d4], eax
00529fad eb6b                     jmp        0x52a01a
00529faf 6a00                     push       0
00529fb1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529fb4 e897270000               call       0x52c750
00529fb9 85c0                     test       eax, eax
00529fbb 7515                     jne        0x529fd2
00529fbd 6a00                     push       0
00529fbf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529fc2 e8b92f0000               call       0x52cf80
00529fc7 6a00                     push       0
00529fc9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529fcc e81f2c0000               call       0x52cbf0
00529fd1 90                       nop        
00529fd2 6a1f                     push       0x1f
00529fd4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529fd7 e874270000               call       0x52c750
00529fdc 85c0                     test       eax, eax
00529fde 7515                     jne        0x529ff5
00529fe0 6a1f                     push       0x1f
00529fe2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529fe5 e8962f0000               call       0x52cf80
00529fea 6a1f                     push       0x1f
00529fec 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529fef e8fc2b0000               call       0x52cbf0
00529ff4 90                       nop        
00529ff5 8b55fc                   mov        edx, dword ptr [ebp - 4]
00529ff8 837a1c03                 cmp        dword ptr [edx + 0x1c], 3
00529ffc 740b                     je         0x52a009
00529ffe 6a00                     push       0
0052a000 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a003 e8e82b0000               call       0x52cbf0
0052a008 90                       nop        
0052a009 6a78                     push       0x78
0052a00b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a00e 81c154010000             add        ecx, 0x154
0052a014 e80795efff               call       0x423520
0052a019 90                       nop        
0052a01a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052a01d 0554010000               add        eax, 0x154
0052a022 8945d0                   mov        dword ptr [ebp - 0x30], eax
0052a025 6a78                     push       0x78
0052a027 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0052a02a e8b1e5f4ff               call       0x4785e0
0052a02f 0fb6c8                   movzx      ecx, al
0052a032 85c9                     test       ecx, ecx
0052a034 743d                     je         0x52a073
0052a036 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a039 81c174040000             add        ecx, 0x474
0052a03f e8fc02f5ff               call       0x47a340
0052a044 0fb6d0                   movzx      edx, al
0052a047 85d2                     test       edx, edx
0052a049 7528                     jne        0x52a073
0052a04b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052a04e 8b4814                   mov        ecx, dword ptr [eax + 0x14]
0052a051 894dcc                   mov        dword ptr [ebp - 0x34], ecx
0052a054 6a00                     push       0
0052a056 6aff                     push       -1
0052a058 6a00                     push       0
0052a05a 6a00                     push       0
0052a05c 8d55a4                   lea        edx, [ebp - 0x5c]
0052a05f 52                       push       edx
0052a060 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0052a063 e8086cf2ff               call       0x450c70
0052a068 8b00                     mov        eax, dword ptr [eax]
0052a06a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a06d 898174040000             mov        dword ptr [ecx + 0x474], eax
0052a073 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052a076 81c254010000             add        edx, 0x154
0052a07c 8955c8                   mov        dword ptr [ebp - 0x38], edx
0052a07f 6882000000               push       0x82
0052a084 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0052a087 e8e46ff3ff               call       0x461070
0052a08c 0fb6c0                   movzx      eax, al
0052a08f 85c0                     test       eax, eax
0052a091 740b                     je         0x52a09e
0052a093 6a02                     push       2
0052a095 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a098 e8b32d0000               call       0x52ce50
0052a09d 90                       nop        
0052a09e e9e1050000               jmp        0x52a684
0052a0a3 833d385b5c0000           cmp        dword ptr [0x5c5b38], 0
0052a0aa 7530                     jne        0x52a0dc
0052a0ac 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052a0b2 e839290000               call       0x52c9f0
0052a0b7 85c0                     test       eax, eax
0052a0b9 741f                     je         0x52a0da
0052a0bb 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052a0c1 e85a220000               call       0x52c320
0052a0c6 8945c4                   mov        dword ptr [ebp - 0x3c], eax
0052a0c9 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
0052a0cc 51                       push       ecx
0052a0cd e8be5afbff               call       0x4dfb90
0052a0d2 83c404                   add        esp, 4
0052a0d5 e9aa050000               jmp        0x52a684
0052a0da eb25                     jmp        0x52a101
0052a0dc 8b0d385b5c00             mov        ecx, dword ptr [0x5c5b38]
0052a0e2 e889c2efff               call       0x426370
0052a0e7 85c0                     test       eax, eax
0052a0e9 7411                     je         0x52a0fc
0052a0eb 8b15385b5c00             mov        edx, dword ptr [0x5c5b38]
0052a0f1 52                       push       edx
0052a0f2 e8c976efff               call       0x4217c0
0052a0f7 83c404                   add        esp, 4
0052a0fa eb05                     jmp        0x52a101
0052a0fc e983050000               jmp        0x52a684
0052a101 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a104 83c124                   add        ecx, 0x24
0052a107 e8f45af9ff               call       0x4bfc00
0052a10c 90                       nop        
0052a10d 33c0                     xor        eax, eax
0052a10f 7531                     jne        0x52a142
0052a111 b904000000               mov        ecx, 4
0052a116 6bd100                   imul       edx, ecx, 0
0052a119 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
0052a120 7420                     je         0x52a142
0052a122 b804000000               mov        eax, 4
0052a127 6bc800                   imul       ecx, eax, 0
0052a12a 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
0052a130 8955c0                   mov        dword ptr [ebp - 0x40], edx
0052a133 6a10                     push       0x10
0052a135 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0052a138 e84301efff               call       0x41a280
0052a13d 8945f4                   mov        dword ptr [ebp - 0xc], eax
0052a140 eb07                     jmp        0x52a149
0052a142 c745f400000000           mov        dword ptr [ebp - 0xc], 0
0052a149 837df400                 cmp        dword ptr [ebp - 0xc], 0
0052a14d 7414                     je         0x52a163
0052a14f 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052a152 83c024                   add        eax, 0x24
0052a155 8945bc                   mov        dword ptr [ebp - 0x44], eax
0052a158 6a00                     push       0
0052a15a 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
0052a15d e8de4ef9ff               call       0x4bf040
0052a162 90                       nop        
0052a163 33c9                     xor        ecx, ecx
0052a165 7531                     jne        0x52a198
0052a167 ba04000000               mov        edx, 4
0052a16c 6bc200                   imul       eax, edx, 0
0052a16f 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
0052a176 7420                     je         0x52a198
0052a178 b904000000               mov        ecx, 4
0052a17d 6bd100                   imul       edx, ecx, 0
0052a180 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
0052a186 8945b8                   mov        dword ptr [ebp - 0x48], eax
0052a189 6a20                     push       0x20
0052a18b 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
0052a18e e8ed00efff               call       0x41a280
0052a193 8945f0                   mov        dword ptr [ebp - 0x10], eax
0052a196 eb07                     jmp        0x52a19f
0052a198 c745f000000000           mov        dword ptr [ebp - 0x10], 0
0052a19f 837df000                 cmp        dword ptr [ebp - 0x10], 0
0052a1a3 7414                     je         0x52a1b9
0052a1a5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a1a8 83c124                   add        ecx, 0x24
0052a1ab 894db4                   mov        dword ptr [ebp - 0x4c], ecx
0052a1ae 6a00                     push       0
0052a1b0 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
0052a1b3 e8684ef9ff               call       0x4bf020
0052a1b8 90                       nop        
0052a1b9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a1bc 83c124                   add        ecx, 0x24
0052a1bf e86c2ef3ff               call       0x45d030
0052a1c4 85c0                     test       eax, eax
0052a1c6 741f                     je         0x52a1e7
0052a1c8 6a00                     push       0
0052a1ca 6a0a                     push       0xa
0052a1cc b930a85b00               mov        ecx, 0x5ba830
0052a1d1 e89acbefff               call       0x426d70
0052a1d6 6a08                     push       8
0052a1d8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a1db 81c104590000             add        ecx, 0x5904
0052a1e1 e83a93efff               call       0x423520
0052a1e6 90                       nop        
0052a1e7 33d2                     xor        edx, edx
0052a1e9 7534                     jne        0x52a21f
0052a1eb b804000000               mov        eax, 4
0052a1f0 6bc800                   imul       ecx, eax, 0
0052a1f3 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
0052a1fa 7423                     je         0x52a21f
0052a1fc ba04000000               mov        edx, 4
0052a201 6bc200                   imul       eax, edx, 0
0052a204 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
0052a20a 894db0                   mov        dword ptr [ebp - 0x50], ecx
0052a20d 6806010000               push       0x106
0052a212 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0052a215 e8e6f9eeff               call       0x419c00
0052a21a 8945ec                   mov        dword ptr [ebp - 0x14], eax
0052a21d eb07                     jmp        0x52a226
0052a21f c745ec00000000           mov        dword ptr [ebp - 0x14], 0
0052a226 837dec00                 cmp        dword ptr [ebp - 0x14], 0
0052a22a 7438                     je         0x52a264
0052a22c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a22f 83c124                   add        ecx, 0x24
0052a232 e8c920eeff               call       0x40c300
0052a237 83f809                   cmp        eax, 9
0052a23a 750c                     jne        0x52a248
0052a23c e9da010000               jmp        0x52a41b
0052a241 e9d5010000               jmp        0x52a41b
0052a246 eb1c                     jmp        0x52a264
0052a248 6a00                     push       0
0052a24a 6a09                     push       9
0052a24c b930a85b00               mov        ecx, 0x5ba830
0052a251 e81acbefff               call       0x426d70
0052a256 6a09                     push       9
0052a258 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a25b 83c124                   add        ecx, 0x24
0052a25e e8ed4af9ff               call       0x4bed50
0052a263 90                       nop        
0052a264 33d2                     xor        edx, edx
0052a266 7534                     jne        0x52a29c
0052a268 b804000000               mov        eax, 4
0052a26d 6bc800                   imul       ecx, eax, 0
0052a270 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
0052a277 7423                     je         0x52a29c
0052a279 ba04000000               mov        edx, 4
0052a27e 6bc200                   imul       eax, edx, 0
0052a281 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
0052a287 894dac                   mov        dword ptr [ebp - 0x54], ecx
0052a28a 6801000800               push       0x80001
0052a28f 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
0052a292 e869f9eeff               call       0x419c00
0052a297 8945e8                   mov        dword ptr [ebp - 0x18], eax
0052a29a eb07                     jmp        0x52a2a3
0052a29c c745e800000000           mov        dword ptr [ebp - 0x18], 0
0052a2a3 837de800                 cmp        dword ptr [ebp - 0x18], 0
0052a2a7 0f8487010000             je         0x52a434
0052a2ad 6a1e                     push       0x1e
0052a2af 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a2b2 81c114590000             add        ecx, 0x5914
0052a2b8 e86392efff               call       0x423520
0052a2bd 90                       nop        
0052a2be 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a2c1 83c124                   add        ecx, 0x24
0052a2c4 e83720eeff               call       0x40c300
0052a2c9 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0052a2cc 837de409                 cmp        dword ptr [ebp - 0x1c], 9
0052a2d0 0f875e010000             ja         0x52a434
0052a2d6 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0052a2d9 ff2495a4a65200           jmp        dword ptr [edx*4 + 0x52a6a4]
0052a2e0 6a00                     push       0
0052a2e2 6a07                     push       7
0052a2e4 b930a85b00               mov        ecx, 0x5ba830
0052a2e9 e882caefff               call       0x426d70
0052a2ee 6a1f                     push       0x1f
0052a2f0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a2f3 e838290000               call       0x52cc30
0052a2f8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a2fb 81c174040000             add        ecx, 0x474
0052a301 e83aedf4ff               call       0x479040
0052a306 6a04                     push       4
0052a308 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a30b e8402b0000               call       0x52ce50
0052a310 90                       nop        
0052a311 e91e010000               jmp        0x52a434
0052a316 6a00                     push       0
0052a318 6a07                     push       7
0052a31a b930a85b00               mov        ecx, 0x5ba830
0052a31f e84ccaefff               call       0x426d70
0052a324 6a1f                     push       0x1f
0052a326 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a329 e802290000               call       0x52cc30
0052a32e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a331 81c174040000             add        ecx, 0x474
0052a337 e804edf4ff               call       0x479040
0052a33c 6a04                     push       4
0052a33e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a341 e80a2b0000               call       0x52ce50
0052a346 90                       nop        
0052a347 e9e8000000               jmp        0x52a434
0052a34c 6a00                     push       0
0052a34e 6a07                     push       7
0052a350 b930a85b00               mov        ecx, 0x5ba830
0052a355 e816caefff               call       0x426d70
0052a35a 6a1f                     push       0x1f
0052a35c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a35f e8cc280000               call       0x52cc30
0052a364 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a367 81c174040000             add        ecx, 0x474
0052a36d e8ceecf4ff               call       0x479040
0052a372 6a04                     push       4
0052a374 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a377 e8d42a0000               call       0x52ce50
0052a37c 90                       nop        
0052a37d e9b2000000               jmp        0x52a434
0052a382 6a00                     push       0
0052a384 6a07                     push       7
0052a386 b930a85b00               mov        ecx, 0x5ba830
0052a38b e8e0c9efff               call       0x426d70
0052a390 6a1f                     push       0x1f
0052a392 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a395 e896280000               call       0x52cc30
0052a39a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a39d 81c174040000             add        ecx, 0x474
0052a3a3 e898ecf4ff               call       0x479040
0052a3a8 6a04                     push       4
0052a3aa 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a3ad e89e2a0000               call       0x52ce50
0052a3b2 90                       nop        
0052a3b3 eb7f                     jmp        0x52a434
0052a3b5 6a00                     push       0
0052a3b7 6a07                     push       7
0052a3b9 b930a85b00               mov        ecx, 0x5ba830
0052a3be e8adc9efff               call       0x426d70
0052a3c3 6a1f                     push       0x1f
0052a3c5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a3c8 e863280000               call       0x52cc30
0052a3cd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a3d0 81c174040000             add        ecx, 0x474
0052a3d6 e865ecf4ff               call       0x479040
0052a3db 6a04                     push       4
0052a3dd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a3e0 e86b2a0000               call       0x52ce50
0052a3e5 90                       nop        
0052a3e6 eb4c                     jmp        0x52a434
0052a3e8 6a00                     push       0
0052a3ea 6a07                     push       7
0052a3ec b930a85b00               mov        ecx, 0x5ba830
0052a3f1 e87ac9efff               call       0x426d70
0052a3f6 6a1f                     push       0x1f
0052a3f8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a3fb e830280000               call       0x52cc30
0052a400 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a403 81c174040000             add        ecx, 0x474
0052a409 e832ecf4ff               call       0x479040
0052a40e 6a04                     push       4
0052a410 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a413 e8382a0000               call       0x52ce50
0052a418 90                       nop        
0052a419 eb19                     jmp        0x52a434
0052a41b 6a00                     push       0
0052a41d 6a09                     push       9
0052a41f b930a85b00               mov        ecx, 0x5ba830
0052a424 e847c9efff               call       0x426d70
0052a429 6a04                     push       4
0052a42b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a42e e81d2a0000               call       0x52ce50
0052a433 90                       nop        
0052a434 e94b020000               jmp        0x52a684
0052a439 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052a43c 0554010000               add        eax, 0x154
0052a441 8945a8                   mov        dword ptr [ebp - 0x58], eax
0052a444 6a14                     push       0x14
0052a446 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0052a449 e87291efff               call       0x4235c0
0052a44e 0fb6c8                   movzx      ecx, al
0052a451 85c9                     test       ecx, ecx
0052a453 0f842b020000             je         0x52a684
0052a459 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a45c 83c124                   add        ecx, 0x24
0052a45f e89c1eeeff               call       0x40c300
0052a464 8945e0                   mov        dword ptr [ebp - 0x20], eax
0052a467 837de009                 cmp        dword ptr [ebp - 0x20], 9
0052a46b 0f8713020000             ja         0x52a684
0052a471 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0052a474 ff2495cca65200           jmp        dword ptr [edx*4 + 0x52a6cc]
0052a47b 6a00                     push       0
0052a47d b968a55b00               mov        ecx, 0x5ba568
0052a482 e81900feff               call       0x50a4a0
0052a487 6a00                     push       0
0052a489 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a48c e8ff270000               call       0x52cc90
0052a491 6a05                     push       5
0052a493 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a496 e8352a0000               call       0x52ced0
0052a49b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a49e 83c124                   add        ecx, 0x24
0052a4a1 e80abcf9ff               call       0x4c60b0
0052a4a6 a1600a5b00               mov        eax, dword ptr [0x5b0a60]
0052a4ab 50                       push       eax
0052a4ac 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a4af 83c124                   add        ecx, 0x24
0052a4b2 e89948f9ff               call       0x4bed50
0052a4b7 8b0d600a5b00             mov        ecx, dword ptr [0x5b0a60]
0052a4bd 51                       push       ecx
0052a4be b968a55b00               mov        ecx, 0x5ba568
0052a4c3 e8e83cf9ff               call       0x4be1b0
0052a4c8 90                       nop        
0052a4c9 e9b6010000               jmp        0x52a684
0052a4ce 6a00                     push       0
0052a4d0 b968a55b00               mov        ecx, 0x5ba568
0052a4d5 e8c6fffdff               call       0x50a4a0
0052a4da 6a00                     push       0
0052a4dc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a4df e8ac270000               call       0x52cc90
0052a4e4 6a05                     push       5
0052a4e6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a4e9 e8e2290000               call       0x52ced0
0052a4ee 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a4f1 83c124                   add        ecx, 0x24
0052a4f4 e8b7bbf9ff               call       0x4c60b0
0052a4f9 6a04                     push       4
0052a4fb b968a55b00               mov        ecx, 0x5ba568
0052a500 e8ab3cf9ff               call       0x4be1b0
0052a505 6a00                     push       0
0052a507 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a50a 83c124                   add        ecx, 0x24
0052a50d e83e48f9ff               call       0x4bed50
0052a512 90                       nop        
0052a513 e96c010000               jmp        0x52a684
0052a518 6a01                     push       1
0052a51a b968a55b00               mov        ecx, 0x5ba568
0052a51f e87cfffdff               call       0x50a4a0
0052a524 6a00                     push       0
0052a526 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a529 e862270000               call       0x52cc90
0052a52e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a531 83c124                   add        ecx, 0x24
0052a534 e877bbf9ff               call       0x4c60b0
0052a539 8b15600a5b00             mov        edx, dword ptr [0x5b0a60]
0052a53f 52                       push       edx
0052a540 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a543 83c124                   add        ecx, 0x24
0052a546 e80548f9ff               call       0x4bed50
0052a54b a1600a5b00               mov        eax, dword ptr [0x5b0a60]
0052a550 50                       push       eax
0052a551 b968a55b00               mov        ecx, 0x5ba568
0052a556 e8553cf9ff               call       0x4be1b0
0052a55b 6a05                     push       5
0052a55d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a560 e86b290000               call       0x52ced0
0052a565 90                       nop        
0052a566 e919010000               jmp        0x52a684
0052a56b 6a02                     push       2
0052a56d b968a55b00               mov        ecx, 0x5ba568
0052a572 e829fffdff               call       0x50a4a0
0052a577 6a00                     push       0
0052a579 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a57c e80f270000               call       0x52cc90
0052a581 6a06                     push       6
0052a583 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a586 e845290000               call       0x52ced0
0052a58b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a58e 83c124                   add        ecx, 0x24
0052a591 e81abbf9ff               call       0x4c60b0
0052a596 8b0d40615c00             mov        ecx, dword ptr [0x5c6140]
0052a59c 51                       push       ecx
0052a59d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a5a0 83c124                   add        ecx, 0x24
0052a5a3 e8a847f9ff               call       0x4bed50
0052a5a8 8b1540615c00             mov        edx, dword ptr [0x5c6140]
0052a5ae 52                       push       edx
0052a5af b968a55b00               mov        ecx, 0x5ba568
0052a5b4 e827fefdff               call       0x50a3e0
0052a5b9 90                       nop        
0052a5ba e9c5000000               jmp        0x52a684
0052a5bf 6a00                     push       0
0052a5c1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a5c4 e8c7260000               call       0x52cc90
0052a5c9 6a0c                     push       0xc
0052a5cb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a5ce e8fd280000               call       0x52ced0
0052a5d3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a5d6 83c124                   add        ecx, 0x24
0052a5d9 e8d2baf9ff               call       0x4c60b0
0052a5de 90                       nop        
0052a5df e9a0000000               jmp        0x52a684
0052a5e4 6a00                     push       0
0052a5e6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a5e9 e8a2260000               call       0x52cc90
0052a5ee 6a0a                     push       0xa
0052a5f0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a5f3 e8d8280000               call       0x52ced0
0052a5f8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a5fb 83c124                   add        ecx, 0x24
0052a5fe e8adbaf9ff               call       0x4c60b0
0052a603 6a00                     push       0
0052a605 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a608 83c124                   add        ecx, 0x24
0052a60b e84047f9ff               call       0x4bed50
0052a610 90                       nop        
0052a611 eb71                     jmp        0x52a684
0052a613 6a00                     push       0
0052a615 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a618 e873260000               call       0x52cc90
0052a61d 6a0e                     push       0xe
0052a61f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a622 e8a9280000               call       0x52ced0
0052a627 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a62a 83c124                   add        ecx, 0x24
0052a62d e87ebaf9ff               call       0x4c60b0
0052a632 90                       nop        
0052a633 eb4f                     jmp        0x52a684
0052a635 6a00                     push       0
0052a637 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a63a e851260000               call       0x52cc90
0052a63f 6a03                     push       3
0052a641 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a644 e887280000               call       0x52ced0
0052a649 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a64c 83c124                   add        ecx, 0x24
0052a64f e85cbaf9ff               call       0x4c60b0
0052a654 90                       nop        
0052a655 eb2d                     jmp        0x52a684
0052a657 6a00                     push       0
0052a659 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a65c e82f260000               call       0x52cc90
0052a661 6a11                     push       0x11
0052a663 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a666 e865280000               call       0x52ced0
0052a66b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a66e 83c124                   add        ecx, 0x24
0052a671 e83abaf9ff               call       0x4c60b0
0052a676 90                       nop        
0052a677 eb0b                     jmp        0x52a684
0052a679 6a02                     push       2
0052a67b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052a67e e84d280000               call       0x52ced0
0052a683 90                       nop        
0052a684 b801000000               mov        eax, 1
0052a689 8be5                     mov        esp, ebp
0052a68b 5d                       pop        ebp
0052a68c c3                       ret        
0052a68d 0f1f00                   nop        dword ptr [eax]
0052a690 a6                       cmpsb      byte ptr [esi], byte ptr es:[edi]
0052a691 9e                       sahf       
0052a692 52                       push       edx
0052a693 001a                     add        byte ptr [edx], bl
0052a695 a05200a3a0               mov        al, byte ptr [0xa0a30052]
0052a69a 52                       push       edx
0052a69b 0084a6520039a4           add        byte ptr [esi - 0x5bc6ffae], al
0052a6a2 52                       push       edx
0052a6a3 00e0                     add        al, ah
0052a6a5 a25200e0a2               mov        byte ptr [0xa2e00052], al
0052a6aa 52                       push       edx
0052a6ab 00e0                     add        al, ah
0052a6ad a25200e0a2               mov        byte ptr [0xa2e00052], al
0052a6b2 52                       push       edx
0052a6b3 0016                     add        byte ptr [esi], dl
0052a6b5 a352004ca3               mov        dword ptr [0xa34c0052], eax
0052a6ba 52                       push       edx
0052a6bb 0082a35200e8             add        byte ptr [edx - 0x17ffad5d], al
0052a6c1 a35200b5a3               mov        dword ptr [0xa3b50052], eax
0052a6c6 52                       push       edx
0052a6c7 001b                     add        byte ptr [ebx], bl
0052a6c9 a4                       movsb      byte ptr es:[edi], byte ptr [esi]
0052a6ca 52                       push       edx
0052a6cb 007ba4                   add        byte ptr [ebx - 0x5c], bh
0052a6ce 52                       push       edx
0052a6cf 00ce                     add        dh, cl
0052a6d1 a4                       movsb      byte ptr es:[edi], byte ptr [esi]
0052a6d2 52                       push       edx
0052a6d3 0018                     add        byte ptr [eax], bl
0052a6d5 a5                       movsd      dword ptr es:[edi], dword ptr [esi]
0052a6d6 52                       push       edx
0052a6d7 006ba5                   add        byte ptr [ebx - 0x5b], ch
0052a6da 52                       push       edx
0052a6db 00bfa55200e4             add        byte ptr [edi - 0x1bffad5b], bh
0052a6e1 a5                       movsd      dword ptr es:[edi], dword ptr [esi]
0052a6e2 52                       push       edx
0052a6e3 0013                     add        byte ptr [ebx], dl
0052a6e5 a6                       cmpsb      byte ptr [esi], byte ptr es:[edi]
0052a6e6 52                       push       edx
0052a6e7 0035a6520057             add        byte ptr [0x570052a6], dh
0052a6ed a6                       cmpsb      byte ptr [esi], byte ptr es:[edi]
0052a6ee 52                       push       edx
0052a6ef 0079a6                   add        byte ptr [ecx - 0x5a], bh
0052a6f2 52                       push       edx
0052a6f3 00cc                     add        ah, cl
0052a6f5 cc                       int3       
0052a6f6 cc                       int3       
0052a6f7 cc                       int3       
0052a6f8 cc                       int3       
0052a6f9 cc                       int3       
0052a6fa cc                       int3       
0052a6fb cc                       int3       
0052a6fc cc                       int3       
0052a6fd cc                       int3       
0052a6fe cc                       int3       
0052a6ff cc                       int3       
