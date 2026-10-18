0050fb60  55                                 push       ebp
0050fb61  8b ec                              mov        ebp, esp
0050fb63  6a ff                              push       -1
0050fb65  68 2d 76 56 00                     push       0x56762d
0050fb6a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0050fb70  50                                 push       eax
0050fb71  83 ec 0c                           sub        esp, 0xc
0050fb74  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0050fb79  33 c5                              xor        eax, ebp
0050fb7b  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0050fb7e  50                                 push       eax
0050fb7f  8d 45 f4                           lea        eax, [ebp - 0xc]
0050fb82  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0050fb88  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
0050fb8b  6a 14                              push       0x14
0050fb8d  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0050fb92  e8 79 c7 ef ff                     call       0x40c310
0050fb97  50                                 push       eax
0050fb98  8d 4d ec                           lea        ecx, [ebp - 0x14]
0050fb9b  e8 90 bb ef ff                     call       0x40b730
0050fba0  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0050fba7  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
0050fbaa  05 40 21 09 00                     add        eax, 0x92140
0050fbaf  50                                 push       eax
0050fbb0  68 1c 37 57 00                     push       0x57371c ; string='scoreth20bak.dat'
0050fbb5  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0050fbb8  e8 f3 fa ff ff                     call       0x50f6b0
0050fbbd  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0050fbc0  51                                 push       ecx
0050fbc1  68 30 37 57 00                     push       0x573730 ; string='scoreth20.dat'
0050fbc6  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0050fbc9  e8 e2 fa ff ff                     call       0x50f6b0
0050fbce  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0050fbd1  e8 da f2 ff ff                     call       0x50eeb0
0050fbd6  68 58 37 57 00                     push       0x573758 ; string='Score.SaveThread end'
0050fbdb  e8 d0 ca ef ff                     call       0x40c6b0
0050fbe0  83 c4 04                           add        esp, 4
0050fbe3  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0050fbea  8d 4d ec                           lea        ecx, [ebp - 0x14]
0050fbed  e8 0e bd ef ff                     call       0x40b900
0050fbf2  90                                 nop        
0050fbf3  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0050fbf6  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0050fbfd  59                                 pop        ecx
0050fbfe  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050fc01  33 cd                              xor        ecx, ebp
0050fc03  e8 e6 2d 03 00                     call       0x5429ee
0050fc08  8b e5                              mov        esp, ebp
0050fc0a  5d                                 pop        ebp
0050fc0b  c2 04 00                           ret        4
