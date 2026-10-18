00463f20  55                                 push       ebp
00463f21  8b ec                              mov        ebp, esp
00463f23  83 ec 14                           sub        esp, 0x14
00463f26  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
00463f2b  33 c5                              xor        eax, ebp
00463f2d  89 45 fc                           mov        dword ptr [ebp - 4], eax
00463f30  89 4d ec                           mov        dword ptr [ebp - 0x14], ecx
00463f33  6a 14                              push       0x14
00463f35  b9 40 02 5c 00                     mov        ecx, 0x5c0240
00463f3a  e8 d1 83 fa ff                     call       0x40c310
00463f3f  50                                 push       eax
00463f40  8d 4d f8                           lea        ecx, [ebp - 8]
00463f43  e8 e8 77 fa ff                     call       0x40b730
00463f48  b8 01 00 00 00                     mov        eax, 1
00463f4d  6b c8 00                           imul       ecx, eax, 0
00463f50  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
00463f53  8d 44 0a 58                        lea        eax, [edx + ecx + 0x58]
00463f57  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
00463f5a  c6 45 f7 00                        mov        byte ptr [ebp - 9], 0
00463f5e  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
00463f61  81 c1 d0 01 00 00                  add        ecx, 0x1d0
00463f67  39 4d f0                           cmp        dword ptr [ebp - 0x10], ecx
00463f6a  7d 1a                              jge        0x463f86
00463f6c  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00463f6f  0f b6 02                           movzx      eax, byte ptr [edx]
00463f72  0f b6 4d f7                        movzx      ecx, byte ptr [ebp - 9]
00463f76  03 c8                              add        ecx, eax
00463f78  88 4d f7                           mov        byte ptr [ebp - 9], cl
00463f7b  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00463f7e  83 c2 01                           add        edx, 1
00463f81  89 55 f0                           mov        dword ptr [ebp - 0x10], edx
00463f84  eb d8                              jmp        0x463f5e
00463f86  8a 45 f7                           mov        al, byte ptr [ebp - 9]
00463f89  88 45 f6                           mov        byte ptr [ebp - 0xa], al
00463f8c  8d 4d f8                           lea        ecx, [ebp - 8]
00463f8f  e8 6c 79 fa ff                     call       0x40b900
00463f94  0f b6 45 f6                        movzx      eax, byte ptr [ebp - 0xa]
00463f98  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00463f9b  33 cd                              xor        ecx, ebp
00463f9d  e8 4c ea 0d 00                     call       0x5429ee
00463fa2  8b e5                              mov        esp, ebp
00463fa4  5d                                 pop        ebp
00463fa5  c3                                 ret        
