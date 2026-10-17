0051b0c0  55                                 push       ebp
0051b0c1  8b ec                              mov        ebp, esp
0051b0c3  6a ff                              push       -1
0051b0c5  68 cd 79 56 00                     push       0x5679cd
0051b0ca  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0051b0d0  50                                 push       eax
0051b0d1  83 ec 10                           sub        esp, 0x10
0051b0d4  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0051b0d9  33 c5                              xor        eax, ebp
0051b0db  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0051b0de  50                                 push       eax
0051b0df  8d 45 f4                           lea        eax, [ebp - 0xc]
0051b0e2  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0051b0e8  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
0051b0eb  6a 14                              push       0x14
0051b0ed  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0051b0f2  e8 19 12 ef ff                     call       0x40c310
0051b0f7  50                                 push       eax
0051b0f8  8d 4d ec                           lea        ecx, [ebp - 0x14]
0051b0fb  e8 30 06 ef ff                     call       0x40b730
0051b100  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0051b107  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0051b10a  e8 d1 8f f4 ff                     call       0x4640e0
0051b10f  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
0051b112  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0051b115  50                                 push       eax
0051b116  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0051b119  e8 42 00 00 00                     call       0x51b160
0051b11e  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0051b121  e8 1a 3a fa ff                     call       0x4beb40
0051b126  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0051b12d  8d 4d ec                           lea        ecx, [ebp - 0x14]
0051b130  e8 cb 07 ef ff                     call       0x40b900
0051b135  90                                 nop        
0051b136  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0051b139  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0051b140  59                                 pop        ecx
0051b141  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0051b144  33 cd                              xor        ecx, ebp
0051b146  e8 a3 78 02 00                     call       0x5429ee
0051b14b  8b e5                              mov        esp, ebp
0051b14d  5d                                 pop        ebp
0051b14e  c2 04 00                           ret        4
