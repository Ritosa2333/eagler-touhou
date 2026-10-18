00527f00 55                       push       ebp
00527f01 8bec                     mov        ebp, esp
00527f03 83ec7c                   sub        esp, 0x7c
00527f06 894dfc                   mov        dword ptr [ebp - 4], ecx
00527f09 8b45fc                   mov        eax, dword ptr [ebp - 4]
00527f0c 8b4820                   mov        ecx, dword ptr [eax + 0x20]
00527f0f 894df4                   mov        dword ptr [ebp - 0xc], ecx
00527f12 837df404                 cmp        dword ptr [ebp - 0xc], 4
00527f16 0f8730050000             ja         0x52844c
00527f1c 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00527f1f ff2495dc845200           jmp        dword ptr [edx*4 + 0x5284dc]
00527f26 8b45fc                   mov        eax, dword ptr [ebp - 4]
00527f29 83c024                   add        eax, 0x24
00527f2c 8945e0                   mov        dword ptr [ebp - 0x20], eax
00527f2f 6a05                     push       5
00527f31 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00527f34 e8a745f7ff               call       0x49c4e0
00527f39 6a00                     push       0
00527f3b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00527f3e 83c124                   add        ecx, 0x24
00527f41 e80a6ef9ff               call       0x4bed50
00527f46 90                       nop        
00527f47 833d580a5b0000           cmp        dword ptr [0x5b0a58], 0
00527f4e 7c1c                     jl         0x527f6c
00527f50 8b0d580a5b00             mov        ecx, dword ptr [0x5b0a58]
00527f56 51                       push       ecx
00527f57 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00527f5a 83c124                   add        ecx, 0x24
00527f5d e8ee6df9ff               call       0x4bed50
00527f62 c705580a5b00ffffffff     mov        dword ptr [0x5b0a58], 0xffffffff
00527f6c c70528615c0001000000     mov        dword ptr [0x5c6128], 1
00527f76 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00527f79 83c124                   add        ecx, 0x24
00527f7c e87f43eeff               call       0x40c300
00527f81 50                       push       eax
00527f82 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00527f85 e8f67dffff               call       0x51fd80
00527f8a 6a01                     push       1
00527f8c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00527f8f e8bc4e0000               call       0x52ce50
00527f94 90                       nop        
00527f95 8b55fc                   mov        edx, dword ptr [ebp - 4]
00527f98 81c254010000             add        edx, 0x154
00527f9e 8955dc                   mov        dword ptr [ebp - 0x24], edx
00527fa1 6a0a                     push       0xa
00527fa3 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00527fa6 e8c590f3ff               call       0x461070
00527fab 0fb6c0                   movzx      eax, al
00527fae 85c0                     test       eax, eax
00527fb0 740b                     je         0x527fbd
00527fb2 6a02                     push       2
00527fb4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00527fb7 e8944e0000               call       0x52ce50
00527fbc 90                       nop        
00527fbd e98a040000               jmp        0x52844c
00527fc2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00527fc5 83c124                   add        ecx, 0x24
00527fc8 e8337cf9ff               call       0x4bfc00
00527fcd 90                       nop        
00527fce 33c9                     xor        ecx, ecx
00527fd0 7531                     jne        0x528003
00527fd2 ba04000000               mov        edx, 4
00527fd7 6bc200                   imul       eax, edx, 0
00527fda 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
00527fe1 7420                     je         0x528003
00527fe3 b904000000               mov        ecx, 4
00527fe8 6bd100                   imul       edx, ecx, 0
00527feb 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
00527ff1 8945d8                   mov        dword ptr [ebp - 0x28], eax
00527ff4 6a10                     push       0x10
00527ff6 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00527ff9 e88222efff               call       0x41a280
00527ffe 8945f0                   mov        dword ptr [ebp - 0x10], eax
00528001 eb07                     jmp        0x52800a
00528003 c745f000000000           mov        dword ptr [ebp - 0x10], 0
0052800a 837df000                 cmp        dword ptr [ebp - 0x10], 0
0052800e 7414                     je         0x528024
00528010 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528013 83c124                   add        ecx, 0x24
00528016 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
00528019 6a00                     push       0
0052801b 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0052801e e81d70f9ff               call       0x4bf040
00528023 90                       nop        
00528024 33d2                     xor        edx, edx
00528026 7531                     jne        0x528059
00528028 b804000000               mov        eax, 4
0052802d 6bc800                   imul       ecx, eax, 0
00528030 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
00528037 7420                     je         0x528059
00528039 ba04000000               mov        edx, 4
0052803e 6bc200                   imul       eax, edx, 0
00528041 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
00528047 894dd0                   mov        dword ptr [ebp - 0x30], ecx
0052804a 6a20                     push       0x20
0052804c 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0052804f e82c22efff               call       0x41a280
00528054 8945ec                   mov        dword ptr [ebp - 0x14], eax
00528057 eb07                     jmp        0x528060
00528059 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
00528060 837dec00                 cmp        dword ptr [ebp - 0x14], 0
00528064 7414                     je         0x52807a
00528066 8b55fc                   mov        edx, dword ptr [ebp - 4]
00528069 83c224                   add        edx, 0x24
0052806c 8955cc                   mov        dword ptr [ebp - 0x34], edx
0052806f 6a00                     push       0
00528071 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00528074 e8a76ff9ff               call       0x4bf020
00528079 90                       nop        
0052807a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052807d 83c124                   add        ecx, 0x24
00528080 e8ab4ff3ff               call       0x45d030
00528085 85c0                     test       eax, eax
00528087 7423                     je         0x5280ac
00528089 6a00                     push       0
0052808b 6a0a                     push       0xa
0052808d b930a85b00               mov        ecx, 0x5ba830
00528092 e8d9ecefff               call       0x426d70
00528097 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052809a 83c124                   add        ecx, 0x24
0052809d e85e42eeff               call       0x40c300
005280a2 50                       push       eax
005280a3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005280a6 e8d57cffff               call       0x51fd80
005280ab 90                       nop        
005280ac 33c0                     xor        eax, eax
005280ae 7534                     jne        0x5280e4
005280b0 b904000000               mov        ecx, 4
005280b5 6bd100                   imul       edx, ecx, 0
005280b8 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
005280bf 7423                     je         0x5280e4
005280c1 b804000000               mov        eax, 4
005280c6 6bc800                   imul       ecx, eax, 0
005280c9 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
005280cf 8955c8                   mov        dword ptr [ebp - 0x38], edx
005280d2 6806010000               push       0x106
005280d7 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
005280da e8211befff               call       0x419c00
005280df 8945e8                   mov        dword ptr [ebp - 0x18], eax
005280e2 eb07                     jmp        0x5280eb
005280e4 c745e800000000           mov        dword ptr [ebp - 0x18], 0
005280eb 837de800                 cmp        dword ptr [ebp - 0x18], 0
005280ef 741e                     je         0x52810f
005280f1 6a04                     push       4
005280f3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005280f6 e8554d0000               call       0x52ce50
005280fb 6a00                     push       0
005280fd 6a09                     push       9
005280ff b930a85b00               mov        ecx, 0x5ba830
00528104 e867ecefff               call       0x426d70
00528109 90                       nop        
0052810a e93d030000               jmp        0x52844c
0052810f 33c0                     xor        eax, eax
00528111 7534                     jne        0x528147
00528113 b904000000               mov        ecx, 4
00528118 6bd100                   imul       edx, ecx, 0
0052811b 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00528122 7423                     je         0x528147
00528124 b804000000               mov        eax, 4
00528129 6bc800                   imul       ecx, eax, 0
0052812c 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00528132 8955c4                   mov        dword ptr [ebp - 0x3c], edx
00528135 6801000800               push       0x80001
0052813a 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
0052813d e8be1aefff               call       0x419c00
00528142 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00528145 eb07                     jmp        0x52814e
00528147 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
0052814e 837de400                 cmp        dword ptr [ebp - 0x1c], 0
00528152 0f8417010000             je         0x52826f
00528158 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052815e e8ed7afeff               call       0x50fc50
00528163 05080b0000               add        eax, 0xb08
00528168 8945bc                   mov        dword ptr [ebp - 0x44], eax
0052816b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052816e 83c124                   add        ecx, 0x24
00528171 e88a41eeff               call       0x40c300
00528176 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528179 8b9481e4580000           mov        edx, dword ptr [ecx + eax*4 + 0x58e4]
00528180 8955c0                   mov        dword ptr [ebp - 0x40], edx
00528183 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
00528186 50                       push       eax
00528187 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
0052818a e881ecf5ff               call       0x486e10
0052818f b904000000               mov        ecx, 4
00528194 6bd100                   imul       edx, ecx, 0
00528197 83bc10c800000000         cmp        dword ptr [eax + edx + 0xc8], 0
0052819f 755d                     jne        0x5281fe
005281a1 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
005281a7 e8a47afeff               call       0x50fc50
005281ac 05080b0000               add        eax, 0xb08
005281b1 8945b4                   mov        dword ptr [ebp - 0x4c], eax
005281b4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005281b7 83c124                   add        ecx, 0x24
005281ba e84141eeff               call       0x40c300
005281bf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005281c2 8b9481e4580000           mov        edx, dword ptr [ecx + eax*4 + 0x58e4]
005281c9 8955b8                   mov        dword ptr [ebp - 0x48], edx
005281cc 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
005281cf 50                       push       eax
005281d0 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
005281d3 e838ecf5ff               call       0x486e10
005281d8 b904000000               mov        ecx, 4
005281dd c1e100                   shl        ecx, 0
005281e0 83bc08c800000000         cmp        dword ptr [eax + ecx + 0xc8], 0
005281e8 7514                     jne        0x5281fe
005281ea 6a00                     push       0
005281ec 6a10                     push       0x10
005281ee b930a85b00               mov        ecx, 0x5ba830
005281f3 e878ebefff               call       0x426d70
005281f8 90                       nop        
005281f9 e94e020000               jmp        0x52844c
005281fe 8b55fc                   mov        edx, dword ptr [ebp - 4]
00528201 81c294030000             add        edx, 0x394
00528207 8955b0                   mov        dword ptr [ebp - 0x50], edx
0052820a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052820d 83c124                   add        ecx, 0x24
00528210 e8eb40eeff               call       0x40c300
00528215 50                       push       eax
00528216 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00528219 e862c3eeff               call       0x414580
0052821e 8945ac                   mov        dword ptr [ebp - 0x54], eax
00528221 6a06                     push       6
00528223 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00528226 e8656df2ff               call       0x44ef90
0052822b 6a03                     push       3
0052822d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528230 e81b4c0000               call       0x52ce50
00528235 6a00                     push       0
00528237 6a07                     push       7
00528239 b930a85b00               mov        ecx, 0x5ba830
0052823e e82debefff               call       0x426d70
00528243 51                       push       ecx
00528244 f30f10055cfe5600         movss      xmm0, dword ptr [0x56fe5c]
0052824c f30f110424               movss      dword ptr [esp], xmm0
00528251 b9404d5c00               mov        ecx, 0x5c4d40
00528256 e87517fbff               call       0x4d99d0
0052825b 6a00                     push       0
0052825d 6a32                     push       0x32
0052825f b930a85b00               mov        ecx, 0x5ba830
00528264 e807ebefff               call       0x426d70
00528269 90                       nop        
0052826a e9dd010000               jmp        0x52844c
0052826f e9d8010000               jmp        0x52844c
00528274 6a00                     push       0
00528276 e8a5f2f0ff               call       0x437520
0052827b 83c404                   add        esp, 4
0052827e 8bc8                     mov        ecx, eax
00528280 e89b02f1ff               call       0x438520
00528285 85c0                     test       eax, eax
00528287 751f                     jne        0x5282a8
00528289 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052828c 0554010000               add        eax, 0x154
00528291 8945a8                   mov        dword ptr [ebp - 0x58], eax
00528294 6a00                     push       0
00528296 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00528299 e88211f0ff               call       0x429420
0052829e b801000000               mov        eax, 1
005282a3 e92d020000               jmp        0x5284d5
005282a8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005282ab 81c154010000             add        ecx, 0x154
005282b1 894da4                   mov        dword ptr [ebp - 0x5c], ecx
005282b4 6a0a                     push       0xa
005282b6 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
005282b9 e82203f5ff               call       0x4785e0
005282be 0fb6d0                   movzx      edx, al
005282c1 85d2                     test       edx, edx
005282c3 745c                     je         0x528321
005282c5 51                       push       ecx
005282c6 f30f100588035700         movss      xmm0, dword ptr [0x570388]
005282ce f30f110424               movss      dword ptr [esp], xmm0
005282d3 51                       push       ecx
005282d4 f30f1005a8cd5600         movss      xmm0, dword ptr [0x56cda8]
005282dc f30f110424               movss      dword ptr [esp], xmm0
005282e1 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005282e7 e8b487f7ff               call       0x4a0aa0
005282ec 90                       nop        
005282ed 6a00                     push       0
005282ef e82cf2f0ff               call       0x437520
005282f4 83c404                   add        esp, 4
005282f7 8945a0                   mov        dword ptr [ebp - 0x60], eax
005282fa 6a00                     push       0
005282fc 6a00                     push       0
005282fe 6a00                     push       0
00528300 8d4584                   lea        eax, [ebp - 0x7c]
00528303 50                       push       eax
00528304 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00528307 e8e459f7ff               call       0x49dcf0
0052830c 8b08                     mov        ecx, dword ptr [eax]
0052830e 890d044f5c00             mov        dword ptr [0x5c4f04], ecx
00528314 6a00                     push       0
00528316 b9044f5c00               mov        ecx, 0x5c4f04
0052831b e89043ffff               call       0x51c6b0
00528320 90                       nop        
00528321 8b55fc                   mov        edx, dword ptr [ebp - 4]
00528324 81c254010000             add        edx, 0x154
0052832a 89559c                   mov        dword ptr [ebp - 0x64], edx
0052832d 6a28                     push       0x28
0052832f 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
00528332 e889b2efff               call       0x4235c0
00528337 0fb6c0                   movzx      eax, al
0052833a 85c0                     test       eax, eax
0052833c 0f84b7000000             je         0x5283f9
00528342 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528345 83c124                   add        ecx, 0x24
00528348 e863ddf9ff               call       0x4c60b0
0052834d 6a02                     push       2
0052834f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528352 e8794b0000               call       0x52ced0
00528357 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052835a 8b91d8580000             mov        edx, dword ptr [ecx + 0x58d8]
00528360 83c201                   add        edx, 1
00528363 52                       push       edx
00528364 b968a55b00               mov        ecx, 0x5ba568
00528369 e8f25ff9ff               call       0x4be360
0052836e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528371 83c124                   add        ecx, 0x24
00528374 e8873feeff               call       0x40c300
00528379 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052837c 8b9481e4580000           mov        edx, dword ptr [ecx + eax*4 + 0x58e4]
00528383 52                       push       edx
00528384 b968a55b00               mov        ecx, 0x5ba568
00528389 e86224feff               call       0x50a7f0
0052838e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528391 83c124                   add        ecx, 0x24
00528394 e8673feeff               call       0x40c300
00528399 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052839c 8b9481e4580000           mov        edx, dword ptr [ecx + eax*4 + 0x58e4]
005283a3 8b049558f05a00           mov        eax, dword ptr [edx*4 + 0x5af058]
005283aa 50                       push       eax
005283ab b968a55b00               mov        ecx, 0x5ba568
005283b0 e8fb5df9ff               call       0x4be1b0
005283b5 6a07                     push       7
005283b7 b9404d5c00               mov        ecx, 0x5c4d40
005283bc e88f5ef9ff               call       0x4be250
005283c1 c70528615c0005000000     mov        dword ptr [0x5c6128], 5
005283cb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005283ce 8b91d8580000             mov        edx, dword ptr [ecx + 0x58d8]
005283d4 8915540a5b00             mov        dword ptr [0x5b0a54], edx
005283da 8b45fc                   mov        eax, dword ptr [ebp - 4]
005283dd 8b88dc580000             mov        ecx, dword ptr [eax + 0x58dc]
005283e3 890d5c0a5b00             mov        dword ptr [0x5b0a5c], ecx
005283e9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005283ec 83c124                   add        ecx, 0x24
005283ef e80c3feeff               call       0x40c300
005283f4 a3580a5b00               mov        dword ptr [0x5b0a58], eax
005283f9 eb51                     jmp        0x52844c
005283fb 8b55fc                   mov        edx, dword ptr [ebp - 4]
005283fe 81c254010000             add        edx, 0x154
00528404 895598                   mov        dword ptr [ebp - 0x68], edx
00528407 6a06                     push       6
00528409 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
0052840c e8afb1efff               call       0x4235c0
00528411 0fb6c0                   movzx      eax, al
00528414 85c0                     test       eax, eax
00528416 7434                     je         0x52844c
00528418 6a56                     push       0x56
0052841a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052841d e80e480000               call       0x52cc30
00528422 6a13                     push       0x13
00528424 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528427 e8a44a0000               call       0x52ced0
0052842c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052842f 83c124                   add        ecx, 0x24
00528432 e869e4fbff               call       0x4e68a0
00528437 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052843a 83c124                   add        ecx, 0x24
0052843d e8cedbf9ff               call       0x4c6010
00528442 b801000000               mov        eax, 1
00528447 e989000000               jmp        0x5284d5
0052844c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052844f 83c124                   add        ecx, 0x24
00528452 e8a93eeeff               call       0x40c300
00528457 894594                   mov        dword ptr [ebp - 0x6c], eax
0052845a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052845d 8b91dc580000             mov        edx, dword ptr [ecx + 0x58dc]
00528463 895590                   mov        dword ptr [ebp - 0x70], edx
00528466 8b45fc                   mov        eax, dword ptr [ebp - 4]
00528469 8b88d8580000             mov        ecx, dword ptr [eax + 0x58d8]
0052846f 894d8c                   mov        dword ptr [ebp - 0x74], ecx
00528472 8b5594                   mov        edx, dword ptr [ebp - 0x6c]
00528475 52                       push       edx
00528476 8b45fc                   mov        eax, dword ptr [ebp - 4]
00528479 05e4580000               add        eax, 0x58e4
0052847e 50                       push       eax
0052847f 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00528482 51                       push       ecx
00528483 8b558c                   mov        edx, dword ptr [ebp - 0x74]
00528486 52                       push       edx
00528487 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052848a e8217affff               call       0x51feb0
0052848f 90                       nop        
00528490 c745f800000000           mov        dword ptr [ebp - 8], 0
00528497 eb09                     jmp        0x5284a2
00528499 8b45f8                   mov        eax, dword ptr [ebp - 8]
0052849c 83c001                   add        eax, 1
0052849f 8945f8                   mov        dword ptr [ebp - 8], eax
005284a2 837df805                 cmp        dword ptr [ebp - 8], 5
005284a6 7d28                     jge        0x5284d0
005284a8 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005284ab 8b55fc                   mov        edx, dword ptr [ebp - 4]
005284ae 83bc8ae458000000         cmp        dword ptr [edx + ecx*4 + 0x58e4], 0
005284b6 7d16                     jge        0x5284ce
005284b8 8b45fc                   mov        eax, dword ptr [ebp - 4]
005284bb 83c024                   add        eax, 0x24
005284be 894588                   mov        dword ptr [ebp - 0x78], eax
005284c1 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005284c4 51                       push       ecx
005284c5 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
005284c8 e8c352ffff               call       0x51d790
005284cd 90                       nop        
005284ce ebc9                     jmp        0x528499
005284d0 b801000000               mov        eax, 1
005284d5 8be5                     mov        esp, ebp
005284d7 5d                       pop        ebp
005284d8 c3                       ret        
005284d9 0f1f00                   nop        dword ptr [eax]
005284dc 267f52                   jg         0x528531
005284df 00957f5200c2             add        byte ptr [ebp - 0x3dffad81], dl
005284e5 7f52                     jg         0x528539
005284e7 00748252                 add        byte ptr [edx + eax*4 + 0x52], dh
005284eb 00fb                     add        bl, bh
