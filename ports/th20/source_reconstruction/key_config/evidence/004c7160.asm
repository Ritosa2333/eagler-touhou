004c7160 55                       push       ebp
004c7161 8bec                     mov        ebp, esp
004c7163 83ec68                   sub        esp, 0x68
004c7166 56                       push       esi
004c7167 57                       push       edi
004c7168 894dfc                   mov        dword ptr [ebp - 4], ecx
004c716b 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c716e 8b8800010000             mov        ecx, dword ptr [eax + 0x100]
004c7174 894df4                   mov        dword ptr [ebp - 0xc], ecx
004c7177 837df404                 cmp        dword ptr [ebp - 0xc], 4
004c717b 0f87ca050000             ja         0x4c774b
004c7181 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004c7184 ff249558774c00           jmp        dword ptr [edx*4 + 0x4c7758]
004c718b 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c718e 83c014                   add        eax, 0x14
004c7191 8945cc                   mov        dword ptr [ebp - 0x34], eax
004c7194 6a03                     push       3
004c7196 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004c7199 e84253fdff               call       0x49c4e0
004c719e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c71a1 8b910c010000             mov        edx, dword ptr [ecx + 0x10c]
004c71a7 52                       push       edx
004c71a8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c71ab 83c114                   add        ecx, 0x14
004c71ae e89d7bffff               call       0x4bed50
004c71b3 6a00                     push       0
004c71b5 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c71bb e8d0a7f5ff               call       0x421990
004c71c0 b904000000               mov        ecx, 4
004c71c5 6bd100                   imul       edx, ecx, 0
004c71c8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c71cb 89841104010000           mov        dword ptr [ecx + edx + 0x104], eax
004c71d2 6a01                     push       1
004c71d4 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c71da e8b1a7f5ff               call       0x421990
004c71df ba04000000               mov        edx, 4
004c71e4 c1e200                   shl        edx, 0
004c71e7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c71ea 89841104010000           mov        dword ptr [ecx + edx + 0x104], eax
004c71f1 6a00                     push       0
004c71f3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c71f6 81c1d0000000             add        ecx, 0xd0
004c71fc e81fc3f5ff               call       0x423520
004c7201 6a01                     push       1
004c7203 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7206 e895efffff               call       0x4c61a0
004c720b 90                       nop        
004c720c 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c720f 81c2c0000000             add        edx, 0xc0
004c7215 8955c8                   mov        dword ptr [ebp - 0x38], edx
004c7218 6a06                     push       6
004c721a 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004c721d e84e9ef9ff               call       0x461070
004c7222 0fb6c0                   movzx      eax, al
004c7225 85c0                     test       eax, eax
004c7227 742f                     je         0x4c7258
004c7229 6a02                     push       2
004c722b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c722e e86defffff               call       0x4c61a0
004c7233 90                       nop        
004c7234 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7237 83b91001000000           cmp        dword ptr [ecx + 0x110], 0
004c723e 7418                     je         0x4c7258
004c7240 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c7246 e8359bf5ff               call       0x420d80
004c724b 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c724e c7821001000000000000     mov        dword ptr [edx + 0x110], 0
004c7258 e9ee040000               jmp        0x4c774b
004c725d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7260 83c114                   add        ecx, 0x14
004c7263 e89889ffff               call       0x4bfc00
004c7268 90                       nop        
004c7269 33c0                     xor        eax, eax
004c726b 752f                     jne        0x4c729c
004c726d b904000000               mov        ecx, 4
004c7272 d1e1                     shl        ecx, 1
004c7274 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
004c727b 741f                     je         0x4c729c
004c727d ba04000000               mov        edx, 4
004c7282 d1e2                     shl        edx, 1
004c7284 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
004c728a 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004c728d 6a10                     push       0x10
004c728f 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004c7292 e8e92ff5ff               call       0x41a280
004c7297 8945f0                   mov        dword ptr [ebp - 0x10], eax
004c729a eb07                     jmp        0x4c72a3
004c729c c745f000000000           mov        dword ptr [ebp - 0x10], 0
004c72a3 837df000                 cmp        dword ptr [ebp - 0x10], 0
004c72a7 7414                     je         0x4c72bd
004c72a9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c72ac 83c114                   add        ecx, 0x14
004c72af 894dc0                   mov        dword ptr [ebp - 0x40], ecx
004c72b2 6a00                     push       0
004c72b4 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004c72b7 e8847dffff               call       0x4bf040
004c72bc 90                       nop        
004c72bd 33d2                     xor        edx, edx
004c72bf 752f                     jne        0x4c72f0
004c72c1 b804000000               mov        eax, 4
004c72c6 d1e0                     shl        eax, 1
004c72c8 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
004c72cf 741f                     je         0x4c72f0
004c72d1 b904000000               mov        ecx, 4
004c72d6 d1e1                     shl        ecx, 1
004c72d8 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
004c72de 8955bc                   mov        dword ptr [ebp - 0x44], edx
004c72e1 6a20                     push       0x20
004c72e3 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004c72e6 e8952ff5ff               call       0x41a280
004c72eb 8945ec                   mov        dword ptr [ebp - 0x14], eax
004c72ee eb07                     jmp        0x4c72f7
004c72f0 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
004c72f7 837dec00                 cmp        dword ptr [ebp - 0x14], 0
004c72fb 7414                     je         0x4c7311
004c72fd 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c7300 83c014                   add        eax, 0x14
004c7303 8945b8                   mov        dword ptr [ebp - 0x48], eax
004c7306 6a00                     push       0
004c7308 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004c730b e8107dffff               call       0x4bf020
004c7310 90                       nop        
004c7311 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7314 83c114                   add        ecx, 0x14
004c7317 e8145df9ff               call       0x45d030
004c731c 85c0                     test       eax, eax
004c731e 741f                     je         0x4c733f
004c7320 6a00                     push       0
004c7322 6a0a                     push       0xa
004c7324 b930a85b00               mov        ecx, 0x5ba830
004c7329 e842faf5ff               call       0x426d70
004c732e 6a08                     push       8
004c7330 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7333 81c1ec000000             add        ecx, 0xec
004c7339 e8e2c1f5ff               call       0x423520
004c733e 90                       nop        
004c733f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7342 83c114                   add        ecx, 0x14
004c7345 e8b64ff4ff               call       0x40c300
004c734a 8945f8                   mov        dword ptr [ebp - 8], eax
004c734d 837df800                 cmp        dword ptr [ebp - 8], 0
004c7351 7419                     je         0x4c736c
004c7353 837df801                 cmp        dword ptr [ebp - 8], 1
004c7357 0f843c020000             je         0x4c7599
004c735d 837df802                 cmp        dword ptr [ebp - 8], 2
004c7361 0f84df020000             je         0x4c7646
004c7367 e93a030000               jmp        0x4c76a6
004c736c 33c9                     xor        ecx, ecx
004c736e 752f                     jne        0x4c739f
004c7370 ba04000000               mov        edx, 4
004c7375 d1e2                     shl        edx, 1
004c7377 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
004c737e 741f                     je         0x4c739f
004c7380 b804000000               mov        eax, 4
004c7385 d1e0                     shl        eax, 1
004c7387 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
004c738d 894db4                   mov        dword ptr [ebp - 0x4c], ecx
004c7390 6a40                     push       0x40
004c7392 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004c7395 e86628f5ff               call       0x419c00
004c739a 8945e8                   mov        dword ptr [ebp - 0x18], eax
004c739d eb07                     jmp        0x4c73a6
004c739f c745e800000000           mov        dword ptr [ebp - 0x18], 0
004c73a6 837de800                 cmp        dword ptr [ebp - 0x18], 0
004c73aa 0f8485000000             je         0x4c7435
004c73b0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c73b3 83c114                   add        ecx, 0x14
004c73b6 e8454ff4ff               call       0x40c300
004c73bb 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c73be 8d848204010000           lea        eax, [edx + eax*4 + 0x104]
004c73c5 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004c73c8 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004c73cb 8b11                     mov        edx, dword ptr [ecx]
004c73cd 83ea01                   sub        edx, 1
004c73d0 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004c73d3 8910                     mov        dword ptr [eax], edx
004c73d5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c73d8 83c114                   add        ecx, 0x14
004c73db e8204ff4ff               call       0x40c300
004c73e0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c73e3 83bc810401000000         cmp        dword ptr [ecx + eax*4 + 0x104], 0
004c73eb 7d29                     jge        0x4c7416
004c73ed 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c73f3 e89856f5ff               call       0x41ca90
004c73f8 83e801                   sub        eax, 1
004c73fb 8945b0                   mov        dword ptr [ebp - 0x50], eax
004c73fe 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7401 83c114                   add        ecx, 0x14
004c7404 e8f74ef4ff               call       0x40c300
004c7409 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c740c 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
004c740f 898c8204010000           mov        dword ptr [edx + eax*4 + 0x104], ecx
004c7416 6a00                     push       0
004c7418 6a0a                     push       0xa
004c741a b930a85b00               mov        ecx, 0x5ba830
004c741f e84cf9f5ff               call       0x426d70
004c7424 6a08                     push       8
004c7426 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7429 81c1ec000000             add        ecx, 0xec
004c742f e8ecc0f5ff               call       0x423520
004c7434 90                       nop        
004c7435 33d2                     xor        edx, edx
004c7437 7532                     jne        0x4c746b
004c7439 b804000000               mov        eax, 4
004c743e d1e0                     shl        eax, 1
004c7440 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
004c7447 7422                     je         0x4c746b
004c7449 b904000000               mov        ecx, 4
004c744e d1e1                     shl        ecx, 1
004c7450 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
004c7456 8955ac                   mov        dword ptr [ebp - 0x54], edx
004c7459 6880000000               push       0x80
004c745e 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
004c7461 e89a27f5ff               call       0x419c00
004c7466 8945e0                   mov        dword ptr [ebp - 0x20], eax
004c7469 eb07                     jmp        0x4c7472
004c746b c745e000000000           mov        dword ptr [ebp - 0x20], 0
004c7472 837de000                 cmp        dword ptr [ebp - 0x20], 0
004c7476 0f8481000000             je         0x4c74fd
004c747c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c747f 83c114                   add        ecx, 0x14
004c7482 e8794ef4ff               call       0x40c300
004c7487 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c748a 8d948104010000           lea        edx, [ecx + eax*4 + 0x104]
004c7491 8955dc                   mov        dword ptr [ebp - 0x24], edx
004c7494 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004c7497 8b08                     mov        ecx, dword ptr [eax]
004c7499 83c101                   add        ecx, 1
004c749c 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
004c749f 890a                     mov        dword ptr [edx], ecx
004c74a1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c74a4 83c114                   add        ecx, 0x14
004c74a7 e8544ef4ff               call       0x40c300
004c74ac 8bf0                     mov        esi, eax
004c74ae 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c74b4 e8d755f5ff               call       0x41ca90
004c74b9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c74bc 3984b104010000           cmp        dword ptr [ecx + esi*4 + 0x104], eax
004c74c3 7c19                     jl         0x4c74de
004c74c5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c74c8 83c114                   add        ecx, 0x14
004c74cb e8304ef4ff               call       0x40c300
004c74d0 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c74d3 c784820401000000000000   mov        dword ptr [edx + eax*4 + 0x104], 0
004c74de 6a00                     push       0
004c74e0 6a0a                     push       0xa
004c74e2 b930a85b00               mov        ecx, 0x5ba830
004c74e7 e884f8f5ff               call       0x426d70
004c74ec 6a08                     push       8
004c74ee 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c74f1 81c1ec000000             add        ecx, 0xec
004c74f7 e824c0f5ff               call       0x423520
004c74fc 90                       nop        
004c74fd b804000000               mov        eax, 4
004c7502 6bc800                   imul       ecx, eax, 0
004c7505 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c7508 8b840a04010000           mov        eax, dword ptr [edx + ecx + 0x104]
004c750f 50                       push       eax
004c7510 6a00                     push       0
004c7512 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c7518 e8f3ecffff               call       0x4c6210
004c751d 90                       nop        
004c751e 33c9                     xor        ecx, ecx
004c7520 7532                     jne        0x4c7554
004c7522 ba04000000               mov        edx, 4
004c7527 d1e2                     shl        edx, 1
004c7529 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
004c7530 7422                     je         0x4c7554
004c7532 b804000000               mov        eax, 4
004c7537 d1e0                     shl        eax, 1
004c7539 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
004c753f 894da8                   mov        dword ptr [ebp - 0x58], ecx
004c7542 680f010800               push       0x8010f
004c7547 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004c754a e8b126f5ff               call       0x419c00
004c754f 8945d8                   mov        dword ptr [ebp - 0x28], eax
004c7552 eb07                     jmp        0x4c755b
004c7554 c745d800000000           mov        dword ptr [ebp - 0x28], 0
004c755b 837dd800                 cmp        dword ptr [ebp - 0x28], 0
004c755f 7433                     je         0x4c7594
004c7561 6a03                     push       3
004c7563 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7566 e835ecffff               call       0x4c61a0
004c756b 6a00                     push       0
004c756d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7570 81c1d0000000             add        ecx, 0xd0
004c7576 e8a5bff5ff               call       0x423520
004c757b 6a00                     push       0
004c757d 6a07                     push       7
004c757f b930a85b00               mov        ecx, 0x5ba830
004c7584 e8e7f7f5ff               call       0x426d70
004c7589 90                       nop        
004c758a e91c010000               jmp        0x4c76ab
004c758f e917010000               jmp        0x4c76ab
004c7594 e90d010000               jmp        0x4c76a6
004c7599 33d2                     xor        edx, edx
004c759b 7532                     jne        0x4c75cf
004c759d b804000000               mov        eax, 4
004c75a2 d1e0                     shl        eax, 1
004c75a4 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
004c75ab 7422                     je         0x4c75cf
004c75ad b904000000               mov        ecx, 4
004c75b2 d1e1                     shl        ecx, 1
004c75b4 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
004c75ba 8955a4                   mov        dword ptr [ebp - 0x5c], edx
004c75bd 680f010800               push       0x8010f
004c75c2 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
004c75c5 e83626f5ff               call       0x419c00
004c75ca 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004c75cd eb07                     jmp        0x4c75d6
004c75cf c745d400000000           mov        dword ptr [ebp - 0x2c], 0
004c75d6 837dd400                 cmp        dword ptr [ebp - 0x2c], 0
004c75da 7468                     je         0x4c7644
004c75dc 6a00                     push       0
004c75de 6a07                     push       7
004c75e0 b930a85b00               mov        ecx, 0x5ba830
004c75e5 e886f7f5ff               call       0x426d70
004c75ea 6a00                     push       0
004c75ec 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c75f2 e839ebffff               call       0x4c6130
004c75f7 6a01                     push       1
004c75f9 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c75ff e82cebffff               call       0x4c6130
004c7604 b830000000               mov        eax, 0x30
004c7609 6bf800                   imul       edi, eax, 0
004c760c 81c7104f5c00             add        edi, 0x5c4f10
004c7612 b90c000000               mov        ecx, 0xc
004c7617 beb09e5b00               mov        esi, 0x5b9eb0
004c761c f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
004c761e bf30000000               mov        edi, 0x30
004c7623 c1e700                   shl        edi, 0
004c7626 81c7104f5c00             add        edi, 0x5c4f10
004c762c b90c000000               mov        ecx, 0xc
004c7631 beb09e5b00               mov        esi, 0x5b9eb0
004c7636 f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
004c7638 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c763e e83d97f5ff               call       0x420d80
004c7643 90                       nop        
004c7644 eb60                     jmp        0x4c76a6
004c7646 33c9                     xor        ecx, ecx
004c7648 7532                     jne        0x4c767c
004c764a ba04000000               mov        edx, 4
004c764f d1e2                     shl        edx, 1
004c7651 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
004c7658 7422                     je         0x4c767c
004c765a b804000000               mov        eax, 4
004c765f d1e0                     shl        eax, 1
004c7661 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
004c7667 894da0                   mov        dword ptr [ebp - 0x60], ecx
004c766a 680f010800               push       0x8010f
004c766f 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
004c7672 e88925f5ff               call       0x419c00
004c7677 8945d0                   mov        dword ptr [ebp - 0x30], eax
004c767a eb07                     jmp        0x4c7683
004c767c c745d000000000           mov        dword ptr [ebp - 0x30], 0
004c7683 837dd000                 cmp        dword ptr [ebp - 0x30], 0
004c7687 741d                     je         0x4c76a6
004c7689 6a04                     push       4
004c768b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c768e e80debffff               call       0x4c61a0
004c7693 6a00                     push       0
004c7695 6a07                     push       7
004c7697 b930a85b00               mov        ecx, 0x5ba830
004c769c e8cff6f5ff               call       0x426d70
004c76a1 90                       nop        
004c76a2 eb66                     jmp        0x4c770a
004c76a4 eb64                     jmp        0x4c770a
004c76a6 e9a0000000               jmp        0x4c774b
004c76ab 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c76ae 81c2d0000000             add        edx, 0xd0
004c76b4 89559c                   mov        dword ptr [ebp - 0x64], edx
004c76b7 6a00                     push       0
004c76b9 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
004c76bc e87fbef5ff               call       0x423540
004c76c1 90                       nop        
004c76c2 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c76c5 05c0000000               add        eax, 0xc0
004c76ca 894598                   mov        dword ptr [ebp - 0x68], eax
004c76cd 6a0a                     push       0xa
004c76cf 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
004c76d2 e8e9bef5ff               call       0x4235c0
004c76d7 0fb6c8                   movzx      ecx, al
004c76da 85c9                     test       ecx, ecx
004c76dc 742a                     je         0x4c7708
004c76de 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c76e1 83c114                   add        ecx, 0x14
004c76e4 e8174cf4ff               call       0x40c300
004c76e9 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c76ec 89820c010000             mov        dword ptr [edx + 0x10c], eax
004c76f2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c76f5 83c114                   add        ecx, 0x14
004c76f8 e8b3e9ffff               call       0x4c60b0
004c76fd 6a02                     push       2
004c76ff 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7702 e8c9eaffff               call       0x4c61d0
004c7707 90                       nop        
004c7708 eb41                     jmp        0x4c774b
004c770a 6a00                     push       0
004c770c 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c7712 e879a2f5ff               call       0x421990
004c7717 b904000000               mov        ecx, 4
004c771c 6bd100                   imul       edx, ecx, 0
004c771f 8982704f5c00             mov        dword ptr [edx + 0x5c4f70], eax
004c7725 6a01                     push       1
004c7727 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c772d e85ea2f5ff               call       0x421990
004c7732 b904000000               mov        ecx, 4
004c7737 c1e100                   shl        ecx, 0
004c773a 8981704f5c00             mov        dword ptr [ecx + 0x5c4f70], eax
004c7740 6a03                     push       3
004c7742 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7745 e886eaffff               call       0x4c61d0
004c774a 90                       nop        
004c774b b801000000               mov        eax, 1
004c7750 5f                       pop        edi
004c7751 5e                       pop        esi
004c7752 8be5                     mov        esp, ebp
004c7754 5d                       pop        ebp
004c7755 c3                       ret        
004c7756 6690                     nop        
004c7758 8b714c                   mov        esi, dword ptr [ecx + 0x4c]
004c775b 000c72                   add        byte ptr [edx + esi*2], cl
004c775e 4c                       dec        esp
004c775f 005d72                   add        byte ptr [ebp + 0x72], bl
004c7762 4c                       dec        esp
004c7763 00ab764c000a             add        byte ptr [ebx + 0xa004c76], ch
004c7769 774c                     ja         0x4c77b7
004c776b 00cc                     add        ah, cl
004c776d cc                       int3       
004c776e cc                       int3       
004c776f cc                       int3       
