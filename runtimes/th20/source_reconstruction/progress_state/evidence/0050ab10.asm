0050ab10  55                                 push       ebp
0050ab11  8b ec                              mov        ebp, esp
0050ab13  6a ff                              push       -1
0050ab15  68 5d a8 56 00                     push       0x56a85d
0050ab1a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0050ab20  50                                 push       eax
0050ab21  83 ec 28                           sub        esp, 0x28
0050ab24  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0050ab29  33 c5                              xor        eax, ebp
0050ab2b  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0050ab2e  50                                 push       eax
0050ab2f  8d 45 f4                           lea        eax, [ebp - 0xc]
0050ab32  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0050ab38  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
0050ab3b  6a 06                              push       6
0050ab3d  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0050ab42  e8 c9 17 f0 ff                     call       0x40c310
0050ab47  50                                 push       eax
0050ab48  8d 4d ec                           lea        ecx, [ebp - 0x14]
0050ab4b  e8 e0 0b f0 ff                     call       0x40b730
0050ab50  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0050ab57  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0050ab5a  e8 01 11 f0 ff                     call       0x40bc60
0050ab5f  6a 00                              push       0
0050ab61  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0050ab64  83 c1 0c                           add        ecx, 0xc
0050ab67  e8 34 0f f0 ff                     call       0x40baa0
0050ab6c  90                                 nop        
0050ab6d  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
0050ab70  50                                 push       eax
0050ab71  8d 4d 0c                           lea        ecx, [ebp + 0xc]
0050ab74  51                                 push       ecx
0050ab75  8d 55 08                           lea        edx, [ebp + 8]
0050ab78  52                                 push       edx
0050ab79  8d 4d cc                           lea        ecx, [ebp - 0x34]
0050ab7c  e8 ef f9 f5 ff                     call       0x46a570
0050ab81  50                                 push       eax
0050ab82  8d 4d d8                           lea        ecx, [ebp - 0x28]
0050ab85  e8 d6 fe ff ff                     call       0x50aa60
0050ab8a  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
0050ab8d  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
0050ab90  50                                 push       eax
0050ab91  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0050ab94  e8 27 0f f0 ff                     call       0x40bac0
0050ab99  8d 4d d8                           lea        ecx, [ebp - 0x28]
0050ab9c  e8 2f 0e f0 ff                     call       0x40b9d0
0050aba1  90                                 nop        
0050aba2  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0050aba9  8d 4d ec                           lea        ecx, [ebp - 0x14]
0050abac  e8 4f 0d f0 ff                     call       0x40b900
0050abb1  90                                 nop        
0050abb2  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0050abb5  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0050abbc  59                                 pop        ecx
0050abbd  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050abc0  33 cd                              xor        ecx, ebp
0050abc2  e8 27 7e 03 00                     call       0x5429ee
0050abc7  8b e5                              mov        esp, ebp
0050abc9  5d                                 pop        ebp
0050abca  c2 0c 00                           ret        0xc
