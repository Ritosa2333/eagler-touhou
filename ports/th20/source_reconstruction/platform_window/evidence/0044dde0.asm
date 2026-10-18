0044dde0  55                                 push       ebp
0044dde1  8b ec                              mov        ebp, esp
0044dde3  83 ec 78                           sub        esp, 0x78
0044dde6  e8 92 3a 0f 00                     call       0x54187d
0044ddeb  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0044ddee  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
0044ddf1  e8 70 3a 0f 00                     call       0x541866
0044ddf6  89 45 f8                           mov        dword ptr [ebp - 8], eax
0044ddf9  89 55 fc                           mov        dword ptr [ebp - 4], edx
0044ddfc  33 c0                              xor        eax, eax
0044ddfe  c7 45 b0 80 96 98 00               mov        dword ptr [ebp - 0x50], 0x989680
0044de05  89 45 b4                           mov        dword ptr [ebp - 0x4c], eax
0044de08  33 c9                              xor        ecx, ecx
0044de0a  c7 45 a8 00 36 6e 01               mov        dword ptr [ebp - 0x58], 0x16e3600
0044de11  89 4d ac                           mov        dword ptr [ebp - 0x54], ecx
0044de14  81 7d f0 80 96 98 00               cmp        dword ptr [ebp - 0x10], 0x989680
0044de1b  75 4c                              jne        0x44de69
0044de1d  83 7d f4 00                        cmp        dword ptr [ebp - 0xc], 0
0044de21  75 46                              jne        0x44de69
0044de23  33 d2                              xor        edx, edx
0044de25  c7 45 a0 64 00 00 00               mov        dword ptr [ebp - 0x60], 0x64
0044de2c  89 55 a4                           mov        dword ptr [ebp - 0x5c], edx
0044de2f  6a 00                              push       0
0044de31  6a 64                              push       0x64
0044de33  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0044de36  50                                 push       eax
0044de37  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0044de3a  51                                 push       ecx
0044de3b  e8 c0 52 0f 00                     call       0x543100
0044de40  89 45 c8                           mov        dword ptr [ebp - 0x38], eax
0044de43  89 55 cc                           mov        dword ptr [ebp - 0x34], edx
0044de46  8d 55 c8                           lea        edx, [ebp - 0x38]
0044de49  52                                 push       edx
0044de4a  8d 4d 98                           lea        ecx, [ebp - 0x68]
0044de4d  e8 4e 8f ff ff                     call       0x446da0
0044de52  50                                 push       eax
0044de53  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0044de56  e8 c5 9e ff ff                     call       0x447d20
0044de5b  90                                 nop        
0044de5c  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0044de5f  e9 3b 01 00 00                     jmp        0x44df9f
0044de69  81 7d f0 00 36 6e 01               cmp        dword ptr [ebp - 0x10], 0x16e3600
0044de70  0f 85 9d 00 00 00                  jne        0x44df13
0044de76  83 7d f4 00                        cmp        dword ptr [ebp - 0xc], 0
0044de7a  0f 85 93 00 00 00                  jne        0x44df13
0044de80  6a 00                              push       0
0044de82  68 00 36 6e 01                     push       0x16e3600
0044de87  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0044de8a  50                                 push       eax
0044de8b  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0044de8e  51                                 push       ecx
0044de8f  e8 bc 51 0f 00                     call       0x543050
0044de94  6a 00                              push       0
0044de96  68 00 ca 9a 3b                     push       0x3b9aca00
0044de9b  52                                 push       edx
0044de9c  50                                 push       eax
0044de9d  e8 5e 52 0f 00                     call       0x543100
0044dea2  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
0044dea5  89 55 ec                           mov        dword ptr [ebp - 0x14], edx
0044dea8  6a 00                              push       0
0044deaa  68 00 36 6e 01                     push       0x16e3600
0044deaf  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0044deb2  52                                 push       edx
0044deb3  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0044deb6  50                                 push       eax
0044deb7  e8 84 52 0f 00                     call       0x543140
0044debc  6a 00                              push       0
0044debe  68 00 ca 9a 3b                     push       0x3b9aca00
0044dec3  52                                 push       edx
0044dec4  50                                 push       eax
0044dec5  e8 36 52 0f 00                     call       0x543100
0044deca  6a 00                              push       0
0044decc  68 00 36 6e 01                     push       0x16e3600
0044ded1  52                                 push       edx
0044ded2  50                                 push       eax
0044ded3  e8 78 51 0f 00                     call       0x543050
0044ded8  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
0044dedb  89 55 e4                           mov        dword ptr [ebp - 0x1c], edx
0044dede  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0044dee1  03 4d e0                           add        ecx, dword ptr [ebp - 0x20]
0044dee4  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
0044dee7  13 55 e4                           adc        edx, dword ptr [ebp - 0x1c]
0044deea  89 4d c0                           mov        dword ptr [ebp - 0x40], ecx
0044deed  89 55 c4                           mov        dword ptr [ebp - 0x3c], edx
0044def0  8d 45 c0                           lea        eax, [ebp - 0x40]
0044def3  50                                 push       eax
0044def4  8d 4d 90                           lea        ecx, [ebp - 0x70]
0044def7  e8 a4 8e ff ff                     call       0x446da0
0044defc  50                                 push       eax
0044defd  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0044df00  e8 1b 9e ff ff                     call       0x447d20
0044df05  90                                 nop        
0044df06  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0044df09  e9 91 00 00 00                     jmp        0x44df9f
0044df13  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0044df16  51                                 push       ecx
0044df17  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0044df1a  52                                 push       edx
0044df1b  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0044df1e  50                                 push       eax
0044df1f  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0044df22  51                                 push       ecx
0044df23  e8 28 51 0f 00                     call       0x543050
0044df28  6a 00                              push       0
0044df2a  68 00 ca 9a 3b                     push       0x3b9aca00
0044df2f  52                                 push       edx
0044df30  50                                 push       eax
0044df31  e8 ca 51 0f 00                     call       0x543100
0044df36  89 45 d8                           mov        dword ptr [ebp - 0x28], eax
0044df39  89 55 dc                           mov        dword ptr [ebp - 0x24], edx
0044df3c  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
0044df3f  52                                 push       edx
0044df40  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0044df43  50                                 push       eax
0044df44  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0044df47  51                                 push       ecx
0044df48  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0044df4b  52                                 push       edx
0044df4c  e8 ef 51 0f 00                     call       0x543140
0044df51  6a 00                              push       0
0044df53  68 00 ca 9a 3b                     push       0x3b9aca00
0044df58  52                                 push       edx
0044df59  50                                 push       eax
0044df5a  e8 a1 51 0f 00                     call       0x543100
0044df5f  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0044df62  51                                 push       ecx
0044df63  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0044df66  51                                 push       ecx
0044df67  52                                 push       edx
0044df68  50                                 push       eax
0044df69  e8 e2 50 0f 00                     call       0x543050
0044df6e  89 45 d0                           mov        dword ptr [ebp - 0x30], eax
0044df71  89 55 d4                           mov        dword ptr [ebp - 0x2c], edx
0044df74  8b 55 d8                           mov        edx, dword ptr [ebp - 0x28]
0044df77  03 55 d0                           add        edx, dword ptr [ebp - 0x30]
0044df7a  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
0044df7d  13 45 d4                           adc        eax, dword ptr [ebp - 0x2c]
0044df80  89 55 b8                           mov        dword ptr [ebp - 0x48], edx
0044df83  89 45 bc                           mov        dword ptr [ebp - 0x44], eax
0044df86  8d 4d b8                           lea        ecx, [ebp - 0x48]
0044df89  51                                 push       ecx
0044df8a  8d 4d 88                           lea        ecx, [ebp - 0x78]
0044df8d  e8 0e 8e ff ff                     call       0x446da0
0044df92  50                                 push       eax
0044df93  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0044df96  e8 85 9d ff ff                     call       0x447d20
0044df9b  90                                 nop        
0044df9c  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0044df9f  8b e5                              mov        esp, ebp
0044dfa1  5d                                 pop        ebp
0044dfa2  c3                                 ret        
