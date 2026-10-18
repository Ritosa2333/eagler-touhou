00463fe0  55                                 push       ebp
00463fe1  8b ec                              mov        ebp, esp
00463fe3  6a ff                              push       -1
00463fe5  68 8d 80 56 00                     push       0x56808d
00463fea  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
00463ff0  50                                 push       eax
00463ff1  83 ec 10                           sub        esp, 0x10
00463ff4  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
00463ff9  33 c5                              xor        eax, ebp
00463ffb  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
00463ffe  56                                 push       esi
00463fff  50                                 push       eax
00464000  8d 45 f4                           lea        eax, [ebp - 0xc]
00464003  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
00464009  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
0046400c  6a 14                              push       0x14
0046400e  b9 40 02 5c 00                     mov        ecx, 0x5c0240
00464013  e8 f8 82 fa ff                     call       0x40c310
00464018  50                                 push       eax
00464019  8d 4d ec                           lea        ecx, [ebp - 0x14]
0046401c  e8 0f 77 fa ff                     call       0x40b730
00464021  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
00464028  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
0046402b  0f b6 b0 d1 01 00 00               movzx      esi, byte ptr [eax + 0x1d1]
00464032  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
00464035  e8 e6 fe ff ff                     call       0x463f20
0046403a  0f b6 c8                           movzx      ecx, al
0046403d  3b f1                              cmp        esi, ecx
0046403f  74 0b                              je         0x46404c
00464041  b9 58 67 5b 00                     mov        ecx, 0x5b6758
00464046  e8 e5 04 00 00                     call       0x464530
0046404b  90                                 nop        
0046404c  c7 45 e4 00 00 00 00               mov        dword ptr [ebp - 0x1c], 0
00464053  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0046405a  8d 4d ec                           lea        ecx, [ebp - 0x14]
0046405d  e8 9e 78 fa ff                     call       0x40b900
00464062  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
00464065  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
00464068  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0046406f  59                                 pop        ecx
00464070  5e                                 pop        esi
00464071  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00464074  33 cd                              xor        ecx, ebp
00464076  e8 73 e9 0d 00                     call       0x5429ee
0046407b  8b e5                              mov        esp, ebp
0046407d  5d                                 pop        ebp
0046407e  c3                                 ret        
