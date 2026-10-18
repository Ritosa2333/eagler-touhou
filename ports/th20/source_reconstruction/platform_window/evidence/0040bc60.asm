0040bc60  55                                 push       ebp
0040bc61  8b ec                              mov        ebp, esp
0040bc63  6a ff                              push       -1
0040bc65  68 2d 76 56 00                     push       0x56762d
0040bc6a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0040bc70  50                                 push       eax
0040bc71  83 ec 0c                           sub        esp, 0xc
0040bc74  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0040bc79  33 c5                              xor        eax, ebp
0040bc7b  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0040bc7e  50                                 push       eax
0040bc7f  8d 45 f4                           lea        eax, [ebp - 0xc]
0040bc82  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0040bc88  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
0040bc8b  6a 06                              push       6
0040bc8d  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0040bc92  e8 79 06 00 00                     call       0x40c310
0040bc97  50                                 push       eax
0040bc98  8d 4d ec                           lea        ecx, [ebp - 0x14]
0040bc9b  e8 90 fa ff ff                     call       0x40b730
0040bca0  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0040bca7  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0040bcaa  e8 21 07 00 00                     call       0x40c3d0
0040bcaf  0f b6 c0                           movzx      eax, al
0040bcb2  85 c0                              test       eax, eax
0040bcb4  74 09                              je         0x40bcbf
0040bcb6  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0040bcb9  e8 a2 04 00 00                     call       0x40c160
0040bcbe  90                                 nop        
0040bcbf  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0040bcc6  8d 4d ec                           lea        ecx, [ebp - 0x14]
0040bcc9  e8 32 fc ff ff                     call       0x40b900
0040bcce  90                                 nop        
0040bccf  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0040bcd2  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0040bcd9  59                                 pop        ecx
0040bcda  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0040bcdd  33 cd                              xor        ecx, ebp
0040bcdf  e8 0a 6d 13 00                     call       0x5429ee
0040bce4  8b e5                              mov        esp, ebp
0040bce6  5d                                 pop        ebp
0040bce7  c3                                 ret        
