00422f20  55                                 push       ebp
00422f21  8b ec                              mov        ebp, esp
00422f23  6a ff                              push       -1
00422f25  68 00 76 56 00                     push       0x567600
00422f2a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
00422f30  50                                 push       eax
00422f31  51                                 push       ecx
00422f32  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
00422f37  33 c5                              xor        eax, ebp
00422f39  50                                 push       eax
00422f3a  8d 45 f4                           lea        eax, [ebp - 0xc]
00422f3d  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
00422f43  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
00422f46  68 50 30 42 00                     push       0x423050
00422f4b  6a 02                              push       2
00422f4d  68 f0 00 00 00                     push       0xf0
00422f52  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00422f55  50                                 push       eax
00422f56  e8 c5 8c fe ff                     call       0x40bc20
00422f5b  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00422f5e  c7 81 e0 01 00 00 01 00 00 00      mov        dword ptr [ecx + 0x1e0], 1
00422f68  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00422f6b  c7 82 e4 01 00 00 00 00 00 00      mov        dword ptr [edx + 0x1e4], 0
00422f75  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00422f78  c7 80 e8 01 00 00 00 00 00 00      mov        dword ptr [eax + 0x1e8], 0
00422f82  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00422f85  c7 81 ec 01 00 00 00 00 00 00      mov        dword ptr [ecx + 0x1ec], 0
00422f8f  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00422f92  c7 82 f0 01 00 00 00 00 00 00      mov        dword ptr [edx + 0x1f0], 0
00422f9c  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00422f9f  c7 80 f4 01 00 00 00 00 00 00      mov        dword ptr [eax + 0x1f4], 0
00422fa9  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00422fac  c7 81 f8 01 00 00 00 00 00 00      mov        dword ptr [ecx + 0x1f8], 0
00422fb6  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00422fb9  c7 82 fc 01 00 00 00 00 00 00      mov        dword ptr [edx + 0x1fc], 0
00422fc3  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00422fc6  c7 80 00 02 00 00 ff ff ff ff      mov        dword ptr [eax + 0x200], 0xffffffff
00422fd0  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00422fd3  c7 81 04 02 00 00 ff ff ff ff      mov        dword ptr [ecx + 0x204], 0xffffffff
00422fdd  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00422fe0  c7 82 08 02 00 00 00 00 00 00      mov        dword ptr [edx + 0x208], 0
00422fea  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00422fed  c7 80 0c 02 00 00 00 00 00 00      mov        dword ptr [eax + 0x20c], 0
00422ff7  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00422ffa  c7 81 10 02 00 00 00 00 00 00      mov        dword ptr [ecx + 0x210], 0
00423004  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00423007  c7 82 14 02 00 00 00 00 00 00      mov        dword ptr [edx + 0x214], 0
00423011  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00423014  c7 80 18 02 00 00 00 00 00 00      mov        dword ptr [eax + 0x218], 0
0042301e  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00423021  c7 81 1c 02 00 00 00 00 00 00      mov        dword ptr [ecx + 0x21c], 0
0042302b  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0042302e  c7 82 20 02 00 00 ff ff ff ff      mov        dword ptr [edx + 0x220], 0xffffffff
00423038  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0042303b  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0042303e  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
00423045  59                                 pop        ecx
00423046  8b e5                              mov        esp, ebp
00423048  5d                                 pop        ebp
00423049  c3                                 ret        
