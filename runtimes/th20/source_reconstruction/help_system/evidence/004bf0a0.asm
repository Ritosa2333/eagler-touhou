004bf0a0 55                       push       ebp
004bf0a1 8bec                     mov        ebp, esp
004bf0a3 81ecb8000000             sub        esp, 0xb8
004bf0a9 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004bf0ae 33c5                     xor        eax, ebp
004bf0b0 8945fc                   mov        dword ptr [ebp - 4], eax
004bf0b3 894dec                   mov        dword ptr [ebp - 0x14], ecx
004bf0b6 51                       push       ecx
004bf0b7 0f57c0                   xorps      xmm0, xmm0
004bf0ba f30f110424               movss      dword ptr [esp], xmm0
004bf0bf 51                       push       ecx
004bf0c0 0f57c0                   xorps      xmm0, xmm0
004bf0c3 f30f110424               movss      dword ptr [esp], xmm0
004bf0c8 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004bf0cb 51                       push       ecx
004bf0cc f30f1080ac000000         movss      xmm0, dword ptr [eax + 0xac]
004bf0d4 f30f110424               movss      dword ptr [esp], xmm0
004bf0d9 8d4df0                   lea        ecx, [ebp - 0x10]
004bf0dc e8ef3cf6ff               call       0x422dd0
004bf0e1 90                       nop        
004bf0e2 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf0e5 8b5110                   mov        edx, dword ptr [ecx + 0x10]
004bf0e8 8955d4                   mov        dword ptr [ebp - 0x2c], edx
004bf0eb 837dd400                 cmp        dword ptr [ebp - 0x2c], 0
004bf0ef 7415                     je         0x4bf106
004bf0f1 837dd401                 cmp        dword ptr [ebp - 0x2c], 1
004bf0f5 741e                     je         0x4bf115
004bf0f7 837dd402                 cmp        dword ptr [ebp - 0x2c], 2
004bf0fb 0f845e070000             je         0x4bf85f
004bf101 e986070000               jmp        0x4bf88c
004bf106 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004bf109 c7401001000000           mov        dword ptr [eax + 0x10], 1
004bf110 e977070000               jmp        0x4bf88c
004bf115 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf118 8b91b8000000             mov        edx, dword ptr [ecx + 0xb8]
004bf11e 8955d0                   mov        dword ptr [ebp - 0x30], edx
004bf121 837dd005                 cmp        dword ptr [ebp - 0x30], 5
004bf125 0f8732070000             ja         0x4bf85d
004bf12b 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004bf12e ff2485b8f84b00           jmp        dword ptr [eax*4 + 0x4bf8b8]
004bf135 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf138 83c124                   add        ecx, 0x24
004bf13b 898d70ffffff             mov        dword ptr [ebp - 0x90], ecx
004bf141 6a09                     push       9
004bf143 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
004bf149 e892d3fdff               call       0x49c4e0
004bf14e 6a00                     push       0
004bf150 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf153 83c124                   add        ecx, 0x24
004bf156 e8f5fbffff               call       0x4bed50
004bf15b 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004bf15e 83c224                   add        edx, 0x24
004bf161 8955a8                   mov        dword ptr [ebp - 0x58], edx
004bf164 6a01                     push       1
004bf166 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004bf169 e8720a0000               call       0x4bfbe0
004bf16e 90                       nop        
004bf16f c745e800000000           mov        dword ptr [ebp - 0x18], 0
004bf176 eb09                     jmp        0x4bf181
004bf178 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004bf17b 83c001                   add        eax, 1
004bf17e 8945e8                   mov        dword ptr [ebp - 0x18], eax
004bf181 837de809                 cmp        dword ptr [ebp - 0x18], 9
004bf185 7d3f                     jge        0x4bf1c6
004bf187 8d4df0                   lea        ecx, [ebp - 0x10]
004bf18a 51                       push       ecx
004bf18b 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004bf18e 52                       push       edx
004bf18f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf192 e8890a0000               call       0x4bfc20
004bf197 90                       nop        
004bf198 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf19b 83c124                   add        ecx, 0x24
004bf19e e85dd1f4ff               call       0x40c300
004bf1a3 3b45e8                   cmp        eax, dword ptr [ebp - 0x18]
004bf1a6 750f                     jne        0x4bf1b7
004bf1a8 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004bf1ab 50                       push       eax
004bf1ac 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf1af e86c090000               call       0x4bfb20
004bf1b4 90                       nop        
004bf1b5 eb0d                     jmp        0x4bf1c4
004bf1b7 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004bf1ba 51                       push       ecx
004bf1bb 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf1be e8bd090000               call       0x4bfb80
004bf1c3 90                       nop        
004bf1c4 ebb2                     jmp        0x4bf178
004bf1c6 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004bf1c9 8b82b0000000             mov        eax, dword ptr [edx + 0xb0]
004bf1cf 8945a4                   mov        dword ptr [ebp - 0x5c], eax
004bf1d2 6a01                     push       1
004bf1d4 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
004bf1d7 e804090000               call       0x4bfae0
004bf1dc 8bc8                     mov        ecx, eax
004bf1de e89dabf8ff               call       0x449d80
004bf1e3 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf1e6 c781b800000001000000     mov        dword ptr [ecx + 0xb8], 1
004bf1f0 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004bf1f3 83c214                   add        edx, 0x14
004bf1f6 8955a0                   mov        dword ptr [ebp - 0x60], edx
004bf1f9 6a14                     push       0x14
004bf1fb 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
004bf1fe e85d43f6ff               call       0x423560
004bf203 0fb6c0                   movzx      eax, al
004bf206 85c0                     test       eax, eax
004bf208 7405                     je         0x4bf20f
004bf20a e94e060000               jmp        0x4bf85d
004bf20f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf212 83c124                   add        ecx, 0x24
004bf215 e8e6090000               call       0x4bfc00
004bf21a 90                       nop        
004bf21b 33c9                     xor        ecx, ecx
004bf21d 752f                     jne        0x4bf24e
004bf21f ba04000000               mov        edx, 4
004bf224 d1e2                     shl        edx, 1
004bf226 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
004bf22d 741f                     je         0x4bf24e
004bf22f b804000000               mov        eax, 4
004bf234 d1e0                     shl        eax, 1
004bf236 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
004bf23c 894d9c                   mov        dword ptr [ebp - 0x64], ecx
004bf23f 6a10                     push       0x10
004bf241 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
004bf244 e837b0f5ff               call       0x41a280
004bf249 8945cc                   mov        dword ptr [ebp - 0x34], eax
004bf24c eb07                     jmp        0x4bf255
004bf24e c745cc00000000           mov        dword ptr [ebp - 0x34], 0
004bf255 837dcc00                 cmp        dword ptr [ebp - 0x34], 0
004bf259 7414                     je         0x4bf26f
004bf25b 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004bf25e 83c224                   add        edx, 0x24
004bf261 895598                   mov        dword ptr [ebp - 0x68], edx
004bf264 6a00                     push       0
004bf266 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
004bf269 e8d2fdffff               call       0x4bf040
004bf26e 90                       nop        
004bf26f 33c0                     xor        eax, eax
004bf271 752f                     jne        0x4bf2a2
004bf273 b904000000               mov        ecx, 4
004bf278 d1e1                     shl        ecx, 1
004bf27a 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
004bf281 741f                     je         0x4bf2a2
004bf283 ba04000000               mov        edx, 4
004bf288 d1e2                     shl        edx, 1
004bf28a 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
004bf290 894594                   mov        dword ptr [ebp - 0x6c], eax
004bf293 6a20                     push       0x20
004bf295 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
004bf298 e8e3aff5ff               call       0x41a280
004bf29d 8945c8                   mov        dword ptr [ebp - 0x38], eax
004bf2a0 eb07                     jmp        0x4bf2a9
004bf2a2 c745c800000000           mov        dword ptr [ebp - 0x38], 0
004bf2a9 837dc800                 cmp        dword ptr [ebp - 0x38], 0
004bf2ad 7414                     je         0x4bf2c3
004bf2af 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf2b2 83c124                   add        ecx, 0x24
004bf2b5 894d90                   mov        dword ptr [ebp - 0x70], ecx
004bf2b8 6a00                     push       0
004bf2ba 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
004bf2bd e85efdffff               call       0x4bf020
004bf2c2 90                       nop        
004bf2c3 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf2c6 83c124                   add        ecx, 0x24
004bf2c9 e862ddf9ff               call       0x45d030
004bf2ce 85c0                     test       eax, eax
004bf2d0 7455                     je         0x4bf327
004bf2d2 6a00                     push       0
004bf2d4 6a0a                     push       0xa
004bf2d6 b930a85b00               mov        ecx, 0x5ba830
004bf2db e8907af6ff               call       0x426d70
004bf2e0 90                       nop        
004bf2e1 c745e000000000           mov        dword ptr [ebp - 0x20], 0
004bf2e8 eb09                     jmp        0x4bf2f3
004bf2ea 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004bf2ed 83c201                   add        edx, 1
004bf2f0 8955e0                   mov        dword ptr [ebp - 0x20], edx
004bf2f3 837de009                 cmp        dword ptr [ebp - 0x20], 9
004bf2f7 7d2e                     jge        0x4bf327
004bf2f9 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf2fc 83c124                   add        ecx, 0x24
004bf2ff e8fccff4ff               call       0x40c300
004bf304 3b45e0                   cmp        eax, dword ptr [ebp - 0x20]
004bf307 750f                     jne        0x4bf318
004bf309 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004bf30c 50                       push       eax
004bf30d 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf310 e80b080000               call       0x4bfb20
004bf315 90                       nop        
004bf316 eb0d                     jmp        0x4bf325
004bf318 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004bf31b 51                       push       ecx
004bf31c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf31f e85c080000               call       0x4bfb80
004bf324 90                       nop        
004bf325 ebc3                     jmp        0x4bf2ea
004bf327 33d2                     xor        edx, edx
004bf329 7532                     jne        0x4bf35d
004bf32b b804000000               mov        eax, 4
004bf330 d1e0                     shl        eax, 1
004bf332 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
004bf339 7422                     je         0x4bf35d
004bf33b b904000000               mov        ecx, 4
004bf340 d1e1                     shl        ecx, 1
004bf342 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
004bf348 89558c                   mov        dword ptr [ebp - 0x74], edx
004bf34b 6801000800               push       0x80001
004bf350 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004bf353 e8a8a8f5ff               call       0x419c00
004bf358 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004bf35b eb07                     jmp        0x4bf364
004bf35d c745c400000000           mov        dword ptr [ebp - 0x3c], 0
004bf364 837dc400                 cmp        dword ptr [ebp - 0x3c], 0
004bf368 0f848d000000             je         0x4bf3fb
004bf36e 6a00                     push       0
004bf370 6a07                     push       7
004bf372 b930a85b00               mov        ecx, 0x5ba830
004bf377 e8f479f6ff               call       0x426d70
004bf37c 90                       nop        
004bf37d 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004bf380 c780b800000002000000     mov        dword ptr [eax + 0xb8], 2
004bf38a 6a00                     push       0
004bf38c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf38f 83c114                   add        ecx, 0x14
004bf392 e88941f6ff               call       0x423520
004bf397 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf39a 83c124                   add        ecx, 0x24
004bf39d e85ecff4ff               call       0x40c300
004bf3a2 83c001                   add        eax, 1
004bf3a5 50                       push       eax
004bf3a6 68680c5700               push       0x570c68
004bf3ab 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf3ae 81c1bc000000             add        ecx, 0xbc
004bf3b4 51                       push       ecx
004bf3b5 e886faffff               call       0x4bee40
004bf3ba 83c40c                   add        esp, 0xc
004bf3bd 6a00                     push       0
004bf3bf 68d0f84b00               push       0x4bf8d0
004bf3c4 b9404d5c00               mov        ecx, 0x5c4d40
004bf3c9 e822a6ffff               call       0x4b99f0
004bf3ce 90                       nop        
004bf3cf c745dc00000000           mov        dword ptr [ebp - 0x24], 0
004bf3d6 eb09                     jmp        0x4bf3e1
004bf3d8 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
004bf3db 83c201                   add        edx, 1
004bf3de 8955dc                   mov        dword ptr [ebp - 0x24], edx
004bf3e1 837ddc09                 cmp        dword ptr [ebp - 0x24], 9
004bf3e5 7d0f                     jge        0x4bf3f6
004bf3e7 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004bf3ea 50                       push       eax
004bf3eb 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf3ee e85d070000               call       0x4bfb50
004bf3f3 90                       nop        
004bf3f4 ebe2                     jmp        0x4bf3d8
004bf3f6 e962040000               jmp        0x4bf85d
004bf3fb 33c9                     xor        ecx, ecx
004bf3fd 7532                     jne        0x4bf431
004bf3ff ba04000000               mov        edx, 4
004bf404 d1e2                     shl        edx, 1
004bf406 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
004bf40d 7422                     je         0x4bf431
004bf40f b804000000               mov        eax, 4
004bf414 d1e0                     shl        eax, 1
004bf416 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
004bf41c 894d88                   mov        dword ptr [ebp - 0x78], ecx
004bf41f 6806010000               push       0x106
004bf424 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004bf427 e8d4a7f5ff               call       0x419c00
004bf42c 8945c0                   mov        dword ptr [ebp - 0x40], eax
004bf42f eb07                     jmp        0x4bf438
004bf431 c745c000000000           mov        dword ptr [ebp - 0x40], 0
004bf438 837dc000                 cmp        dword ptr [ebp - 0x40], 0
004bf43c 7460                     je         0x4bf49e
004bf43e 6a00                     push       0
004bf440 6a09                     push       9
004bf442 b930a85b00               mov        ecx, 0x5ba830
004bf447 e82479f6ff               call       0x426d70
004bf44c 90                       nop        
004bf44d c745d800000000           mov        dword ptr [ebp - 0x28], 0
004bf454 eb09                     jmp        0x4bf45f
004bf456 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004bf459 83c201                   add        edx, 1
004bf45c 8955d8                   mov        dword ptr [ebp - 0x28], edx
004bf45f 837dd809                 cmp        dword ptr [ebp - 0x28], 9
004bf463 7d0f                     jge        0x4bf474
004bf465 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004bf468 50                       push       eax
004bf469 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf46c e8df060000               call       0x4bfb50
004bf471 90                       nop        
004bf472 ebe2                     jmp        0x4bf456
004bf474 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf477 c7411002000000           mov        dword ptr [ecx + 0x10], 2
004bf47e 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004bf481 c782b800000000000000     mov        dword ptr [edx + 0xb8], 0
004bf48b 6a00                     push       0
004bf48d 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf490 83c114                   add        ecx, 0x14
004bf493 e88840f6ff               call       0x423520
004bf498 90                       nop        
004bf499 e9bf030000               jmp        0x4bf85d
004bf49e e9ba030000               jmp        0x4bf85d
004bf4a3 e9b5030000               jmp        0x4bf85d
004bf4a8 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004bf4ab 8b883c010000             mov        ecx, dword ptr [eax + 0x13c]
004bf4b1 894d80                   mov        dword ptr [ebp - 0x80], ecx
004bf4b4 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004bf4b7 8b82b4000000             mov        eax, dword ptr [edx + 0xb4]
004bf4bd 89857cffffff             mov        dword ptr [ebp - 0x84], eax
004bf4c3 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf4c6 8b91b0000000             mov        edx, dword ptr [ecx + 0xb0]
004bf4cc 895584                   mov        dword ptr [ebp - 0x7c], edx
004bf4cf 6a01                     push       1
004bf4d1 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
004bf4d4 e807060000               call       0x4bfae0
004bf4d9 898578ffffff             mov        dword ptr [ebp - 0x88], eax
004bf4df 6a00                     push       0
004bf4e1 6a00                     push       0
004bf4e3 6a01                     push       1
004bf4e5 8b4580                   mov        eax, dword ptr [ebp - 0x80]
004bf4e8 50                       push       eax
004bf4e9 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004bf4ef 51                       push       ecx
004bf4f0 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
004bf4f6 52                       push       edx
004bf4f7 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004bf4fd e81ee7f8ff               call       0x44dc20
004bf502 90                       nop        
004bf503 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004bf506 83b8b400000000           cmp        dword ptr [eax + 0xb4], 0
004bf50d 7422                     je         0x4bf531
004bf50f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf512 8b91b4000000             mov        edx, dword ptr [ecx + 0xb4]
004bf518 52                       push       edx
004bf519 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004bf51f e84c01f6ff               call       0x41f670
004bf524 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004bf527 c780b400000000000000     mov        dword ptr [eax + 0xb4], 0
004bf531 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf534 c781b400000000000000     mov        dword ptr [ecx + 0xb4], 0
004bf53e 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004bf541 8b82b0000000             mov        eax, dword ptr [edx + 0xb0]
004bf547 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
004bf54d 6a01                     push       1
004bf54f 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
004bf555 e8a6050000               call       0x4bfb00
004bf55a 8945bc                   mov        dword ptr [ebp - 0x44], eax
004bf55d 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004bf560 8b11                     mov        edx, dword ptr [ecx]
004bf562 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004bf565 50                       push       eax
004bf566 8b4a24                   mov        ecx, dword ptr [edx + 0x24]
004bf569 ffd1                     call       ecx
004bf56b 8d55f0                   lea        edx, [ebp - 0x10]
004bf56e 52                       push       edx
004bf56f 6a0d                     push       0xd
004bf571 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf574 e8a7060000               call       0x4bfc20
004bf579 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004bf57c c780b800000004000000     mov        dword ptr [eax + 0xb8], 4
004bf586 6a00                     push       0
004bf588 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf58b 83c114                   add        ecx, 0x14
004bf58e e88d3ff6ff               call       0x423520
004bf593 90                       nop        
004bf594 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf597 83c114                   add        ecx, 0x14
004bf59a 898d5cffffff             mov        dword ptr [ebp - 0xa4], ecx
004bf5a0 6a14                     push       0x14
004bf5a2 8b8d5cffffff             mov        ecx, dword ptr [ebp - 0xa4]
004bf5a8 e8b33ff6ff               call       0x423560
004bf5ad 0fb6d0                   movzx      edx, al
004bf5b0 85d2                     test       edx, edx
004bf5b2 7405                     je         0x4bf5b9
004bf5b4 e9a4020000               jmp        0x4bf85d
004bf5b9 33c0                     xor        eax, eax
004bf5bb 7535                     jne        0x4bf5f2
004bf5bd b904000000               mov        ecx, 4
004bf5c2 d1e1                     shl        ecx, 1
004bf5c4 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
004bf5cb 7425                     je         0x4bf5f2
004bf5cd ba04000000               mov        edx, 4
004bf5d2 d1e2                     shl        edx, 1
004bf5d4 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
004bf5da 89856cffffff             mov        dword ptr [ebp - 0x94], eax
004bf5e0 6a20                     push       0x20
004bf5e2 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
004bf5e8 e813a6f5ff               call       0x419c00
004bf5ed 8945b8                   mov        dword ptr [ebp - 0x48], eax
004bf5f0 eb07                     jmp        0x4bf5f9
004bf5f2 c745b800000000           mov        dword ptr [ebp - 0x48], 0
004bf5f9 837db800                 cmp        dword ptr [ebp - 0x48], 0
004bf5fd 7468                     je         0x4bf667
004bf5ff 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf602 83c124                   add        ecx, 0x24
004bf605 e8f6ccf4ff               call       0x40c300
004bf60a 83f808                   cmp        eax, 8
004bf60d 7d58                     jge        0x4bf667
004bf60f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf612 c781b800000005000000     mov        dword ptr [ecx + 0xb8], 5
004bf61c 6a00                     push       0
004bf61e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf621 83c114                   add        ecx, 0x14
004bf624 e8f73ef6ff               call       0x423520
004bf629 6a00                     push       0
004bf62b 6a07                     push       7
004bf62d b930a85b00               mov        ecx, 0x5ba830
004bf632 e83977f6ff               call       0x426d70
004bf637 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004bf63a 83c224                   add        edx, 0x24
004bf63d 899568ffffff             mov        dword ptr [ebp - 0x98], edx
004bf643 6a00                     push       0
004bf645 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
004bf64b e8d0f9ffff               call       0x4bf020
004bf650 6a07                     push       7
004bf652 6a0d                     push       0xd
004bf654 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf657 e854050000               call       0x4bfbb0
004bf65c 90                       nop        
004bf65d e9d6010000               jmp        0x4bf838
004bf662 e9d1010000               jmp        0x4bf838
004bf667 33c0                     xor        eax, eax
004bf669 7535                     jne        0x4bf6a0
004bf66b b904000000               mov        ecx, 4
004bf670 d1e1                     shl        ecx, 1
004bf672 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
004bf679 7425                     je         0x4bf6a0
004bf67b ba04000000               mov        edx, 4
004bf680 d1e2                     shl        edx, 1
004bf682 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
004bf688 898564ffffff             mov        dword ptr [ebp - 0x9c], eax
004bf68e 6a10                     push       0x10
004bf690 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
004bf696 e865a5f5ff               call       0x419c00
004bf69b 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004bf69e eb07                     jmp        0x4bf6a7
004bf6a0 c745b400000000           mov        dword ptr [ebp - 0x4c], 0
004bf6a7 837db400                 cmp        dword ptr [ebp - 0x4c], 0
004bf6ab 7467                     je         0x4bf714
004bf6ad 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf6b0 83c124                   add        ecx, 0x24
004bf6b3 e848ccf4ff               call       0x40c300
004bf6b8 85c0                     test       eax, eax
004bf6ba 7e58                     jle        0x4bf714
004bf6bc 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf6bf c781b800000005000000     mov        dword ptr [ecx + 0xb8], 5
004bf6c9 6a00                     push       0
004bf6cb 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf6ce 83c114                   add        ecx, 0x14
004bf6d1 e84a3ef6ff               call       0x423520
004bf6d6 6a00                     push       0
004bf6d8 6a07                     push       7
004bf6da b930a85b00               mov        ecx, 0x5ba830
004bf6df e88c76f6ff               call       0x426d70
004bf6e4 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004bf6e7 83c224                   add        edx, 0x24
004bf6ea 899560ffffff             mov        dword ptr [ebp - 0xa0], edx
004bf6f0 6a00                     push       0
004bf6f2 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
004bf6f8 e843f9ffff               call       0x4bf040
004bf6fd 6a08                     push       8
004bf6ff 6a0d                     push       0xd
004bf701 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf704 e8a7040000               call       0x4bfbb0
004bf709 90                       nop        
004bf70a e929010000               jmp        0x4bf838
004bf70f e924010000               jmp        0x4bf838
004bf714 33c0                     xor        eax, eax
004bf716 7538                     jne        0x4bf750
004bf718 b904000000               mov        ecx, 4
004bf71d d1e1                     shl        ecx, 1
004bf71f 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
004bf726 7428                     je         0x4bf750
004bf728 ba04000000               mov        edx, 4
004bf72d d1e2                     shl        edx, 1
004bf72f 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
004bf735 898548ffffff             mov        dword ptr [ebp - 0xb8], eax
004bf73b 6801000800               push       0x80001
004bf740 8b8d48ffffff             mov        ecx, dword ptr [ebp - 0xb8]
004bf746 e8b5a4f5ff               call       0x419c00
004bf74b 8945b0                   mov        dword ptr [ebp - 0x50], eax
004bf74e eb07                     jmp        0x4bf757
004bf750 c745b000000000           mov        dword ptr [ebp - 0x50], 0
004bf757 837db000                 cmp        dword ptr [ebp - 0x50], 0
004bf75b 754d                     jne        0x4bf7aa
004bf75d 33c9                     xor        ecx, ecx
004bf75f 7538                     jne        0x4bf799
004bf761 ba04000000               mov        edx, 4
004bf766 d1e2                     shl        edx, 1
004bf768 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
004bf76f 7428                     je         0x4bf799
004bf771 b804000000               mov        eax, 4
004bf776 d1e0                     shl        eax, 1
004bf778 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
004bf77e 898d58ffffff             mov        dword ptr [ebp - 0xa8], ecx
004bf784 6806010000               push       0x106
004bf789 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
004bf78f e86ca4f5ff               call       0x419c00
004bf794 8945ac                   mov        dword ptr [ebp - 0x54], eax
004bf797 eb07                     jmp        0x4bf7a0
004bf799 c745ac00000000           mov        dword ptr [ebp - 0x54], 0
004bf7a0 837dac00                 cmp        dword ptr [ebp - 0x54], 0
004bf7a4 0f848c000000             je         0x4bf836
004bf7aa 6a00                     push       0
004bf7ac 6a09                     push       9
004bf7ae b930a85b00               mov        ecx, 0x5ba830
004bf7b3 e8b875f6ff               call       0x426d70
004bf7b8 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004bf7bb c782b800000001000000     mov        dword ptr [edx + 0xb8], 1
004bf7c5 6a00                     push       0
004bf7c7 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf7ca 83c114                   add        ecx, 0x14
004bf7cd e84e3df6ff               call       0x423520
004bf7d2 6a0d                     push       0xd
004bf7d4 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf7d7 e874030000               call       0x4bfb50
004bf7dc 90                       nop        
004bf7dd c745e400000000           mov        dword ptr [ebp - 0x1c], 0
004bf7e4 eb09                     jmp        0x4bf7ef
004bf7e6 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004bf7e9 83c001                   add        eax, 1
004bf7ec 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004bf7ef 837de409                 cmp        dword ptr [ebp - 0x1c], 9
004bf7f3 7d3f                     jge        0x4bf834
004bf7f5 8d4df0                   lea        ecx, [ebp - 0x10]
004bf7f8 51                       push       ecx
004bf7f9 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004bf7fc 52                       push       edx
004bf7fd 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf800 e81b040000               call       0x4bfc20
004bf805 90                       nop        
004bf806 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf809 83c124                   add        ecx, 0x24
004bf80c e8efcaf4ff               call       0x40c300
004bf811 3b45e4                   cmp        eax, dword ptr [ebp - 0x1c]
004bf814 750f                     jne        0x4bf825
004bf816 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004bf819 50                       push       eax
004bf81a 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf81d e8fe020000               call       0x4bfb20
004bf822 90                       nop        
004bf823 eb0d                     jmp        0x4bf832
004bf825 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004bf828 51                       push       ecx
004bf829 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf82c e84f030000               call       0x4bfb80
004bf831 90                       nop        
004bf832 ebb2                     jmp        0x4bf7e6
004bf834 eb27                     jmp        0x4bf85d
004bf836 eb25                     jmp        0x4bf85d
004bf838 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004bf83b 83c214                   add        edx, 0x14
004bf83e 899554ffffff             mov        dword ptr [ebp - 0xac], edx
004bf844 6a14                     push       0x14
004bf846 8b8d54ffffff             mov        ecx, dword ptr [ebp - 0xac]
004bf84c e86f3df6ff               call       0x4235c0
004bf851 0fb6c0                   movzx      eax, al
004bf854 85c0                     test       eax, eax
004bf856 7405                     je         0x4bf85d
004bf858 e920fbffff               jmp        0x4bf37d
004bf85d eb2d                     jmp        0x4bf88c
004bf85f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf862 83c114                   add        ecx, 0x14
004bf865 898d50ffffff             mov        dword ptr [ebp - 0xb0], ecx
004bf86b 6a1e                     push       0x1e
004bf86d 8b8d50ffffff             mov        ecx, dword ptr [ebp - 0xb0]
004bf873 e8483df6ff               call       0x4235c0
004bf878 0fb6d0                   movzx      edx, al
004bf87b 85d2                     test       edx, edx
004bf87d 740d                     je         0x4bf88c
004bf87f 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004bf882 c780a800000001000000     mov        dword ptr [eax + 0xa8], 1
004bf88c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bf88f 83c114                   add        ecx, 0x14
004bf892 898d4cffffff             mov        dword ptr [ebp - 0xb4], ecx
004bf898 6a00                     push       0
004bf89a 8b8d4cffffff             mov        ecx, dword ptr [ebp - 0xb4]
004bf8a0 e89b3cf6ff               call       0x423540
004bf8a5 b801000000               mov        eax, 1
004bf8aa 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bf8ad 33cd                     xor        ecx, ebp
004bf8af e83a310800               call       0x5429ee
004bf8b4 8be5                     mov        esp, ebp
004bf8b6 5d                       pop        ebp
004bf8b7 c3                       ret        
004bf8b8 35f14b00f0               xor        eax, 0xf0004bf1
004bf8bd f1                       int1       
004bf8be 4b                       dec        ebx
004bf8bf 005df8                   add        byte ptr [ebp - 8], bl
004bf8c2 4b                       dec        ebx
004bf8c3 00a8f44b0094             add        byte ptr [eax - 0x6bffb40c], ch
004bf8c9 f5                       cmc        
004bf8ca 4b                       dec        ebx
004bf8cb 0038                     add        byte ptr [eax], bh
004bf8cd f8                       clc        
004bf8ce 4b                       dec        ebx
