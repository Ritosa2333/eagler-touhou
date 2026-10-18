0051b970  55                                 push       ebp
0051b971  8b ec                              mov        ebp, esp
0051b973  6a ff                              push       -1
0051b975  68 8d 80 56 00                     push       0x56808d
0051b97a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0051b980  50                                 push       eax
0051b981  83 ec 14                           sub        esp, 0x14
0051b984  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0051b989  33 c5                              xor        eax, ebp
0051b98b  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0051b98e  50                                 push       eax
0051b98f  8d 45 f4                           lea        eax, [ebp - 0xc]
0051b992  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0051b998  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
0051b99b  6a 14                              push       0x14
0051b99d  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0051b9a2  e8 69 09 ef ff                     call       0x40c310
0051b9a7  50                                 push       eax
0051b9a8  8d 4d ec                           lea        ecx, [ebp - 0x14]
0051b9ab  e8 80 fd ee ff                     call       0x40b730
0051b9b0  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0051b9b7  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0051b9ba  e8 f1 85 f4 ff                     call       0x463fb0
0051b9bf  90                                 nop        
0051b9c0  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0051b9c3  e8 18 87 f4 ff                     call       0x4640e0
0051b9c8  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
0051b9cb  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0051b9ce  50                                 push       eax
0051b9cf  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0051b9d2  e8 39 00 00 00                     call       0x51ba10
0051b9d7  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
0051b9da  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0051b9e1  8d 4d ec                           lea        ecx, [ebp - 0x14]
0051b9e4  e8 17 ff ee ff                     call       0x40b900
0051b9e9  8b 45 e0                           mov        eax, dword ptr [ebp - 0x20]
0051b9ec  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0051b9ef  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0051b9f6  59                                 pop        ecx
0051b9f7  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0051b9fa  33 cd                              xor        ecx, ebp
0051b9fc  e8 ed 6f 02 00                     call       0x5429ee
0051ba01  8b e5                              mov        esp, ebp
0051ba03  5d                                 pop        ebp
0051ba04  c2 04 00                           ret        4
