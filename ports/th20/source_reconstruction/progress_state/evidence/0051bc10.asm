0051bc10  55                                 push       ebp
0051bc11  8b ec                              mov        ebp, esp
0051bc13  6a ff                              push       -1
0051bc15  68 cd 79 56 00                     push       0x5679cd
0051bc1a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0051bc20  50                                 push       eax
0051bc21  83 ec 10                           sub        esp, 0x10
0051bc24  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0051bc29  33 c5                              xor        eax, ebp
0051bc2b  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0051bc2e  50                                 push       eax
0051bc2f  8d 45 f4                           lea        eax, [ebp - 0xc]
0051bc32  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0051bc38  89 4d e4                           mov        dword ptr [ebp - 0x1c], ecx
0051bc3b  6a 14                              push       0x14
0051bc3d  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0051bc42  e8 c9 06 ef ff                     call       0x40c310
0051bc47  50                                 push       eax
0051bc48  8d 4d ec                           lea        ecx, [ebp - 0x14]
0051bc4b  e8 e0 fa ee ff                     call       0x40b730
0051bc50  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0051bc57  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0051bc5a  e8 51 83 f4 ff                     call       0x463fb0
0051bc5f  90                                 nop        
0051bc60  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0051bc63  50                                 push       eax
0051bc64  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0051bc67  51                                 push       ecx
0051bc68  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0051bc6b  e8 f0 17 fa ff                     call       0x4bd460
0051bc70  83 b8 f0 76 00 00 00               cmp        dword ptr [eax + 0x76f0], 0
0051bc77  74 06                              je         0x51bc7f
0051bc79  c6 45 eb 01                        mov        byte ptr [ebp - 0x15], 1
0051bc7d  eb 04                              jmp        0x51bc83
0051bc7f  c6 45 eb 00                        mov        byte ptr [ebp - 0x15], 0
0051bc83  8a 55 eb                           mov        dl, byte ptr [ebp - 0x15]
0051bc86  88 55 ea                           mov        byte ptr [ebp - 0x16], dl
0051bc89  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0051bc90  8d 4d ec                           lea        ecx, [ebp - 0x14]
0051bc93  e8 68 fc ee ff                     call       0x40b900
0051bc98  0f b6 45 ea                        movzx      eax, byte ptr [ebp - 0x16]
0051bc9c  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0051bc9f  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0051bca6  59                                 pop        ecx
0051bca7  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0051bcaa  33 cd                              xor        ecx, ebp
0051bcac  e8 3d 6d 02 00                     call       0x5429ee
0051bcb1  8b e5                              mov        esp, ebp
0051bcb3  5d                                 pop        ebp
0051bcb4  c2 08 00                           ret        8
