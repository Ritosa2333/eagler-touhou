0050e540  55                                 push       ebp
0050e541  8b ec                              mov        ebp, esp
0050e543  6a ff                              push       -1
0050e545  68 00 76 56 00                     push       0x567600
0050e54a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0050e550  50                                 push       eax
0050e551  51                                 push       ecx
0050e552  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0050e557  33 c5                              xor        eax, ebp
0050e559  50                                 push       eax
0050e55a  8d 45 f4                           lea        eax, [ebp - 0xc]
0050e55d  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0050e563  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
0050e566  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0050e569  c7 00 00 00 00 00                  mov        dword ptr [eax], 0
0050e56f  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050e572  c7 41 04 00 00 00 00               mov        dword ptr [ecx + 4], 0
0050e579  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0050e57c  c7 42 08 00 00 00 00               mov        dword ptr [edx + 8], 0
0050e583  68 20 af 50 00                     push       0x50af20
0050e588  6a 12                              push       0x12
0050e58a  68 e8 7a 00 00                     push       0x7ae8
0050e58f  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0050e592  83 c0 10                           add        eax, 0x10
0050e595  50                                 push       eax
0050e596  e8 85 d6 ef ff                     call       0x40bc20
0050e59b  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050e59e  81 c1 60 a4 08 00                  add        ecx, 0x8a460
0050e5a4  e8 77 c9 ff ff                     call       0x50af20
0050e5a9  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050e5ac  81 c1 48 1f 09 00                  add        ecx, 0x91f48
0050e5b2  e8 29 01 00 00                     call       0x50e6e0
0050e5b7  90                                 nop        
0050e5b8  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0050e5bb  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0050e5be  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0050e5c5  59                                 pop        ecx
0050e5c6  8b e5                              mov        esp, ebp
0050e5c8  5d                                 pop        ebp
0050e5c9  c3                                 ret        
