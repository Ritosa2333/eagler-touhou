0046aef0  55                                 push       ebp
0046aef1  8b ec                              mov        ebp, esp
0046aef3  6a ff                              push       -1
0046aef5  68 50 8d 56 00                     push       0x568d50
0046aefa  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0046af00  50                                 push       eax
0046af01  83 ec 20                           sub        esp, 0x20
0046af04  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0046af09  33 c5                              xor        eax, ebp
0046af0b  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0046af0e  50                                 push       eax
0046af0f  8d 45 f4                           lea        eax, [ebp - 0xc]
0046af12  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0046af18  89 4d e4                           mov        dword ptr [ebp - 0x1c], ecx
0046af1b  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
0046af1e  c7 00 c0 f5 56 00                  mov        dword ptr [eax], 0x56f5c0
0046af24  68 28 f6 56 00                     push       0x56f628 ; string='shutdown AsciiInf'
0046af29  e8 82 17 fa ff                     call       0x40c6b0
0046af2e  83 c4 04                           add        esp, 4
0046af31  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0046af34  8b 51 08                           mov        edx, dword ptr [ecx + 8]
0046af37  52                                 push       edx
0046af38  8b 0d d8 66 5b 00                  mov        ecx, dword ptr [0x5b66d8]
0046af3e  e8 6d 75 fa ff                     call       0x4124b0
0046af43  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
0046af46  8b 48 0c                           mov        ecx, dword ptr [eax + 0xc]
0046af49  51                                 push       ecx
0046af4a  8b 0d d8 66 5b 00                  mov        ecx, dword ptr [0x5b66d8]
0046af50  e8 5b 75 fa ff                     call       0x4124b0
0046af55  8b 55 e4                           mov        edx, dword ptr [ebp - 0x1c]
0046af58  8b 82 50 a2 01 00                  mov        eax, dword ptr [edx + 0x1a250]
0046af5e  50                                 push       eax
0046af5f  8b 0d d8 66 5b 00                  mov        ecx, dword ptr [0x5b66d8]
0046af65  e8 46 75 fa ff                     call       0x4124b0
0046af6a  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0046af6d  8b 91 54 a2 01 00                  mov        edx, dword ptr [ecx + 0x1a254]
0046af73  52                                 push       edx
0046af74  8b 0d d8 66 5b 00                  mov        ecx, dword ptr [0x5b66d8]
0046af7a  e8 31 75 fa ff                     call       0x4124b0
0046af7f  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
0046af82  8b 88 58 a2 01 00                  mov        ecx, dword ptr [eax + 0x1a258]
0046af88  51                                 push       ecx
0046af89  8b 0d d8 66 5b 00                  mov        ecx, dword ptr [0x5b66d8]
0046af8f  e8 1c 75 fa ff                     call       0x4124b0
0046af94  8b 55 e4                           mov        edx, dword ptr [ebp - 0x1c]
0046af97  8b 82 5c a2 01 00                  mov        eax, dword ptr [edx + 0x1a25c]
0046af9d  50                                 push       eax
0046af9e  8b 0d d8 66 5b 00                  mov        ecx, dword ptr [0x5b66d8]
0046afa4  e8 07 75 fa ff                     call       0x4124b0
0046afa9  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0046afac  81 c1 24 a2 01 00                  add        ecx, 0x1a224
0046afb2  e8 39 0d fa ff                     call       0x40bcf0
0046afb7  6a 02                              push       2
0046afb9  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0046afbf  e8 6c 14 fe ff                     call       0x44c430
0046afc4  6a 00                              push       0
0046afc6  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0046afcc  e8 5f 14 fe ff                     call       0x44c430
0046afd1  c7 05 98 06 5c 00 00 00 00 00      mov        dword ptr [0x5c0698], 0
0046afdb  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0046afde  81 c1 0c a2 01 00                  add        ecx, 0x1a20c
0046afe4  89 4d e0                           mov        dword ptr [ebp - 0x20], ecx
0046afe7  6a 08                              push       8
0046afe9  8d 4d e8                           lea        ecx, [ebp - 0x18]
0046afec  e8 8f 10 fa ff                     call       0x40c080
0046aff1  8d 55 e8                           lea        edx, [ebp - 0x18]
0046aff4  52                                 push       edx
0046aff5  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
0046aff8  e8 83 72 fa ff                     call       0x412280
0046affd  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
0046b000  e8 3b 75 fa ff                     call       0x412540
0046b005  89 45 dc                           mov        dword ptr [ebp - 0x24], eax
0046b008  eb 09                              jmp        0x46b013
0046b00a  8d 4d e8                           lea        ecx, [ebp - 0x18]
0046b00d  e8 1e 6c fa ff                     call       0x411c30
0046b012  90                                 nop        
0046b013  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
0046b016  50                                 push       eax
0046b017  8d 4d e8                           lea        ecx, [ebp - 0x18]
0046b01a  e8 91 6b fa ff                     call       0x411bb0
0046b01f  0f b6 c8                           movzx      ecx, al
0046b022  85 c9                              test       ecx, ecx
0046b024  74 28                              je         0x46b04e
0046b026  8d 4d e8                           lea        ecx, [ebp - 0x18]
0046b029  e8 d2 12 fa ff                     call       0x40c300
0046b02e  89 45 d8                           mov        dword ptr [ebp - 0x28], eax
0046b031  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
0046b034  e8 27 41 fa ff                     call       0x40f160
0046b039  89 45 d4                           mov        dword ptr [ebp - 0x2c], eax
0046b03c  8b 55 d4                           mov        edx, dword ptr [ebp - 0x2c]
0046b03f  52                                 push       edx
0046b040  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0046b046  e8 45 f0 ff ff                     call       0x46a090
0046b04b  90                                 nop        
0046b04c  eb bc                              jmp        0x46b00a
0046b04e  8d 4d e8                           lea        ecx, [ebp - 0x18]
0046b051  e8 aa 6a fa ff                     call       0x411b00
0046b056  90                                 nop        
0046b057  e8 54 a5 fa ff                     call       0x4155b0
0046b05c  90                                 nop        
0046b05d  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0046b060  81 c1 34 a2 01 00                  add        ecx, 0x1a234
0046b066  e8 65 fe ff ff                     call       0x46aed0
0046b06b  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0046b06e  81 c1 24 a2 01 00                  add        ecx, 0x1a224
0046b074  e8 07 09 fa ff                     call       0x40b980
0046b079  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0046b07c  81 c1 d8 0b 00 00                  add        ecx, 0xbd8
0046b082  e8 e9 e2 fd ff                     call       0x449370
0046b087  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0046b08a  81 c1 f4 05 00 00                  add        ecx, 0x5f4
0046b090  e8 db e2 fd ff                     call       0x449370
0046b095  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0046b098  83 c1 10                           add        ecx, 0x10
0046b09b  e8 d0 e2 fd ff                     call       0x449370
0046b0a0  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0046b0a3  e8 48 4d fb ff                     call       0x41fdf0
0046b0a8  90                                 nop        
0046b0a9  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0046b0ac  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0046b0b3  59                                 pop        ecx
0046b0b4  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046b0b7  33 cd                              xor        ecx, ebp
0046b0b9  e8 30 79 0d 00                     call       0x5429ee
0046b0be  8b e5                              mov        esp, ebp
0046b0c0  5d                                 pop        ebp
0046b0c1  c3                                 ret        
