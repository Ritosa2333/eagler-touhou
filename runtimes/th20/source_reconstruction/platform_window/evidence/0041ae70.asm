0041ae70  55                                 push       ebp
0041ae71  8b ec                              mov        ebp, esp
0041ae73  81 ec 5c 01 00 00                  sub        esp, 0x15c
0041ae79  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0041ae7e  33 c5                              xor        eax, ebp
0041ae80  89 45 fc                           mov        dword ptr [ebp - 4], eax
0041ae83  c7 85 a4 fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x15c], 0
0041ae8d  c7 85 a8 fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x158], 0
0041ae97  c7 85 ac fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x154], 0
0041aea1  c7 85 b0 fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x150], 0
0041aeab  c7 85 b4 fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x14c], 0
0041aeb5  c7 85 b8 fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x148], 0
0041aebf  c7 85 bc fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x144], 0
0041aec9  c7 85 c0 fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x140], 0
0041aed3  c7 85 c4 fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x13c], 0
0041aedd  c7 85 c8 fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x138], 0
0041aee7  a1 8c 4f 5c 00                     mov        eax, dword ptr [0x5c4f8c]
0041aeec  c1 e8 07                           shr        eax, 7
0041aeef  83 e0 01                           and        eax, 1
0041aef2  75 2a                              jne        0x41af1e
0041aef4  6a 10                              push       0x10
0041aef6  ff 15 40 c2 56 00                  call       dword ptr [0x56c240] ; USER32.dll!GetKeyState
0041aefc  0f bf c8                           movsx      ecx, ax
0041aeff  81 e1 80 00 00 00                  and        ecx, 0x80
0041af05  75 17                              jne        0x41af1e
0041af07  6a 11                              push       0x11
0041af09  ff 15 40 c2 56 00                  call       dword ptr [0x56c240] ; USER32.dll!GetKeyState
0041af0f  0f bf d0                           movsx      edx, ax
0041af12  81 e2 80 00 00 00                  and        edx, 0x80
0041af18  0f 84 4e 05 00 00                  je         0x41b46c
0041af1e  c7 85 00 ff ff ff 00 00 00 00      mov        dword ptr [ebp - 0x100], 0
0041af28  e8 93 21 00 00                     call       0x41d0c0
0041af2d  85 c0                              test       eax, eax
0041af2f  74 2d                              je         0x41af5e
0041af31  6a 00                              push       0
0041af33  68 e0 ab 41 00                     push       0x41abe0
0041af38  6a 00                              push       0
0041af3a  68 cb 00 00 00                     push       0xcb
0041af3f  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041af44  e8 67 1b 00 00                     call       0x41cab0
0041af49  50                                 push       eax
0041af4a  ff 15 30 c2 56 00                  call       dword ptr [0x56c230] ; USER32.dll!CreateDialogParamW
0041af50  50                                 push       eax
0041af51  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041af56  e8 15 66 ff ff                     call       0x411570
0041af5b  90                                 nop        
0041af5c  eb 2b                              jmp        0x41af89
0041af5e  6a 00                              push       0
0041af60  68 e0 ab 41 00                     push       0x41abe0
0041af65  6a 00                              push       0
0041af67  68 cc 00 00 00                     push       0xcc
0041af6c  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041af71  e8 3a 1b 00 00                     call       0x41cab0
0041af76  50                                 push       eax
0041af77  ff 15 30 c2 56 00                  call       dword ptr [0x56c230] ; USER32.dll!CreateDialogParamW
0041af7d  50                                 push       eax
0041af7e  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041af83  e8 e8 65 ff ff                     call       0x411570
0041af88  90                                 nop        
0041af89  68 dc 00 00 00                     push       0xdc
0041af8e  6a 00                              push       0
0041af90  8d 85 04 ff ff ff                  lea        eax, [ebp - 0xfc]
0041af96  50                                 push       eax
0041af97  e8 c4 97 12 00                     call       0x544760
0041af9c  83 c4 0c                           add        esp, 0xc
0041af9f  c7 45 b0 00 00 00 00               mov        dword ptr [ebp - 0x50], 0
0041afa6  8d 8d 04 ff ff ff                  lea        ecx, [ebp - 0xfc]
0041afac  51                                 push       ecx
0041afad  6a ff                              push       -1
0041afaf  6a 00                              push       0
0041afb1  ff 15 80 c2 56 00                  call       dword ptr [0x56c280] ; USER32.dll!EnumDisplaySettingsW
0041afb7  90                                 nop        
0041afb8  81 7d b0 00 0a 00 00               cmp        dword ptr [ebp - 0x50], 0xa00
0041afbf  72 09                              jb         0x41afca
0041afc1  81 7d b4 80 07 00 00               cmp        dword ptr [ebp - 0x4c], 0x780
0041afc8  73 33                              jae        0x41affd
0041afca  ba 04 00 00 00                     mov        edx, 4
0041afcf  6b c2 05                           imul       eax, edx, 5
0041afd2  c7 84 05 a4 fe ff ff 01 00 00 00   mov        dword ptr [ebp + eax - 0x15c], 1
0041afdd  6a 00                              push       0
0041afdf  68 d4 00 00 00                     push       0xd4
0041afe4  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041afe9  e8 a2 4f ff ff                     call       0x40ff90
0041afee  50                                 push       eax
0041afef  ff 15 34 c2 56 00                  call       dword ptr [0x56c234] ; USER32.dll!GetDlgItem
0041aff5  50                                 push       eax
0041aff6  ff 15 44 c2 56 00                  call       dword ptr [0x56c244] ; USER32.dll!EnableWindow
0041affc  90                                 nop        
0041affd  81 7d b0 80 07 00 00               cmp        dword ptr [ebp - 0x50], 0x780
0041b004  73 3c                              jae        0x41b042
0041b006  81 7d b4 a0 05 00 00               cmp        dword ptr [ebp - 0x4c], 0x5a0
0041b00d  73 33                              jae        0x41b042
0041b00f  b9 04 00 00 00                     mov        ecx, 4
0041b014  6b d1 06                           imul       edx, ecx, 6
0041b017  c7 84 15 a4 fe ff ff 01 00 00 00   mov        dword ptr [ebp + edx - 0x15c], 1
0041b022  6a 00                              push       0
0041b024  68 d5 00 00 00                     push       0xd5
0041b029  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041b02e  e8 5d 4f ff ff                     call       0x40ff90
0041b033  50                                 push       eax
0041b034  ff 15 34 c2 56 00                  call       dword ptr [0x56c234] ; USER32.dll!GetDlgItem
0041b03a  50                                 push       eax
0041b03b  ff 15 44 c2 56 00                  call       dword ptr [0x56c244] ; USER32.dll!EnableWindow
0041b041  90                                 nop        
0041b042  81 7d b0 00 05 00 00               cmp        dword ptr [ebp - 0x50], 0x500
0041b049  73 6d                              jae        0x41b0b8
0041b04b  81 7d b4 c0 03 00 00               cmp        dword ptr [ebp - 0x4c], 0x3c0
0041b052  73 64                              jae        0x41b0b8
0041b054  b8 04 00 00 00                     mov        eax, 4
0041b059  6b c8 07                           imul       ecx, eax, 7
0041b05c  c7 84 0d a4 fe ff ff 01 00 00 00   mov        dword ptr [ebp + ecx - 0x15c], 1
0041b067  6a 00                              push       0
0041b069  68 d6 00 00 00                     push       0xd6
0041b06e  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041b073  e8 18 4f ff ff                     call       0x40ff90
0041b078  50                                 push       eax
0041b079  ff 15 34 c2 56 00                  call       dword ptr [0x56c234] ; USER32.dll!GetDlgItem
0041b07f  50                                 push       eax
0041b080  ff 15 44 c2 56 00                  call       dword ptr [0x56c244] ; USER32.dll!EnableWindow
0041b086  ba 04 00 00 00                     mov        edx, 4
0041b08b  d1 e2                              shl        edx, 1
0041b08d  c7 84 15 a4 fe ff ff 01 00 00 00   mov        dword ptr [ebp + edx - 0x15c], 1
0041b098  6a 00                              push       0
0041b09a  68 d1 00 00 00                     push       0xd1
0041b09f  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041b0a4  e8 e7 4e ff ff                     call       0x40ff90
0041b0a9  50                                 push       eax
0041b0aa  ff 15 34 c2 56 00                  call       dword ptr [0x56c234] ; USER32.dll!GetDlgItem
0041b0b0  50                                 push       eax
0041b0b1  ff 15 44 c2 56 00                  call       dword ptr [0x56c244] ; USER32.dll!EnableWindow
0041b0b7  90                                 nop        
0041b0b8  81 7d b0 c0 03 00 00               cmp        dword ptr [ebp - 0x50], 0x3c0
0041b0bf  73 6e                              jae        0x41b12f
0041b0c1  81 7d b4 d0 02 00 00               cmp        dword ptr [ebp - 0x4c], 0x2d0
0041b0c8  73 65                              jae        0x41b12f
0041b0ca  b8 04 00 00 00                     mov        eax, 4
0041b0cf  c1 e0 03                           shl        eax, 3
0041b0d2  c7 84 05 a4 fe ff ff 01 00 00 00   mov        dword ptr [ebp + eax - 0x15c], 1
0041b0dd  6a 00                              push       0
0041b0df  68 d7 00 00 00                     push       0xd7
0041b0e4  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041b0e9  e8 a2 4e ff ff                     call       0x40ff90
0041b0ee  50                                 push       eax
0041b0ef  ff 15 34 c2 56 00                  call       dword ptr [0x56c234] ; USER32.dll!GetDlgItem
0041b0f5  50                                 push       eax
0041b0f6  ff 15 44 c2 56 00                  call       dword ptr [0x56c244] ; USER32.dll!EnableWindow
0041b0fc  b9 04 00 00 00                     mov        ecx, 4
0041b101  6b d1 03                           imul       edx, ecx, 3
0041b104  c7 84 15 a4 fe ff ff 01 00 00 00   mov        dword ptr [ebp + edx - 0x15c], 1
0041b10f  6a 00                              push       0
0041b111  68 d2 00 00 00                     push       0xd2
0041b116  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041b11b  e8 70 4e ff ff                     call       0x40ff90
0041b120  50                                 push       eax
0041b121  ff 15 34 c2 56 00                  call       dword ptr [0x56c234] ; USER32.dll!GetDlgItem
0041b127  50                                 push       eax
0041b128  ff 15 44 c2 56 00                  call       dword ptr [0x56c244] ; USER32.dll!EnableWindow
0041b12e  90                                 nop        
0041b12f  c7 85 fc fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x104], 0
0041b139  eb 0f                              jmp        0x41b14a
0041b13b  8b 85 fc fe ff ff                  mov        eax, dword ptr [ebp - 0x104]
0041b141  83 c0 01                           add        eax, 1
0041b144  89 85 fc fe ff ff                  mov        dword ptr [ebp - 0x104], eax
0041b14a  83 bd fc fe ff ff 0a               cmp        dword ptr [ebp - 0x104], 0xa
0041b151  7d 4c                              jge        0x41b19f
0041b153  8b 8d fc fe ff ff                  mov        ecx, dword ptr [ebp - 0x104]
0041b159  8b 14 8d f8 e1 5a 00               mov        edx, dword ptr [ecx*4 + 0x5ae1f8]
0041b160  89 95 ec fe ff ff                  mov        dword ptr [ebp - 0x114], edx
0041b166  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041b16b  e8 20 4e ff ff                     call       0x40ff90
0041b170  89 85 e8 fe ff ff                  mov        dword ptr [ebp - 0x118], eax
0041b176  8b 85 ec fe ff ff                  mov        eax, dword ptr [ebp - 0x114]
0041b17c  50                                 push       eax
0041b17d  8b 8d e8 fe ff ff                  mov        ecx, dword ptr [ebp - 0x118]
0041b183  51                                 push       ecx
0041b184  ff 15 3c c2 56 00                  call       dword ptr [0x56c23c] ; USER32.dll!IsDlgButtonChecked
0041b18a  83 f8 01                           cmp        eax, 1
0041b18d  75 0e                              jne        0x41b19d
0041b18f  8b 95 fc fe ff ff                  mov        edx, dword ptr [ebp - 0x104]
0041b195  89 95 00 ff ff ff                  mov        dword ptr [ebp - 0x100], edx
0041b19b  eb 02                              jmp        0x41b19f
0041b19d  eb 9c                              jmp        0x41b13b
0041b19f  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041b1a4  e8 e7 4d ff ff                     call       0x40ff90
0041b1a9  50                                 push       eax
0041b1aa  ff 15 20 c2 56 00                  call       dword ptr [0x56c220] ; USER32.dll!IsWindow
0041b1b0  85 c0                              test       eax, eax
0041b1b2  75 12                              jne        0x41b1c6
0041b1b4  68 38 cd 56 00                     push       0x56cd38 ; string='Invalid window handle.'
0041b1b9  e8 f2 14 ff ff                     call       0x40c6b0
0041b1be  83 c4 04                           add        esp, 4
0041b1c1  e9 a6 02 00 00                     jmp        0x41b46c
0041b1c6  6a 05                              push       5
0041b1c8  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041b1cd  e8 be 4d ff ff                     call       0x40ff90
0041b1d2  50                                 push       eax
0041b1d3  ff 15 28 c2 56 00                  call       dword ptr [0x56c228] ; USER32.dll!ShowWindow
0041b1d9  90                                 nop        
0041b1da  b8 01 00 00 00                     mov        eax, 1
0041b1df  85 c0                              test       eax, eax
0041b1e1  0f 84 85 02 00 00                  je         0x41b46c
0041b1e7  e8 94 53 00 00                     call       0x420580
0041b1ec  90                                 nop        
0041b1ed  6a 01                              push       1
0041b1ef  6a 00                              push       0
0041b1f1  6a 00                              push       0
0041b1f3  6a 00                              push       0
0041b1f5  8d 4d e0                           lea        ecx, [ebp - 0x20]
0041b1f8  51                                 push       ecx
0041b1f9  ff 15 d0 c2 56 00                  call       dword ptr [0x56c2d0] ; USER32.dll!PeekMessageW
0041b1ff  85 c0                              test       eax, eax
0041b201  74 30                              je         0x41b233
0041b203  8d 55 e0                           lea        edx, [ebp - 0x20]
0041b206  52                                 push       edx
0041b207  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041b20c  e8 7f 4d ff ff                     call       0x40ff90
0041b211  50                                 push       eax
0041b212  ff 15 7c c2 56 00                  call       dword ptr [0x56c27c] ; USER32.dll!IsDialogMessageW
0041b218  85 c0                              test       eax, eax
0041b21a  74 02                              je         0x41b21e
0041b21c  eb bc                              jmp        0x41b1da
0041b21e  8d 45 e0                           lea        eax, [ebp - 0x20]
0041b221  50                                 push       eax
0041b222  ff 15 b4 c2 56 00                  call       dword ptr [0x56c2b4] ; USER32.dll!TranslateMessage
0041b228  8d 4d e0                           lea        ecx, [ebp - 0x20]
0041b22b  51                                 push       ecx
0041b22c  ff 15 b8 c2 56 00                  call       dword ptr [0x56c2b8] ; USER32.dll!DispatchMessageW
0041b232  90                                 nop        
0041b233  33 d2                              xor        edx, edx
0041b235  75 3b                              jne        0x41b272
0041b237  b8 04 00 00 00                     mov        eax, 4
0041b23c  d1 e0                              shl        eax, 1
0041b23e  83 b8 9c 88 5b 00 00               cmp        dword ptr [eax + 0x5b889c], 0
0041b245  74 2b                              je         0x41b272
0041b247  b9 04 00 00 00                     mov        ecx, 4
0041b24c  d1 e1                              shl        ecx, 1
0041b24e  8b 91 9c 88 5b 00                  mov        edx, dword ptr [ecx + 0x5b889c]
0041b254  89 95 e4 fe ff ff                  mov        dword ptr [ebp - 0x11c], edx
0041b25a  68 01 00 08 00                     push       0x80001
0041b25f  8b 8d e4 fe ff ff                  mov        ecx, dword ptr [ebp - 0x11c]
0041b265  e8 96 e9 ff ff                     call       0x419c00
0041b26a  89 85 f8 fe ff ff                  mov        dword ptr [ebp - 0x108], eax
0041b270  eb 0a                              jmp        0x41b27c
0041b272  c7 85 f8 fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x108], 0
0041b27c  83 bd f8 fe ff ff 00               cmp        dword ptr [ebp - 0x108], 0
0041b283  74 35                              je         0x41b2ba
0041b285  e8 a6 f8 ff ff                     call       0x41ab30
0041b28a  a1 e8 87 5b 00                     mov        eax, dword ptr [0x5b87e8]
0041b28f  83 e0 e7                           and        eax, 0xffffffe7
0041b292  a3 e8 87 5b 00                     mov        dword ptr [0x5b87e8], eax
0041b297  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041b29c  e8 ef 4c ff ff                     call       0x40ff90
0041b2a1  50                                 push       eax
0041b2a2  ff 15 24 c2 56 00                  call       dword ptr [0x56c224] ; USER32.dll!DestroyWindow
0041b2a8  6a 00                              push       0
0041b2aa  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041b2af  e8 bc 62 ff ff                     call       0x411570
0041b2b4  90                                 nop        
0041b2b5  e9 b2 01 00 00                     jmp        0x41b46c
0041b2ba  33 c9                              xor        ecx, ecx
0041b2bc  75 38                              jne        0x41b2f6
0041b2be  ba 04 00 00 00                     mov        edx, 4
0041b2c3  d1 e2                              shl        edx, 1
0041b2c5  83 ba 9c 88 5b 00 00               cmp        dword ptr [edx + 0x5b889c], 0
0041b2cc  74 28                              je         0x41b2f6
0041b2ce  b8 04 00 00 00                     mov        eax, 4
0041b2d3  d1 e0                              shl        eax, 1
0041b2d5  8b 88 9c 88 5b 00                  mov        ecx, dword ptr [eax + 0x5b889c]
0041b2db  89 8d e0 fe ff ff                  mov        dword ptr [ebp - 0x120], ecx
0041b2e1  6a 20                              push       0x20
0041b2e3  8b 8d e0 fe ff ff                  mov        ecx, dword ptr [ebp - 0x120]
0041b2e9  e8 92 ef ff ff                     call       0x41a280
0041b2ee  89 85 f4 fe ff ff                  mov        dword ptr [ebp - 0x10c], eax
0041b2f4  eb 0a                              jmp        0x41b300
0041b2f6  c7 85 f4 fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x10c], 0
0041b300  83 bd f4 fe ff ff 00               cmp        dword ptr [ebp - 0x10c], 0
0041b307  74 7b                              je         0x41b384
0041b309  83 bd 00 ff ff ff 09               cmp        dword ptr [ebp - 0x100], 9
0041b310  7d 72                              jge        0x41b384
0041b312  8b 95 00 ff ff ff                  mov        edx, dword ptr [ebp - 0x100]
0041b318  83 c2 01                           add        edx, 1
0041b31b  89 95 00 ff ff ff                  mov        dword ptr [ebp - 0x100], edx
0041b321  8b 85 00 ff ff ff                  mov        eax, dword ptr [ebp - 0x100]
0041b327  83 bc 85 a4 fe ff ff 00            cmp        dword ptr [ebp + eax*4 - 0x15c], 0
0041b32f  74 11                              je         0x41b342
0041b331  8b 8d 00 ff ff ff                  mov        ecx, dword ptr [ebp - 0x100]
0041b337  83 c1 01                           add        ecx, 1
0041b33a  89 8d 00 ff ff ff                  mov        dword ptr [ebp - 0x100], ecx
0041b340  eb df                              jmp        0x41b321
0041b342  8b 95 00 ff ff ff                  mov        edx, dword ptr [ebp - 0x100]
0041b348  8b 04 95 f8 e1 5a 00               mov        eax, dword ptr [edx*4 + 0x5ae1f8]
0041b34f  89 85 dc fe ff ff                  mov        dword ptr [ebp - 0x124], eax
0041b355  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041b35a  e8 31 4c ff ff                     call       0x40ff90
0041b35f  89 85 d8 fe ff ff                  mov        dword ptr [ebp - 0x128], eax
0041b365  8b 8d dc fe ff ff                  mov        ecx, dword ptr [ebp - 0x124]
0041b36b  51                                 push       ecx
0041b36c  68 d8 00 00 00                     push       0xd8
0041b371  68 cf 00 00 00                     push       0xcf
0041b376  8b 95 d8 fe ff ff                  mov        edx, dword ptr [ebp - 0x128]
0041b37c  52                                 push       edx
0041b37d  ff 15 38 c2 56 00                  call       dword ptr [0x56c238] ; USER32.dll!CheckRadioButton
0041b383  90                                 nop        
0041b384  33 c0                              xor        eax, eax
0041b386  75 38                              jne        0x41b3c0
0041b388  b9 04 00 00 00                     mov        ecx, 4
0041b38d  d1 e1                              shl        ecx, 1
0041b38f  83 b9 9c 88 5b 00 00               cmp        dword ptr [ecx + 0x5b889c], 0
0041b396  74 28                              je         0x41b3c0
0041b398  ba 04 00 00 00                     mov        edx, 4
0041b39d  d1 e2                              shl        edx, 1
0041b39f  8b 82 9c 88 5b 00                  mov        eax, dword ptr [edx + 0x5b889c]
0041b3a5  89 85 d4 fe ff ff                  mov        dword ptr [ebp - 0x12c], eax
0041b3ab  6a 10                              push       0x10
0041b3ad  8b 8d d4 fe ff ff                  mov        ecx, dword ptr [ebp - 0x12c]
0041b3b3  e8 c8 ee ff ff                     call       0x41a280
0041b3b8  89 85 f0 fe ff ff                  mov        dword ptr [ebp - 0x110], eax
0041b3be  eb 0a                              jmp        0x41b3ca
0041b3c0  c7 85 f0 fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x110], 0
0041b3ca  83 bd f0 fe ff ff 00               cmp        dword ptr [ebp - 0x110], 0
0041b3d1  74 7b                              je         0x41b44e
0041b3d3  83 bd 00 ff ff ff 00               cmp        dword ptr [ebp - 0x100], 0
0041b3da  7e 72                              jle        0x41b44e
0041b3dc  8b 8d 00 ff ff ff                  mov        ecx, dword ptr [ebp - 0x100]
0041b3e2  83 e9 01                           sub        ecx, 1
0041b3e5  89 8d 00 ff ff ff                  mov        dword ptr [ebp - 0x100], ecx
0041b3eb  8b 95 00 ff ff ff                  mov        edx, dword ptr [ebp - 0x100]
0041b3f1  83 bc 95 a4 fe ff ff 00            cmp        dword ptr [ebp + edx*4 - 0x15c], 0
0041b3f9  74 11                              je         0x41b40c
0041b3fb  8b 85 00 ff ff ff                  mov        eax, dword ptr [ebp - 0x100]
0041b401  83 e8 01                           sub        eax, 1
0041b404  89 85 00 ff ff ff                  mov        dword ptr [ebp - 0x100], eax
0041b40a  eb df                              jmp        0x41b3eb
0041b40c  8b 8d 00 ff ff ff                  mov        ecx, dword ptr [ebp - 0x100]
0041b412  8b 14 8d f8 e1 5a 00               mov        edx, dword ptr [ecx*4 + 0x5ae1f8]
0041b419  89 95 d0 fe ff ff                  mov        dword ptr [ebp - 0x130], edx
0041b41f  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041b424  e8 67 4b ff ff                     call       0x40ff90
0041b429  89 85 cc fe ff ff                  mov        dword ptr [ebp - 0x134], eax
0041b42f  8b 85 d0 fe ff ff                  mov        eax, dword ptr [ebp - 0x130]
0041b435  50                                 push       eax
0041b436  68 d8 00 00 00                     push       0xd8
0041b43b  68 cf 00 00 00                     push       0xcf
0041b440  8b 8d cc fe ff ff                  mov        ecx, dword ptr [ebp - 0x134]
0041b446  51                                 push       ecx
0041b447  ff 15 38 c2 56 00                  call       dword ptr [0x56c238] ; USER32.dll!CheckRadioButton
0041b44d  90                                 nop        
0041b44e  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041b453  e8 38 4b ff ff                     call       0x40ff90
0041b458  85 c0                              test       eax, eax
0041b45a  75 02                              jne        0x41b45e
0041b45c  eb 0e                              jmp        0x41b46c
0041b45e  6a 06                              push       6
0041b460  ff 15 6c c1 56 00                  call       dword ptr [0x56c16c] ; KERNEL32.dll!Sleep
0041b466  90                                 nop        
0041b467  e9 6e fd ff ff                     jmp        0x41b1da
0041b46c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041b46f  33 cd                              xor        ecx, ebp
0041b471  e8 78 75 12 00                     call       0x5429ee
0041b476  8b e5                              mov        esp, ebp
0041b478  5d                                 pop        ebp
0041b479  c3                                 ret        
