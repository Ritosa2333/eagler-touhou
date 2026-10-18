004c6230 53                       push       ebx
004c6231 8bdc                     mov        ebx, esp
004c6233 83ec08                   sub        esp, 8
004c6236 83e4f8                   and        esp, 0xfffffff8
004c6239 83c404                   add        esp, 4
004c623c 55                       push       ebp
004c623d 8b6b04                   mov        ebp, dword ptr [ebx + 4]
004c6240 896c2404                 mov        dword ptr [esp + 4], ebp
004c6244 8bec                     mov        ebp, esp
004c6246 81ecc8000000             sub        esp, 0xc8
004c624c 894dfc                   mov        dword ptr [ebp - 4], ecx
004c624f 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c6252 8b880c010000             mov        ecx, dword ptr [eax + 0x10c]
004c6258 51                       push       ecx
004c6259 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c625f e8bcfbffff               call       0x4c5e20
004c6264 8bc8                     mov        ecx, eax
004c6266 e8e5fbffff               call       0x4c5e50
004c626b 85c0                     test       eax, eax
004c626d 742a                     je         0x4c6299
004c626f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c6272 83c260                   add        edx, 0x60
004c6275 8955e0                   mov        dword ptr [ebp - 0x20], edx
004c6278 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c627b 8b880c010000             mov        ecx, dword ptr [eax + 0x10c]
004c6281 51                       push       ecx
004c6282 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c6288 e863b5f5ff               call       0x4217f0
004c628d 8945d8                   mov        dword ptr [ebp - 0x28], eax
004c6290 c745dcb09e5b00           mov        dword ptr [ebp - 0x24], 0x5b9eb0
004c6297 eb7e                     jmp        0x4c6317
004c6299 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c629c 8b820c010000             mov        eax, dword ptr [edx + 0x10c]
004c62a2 50                       push       eax
004c62a3 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c62a9 e872fbffff               call       0x4c5e20
004c62ae 8bc8                     mov        ecx, eax
004c62b0 e8fbfbffff               call       0x4c5eb0
004c62b5 85c0                     test       eax, eax
004c62b7 7430                     je         0x4c62e9
004c62b9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c62bc 81c180000000             add        ecx, 0x80
004c62c2 894de0                   mov        dword ptr [ebp - 0x20], ecx
004c62c5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c62c8 8b820c010000             mov        eax, dword ptr [edx + 0x10c]
004c62ce 50                       push       eax
004c62cf 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c62d5 e816b5f5ff               call       0x4217f0
004c62da 83c010                   add        eax, 0x10
004c62dd 8945d8                   mov        dword ptr [ebp - 0x28], eax
004c62e0 c745dcc09e5b00           mov        dword ptr [ebp - 0x24], 0x5b9ec0
004c62e7 eb2e                     jmp        0x4c6317
004c62e9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c62ec 81c1a0000000             add        ecx, 0xa0
004c62f2 894de0                   mov        dword ptr [ebp - 0x20], ecx
004c62f5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c62f8 8b820c010000             mov        eax, dword ptr [edx + 0x10c]
004c62fe 50                       push       eax
004c62ff 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c6305 e8e6b4f5ff               call       0x4217f0
004c630a 83c020                   add        eax, 0x20
004c630d 8945d8                   mov        dword ptr [ebp - 0x28], eax
004c6310 c745dcd09e5b00           mov        dword ptr [ebp - 0x24], 0x5b9ed0
004c6317 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c631a 8b9100010000             mov        edx, dword ptr [ecx + 0x100]
004c6320 8955d4                   mov        dword ptr [ebp - 0x2c], edx
004c6323 837dd404                 cmp        dword ptr [ebp - 0x2c], 4
004c6327 0f87d7070000             ja         0x4c6b04
004c632d 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
004c6330 ff2485106b4c00           jmp        dword ptr [eax*4 + 0x4c6b10]
004c6337 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c633a 8b910c010000             mov        edx, dword ptr [ecx + 0x10c]
004c6340 52                       push       edx
004c6341 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c6347 e8d4faffff               call       0x4c5e20
004c634c 8bc8                     mov        ecx, eax
004c634e e82dfbffff               call       0x4c5e80
004c6353 85c0                     test       eax, eax
004c6355 7416                     je         0x4c636d
004c6357 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c635a 83c014                   add        eax, 0x14
004c635d 8945b8                   mov        dword ptr [ebp - 0x48], eax
004c6360 6a09                     push       9
004c6362 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004c6365 e87661fdff               call       0x49c4e0
004c636a 90                       nop        
004c636b eb14                     jmp        0x4c6381
004c636d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6370 83c114                   add        ecx, 0x14
004c6373 894db4                   mov        dword ptr [ebp - 0x4c], ecx
004c6376 6a06                     push       6
004c6378 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004c637b e86061fdff               call       0x49c4e0
004c6380 90                       nop        
004c6381 6a00                     push       0
004c6383 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6386 83c114                   add        ecx, 0x14
004c6389 e8c289ffff               call       0x4bed50
004c638e 6a00                     push       0
004c6390 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6393 81c1d0000000             add        ecx, 0xd0
004c6399 e882d1f5ff               call       0x423520
004c639e 6a01                     push       1
004c63a0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c63a3 e8f8fdffff               call       0x4c61a0
004c63a8 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c63ab 8b820c010000             mov        eax, dword ptr [edx + 0x10c]
004c63b1 50                       push       eax
004c63b2 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c63b8 e833b4f5ff               call       0x4217f0
004c63bd 50                       push       eax
004c63be 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c63c1 83c160                   add        ecx, 0x60
004c63c4 51                       push       ecx
004c63c5 e816fbffff               call       0x4c5ee0
004c63ca 83c408                   add        esp, 8
004c63cd 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c63d0 8b820c010000             mov        eax, dword ptr [edx + 0x10c]
004c63d6 50                       push       eax
004c63d7 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c63dd e80eb4f5ff               call       0x4217f0
004c63e2 83c010                   add        eax, 0x10
004c63e5 50                       push       eax
004c63e6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c63e9 81c180000000             add        ecx, 0x80
004c63ef 51                       push       ecx
004c63f0 e8ebfaffff               call       0x4c5ee0
004c63f5 83c408                   add        esp, 8
004c63f8 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c63fb 8b820c010000             mov        eax, dword ptr [edx + 0x10c]
004c6401 50                       push       eax
004c6402 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c6408 e8e3b3f5ff               call       0x4217f0
004c640d 83c020                   add        eax, 0x20
004c6410 50                       push       eax
004c6411 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6414 81c1a0000000             add        ecx, 0xa0
004c641a 51                       push       ecx
004c641b e8c0faffff               call       0x4c5ee0
004c6420 83c408                   add        esp, 8
004c6423 6a01                     push       1
004c6425 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6428 e873fdffff               call       0x4c61a0
004c642d 90                       nop        
004c642e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c6431 81c2c0000000             add        edx, 0xc0
004c6437 8955b0                   mov        dword ptr [ebp - 0x50], edx
004c643a 6a06                     push       6
004c643c 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
004c643f e82cacf9ff               call       0x461070
004c6444 0fb6c0                   movzx      eax, al
004c6447 85c0                     test       eax, eax
004c6449 740b                     je         0x4c6456
004c644b 6a02                     push       2
004c644d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6450 e84bfdffff               call       0x4c61a0
004c6455 90                       nop        
004c6456 e9a9060000               jmp        0x4c6b04
004c645b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c645e 83c114                   add        ecx, 0x14
004c6461 e89a97ffff               call       0x4bfc00
004c6466 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6469 8b910c010000             mov        edx, dword ptr [ecx + 0x10c]
004c646f 52                       push       edx
004c6470 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c6476 e8a5f9ffff               call       0x4c5e20
004c647b 8bc8                     mov        ecx, eax
004c647d e87ef9ffff               call       0x4c5e00
004c6482 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004c6485 c745f000000000           mov        dword ptr [ebp - 0x10], 0
004c648c 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c648f 8b880c010000             mov        ecx, dword ptr [eax + 0x10c]
004c6495 51                       push       ecx
004c6496 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c649c e87ff9ffff               call       0x4c5e20
004c64a1 8bc8                     mov        ecx, eax
004c64a3 e8d8f9ffff               call       0x4c5e80
004c64a8 85c0                     test       eax, eax
004c64aa 0f8470020000             je         0x4c6720
004c64b0 33d2                     xor        edx, edx
004c64b2 7531                     jne        0x4c64e5
004c64b4 b804000000               mov        eax, 4
004c64b9 6bc803                   imul       ecx, eax, 3
004c64bc 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
004c64c3 7420                     je         0x4c64e5
004c64c5 ba04000000               mov        edx, 4
004c64ca 6bc203                   imul       eax, edx, 3
004c64cd 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
004c64d3 894dac                   mov        dword ptr [ebp - 0x54], ecx
004c64d6 6a10                     push       0x10
004c64d8 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
004c64db e8a03df5ff               call       0x41a280
004c64e0 8945d0                   mov        dword ptr [ebp - 0x30], eax
004c64e3 eb07                     jmp        0x4c64ec
004c64e5 c745d000000000           mov        dword ptr [ebp - 0x30], 0
004c64ec 837dd000                 cmp        dword ptr [ebp - 0x30], 0
004c64f0 7414                     je         0x4c6506
004c64f2 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c64f5 83c214                   add        edx, 0x14
004c64f8 8955a8                   mov        dword ptr [ebp - 0x58], edx
004c64fb 6a00                     push       0
004c64fd 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004c6500 e83b8bffff               call       0x4bf040
004c6505 90                       nop        
004c6506 33c0                     xor        eax, eax
004c6508 7531                     jne        0x4c653b
004c650a b904000000               mov        ecx, 4
004c650f 6bd103                   imul       edx, ecx, 3
004c6512 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
004c6519 7420                     je         0x4c653b
004c651b b804000000               mov        eax, 4
004c6520 6bc803                   imul       ecx, eax, 3
004c6523 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
004c6529 8955a4                   mov        dword ptr [ebp - 0x5c], edx
004c652c 6a20                     push       0x20
004c652e 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
004c6531 e84a3df5ff               call       0x41a280
004c6536 8945cc                   mov        dword ptr [ebp - 0x34], eax
004c6539 eb07                     jmp        0x4c6542
004c653b c745cc00000000           mov        dword ptr [ebp - 0x34], 0
004c6542 837dcc00                 cmp        dword ptr [ebp - 0x34], 0
004c6546 7414                     je         0x4c655c
004c6548 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c654b 83c014                   add        eax, 0x14
004c654e 8945a0                   mov        dword ptr [ebp - 0x60], eax
004c6551 6a00                     push       0
004c6553 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
004c6556 e8c58affff               call       0x4bf020
004c655b 90                       nop        
004c655c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c655f 83c114                   add        ecx, 0x14
004c6562 e8c96af9ff               call       0x45d030
004c6567 85c0                     test       eax, eax
004c6569 741f                     je         0x4c658a
004c656b 6a00                     push       0
004c656d 6a0a                     push       0xa
004c656f b930a85b00               mov        ecx, 0x5ba830
004c6574 e8f707f6ff               call       0x426d70
004c6579 6a08                     push       8
004c657b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c657e 81c1ec000000             add        ecx, 0xec
004c6584 e897cff5ff               call       0x423520
004c6589 90                       nop        
004c658a c745ec00000000           mov        dword ptr [ebp - 0x14], 0
004c6591 eb09                     jmp        0x4c659c
004c6593 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c6596 83c101                   add        ecx, 1
004c6599 894dec                   mov        dword ptr [ebp - 0x14], ecx
004c659c 837dec48                 cmp        dword ptr [ebp - 0x14], 0x48
004c65a0 7d22                     jge        0x4c65c4
004c65a2 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004c65a5 0fb68270e25a00           movzx      eax, byte ptr [edx + 0x5ae270]
004c65ac 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004c65af 0fb61401                 movzx      edx, byte ptr [ecx + eax]
004c65b3 81e280000000             and        edx, 0x80
004c65b9 7407                     je         0x4c65c2
004c65bb c745f001000000           mov        dword ptr [ebp - 0x10], 1
004c65c2 ebcf                     jmp        0x4c6593
004c65c4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c65c7 83c114                   add        ecx, 0x14
004c65ca e8315df4ff               call       0x40c300
004c65cf 89459c                   mov        dword ptr [ebp - 0x64], eax
004c65d2 837d9c08                 cmp        dword ptr [ebp - 0x64], 8
004c65d6 7402                     je         0x4c65da
004c65d8 eb77                     jmp        0x4c6651
004c65da 33c0                     xor        eax, eax
004c65dc 7534                     jne        0x4c6612
004c65de b904000000               mov        ecx, 4
004c65e3 6bd103                   imul       edx, ecx, 3
004c65e6 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
004c65ed 7423                     je         0x4c6612
004c65ef b804000000               mov        eax, 4
004c65f4 6bc803                   imul       ecx, eax, 3
004c65f7 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
004c65fd 895598                   mov        dword ptr [ebp - 0x68], edx
004c6600 6801000800               push       0x80001
004c6605 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
004c6608 e8733cf5ff               call       0x41a280
004c660d 8945c8                   mov        dword ptr [ebp - 0x38], eax
004c6610 eb07                     jmp        0x4c6619
004c6612 c745c800000000           mov        dword ptr [ebp - 0x38], 0
004c6619 837dc800                 cmp        dword ptr [ebp - 0x38], 0
004c661d 7407                     je         0x4c6626
004c661f c745f001000000           mov        dword ptr [ebp - 0x10], 1
004c6626 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c6629 05c0000000               add        eax, 0xc0
004c662e 894594                   mov        dword ptr [ebp - 0x6c], eax
004c6631 6a0f                     push       0xf
004c6633 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
004c6636 e885cff5ff               call       0x4235c0
004c663b 0fb6c8                   movzx      ecx, al
004c663e 85c9                     test       ecx, ecx
004c6640 740a                     je         0x4c664c
004c6642 e9d3020000               jmp        0x4c691a
004c6647 e9ce020000               jmp        0x4c691a
004c664c e9ca000000               jmp        0x4c671b
004c6651 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c6654 81c2c0000000             add        edx, 0xc0
004c665a 895590                   mov        dword ptr [ebp - 0x70], edx
004c665d 6a0f                     push       0xf
004c665f 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
004c6662 e859cff5ff               call       0x4235c0
004c6667 0fb6c0                   movzx      eax, al
004c666a 85c0                     test       eax, eax
004c666c 0f84a9000000             je         0x4c671b
004c6672 c745f800000000           mov        dword ptr [ebp - 8], 0
004c6679 eb09                     jmp        0x4c6684
004c667b 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c667e 83c101                   add        ecx, 1
004c6681 894df8                   mov        dword ptr [ebp - 8], ecx
004c6684 837df848                 cmp        dword ptr [ebp - 8], 0x48
004c6688 0f8d8d000000             jge        0x4c671b
004c668e 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c6691 0fb68270e25a00           movzx      eax, byte ptr [edx + 0x5ae270]
004c6698 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004c669b 0fb61401                 movzx      edx, byte ptr [ecx + eax]
004c669f 81e280000000             and        edx, 0x80
004c66a5 746f                     je         0x4c6716
004c66a7 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c66aa 0fb68870e25a00           movzx      ecx, byte ptr [eax + 0x5ae270]
004c66b1 894d8c                   mov        dword ptr [ebp - 0x74], ecx
004c66b4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c66b7 83c114                   add        ecx, 0x14
004c66ba e8415cf4ff               call       0x40c300
004c66bf 894588                   mov        dword ptr [ebp - 0x78], eax
004c66c2 8b558c                   mov        edx, dword ptr [ebp - 0x74]
004c66c5 52                       push       edx
004c66c6 8b4588                   mov        eax, dword ptr [ebp - 0x78]
004c66c9 50                       push       eax
004c66ca 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c66cd e8fe160000               call       0x4c7dd0
004c66d2 6a00                     push       0
004c66d4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c66d7 81c1c0000000             add        ecx, 0xc0
004c66dd e83ecef5ff               call       0x423520
004c66e2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c66e5 83c114                   add        ecx, 0x14
004c66e8 894d84                   mov        dword ptr [ebp - 0x7c], ecx
004c66eb 6a00                     push       0
004c66ed 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
004c66f0 e82b89ffff               call       0x4bf020
004c66f5 6a00                     push       0
004c66f7 6a07                     push       7
004c66f9 b930a85b00               mov        ecx, 0x5ba830
004c66fe e86d06f6ff               call       0x426d70
004c6703 6a08                     push       8
004c6705 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6708 81c1ec000000             add        ecx, 0xec
004c670e e80dcef5ff               call       0x423520
004c6713 90                       nop        
004c6714 eb05                     jmp        0x4c671b
004c6716 e960ffffff               jmp        0x4c667b
004c671b e974030000               jmp        0x4c6a94
004c6720 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c6723 83ba0c01000000           cmp        dword ptr [edx + 0x10c], 0
004c672a 7c35                     jl         0x4c6761
004c672c 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c672f 8b880c010000             mov        ecx, dword ptr [eax + 0x10c]
004c6735 833c8d9c885b0000         cmp        dword ptr [ecx*4 + 0x5b889c], 0
004c673d 7422                     je         0x4c6761
004c673f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c6742 8b820c010000             mov        eax, dword ptr [edx + 0x10c]
004c6748 8b0c859c885b00           mov        ecx, dword ptr [eax*4 + 0x5b889c]
004c674f 894d80                   mov        dword ptr [ebp - 0x80], ecx
004c6752 6a10                     push       0x10
004c6754 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004c6757 e8243bf5ff               call       0x41a280
004c675c 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004c675f eb07                     jmp        0x4c6768
004c6761 c745c400000000           mov        dword ptr [ebp - 0x3c], 0
004c6768 837dc400                 cmp        dword ptr [ebp - 0x3c], 0
004c676c 741a                     je         0x4c6788
004c676e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c6771 83c214                   add        edx, 0x14
004c6774 89957cffffff             mov        dword ptr [ebp - 0x84], edx
004c677a 6a00                     push       0
004c677c 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004c6782 e8b988ffff               call       0x4bf040
004c6787 90                       nop        
004c6788 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c678b 83b80c01000000           cmp        dword ptr [eax + 0x10c], 0
004c6792 7c3b                     jl         0x4c67cf
004c6794 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6797 8b910c010000             mov        edx, dword ptr [ecx + 0x10c]
004c679d 833c959c885b0000         cmp        dword ptr [edx*4 + 0x5b889c], 0
004c67a5 7428                     je         0x4c67cf
004c67a7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c67aa 8b880c010000             mov        ecx, dword ptr [eax + 0x10c]
004c67b0 8b148d9c885b00           mov        edx, dword ptr [ecx*4 + 0x5b889c]
004c67b7 899578ffffff             mov        dword ptr [ebp - 0x88], edx
004c67bd 6a20                     push       0x20
004c67bf 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
004c67c5 e8b63af5ff               call       0x41a280
004c67ca 8945c0                   mov        dword ptr [ebp - 0x40], eax
004c67cd eb07                     jmp        0x4c67d6
004c67cf c745c000000000           mov        dword ptr [ebp - 0x40], 0
004c67d6 837dc000                 cmp        dword ptr [ebp - 0x40], 0
004c67da 741a                     je         0x4c67f6
004c67dc 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c67df 83c014                   add        eax, 0x14
004c67e2 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
004c67e8 6a00                     push       0
004c67ea 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
004c67f0 e82b88ffff               call       0x4bf020
004c67f5 90                       nop        
004c67f6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c67f9 83c114                   add        ecx, 0x14
004c67fc e82f68f9ff               call       0x45d030
004c6801 85c0                     test       eax, eax
004c6803 741f                     je         0x4c6824
004c6805 6a00                     push       0
004c6807 6a0a                     push       0xa
004c6809 b930a85b00               mov        ecx, 0x5ba830
004c680e e85d05f6ff               call       0x426d70
004c6813 6a08                     push       8
004c6815 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6818 81c1ec000000             add        ecx, 0xec
004c681e e8fdccf5ff               call       0x423520
004c6823 90                       nop        
004c6824 c745e800000000           mov        dword ptr [ebp - 0x18], 0
004c682b eb09                     jmp        0x4c6836
004c682d 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004c6830 83c101                   add        ecx, 1
004c6833 894de8                   mov        dword ptr [ebp - 0x18], ecx
004c6836 837de81f                 cmp        dword ptr [ebp - 0x18], 0x1f
004c683a 7d19                     jge        0x4c6855
004c683c 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004c683f 0355e8                   add        edx, dword ptr [ebp - 0x18]
004c6842 0fb602                   movzx      eax, byte ptr [edx]
004c6845 2580000000               and        eax, 0x80
004c684a 7407                     je         0x4c6853
004c684c c745f001000000           mov        dword ptr [ebp - 0x10], 1
004c6853 ebd8                     jmp        0x4c682d
004c6855 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6858 83c114                   add        ecx, 0x14
004c685b e8a05af4ff               call       0x40c300
004c6860 8945bc                   mov        dword ptr [ebp - 0x44], eax
004c6863 837dbc04                 cmp        dword ptr [ebp - 0x44], 4
004c6867 7454                     je         0x4c68bd
004c6869 837dbc05                 cmp        dword ptr [ebp - 0x44], 5
004c686d 0f84a7000000             je         0x4c691a
004c6873 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004c687a eb09                     jmp        0x4c6885
004c687c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c687f 83c101                   add        ecx, 1
004c6882 894df4                   mov        dword ptr [ebp - 0xc], ecx
004c6885 837df41f                 cmp        dword ptr [ebp - 0xc], 0x1f
004c6889 7d2d                     jge        0x4c68b8
004c688b 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004c688e 0355f4                   add        edx, dword ptr [ebp - 0xc]
004c6891 0fb602                   movzx      eax, byte ptr [edx]
004c6894 2580000000               and        eax, 0x80
004c6899 741b                     je         0x4c68b6
004c689b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c689e 51                       push       ecx
004c689f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c68a2 83c114                   add        ecx, 0x14
004c68a5 e8565af4ff               call       0x40c300
004c68aa 50                       push       eax
004c68ab 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c68ae e81d150000               call       0x4c7dd0
004c68b3 90                       nop        
004c68b4 eb02                     jmp        0x4c68b8
004c68b6 ebc4                     jmp        0x4c687c
004c68b8 e9d7010000               jmp        0x4c6a94
004c68bd 837df000                 cmp        dword ptr [ebp - 0x10], 0
004c68c1 7452                     je         0x4c6915
004c68c3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c68c6 81c2c0000000             add        edx, 0xc0
004c68cc 899570ffffff             mov        dword ptr [ebp - 0x90], edx
004c68d2 6a14                     push       0x14
004c68d4 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
004c68da e8e1ccf5ff               call       0x4235c0
004c68df 0fb6c0                   movzx      eax, al
004c68e2 85c0                     test       eax, eax
004c68e4 742f                     je         0x4c6915
004c68e6 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004c68e9 51                       push       ecx
004c68ea 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c68ed 52                       push       edx
004c68ee e8edf5ffff               call       0x4c5ee0
004c68f3 83c408                   add        esp, 8
004c68f6 6a00                     push       0
004c68f8 6a07                     push       7
004c68fa b930a85b00               mov        ecx, 0x5ba830
004c68ff e86c04f6ff               call       0x426d70
004c6904 6a00                     push       0
004c6906 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6909 81c1c0000000             add        ecx, 0xc0
004c690f e80cccf5ff               call       0x423520
004c6914 90                       nop        
004c6915 e97a010000               jmp        0x4c6a94
004c691a 837df000                 cmp        dword ptr [ebp - 0x10], 0
004c691e 0f8470010000             je         0x4c6a94
004c6924 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004c6927 50                       push       eax
004c6928 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c692b 51                       push       ecx
004c692c e8df130000               call       0x4c7d10
004c6931 83c408                   add        esp, 8
004c6934 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c6937 8b820c010000             mov        eax, dword ptr [edx + 0x10c]
004c693d 50                       push       eax
004c693e 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c6944 e8a7aef5ff               call       0x4217f0
004c6949 8b08                     mov        ecx, dword ptr [eax]
004c694b 898d58ffffff             mov        dword ptr [ebp - 0xa8], ecx
004c6951 8b5004                   mov        edx, dword ptr [eax + 4]
004c6954 89955cffffff             mov        dword ptr [ebp - 0xa4], edx
004c695a 8b4808                   mov        ecx, dword ptr [eax + 8]
004c695d 898d60ffffff             mov        dword ptr [ebp - 0xa0], ecx
004c6963 8b500c                   mov        edx, dword ptr [eax + 0xc]
004c6966 899564ffffff             mov        dword ptr [ebp - 0x9c], edx
004c696c 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c696f 6b880c01000030           imul       ecx, dword ptr [eax + 0x10c], 0x30
004c6976 81c1104f5c00             add        ecx, 0x5c4f10
004c697c 8b9558ffffff             mov        edx, dword ptr [ebp - 0xa8]
004c6982 8911                     mov        dword ptr [ecx], edx
004c6984 8b855cffffff             mov        eax, dword ptr [ebp - 0xa4]
004c698a 894104                   mov        dword ptr [ecx + 4], eax
004c698d 8b9560ffffff             mov        edx, dword ptr [ebp - 0xa0]
004c6993 895108                   mov        dword ptr [ecx + 8], edx
004c6996 8b8564ffffff             mov        eax, dword ptr [ebp - 0x9c]
004c699c 89410c                   mov        dword ptr [ecx + 0xc], eax
004c699f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c69a2 8b910c010000             mov        edx, dword ptr [ecx + 0x10c]
004c69a8 52                       push       edx
004c69a9 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c69af e83caef5ff               call       0x4217f0
004c69b4 83c010                   add        eax, 0x10
004c69b7 8b08                     mov        ecx, dword ptr [eax]
004c69b9 898d48ffffff             mov        dword ptr [ebp - 0xb8], ecx
004c69bf 8b5004                   mov        edx, dword ptr [eax + 4]
004c69c2 89954cffffff             mov        dword ptr [ebp - 0xb4], edx
004c69c8 8b4808                   mov        ecx, dword ptr [eax + 8]
004c69cb 898d50ffffff             mov        dword ptr [ebp - 0xb0], ecx
004c69d1 8b500c                   mov        edx, dword ptr [eax + 0xc]
004c69d4 899554ffffff             mov        dword ptr [ebp - 0xac], edx
004c69da 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c69dd 6b880c01000030           imul       ecx, dword ptr [eax + 0x10c], 0x30
004c69e4 81c1204f5c00             add        ecx, 0x5c4f20
004c69ea 8b9548ffffff             mov        edx, dword ptr [ebp - 0xb8]
004c69f0 8911                     mov        dword ptr [ecx], edx
004c69f2 8b854cffffff             mov        eax, dword ptr [ebp - 0xb4]
004c69f8 894104                   mov        dword ptr [ecx + 4], eax
004c69fb 8b9550ffffff             mov        edx, dword ptr [ebp - 0xb0]
004c6a01 895108                   mov        dword ptr [ecx + 8], edx
004c6a04 8b8554ffffff             mov        eax, dword ptr [ebp - 0xac]
004c6a0a 89410c                   mov        dword ptr [ecx + 0xc], eax
004c6a0d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6a10 8b910c010000             mov        edx, dword ptr [ecx + 0x10c]
004c6a16 52                       push       edx
004c6a17 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c6a1d e8ceadf5ff               call       0x4217f0
004c6a22 83c020                   add        eax, 0x20
004c6a25 8b08                     mov        ecx, dword ptr [eax]
004c6a27 898d38ffffff             mov        dword ptr [ebp - 0xc8], ecx
004c6a2d 8b5004                   mov        edx, dword ptr [eax + 4]
004c6a30 89953cffffff             mov        dword ptr [ebp - 0xc4], edx
004c6a36 8b4808                   mov        ecx, dword ptr [eax + 8]
004c6a39 898d40ffffff             mov        dword ptr [ebp - 0xc0], ecx
004c6a3f 8b500c                   mov        edx, dword ptr [eax + 0xc]
004c6a42 899544ffffff             mov        dword ptr [ebp - 0xbc], edx
004c6a48 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c6a4b 6b880c01000030           imul       ecx, dword ptr [eax + 0x10c], 0x30
004c6a52 81c1304f5c00             add        ecx, 0x5c4f30
004c6a58 8b9538ffffff             mov        edx, dword ptr [ebp - 0xc8]
004c6a5e 8911                     mov        dword ptr [ecx], edx
004c6a60 8b853cffffff             mov        eax, dword ptr [ebp - 0xc4]
004c6a66 894104                   mov        dword ptr [ecx + 4], eax
004c6a69 8b9540ffffff             mov        edx, dword ptr [ebp - 0xc0]
004c6a6f 895108                   mov        dword ptr [ecx + 8], edx
004c6a72 8b8544ffffff             mov        eax, dword ptr [ebp - 0xbc]
004c6a78 89410c                   mov        dword ptr [ecx + 0xc], eax
004c6a7b 6a00                     push       0
004c6a7d 6a09                     push       9
004c6a7f b930a85b00               mov        ecx, 0x5ba830
004c6a84 e8e702f6ff               call       0x426d70
004c6a89 6a03                     push       3
004c6a8b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6a8e e80df7ffff               call       0x4c61a0
004c6a93 90                       nop        
004c6a94 eb6e                     jmp        0x4c6b04
004c6a96 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6a99 81c1d0000000             add        ecx, 0xd0
004c6a9f 898d6cffffff             mov        dword ptr [ebp - 0x94], ecx
004c6aa5 6a00                     push       0
004c6aa7 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
004c6aad e88ecaf5ff               call       0x423540
004c6ab2 90                       nop        
004c6ab3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c6ab6 81c2c0000000             add        edx, 0xc0
004c6abc 899568ffffff             mov        dword ptr [ebp - 0x98], edx
004c6ac2 6a0a                     push       0xa
004c6ac4 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
004c6aca e8f1caf5ff               call       0x4235c0
004c6acf 0fb6c0                   movzx      eax, al
004c6ad2 85c0                     test       eax, eax
004c6ad4 7416                     je         0x4c6aec
004c6ad6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6ad9 83c114                   add        ecx, 0x14
004c6adc e82ff5ffff               call       0x4c6010
004c6ae1 6a01                     push       1
004c6ae3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6ae6 e8e5f6ffff               call       0x4c61d0
004c6aeb 90                       nop        
004c6aec eb16                     jmp        0x4c6b04
004c6aee 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6af1 83c114                   add        ecx, 0x14
004c6af4 e817f5ffff               call       0x4c6010
004c6af9 6a01                     push       1
004c6afb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6afe e8cdf6ffff               call       0x4c61d0
004c6b03 90                       nop        
004c6b04 b801000000               mov        eax, 1
004c6b09 8be5                     mov        esp, ebp
004c6b0b 5d                       pop        ebp
004c6b0c 8be3                     mov        esp, ebx
004c6b0e 5b                       pop        ebx
004c6b0f c3                       ret        
004c6b10 37                       aaa        
004c6b11 634c002e                 arpl       word ptr [eax + eax + 0x2e], cx
004c6b15 644c                     dec        esp
004c6b17 005b64                   add        byte ptr [ebx + 0x64], bl
004c6b1a 4c                       dec        esp
004c6b1b 00966a4c00ee             add        byte ptr [esi - 0x11ffb396], dl
004c6b21 6a4c                     push       0x4c
004c6b23 00cc                     add        ah, cl
004c6b25 cc                       int3       
004c6b26 cc                       int3       
004c6b27 cc                       int3       
004c6b28 cc                       int3       
004c6b29 cc                       int3       
004c6b2a cc                       int3       
004c6b2b cc                       int3       
004c6b2c cc                       int3       
004c6b2d cc                       int3       
004c6b2e cc                       int3       
004c6b2f cc                       int3       
