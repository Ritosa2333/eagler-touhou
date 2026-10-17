00528a50 55                       push       ebp
00528a51 8bec                     mov        ebp, esp
00528a53 81eca0000000             sub        esp, 0xa0
00528a59 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00528a5e 33c5                     xor        eax, ebp
00528a60 8945fc                   mov        dword ptr [ebp - 4], eax
00528a63 56                       push       esi
00528a64 894de8                   mov        dword ptr [ebp - 0x18], ecx
00528a67 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00528a6a 8b4820                   mov        ecx, dword ptr [eax + 0x20]
00528a6d 894de0                   mov        dword ptr [ebp - 0x20], ecx
00528a70 837de001                 cmp        dword ptr [ebp - 0x20], 1
00528a74 741f                     je         0x528a95
00528a76 837de002                 cmp        dword ptr [ebp - 0x20], 2
00528a7a 7419                     je         0x528a95
00528a7c 837de003                 cmp        dword ptr [ebp - 0x20], 3
00528a80 7405                     je         0x528a87
00528a82 e919050000               jmp        0x528fa0
00528a87 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00528a8a 837a1814                 cmp        dword ptr [edx + 0x18], 0x14
00528a8e 7505                     jne        0x528a95
00528a90 e90b050000               jmp        0x528fa0
00528a95 51                       push       ecx
00528a96 0f57c0                   xorps      xmm0, xmm0
00528a99 f30f110424               movss      dword ptr [esp], xmm0
00528a9e 51                       push       ecx
00528a9f f30f1005e00a5700         movss      xmm0, dword ptr [0x570ae0]
00528aa7 f30f110424               movss      dword ptr [esp], xmm0
00528aac 51                       push       ecx
00528aad f30f100584565700         movss      xmm0, dword ptr [0x575684]
00528ab5 f30f110424               movss      dword ptr [esp], xmm0
00528aba 8d4df0                   lea        ecx, [ebp - 0x10]
00528abd e80ea3efff               call       0x422dd0
00528ac2 6a02                     push       2
00528ac4 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00528aca e83100f6ff               call       0x488b00
00528acf 6a01                     push       1
00528ad1 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00528ad7 e844dffbff               call       0x4e6a20
00528adc 90                       nop        
00528add c745e400000000           mov        dword ptr [ebp - 0x1c], 0
00528ae4 eb09                     jmp        0x528aef
00528ae6 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
00528ae9 83c001                   add        eax, 1
00528aec 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00528aef 837de405                 cmp        dword ptr [ebp - 0x1c], 5
00528af3 0f8d7f040000             jge        0x528f78
00528af9 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00528afc 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00528aff 8b848ae4580000           mov        eax, dword ptr [edx + ecx*4 + 0x58e4]
00528b06 8945ec                   mov        dword ptr [ebp - 0x14], eax
00528b09 837decff                 cmp        dword ptr [ebp - 0x14], -1
00528b0d 0f8e4e040000             jle        0x528f61
00528b13 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00528b16 83791814                 cmp        dword ptr [ecx + 0x18], 0x14
00528b1a 0f8589000000             jne        0x528ba9
00528b20 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00528b23 83c124                   add        ecx, 0x24
00528b26 e8d537eeff               call       0x40c300
00528b2b 3945e4                   cmp        dword ptr [ebp - 0x1c], eax
00528b2e 7579                     jne        0x528ba9
00528b30 b968a55b00               mov        ecx, 0x5ba568
00528b35 e806f8f8ff               call       0x4b8340
00528b3a 8945dc                   mov        dword ptr [ebp - 0x24], eax
00528b3d b968a55b00               mov        ecx, 0x5ba568
00528b42 e829f4f8ff               call       0x4b7f70
00528b47 8945d8                   mov        dword ptr [ebp - 0x28], eax
00528b4a 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
00528b4d 52                       push       edx
00528b4e 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00528b51 50                       push       eax
00528b52 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00528b58 e80349f9ff               call       0x4bd460
00528b5d 05080b0000               add        eax, 0xb08
00528b62 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00528b65 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00528b68 51                       push       ecx
00528b69 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00528b6c e89fe2f5ff               call       0x486e10
00528b71 ba04000000               mov        edx, 4
00528b76 c1e200                   shl        edx, 0
00528b79 83bc10c000000000         cmp        dword ptr [eax + edx + 0xc0], 0
00528b81 7413                     je         0x528b96
00528b83 68ffd090ff               push       0xff90d0ff
00528b88 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00528b8e e83d7ff4ff               call       0x470ad0
00528b93 90                       nop        
00528b94 eb11                     jmp        0x528ba7
00528b96 68b0b0b0ff               push       0xffb0b0b0
00528b9b 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00528ba1 e82a7ff4ff               call       0x470ad0
00528ba6 90                       nop        
00528ba7 eb77                     jmp        0x528c20
00528ba9 b968a55b00               mov        ecx, 0x5ba568
00528bae e88df7f8ff               call       0x4b8340
00528bb3 8945d0                   mov        dword ptr [ebp - 0x30], eax
00528bb6 b968a55b00               mov        ecx, 0x5ba568
00528bbb e8b0f3f8ff               call       0x4b7f70
00528bc0 8945cc                   mov        dword ptr [ebp - 0x34], eax
00528bc3 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00528bc6 50                       push       eax
00528bc7 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00528bca 51                       push       ecx
00528bcb 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00528bd1 e88a48f9ff               call       0x4bd460
00528bd6 05080b0000               add        eax, 0xb08
00528bdb 8945c8                   mov        dword ptr [ebp - 0x38], eax
00528bde 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00528be1 52                       push       edx
00528be2 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00528be5 e826e2f5ff               call       0x486e10
00528bea b904000000               mov        ecx, 4
00528bef c1e100                   shl        ecx, 0
00528bf2 83bc08c000000000         cmp        dword ptr [eax + ecx + 0xc0], 0
00528bfa 7413                     je         0x528c0f
00528bfc 68c0a060ff               push       0xff60a0c0
00528c01 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00528c07 e8c47ef4ff               call       0x470ad0
00528c0c 90                       nop        
00528c0d eb11                     jmp        0x528c20
00528c0f 68404040ff               push       0xff404040
00528c14 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00528c1a e8b17ef4ff               call       0x470ad0
00528c1f 90                       nop        
00528c20 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00528c26 e8d5faf5ff               call       0x488700
00528c2b 05080b0000               add        eax, 0xb08
00528c30 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00528c33 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00528c36 52                       push       edx
00528c37 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00528c3a e8d1e1f5ff               call       0x486e10
00528c3f b904000000               mov        ecx, 4
00528c44 6bd100                   imul       edx, ecx, 0
00528c47 83bc10c800000000         cmp        dword ptr [eax + edx + 0xc8], 0
00528c4f 7535                     jne        0x528c86
00528c51 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00528c57 e8a4faf5ff               call       0x488700
00528c5c 05080b0000               add        eax, 0xb08
00528c61 8945c0                   mov        dword ptr [ebp - 0x40], eax
00528c64 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00528c67 50                       push       eax
00528c68 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00528c6b e8a0e1f5ff               call       0x486e10
00528c70 b904000000               mov        ecx, 4
00528c75 c1e100                   shl        ecx, 0
00528c78 83bc08c800000000         cmp        dword ptr [eax + ecx + 0xc8], 0
00528c80 0f84c3020000             je         0x528f49
00528c86 b968a55b00               mov        ecx, 0x5ba568
00528c8b e8b0f6f8ff               call       0x4b8340
00528c90 8945bc                   mov        dword ptr [ebp - 0x44], eax
00528c93 b968a55b00               mov        ecx, 0x5ba568
00528c98 e8d3f2f8ff               call       0x4b7f70
00528c9d 8945b8                   mov        dword ptr [ebp - 0x48], eax
00528ca0 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
00528ca3 52                       push       edx
00528ca4 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
00528ca7 50                       push       eax
00528ca8 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00528cae e8ad47f9ff               call       0x4bd460
00528cb3 05080b0000               add        eax, 0xb08
00528cb8 8945b4                   mov        dword ptr [ebp - 0x4c], eax
00528cbb 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00528cbe 51                       push       ecx
00528cbf 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00528cc2 e849e1f5ff               call       0x486e10
00528cc7 ba04000000               mov        edx, 4
00528ccc c1e200                   shl        edx, 0
00528ccf 8b8410c8000000           mov        eax, dword ptr [eax + edx + 0xc8]
00528cd6 89459c                   mov        dword ptr [ebp - 0x64], eax
00528cd9 b968a55b00               mov        ecx, 0x5ba568
00528cde e85df6f8ff               call       0x4b8340
00528ce3 8945b0                   mov        dword ptr [ebp - 0x50], eax
00528ce6 b968a55b00               mov        ecx, 0x5ba568
00528ceb e880f2f8ff               call       0x4b7f70
00528cf0 8945ac                   mov        dword ptr [ebp - 0x54], eax
00528cf3 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00528cf6 51                       push       ecx
00528cf7 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
00528cfa 52                       push       edx
00528cfb 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00528d01 e85a47f9ff               call       0x4bd460
00528d06 05080b0000               add        eax, 0xb08
00528d0b 8945a8                   mov        dword ptr [ebp - 0x58], eax
00528d0e be04000000               mov        esi, 4
00528d13 c1e600                   shl        esi, 0
00528d16 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00528d19 50                       push       eax
00528d1a 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00528d1d e8eee0f5ff               call       0x486e10
00528d22 8b8c30c0000000           mov        ecx, dword ptr [eax + esi + 0xc0]
00528d29 894d98                   mov        dword ptr [ebp - 0x68], ecx
00528d2c b968a55b00               mov        ecx, 0x5ba568
00528d31 e80af6f8ff               call       0x4b8340
00528d36 898560ffffff             mov        dword ptr [ebp - 0xa0], eax
00528d3c b968a55b00               mov        ecx, 0x5ba568
00528d41 e82af2f8ff               call       0x4b7f70
00528d46 8945a4                   mov        dword ptr [ebp - 0x5c], eax
00528d49 8b9560ffffff             mov        edx, dword ptr [ebp - 0xa0]
00528d4f 52                       push       edx
00528d50 8b45a4                   mov        eax, dword ptr [ebp - 0x5c]
00528d53 50                       push       eax
00528d54 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00528d5a e80147f9ff               call       0x4bd460
00528d5f 05080b0000               add        eax, 0xb08
00528d64 8945a0                   mov        dword ptr [ebp - 0x60], eax
00528d67 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00528d6a 51                       push       ecx
00528d6b 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00528d6e e89de0f5ff               call       0x486e10
00528d73 8b90d8000000             mov        edx, dword ptr [eax + 0xd8]
00528d79 8b80dc000000             mov        eax, dword ptr [eax + 0xdc]
00528d7f 899564ffffff             mov        dword ptr [ebp - 0x9c], edx
00528d85 898568ffffff             mov        dword ptr [ebp - 0x98], eax
00528d8b 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
00528d8e 51                       push       ecx
00528d8f 8b5598                   mov        edx, dword ptr [ebp - 0x68]
00528d92 52                       push       edx
00528d93 8b8568ffffff             mov        eax, dword ptr [ebp - 0x98]
00528d99 50                       push       eax
00528d9a 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
00528da0 51                       push       ecx
00528da1 687c555700               push       0x57557c
00528da6 8d55f0                   lea        edx, [ebp - 0x10]
00528da9 52                       push       edx
00528daa a198065c00               mov        eax, dword ptr [0x5c0698]
00528daf 50                       push       eax
00528db0 e8db3bf4ff               call       0x46c990
00528db5 83c41c                   add        esp, 0x1c
00528db8 b968a55b00               mov        ecx, 0x5ba568
00528dbd e87ef5f8ff               call       0x4b8340
00528dc2 894594                   mov        dword ptr [ebp - 0x6c], eax
00528dc5 b968a55b00               mov        ecx, 0x5ba568
00528dca e8a1f1f8ff               call       0x4b7f70
00528dcf 894590                   mov        dword ptr [ebp - 0x70], eax
00528dd2 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00528dd5 51                       push       ecx
00528dd6 8b5590                   mov        edx, dword ptr [ebp - 0x70]
00528dd9 52                       push       edx
00528dda 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00528de0 e87b46f9ff               call       0x4bd460
00528de5 05080b0000               add        eax, 0xb08
00528dea 89458c                   mov        dword ptr [ebp - 0x74], eax
00528ded 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00528df0 50                       push       eax
00528df1 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00528df4 e817e0f5ff               call       0x486e10
00528df9 b904000000               mov        ecx, 4
00528dfe 6bd100                   imul       edx, ecx, 0
00528e01 83bc10c000000000         cmp        dword ptr [eax + edx + 0xc0], 0
00528e09 7413                     je         0x528e1e
00528e0b 68606020ff               push       0xff206060
00528e10 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00528e16 e8b57cf4ff               call       0x470ad0
00528e1b 90                       nop        
00528e1c eb11                     jmp        0x528e2f
00528e1e 68404040ff               push       0xff404040
00528e23 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00528e29 e8a27cf4ff               call       0x470ad0
00528e2e 90                       nop        
00528e2f f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00528e34 f30f580534e75600         addss      xmm0, dword ptr [0x56e734]
00528e3c f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00528e41 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00528e44 833c8558f05a0004         cmp        dword ptr [eax*4 + 0x5af058], 4
00528e4c 0f8fe3000000             jg         0x528f35
00528e52 b968a55b00               mov        ecx, 0x5ba568
00528e57 e8e4f4f8ff               call       0x4b8340
00528e5c 894588                   mov        dword ptr [ebp - 0x78], eax
00528e5f b968a55b00               mov        ecx, 0x5ba568
00528e64 e807f1f8ff               call       0x4b7f70
00528e69 894584                   mov        dword ptr [ebp - 0x7c], eax
00528e6c 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00528e6f 51                       push       ecx
00528e70 8b5584                   mov        edx, dword ptr [ebp - 0x7c]
00528e73 52                       push       edx
00528e74 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00528e7a e8e145f9ff               call       0x4bd460
00528e7f 05080b0000               add        eax, 0xb08
00528e84 894580                   mov        dword ptr [ebp - 0x80], eax
00528e87 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00528e8a 50                       push       eax
00528e8b 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00528e8e e87ddff5ff               call       0x486e10
00528e93 b904000000               mov        ecx, 4
00528e98 6bd100                   imul       edx, ecx, 0
00528e9b 8b8410c8000000           mov        eax, dword ptr [eax + edx + 0xc8]
00528ea2 898570ffffff             mov        dword ptr [ebp - 0x90], eax
00528ea8 b968a55b00               mov        ecx, 0x5ba568
00528ead e88ef4f8ff               call       0x4b8340
00528eb2 89857cffffff             mov        dword ptr [ebp - 0x84], eax
00528eb8 b968a55b00               mov        ecx, 0x5ba568
00528ebd e8aef0f8ff               call       0x4b7f70
00528ec2 898578ffffff             mov        dword ptr [ebp - 0x88], eax
00528ec8 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
00528ece 51                       push       ecx
00528ecf 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
00528ed5 52                       push       edx
00528ed6 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00528edc e87f45f9ff               call       0x4bd460
00528ee1 05080b0000               add        eax, 0xb08
00528ee6 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
00528eec b804000000               mov        eax, 4
00528ef1 6bf000                   imul       esi, eax, 0
00528ef4 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00528ef7 51                       push       ecx
00528ef8 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
00528efe e80ddff5ff               call       0x486e10
00528f03 8b9430c0000000           mov        edx, dword ptr [eax + esi + 0xc0]
00528f0a 89956cffffff             mov        dword ptr [ebp - 0x94], edx
00528f10 8b8570ffffff             mov        eax, dword ptr [ebp - 0x90]
00528f16 50                       push       eax
00528f17 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00528f1d 51                       push       ecx
00528f1e 6894555700               push       0x575594
00528f23 8d55f0                   lea        edx, [ebp - 0x10]
00528f26 52                       push       edx
00528f27 a198065c00               mov        eax, dword ptr [0x5c0698]
00528f2c 50                       push       eax
00528f2d e85e3af4ff               call       0x46c990
00528f32 83c414                   add        esp, 0x14
00528f35 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00528f3a f30f5c0534e75600         subss      xmm0, dword ptr [0x56e734]
00528f42 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00528f47 eb18                     jmp        0x528f61
00528f49 68b0555700               push       0x5755b0
00528f4e 8d4df0                   lea        ecx, [ebp - 0x10]
00528f51 51                       push       ecx
00528f52 8b1598065c00             mov        edx, dword ptr [0x5c0698]
00528f58 52                       push       edx
00528f59 e8323af4ff               call       0x46c990
00528f5e 83c40c                   add        esp, 0xc
00528f61 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00528f66 f30f5805cc0a5700         addss      xmm0, dword ptr [0x570acc]
00528f6e f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00528f73 e96efbffff               jmp        0x528ae6
00528f78 6a00                     push       0
00528f7a 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00528f80 e87bfbf5ff               call       0x488b00
00528f85 6a00                     push       0
00528f87 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00528f8d e88edafbff               call       0x4e6a20
00528f92 6aff                     push       -1
00528f94 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00528f9a e8317bf4ff               call       0x470ad0
00528f9f 90                       nop        
00528fa0 b801000000               mov        eax, 1
00528fa5 5e                       pop        esi
00528fa6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00528fa9 33cd                     xor        ecx, ebp
00528fab e83e9a0100               call       0x5429ee
00528fb0 8be5                     mov        esp, ebp
00528fb2 5d                       pop        ebp
00528fb3 c3                       ret        
00528fb4 cc                       int3       
00528fb5 cc                       int3       
00528fb6 cc                       int3       
00528fb7 cc                       int3       
00528fb8 cc                       int3       
00528fb9 cc                       int3       
00528fba cc                       int3       
00528fbb cc                       int3       
00528fbc cc                       int3       
00528fbd cc                       int3       
00528fbe cc                       int3       
00528fbf cc                       int3       
