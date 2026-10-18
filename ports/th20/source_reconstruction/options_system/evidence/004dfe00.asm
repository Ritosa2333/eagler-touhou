004dfe00 55                       push       ebp
004dfe01 8bec                     mov        ebp, esp
004dfe03 81ec9c000000             sub        esp, 0x9c
004dfe09 894dfc                   mov        dword ptr [ebp - 4], ecx
004dfe0c 833d284d5c0000           cmp        dword ptr [0x5c4d28], 0
004dfe13 740a                     je         0x4dfe1f
004dfe15 b801000000               mov        eax, 1
004dfe1a e9d0080000               jmp        0x4e06ef
004dfe1f 8b45fc                   mov        eax, dword ptr [ebp - 4]
004dfe22 8b88a0000000             mov        ecx, dword ptr [eax + 0xa0]
004dfe28 894dec                   mov        dword ptr [ebp - 0x14], ecx
004dfe2b 837dec05                 cmp        dword ptr [ebp - 0x14], 5
004dfe2f 0f8722080000             ja         0x4e0657
004dfe35 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004dfe38 ff2495f4064e00           jmp        dword ptr [edx*4 + 0x4e06f4]
004dfe3f 8b45fc                   mov        eax, dword ptr [ebp - 4]
004dfe42 83c014                   add        eax, 0x14
004dfe45 8945b8                   mov        dword ptr [ebp - 0x48], eax
004dfe48 6a06                     push       6
004dfe4a 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004dfe4d e88ec6fbff               call       0x49c4e0
004dfe52 6a00                     push       0
004dfe54 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004dfe57 83c114                   add        ecx, 0x14
004dfe5a e8f1eefdff               call       0x4bed50
004dfe5f 6a01                     push       1
004dfe61 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004dfe64 e817110000               call       0x4e0f80
004dfe69 90                       nop        
004dfe6a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004dfe6d 83c160                   add        ecx, 0x60
004dfe70 894db4                   mov        dword ptr [ebp - 0x4c], ecx
004dfe73 6a06                     push       6
004dfe75 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004dfe78 e8f311f8ff               call       0x461070
004dfe7d 0fb6d0                   movzx      edx, al
004dfe80 85d2                     test       edx, edx
004dfe82 740b                     je         0x4dfe8f
004dfe84 6a02                     push       2
004dfe86 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004dfe89 e8f2100000               call       0x4e0f80
004dfe8e 90                       nop        
004dfe8f e9c3070000               jmp        0x4e0657
004dfe94 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004dfe97 83c114                   add        ecx, 0x14
004dfe9a e861fdfdff               call       0x4bfc00
004dfe9f 90                       nop        
004dfea0 33c0                     xor        eax, eax
004dfea2 752f                     jne        0x4dfed3
004dfea4 b904000000               mov        ecx, 4
004dfea9 d1e1                     shl        ecx, 1
004dfeab 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
004dfeb2 741f                     je         0x4dfed3
004dfeb4 ba04000000               mov        edx, 4
004dfeb9 d1e2                     shl        edx, 1
004dfebb 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
004dfec1 8945b0                   mov        dword ptr [ebp - 0x50], eax
004dfec4 6a10                     push       0x10
004dfec6 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
004dfec9 e8b2a3f3ff               call       0x41a280
004dfece 8945e8                   mov        dword ptr [ebp - 0x18], eax
004dfed1 eb07                     jmp        0x4dfeda
004dfed3 c745e800000000           mov        dword ptr [ebp - 0x18], 0
004dfeda 837de800                 cmp        dword ptr [ebp - 0x18], 0
004dfede 7414                     je         0x4dfef4
004dfee0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004dfee3 83c114                   add        ecx, 0x14
004dfee6 894dac                   mov        dword ptr [ebp - 0x54], ecx
004dfee9 6a00                     push       0
004dfeeb 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
004dfeee e84df1fdff               call       0x4bf040
004dfef3 90                       nop        
004dfef4 33d2                     xor        edx, edx
004dfef6 752f                     jne        0x4dff27
004dfef8 b804000000               mov        eax, 4
004dfefd d1e0                     shl        eax, 1
004dfeff 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
004dff06 741f                     je         0x4dff27
004dff08 b904000000               mov        ecx, 4
004dff0d d1e1                     shl        ecx, 1
004dff0f 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
004dff15 8955a8                   mov        dword ptr [ebp - 0x58], edx
004dff18 6a20                     push       0x20
004dff1a 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004dff1d e85ea3f3ff               call       0x41a280
004dff22 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004dff25 eb07                     jmp        0x4dff2e
004dff27 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
004dff2e 837de400                 cmp        dword ptr [ebp - 0x1c], 0
004dff32 7414                     je         0x4dff48
004dff34 8b45fc                   mov        eax, dword ptr [ebp - 4]
004dff37 83c014                   add        eax, 0x14
004dff3a 8945a4                   mov        dword ptr [ebp - 0x5c], eax
004dff3d 6a00                     push       0
004dff3f 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
004dff42 e8d9f0fdff               call       0x4bf020
004dff47 90                       nop        
004dff48 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004dff4b 83c114                   add        ecx, 0x14
004dff4e e8ddd0f7ff               call       0x45d030
004dff53 85c0                     test       eax, eax
004dff55 741c                     je         0x4dff73
004dff57 6a00                     push       0
004dff59 6a0a                     push       0xa
004dff5b b930a85b00               mov        ecx, 0x5ba830
004dff60 e80b6ef4ff               call       0x426d70
004dff65 6a08                     push       8
004dff67 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004dff6a 83c17c                   add        ecx, 0x7c
004dff6d e8ae35f4ff               call       0x423520
004dff72 90                       nop        
004dff73 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004dff76 83c114                   add        ecx, 0x14
004dff79 894da0                   mov        dword ptr [ebp - 0x60], ecx
004dff7c 6a02                     push       2
004dff7e 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
004dff81 e82ab2f8ff               call       0x46b1b0
004dff86 85c0                     test       eax, eax
004dff88 7426                     je         0x4dffb0
004dff8a 8b55fc                   mov        edx, dword ptr [ebp - 4]
004dff8d 83c260                   add        edx, 0x60
004dff90 89559c                   mov        dword ptr [ebp - 0x64], edx
004dff93 6a3c                     push       0x3c
004dff95 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
004dff98 e8638ef9ff               call       0x478e00
004dff9d 85c0                     test       eax, eax
004dff9f 740f                     je         0x4dffb0
004dffa1 6a00                     push       0
004dffa3 6a02                     push       2
004dffa5 b930a85b00               mov        ecx, 0x5ba830
004dffaa e8c16df4ff               call       0x426d70
004dffaf 90                       nop        
004dffb0 33c0                     xor        eax, eax
004dffb2 752f                     jne        0x4dffe3
004dffb4 b904000000               mov        ecx, 4
004dffb9 d1e1                     shl        ecx, 1
004dffbb 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
004dffc2 741f                     je         0x4dffe3
004dffc4 ba04000000               mov        edx, 4
004dffc9 d1e2                     shl        edx, 1
004dffcb 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
004dffd1 894598                   mov        dword ptr [ebp - 0x68], eax
004dffd4 6a40                     push       0x40
004dffd6 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
004dffd9 e8a2a2f3ff               call       0x41a280
004dffde 8945e0                   mov        dword ptr [ebp - 0x20], eax
004dffe1 eb07                     jmp        0x4dffea
004dffe3 c745e000000000           mov        dword ptr [ebp - 0x20], 0
004dffea 837de000                 cmp        dword ptr [ebp - 0x20], 0
004dffee 0f84c6010000             je         0x4e01ba
004dfff4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004dfff7 83c114                   add        ecx, 0x14
004dfffa e801c3f2ff               call       0x40c300
004dffff 8945f8                   mov        dword ptr [ebp - 8], eax
004e0002 837df800                 cmp        dword ptr [ebp - 8], 0
004e0006 7477                     je         0x4e007f
004e0008 837df801                 cmp        dword ptr [ebp - 8], 1
004e000c 740b                     je         0x4e0019
004e000e 837df802                 cmp        dword ptr [ebp - 8], 2
004e0012 7438                     je         0x4e004c
004e0014 e9a1010000               jmp        0x4e01ba
004e0019 0fbe0d864f5c00           movsx      ecx, byte ptr [0x5c4f86]
004e0020 83f905                   cmp        ecx, 5
004e0023 7d09                     jge        0x4e002e
004e0025 c605864f5c0000           mov        byte ptr [0x5c4f86], 0
004e002c eb10                     jmp        0x4e003e
004e002e 0fbe15864f5c00           movsx      edx, byte ptr [0x5c4f86]
004e0035 83ea05                   sub        edx, 5
004e0038 8815864f5c00             mov        byte ptr [0x5c4f86], dl
004e003e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e0041 e87a0f0000               call       0x4e0fc0
004e0046 90                       nop        
004e0047 e96e010000               jmp        0x4e01ba
004e004c 0fbe05874f5c00           movsx      eax, byte ptr [0x5c4f87]
004e0053 83f805                   cmp        eax, 5
004e0056 7d09                     jge        0x4e0061
004e0058 c605874f5c0000           mov        byte ptr [0x5c4f87], 0
004e005f eb10                     jmp        0x4e0071
004e0061 0fbe0d874f5c00           movsx      ecx, byte ptr [0x5c4f87]
004e0068 83e905                   sub        ecx, 5
004e006b 880d874f5c00             mov        byte ptr [0x5c4f87], cl
004e0071 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e0074 e8470f0000               call       0x4e0fc0
004e0079 90                       nop        
004e007a e93b010000               jmp        0x4e01ba
004e007f 6a00                     push       0
004e0081 6a00                     push       0
004e0083 6a00                     push       0
004e0085 6a0a                     push       0xa
004e0087 6a09                     push       9
004e0089 e80250f4ff               call       0x425090
004e008e 83c414                   add        esp, 0x14
004e0091 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e0094 c782a000000004000000     mov        dword ptr [edx + 0xa0], 4
004e009e 6a00                     push       0
004e00a0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e00a3 83c160                   add        ecx, 0x60
004e00a6 e87534f4ff               call       0x423520
004e00ab 90                       nop        
004e00ac b958675b00               mov        ecx, 0x5b6758
004e00b1 e8bacbf3ff               call       0x41cc70
004e00b6 8945dc                   mov        dword ptr [ebp - 0x24], eax
004e00b9 837ddc09                 cmp        dword ptr [ebp - 0x24], 9
004e00bd 0f87f7000000             ja         0x4e01ba
004e00c3 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004e00c6 ff24850c074e00           jmp        dword ptr [eax*4 + 0x4e070c]
004e00cd 6a00                     push       0
004e00cf b958675b00               mov        ecx, 0x5b6758
004e00d4 e857dff3ff               call       0x41e030
004e00d9 90                       nop        
004e00da e9db000000               jmp        0x4e01ba
004e00df 6a01                     push       1
004e00e1 b958675b00               mov        ecx, 0x5b6758
004e00e6 e845dff3ff               call       0x41e030
004e00eb 90                       nop        
004e00ec e9c9000000               jmp        0x4e01ba
004e00f1 6a02                     push       2
004e00f3 b958675b00               mov        ecx, 0x5b6758
004e00f8 e833dff3ff               call       0x41e030
004e00fd 90                       nop        
004e00fe e9b7000000               jmp        0x4e01ba
004e0103 6a02                     push       2
004e0105 b958675b00               mov        ecx, 0x5b6758
004e010a e821dff3ff               call       0x41e030
004e010f 90                       nop        
004e0110 e9a5000000               jmp        0x4e01ba
004e0115 6a02                     push       2
004e0117 b958675b00               mov        ecx, 0x5b6758
004e011c e80fdff3ff               call       0x41e030
004e0121 90                       nop        
004e0122 e993000000               jmp        0x4e01ba
004e0127 6a09                     push       9
004e0129 b958675b00               mov        ecx, 0x5b6758
004e012e e8fddef3ff               call       0x41e030
004e0133 90                       nop        
004e0134 e981000000               jmp        0x4e01ba
004e0139 6a08                     push       8
004e013b b958675b00               mov        ecx, 0x5b6758
004e0140 e8ebdef3ff               call       0x41e030
004e0145 90                       nop        
004e0146 eb72                     jmp        0x4e01ba
004e0148 0fb60d8a4f5c00           movzx      ecx, byte ptr [0x5c4f8a]
004e014f 894dd8                   mov        dword ptr [ebp - 0x28], ecx
004e0152 837dd805                 cmp        dword ptr [ebp - 0x28], 5
004e0156 7762                     ja         0x4e01ba
004e0158 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004e015b ff249534074e00           jmp        dword ptr [edx*4 + 0x4e0734]
004e0162 6a03                     push       3
004e0164 b958675b00               mov        ecx, 0x5b6758
004e0169 e8c2def3ff               call       0x41e030
004e016e 90                       nop        
004e016f eb49                     jmp        0x4e01ba
004e0171 6a04                     push       4
004e0173 b958675b00               mov        ecx, 0x5b6758
004e0178 e8b3def3ff               call       0x41e030
004e017d 90                       nop        
004e017e eb3a                     jmp        0x4e01ba
004e0180 6a05                     push       5
004e0182 b958675b00               mov        ecx, 0x5b6758
004e0187 e8a4def3ff               call       0x41e030
004e018c 90                       nop        
004e018d eb2b                     jmp        0x4e01ba
004e018f 6a06                     push       6
004e0191 b958675b00               mov        ecx, 0x5b6758
004e0196 e895def3ff               call       0x41e030
004e019b 90                       nop        
004e019c eb1c                     jmp        0x4e01ba
004e019e 6a07                     push       7
004e01a0 b958675b00               mov        ecx, 0x5b6758
004e01a5 e886def3ff               call       0x41e030
004e01aa 90                       nop        
004e01ab eb0d                     jmp        0x4e01ba
004e01ad 6a05                     push       5
004e01af b958675b00               mov        ecx, 0x5b6758
004e01b4 e877def3ff               call       0x41e030
004e01b9 90                       nop        
004e01ba 33c0                     xor        eax, eax
004e01bc 7532                     jne        0x4e01f0
004e01be b904000000               mov        ecx, 4
004e01c3 d1e1                     shl        ecx, 1
004e01c5 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
004e01cc 7422                     je         0x4e01f0
004e01ce ba04000000               mov        edx, 4
004e01d3 d1e2                     shl        edx, 1
004e01d5 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
004e01db 894594                   mov        dword ptr [ebp - 0x6c], eax
004e01de 6880000000               push       0x80
004e01e3 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
004e01e6 e895a0f3ff               call       0x41a280
004e01eb 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004e01ee eb07                     jmp        0x4e01f7
004e01f0 c745d400000000           mov        dword ptr [ebp - 0x2c], 0
004e01f7 837dd400                 cmp        dword ptr [ebp - 0x2c], 0
004e01fb 0f84d5010000             je         0x4e03d6
004e0201 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e0204 83c114                   add        ecx, 0x14
004e0207 e8f4c0f2ff               call       0x40c300
004e020c 8945f4                   mov        dword ptr [ebp - 0xc], eax
004e020f 837df400                 cmp        dword ptr [ebp - 0xc], 0
004e0213 7472                     je         0x4e0287
004e0215 837df401                 cmp        dword ptr [ebp - 0xc], 1
004e0219 740b                     je         0x4e0226
004e021b 837df402                 cmp        dword ptr [ebp - 0xc], 2
004e021f 7436                     je         0x4e0257
004e0221 e9b0010000               jmp        0x4e03d6
004e0226 0fbe0d864f5c00           movsx      ecx, byte ptr [0x5c4f86]
004e022d 83c105                   add        ecx, 5
004e0230 880d864f5c00             mov        byte ptr [0x5c4f86], cl
004e0236 0fbe15864f5c00           movsx      edx, byte ptr [0x5c4f86]
004e023d 83fa64                   cmp        edx, 0x64
004e0240 7e07                     jle        0x4e0249
004e0242 c605864f5c0064           mov        byte ptr [0x5c4f86], 0x64
004e0249 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e024c e86f0d0000               call       0x4e0fc0
004e0251 90                       nop        
004e0252 e97f010000               jmp        0x4e03d6
004e0257 0fbe05874f5c00           movsx      eax, byte ptr [0x5c4f87]
004e025e 83c005                   add        eax, 5
004e0261 a2874f5c00               mov        byte ptr [0x5c4f87], al
004e0266 0fbe0d874f5c00           movsx      ecx, byte ptr [0x5c4f87]
004e026d 83f964                   cmp        ecx, 0x64
004e0270 7e07                     jle        0x4e0279
004e0272 c605874f5c0064           mov        byte ptr [0x5c4f87], 0x64
004e0279 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e027c e83f0d0000               call       0x4e0fc0
004e0281 90                       nop        
004e0282 e94f010000               jmp        0x4e03d6
004e0287 6a00                     push       0
004e0289 6a00                     push       0
004e028b 6a00                     push       0
004e028d 6a0a                     push       0xa
004e028f 6a09                     push       9
004e0291 e8fa4df4ff               call       0x425090
004e0296 83c414                   add        esp, 0x14
004e0299 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e029c c782a000000004000000     mov        dword ptr [edx + 0xa0], 4
004e02a6 6a00                     push       0
004e02a8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e02ab 83c160                   add        ecx, 0x60
004e02ae e86d32f4ff               call       0x423520
004e02b3 90                       nop        
004e02b4 b958675b00               mov        ecx, 0x5b6758
004e02b9 e8b2c9f3ff               call       0x41cc70
004e02be 8945d0                   mov        dword ptr [ebp - 0x30], eax
004e02c1 837dd009                 cmp        dword ptr [ebp - 0x30], 9
004e02c5 0f870b010000             ja         0x4e03d6
004e02cb 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004e02ce ff24854c074e00           jmp        dword ptr [eax*4 + 0x4e074c]
004e02d5 6a08                     push       8
004e02d7 b958675b00               mov        ecx, 0x5b6758
004e02dc e84fddf3ff               call       0x41e030
004e02e1 90                       nop        
004e02e2 e9ef000000               jmp        0x4e03d6
004e02e7 6a09                     push       9
004e02e9 b958675b00               mov        ecx, 0x5b6758
004e02ee e83dddf3ff               call       0x41e030
004e02f3 90                       nop        
004e02f4 e9dd000000               jmp        0x4e03d6
004e02f9 0fb60d8a4f5c00           movzx      ecx, byte ptr [0x5c4f8a]
004e0300 894dcc                   mov        dword ptr [ebp - 0x34], ecx
004e0303 837dcc05                 cmp        dword ptr [ebp - 0x34], 5
004e0307 7735                     ja         0x4e033e
004e0309 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
004e030c ff249574074e00           jmp        dword ptr [edx*4 + 0x4e0774]
004e0313 6a00                     push       0
004e0315 b958675b00               mov        ecx, 0x5b6758
004e031a e811ddf3ff               call       0x41e030
004e031f 90                       nop        
004e0320 eb1c                     jmp        0x4e033e
004e0322 6a01                     push       1
004e0324 b958675b00               mov        ecx, 0x5b6758
004e0329 e802ddf3ff               call       0x41e030
004e032e 90                       nop        
004e032f eb0d                     jmp        0x4e033e
004e0331 6a02                     push       2
004e0333 b958675b00               mov        ecx, 0x5b6758
004e0338 e8f3dcf3ff               call       0x41e030
004e033d 90                       nop        
004e033e e993000000               jmp        0x4e03d6
004e0343 6a03                     push       3
004e0345 b958675b00               mov        ecx, 0x5b6758
004e034a e8e1dcf3ff               call       0x41e030
004e034f 90                       nop        
004e0350 e981000000               jmp        0x4e03d6
004e0355 6a04                     push       4
004e0357 b958675b00               mov        ecx, 0x5b6758
004e035c e8cfdcf3ff               call       0x41e030
004e0361 90                       nop        
004e0362 eb72                     jmp        0x4e03d6
004e0364 0fb6058a4f5c00           movzx      eax, byte ptr [0x5c4f8a]
004e036b 8945c8                   mov        dword ptr [ebp - 0x38], eax
004e036e 837dc805                 cmp        dword ptr [ebp - 0x38], 5
004e0372 7762                     ja         0x4e03d6
004e0374 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004e0377 ff248d8c074e00           jmp        dword ptr [ecx*4 + 0x4e078c]
004e037e 6a03                     push       3
004e0380 b958675b00               mov        ecx, 0x5b6758
004e0385 e8a6dcf3ff               call       0x41e030
004e038a 90                       nop        
004e038b eb49                     jmp        0x4e03d6
004e038d 6a04                     push       4
004e038f b958675b00               mov        ecx, 0x5b6758
004e0394 e897dcf3ff               call       0x41e030
004e0399 90                       nop        
004e039a eb3a                     jmp        0x4e03d6
004e039c 6a05                     push       5
004e039e b958675b00               mov        ecx, 0x5b6758
004e03a3 e888dcf3ff               call       0x41e030
004e03a8 90                       nop        
004e03a9 eb2b                     jmp        0x4e03d6
004e03ab 6a06                     push       6
004e03ad b958675b00               mov        ecx, 0x5b6758
004e03b2 e879dcf3ff               call       0x41e030
004e03b7 90                       nop        
004e03b8 eb1c                     jmp        0x4e03d6
004e03ba 6a07                     push       7
004e03bc b958675b00               mov        ecx, 0x5b6758
004e03c1 e86adcf3ff               call       0x41e030
004e03c6 90                       nop        
004e03c7 eb0d                     jmp        0x4e03d6
004e03c9 6a05                     push       5
004e03cb b958675b00               mov        ecx, 0x5b6758
004e03d0 e85bdcf3ff               call       0x41e030
004e03d5 90                       nop        
004e03d6 33d2                     xor        edx, edx
004e03d8 7532                     jne        0x4e040c
004e03da b804000000               mov        eax, 4
004e03df d1e0                     shl        eax, 1
004e03e1 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
004e03e8 7422                     je         0x4e040c
004e03ea b904000000               mov        ecx, 4
004e03ef d1e1                     shl        ecx, 1
004e03f1 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
004e03f7 895590                   mov        dword ptr [ebp - 0x70], edx
004e03fa 6806010000               push       0x106
004e03ff 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
004e0402 e8f997f3ff               call       0x419c00
004e0407 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004e040a eb07                     jmp        0x4e0413
004e040c c745c400000000           mov        dword ptr [ebp - 0x3c], 0
004e0413 837dc400                 cmp        dword ptr [ebp - 0x3c], 0
004e0417 742a                     je         0x4e0443
004e0419 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e041c 83c114                   add        ecx, 0x14
004e041f e8dcbef2ff               call       0x40c300
004e0424 83f805                   cmp        eax, 5
004e0427 750c                     jne        0x4e0435
004e0429 e9f6000000               jmp        0x4e0524
004e042e e9f1000000               jmp        0x4e0524
004e0433 eb0e                     jmp        0x4e0443
004e0435 6a05                     push       5
004e0437 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e043a 83c114                   add        ecx, 0x14
004e043d e80ee9fdff               call       0x4bed50
004e0442 90                       nop        
004e0443 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e0446 83b89c00000000           cmp        dword ptr [eax + 0x9c], 0
004e044d 744d                     je         0x4e049c
004e044f 33c9                     xor        ecx, ecx
004e0451 7532                     jne        0x4e0485
004e0453 ba04000000               mov        edx, 4
004e0458 d1e2                     shl        edx, 1
004e045a 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
004e0461 7422                     je         0x4e0485
004e0463 b804000000               mov        eax, 4
004e0468 d1e0                     shl        eax, 1
004e046a 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
004e0470 894d8c                   mov        dword ptr [ebp - 0x74], ecx
004e0473 6800010000               push       0x100
004e0478 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004e047b e88097f3ff               call       0x419c00
004e0480 8945c0                   mov        dword ptr [ebp - 0x40], eax
004e0483 eb07                     jmp        0x4e048c
004e0485 c745c000000000           mov        dword ptr [ebp - 0x40], 0
004e048c 837dc000                 cmp        dword ptr [ebp - 0x40], 0
004e0490 740a                     je         0x4e049c
004e0492 e98d000000               jmp        0x4e0524
004e0497 e988000000               jmp        0x4e0524
004e049c 33d2                     xor        edx, edx
004e049e 7532                     jne        0x4e04d2
004e04a0 b804000000               mov        eax, 4
004e04a5 d1e0                     shl        eax, 1
004e04a7 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
004e04ae 7422                     je         0x4e04d2
004e04b0 b904000000               mov        ecx, 4
004e04b5 d1e1                     shl        ecx, 1
004e04b7 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
004e04bd 895588                   mov        dword ptr [ebp - 0x78], edx
004e04c0 6801000800               push       0x80001
004e04c5 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004e04c8 e83397f3ff               call       0x419c00
004e04cd 8945bc                   mov        dword ptr [ebp - 0x44], eax
004e04d0 eb07                     jmp        0x4e04d9
004e04d2 c745bc00000000           mov        dword ptr [ebp - 0x44], 0
004e04d9 837dbc00                 cmp        dword ptr [ebp - 0x44], 0
004e04dd 0f8495000000             je         0x4e0578
004e04e3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e04e6 83c114                   add        ecx, 0x14
004e04e9 e812bef2ff               call       0x40c300
004e04ee 8945f0                   mov        dword ptr [ebp - 0x10], eax
004e04f1 837df003                 cmp        dword ptr [ebp - 0x10], 3
004e04f5 7448                     je         0x4e053f
004e04f7 837df004                 cmp        dword ptr [ebp - 0x10], 4
004e04fb 7408                     je         0x4e0505
004e04fd 837df005                 cmp        dword ptr [ebp - 0x10], 5
004e0501 7421                     je         0x4e0524
004e0503 eb73                     jmp        0x4e0578
004e0505 c605864f5c0064           mov        byte ptr [0x5c4f86], 0x64
004e050c c605874f5c0050           mov        byte ptr [0x5c4f87], 0x50
004e0513 6a00                     push       0
004e0515 6a07                     push       7
004e0517 b930a85b00               mov        ecx, 0x5ba830
004e051c e84f68f4ff               call       0x426d70
004e0521 90                       nop        
004e0522 eb54                     jmp        0x4e0578
004e0524 6a00                     push       0
004e0526 6a09                     push       9
004e0528 b930a85b00               mov        ecx, 0x5ba830
004e052d e83e68f4ff               call       0x426d70
004e0532 6a05                     push       5
004e0534 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e0537 e8440a0000               call       0x4e0f80
004e053c 90                       nop        
004e053d eb39                     jmp        0x4e0578
004e053f 6a00                     push       0
004e0541 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e0544 83c160                   add        ecx, 0x60
004e0547 e8d42ff4ff               call       0x423520
004e054c 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e054f c780a000000003000000     mov        dword ptr [eax + 0xa0], 3
004e0559 6a1e                     push       0x1e
004e055b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e055e 81c18c000000             add        ecx, 0x8c
004e0564 e8b72ff4ff               call       0x423520
004e0569 6a00                     push       0
004e056b 6a07                     push       7
004e056d b930a85b00               mov        ecx, 0x5ba830
004e0572 e8f967f4ff               call       0x426d70
004e0577 90                       nop        
004e0578 e9da000000               jmp        0x4e0657
004e057d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e0580 83c160                   add        ecx, 0x60
004e0583 894d84                   mov        dword ptr [ebp - 0x7c], ecx
004e0586 6a03                     push       3
004e0588 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
004e058b e8e00af8ff               call       0x461070
004e0590 0fb6d0                   movzx      edx, al
004e0593 85d2                     test       edx, edx
004e0595 7427                     je         0x4e05be
004e0597 6a01                     push       1
004e0599 b958675b00               mov        ecx, 0x5b6758
004e059e e85dd8f3ff               call       0x41de00
004e05a3 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e05a6 c780a000000002000000     mov        dword ptr [eax + 0xa0], 2
004e05b0 6a00                     push       0
004e05b2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e05b5 83c160                   add        ecx, 0x60
004e05b8 e8632ff4ff               call       0x423520
004e05bd 90                       nop        
004e05be e994000000               jmp        0x4e0657
004e05c3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e05c6 83c160                   add        ecx, 0x60
004e05c9 894d80                   mov        dword ptr [ebp - 0x80], ecx
004e05cc 6a1e                     push       0x1e
004e05ce 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004e05d1 e89a0af8ff               call       0x461070
004e05d6 0fb6d0                   movzx      edx, al
004e05d9 85d2                     test       edx, edx
004e05db 7438                     je         0x4e0615
004e05dd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e05e0 83c114                   add        ecx, 0x14
004e05e3 e818bdf2ff               call       0x40c300
004e05e8 89857cffffff             mov        dword ptr [ebp - 0x84], eax
004e05ee 83bd7cffffff03           cmp        dword ptr [ebp - 0x84], 3
004e05f5 7402                     je         0x4e05f9
004e05f7 eb1c                     jmp        0x4e0615
004e05f9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e05fc 83c070                   add        eax, 0x70
004e05ff 50                       push       eax
004e0600 e8ab76feff               call       0x4c7cb0
004e0605 83c404                   add        esp, 4
004e0608 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e060b c781a000000000000000     mov        dword ptr [ecx + 0xa0], 0
004e0615 eb40                     jmp        0x4e0657
004e0617 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e061a 83c260                   add        edx, 0x60
004e061d 899578ffffff             mov        dword ptr [ebp - 0x88], edx
004e0623 6a0a                     push       0xa
004e0625 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
004e062b e8902ff4ff               call       0x4235c0
004e0630 0fb6c0                   movzx      eax, al
004e0633 85c0                     test       eax, eax
004e0635 7420                     je         0x4e0657
004e0637 b9084f5c00               mov        ecx, 0x5c4f08
004e063c e82fa4f3ff               call       0x41aa70
004e0641 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e0644 51                       push       ecx
004e0645 e87611f4ff               call       0x4217c0
004e064a 83c404                   add        esp, 4
004e064d b801000000               mov        eax, 1
004e0652 e998000000               jmp        0x4e06ef
004e0657 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e065a 83c260                   add        edx, 0x60
004e065d 899574ffffff             mov        dword ptr [ebp - 0x8c], edx
004e0663 6a00                     push       0
004e0665 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
004e066b e8d02ef4ff               call       0x423540
004e0670 90                       nop        
004e0671 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e0674 83c07c                   add        eax, 0x7c
004e0677 898570ffffff             mov        dword ptr [ebp - 0x90], eax
004e067d 6a00                     push       0
004e067f 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
004e0685 e8e609f8ff               call       0x461070
004e068a 0fb6c8                   movzx      ecx, al
004e068d 85c9                     test       ecx, ecx
004e068f 741a                     je         0x4e06ab
004e0691 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e0694 83c27c                   add        edx, 0x7c
004e0697 89956cffffff             mov        dword ptr [ebp - 0x94], edx
004e069d 6a00                     push       0
004e069f 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
004e06a5 e8768df4ff               call       0x429420
004e06aa 90                       nop        
004e06ab 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e06ae 058c000000               add        eax, 0x8c
004e06b3 898568ffffff             mov        dword ptr [ebp - 0x98], eax
004e06b9 6a00                     push       0
004e06bb 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
004e06c1 e8aa09f8ff               call       0x461070
004e06c6 0fb6c8                   movzx      ecx, al
004e06c9 85c9                     test       ecx, ecx
004e06cb 741d                     je         0x4e06ea
004e06cd 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e06d0 81c28c000000             add        edx, 0x8c
004e06d6 899564ffffff             mov        dword ptr [ebp - 0x9c], edx
004e06dc 6a00                     push       0
004e06de 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
004e06e4 e8378df4ff               call       0x429420
004e06e9 90                       nop        
004e06ea b801000000               mov        eax, 1
004e06ef 8be5                     mov        esp, ebp
004e06f1 5d                       pop        ebp
004e06f2 c3                       ret        
004e06f3 90                       nop        
004e06f4 3f                       aas        
004e06f5 fe4d00                   dec        byte ptr [ebp]
004e06f8 6afe                     push       -2
004e06fa 4d                       dec        ebp
004e06fb 0094fe4d00c305           add        byte ptr [esi + edi*8 + 0x5c3004d], dl
004e0702 4e                       dec        esi
004e0703 007d05                   add        byte ptr [ebp + 5], bh
004e0706 4e                       dec        esi
004e0707 0017                     add        byte ptr [edi], dl
004e0709 06                       push       es
004e070a 4e                       dec        esi
004e070b 0027                     add        byte ptr [edi], ah
004e070d 014e00                   add        dword ptr [esi], ecx
004e0710 27                       daa        
004e0711 014e00                   add        dword ptr [esi], ecx
004e0714 27                       daa        
004e0715 014e00                   add        dword ptr [esi], ecx
004e0718 cd00                     int        0
004e071a 4e                       dec        esi
004e071b 00df                     add        bh, bl
004e071d 004e00                   add        byte ptr [esi], cl
004e0720 f1                       int1       
004e0721 004e00                   add        byte ptr [esi], cl
004e0724 0301                     add        eax, dword ptr [ecx]
004e0726 4e                       dec        esi
004e0727 0015014e0048             add        byte ptr [0x48004e01], dl
004e072d 014e00                   add        dword ptr [esi], ecx
004e0730 3901                     cmp        dword ptr [ecx], eax
004e0732 4e                       dec        esi
004e0733 006201                   add        byte ptr [edx + 1], ah
004e0736 4e                       dec        esi
004e0737 007101                   add        byte ptr [ecx + 1], dh
004e073a 4e                       dec        esi
004e073b 0080014e008f             add        byte ptr [eax - 0x70ffb1ff], al
004e0741 014e00                   add        dword ptr [esi], ecx
004e0744 9e                       sahf       
004e0745 014e00                   add        dword ptr [esi], ecx
004e0748 ad                       lodsd      eax, dword ptr [esi]
004e0749 014e00                   add        dword ptr [esi], ecx
004e074c 43                       inc        ebx
004e074d 034e00                   add        ecx, dword ptr [esi]
004e0750 55                       push       ebp
004e0751 034e00                   add        ecx, dword ptr [esi]
004e0754 64034e00                 add        ecx, dword ptr fs:[esi]
004e0758 d502                     aad        2
004e075a 4e                       dec        esi
004e075b 00d5                     add        ch, dl
004e075d 024e00                   add        cl, byte ptr [esi]
004e0760 d502                     aad        2
004e0762 4e                       dec        esi
004e0763 00d5                     add        ch, dl
004e0765 024e00                   add        cl, byte ptr [esi]
004e0768 d502                     aad        2
004e076a 4e                       dec        esi
004e076b 00e7                     add        bh, ah
004e076d 024e00                   add        cl, byte ptr [esi]
004e0770 f9                       stc        
004e0771 024e00                   add        cl, byte ptr [esi]
004e0774 1303                     adc        eax, dword ptr [ebx]
004e0776 4e                       dec        esi
004e0777 0022                     add        byte ptr [edx], ah
004e0779 034e00                   add        ecx, dword ptr [esi]
004e077c 3103                     xor        dword ptr [ebx], eax
004e077e 4e                       dec        esi
004e077f 0031                     add        byte ptr [ecx], dh
004e0781 034e00                   add        ecx, dword ptr [esi]
004e0784 3103                     xor        dword ptr [ebx], eax
004e0786 4e                       dec        esi
004e0787 0031                     add        byte ptr [ecx], dh
004e0789 034e00                   add        ecx, dword ptr [esi]
004e078c 7e03                     jle        0x4e0791
004e078e 4e                       dec        esi
004e078f 008d034e009c             add        byte ptr [ebp - 0x63ffb1fd], cl
004e0795 034e00                   add        ecx, dword ptr [esi]
004e0798 ab                       stosd      dword ptr es:[edi], eax
004e0799 034e00                   add        ecx, dword ptr [esi]
004e079c ba034e00c9               mov        edx, 0xc9004e03
004e07a1 034e00                   add        ecx, dword ptr [esi]
004e07a4 cc                       int3       
004e07a5 cc                       int3       
004e07a6 cc                       int3       
004e07a7 cc                       int3       
004e07a8 cc                       int3       
004e07a9 cc                       int3       
004e07aa cc                       int3       
004e07ab cc                       int3       
004e07ac cc                       int3       
004e07ad cc                       int3       
004e07ae cc                       int3       
004e07af cc                       int3       
