00422e40  55                                 push       ebp
00422e41  8b ec                              mov        ebp, esp
00422e43  6a ff                              push       -1
00422e45  68 00 76 56 00                     push       0x567600
00422e4a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
00422e50  50                                 push       eax
00422e51  51                                 push       ecx
00422e52  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
00422e57  33 c5                              xor        eax, ebp
00422e59  50                                 push       eax
00422e5a  8d 45 f4                           lea        eax, [ebp - 0xc]
00422e5d  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
00422e63  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
00422e66  68 20 33 42 00                     push       0x423320
00422e6b  6a 02                              push       2
00422e6d  6a 30                              push       0x30
00422e6f  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00422e72  50                                 push       eax
00422e73  e8 a8 8d fe ff                     call       0x40bc20
00422e78  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00422e7b  33 d2                              xor        edx, edx
00422e7d  33 c0                              xor        eax, eax
00422e7f  89 51 60                           mov        dword ptr [ecx + 0x60], edx
00422e82  89 41 64                           mov        dword ptr [ecx + 0x64], eax
00422e85  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00422e88  c7 41 68 00 00 00 00               mov        dword ptr [ecx + 0x68], 0
00422e8f  33 d2                              xor        edx, edx
00422e91  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00422e94  83 c0 6c                           add        eax, 0x6c
00422e97  89 10                              mov        dword ptr [eax], edx
00422e99  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00422e9c  c7 41 70 00 00 00 00               mov        dword ptr [ecx + 0x70], 0
00422ea3  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00422ea6  c7 42 74 00 00 00 00               mov        dword ptr [edx + 0x74], 0
00422ead  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00422eb0  c7 40 78 00 00 00 00               mov        dword ptr [eax + 0x78], 0
00422eb7  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00422eba  c7 41 7c 00 00 00 00               mov        dword ptr [ecx + 0x7c], 0
00422ec1  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00422ec4  c7 82 80 00 00 00 00 00 00 00      mov        dword ptr [edx + 0x80], 0
00422ece  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00422ed1  81 c1 88 00 00 00                  add        ecx, 0x88
00422ed7  e8 44 00 00 00                     call       0x422f20
00422edc  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00422edf  0f 57 c0                           xorps      xmm0, xmm0
00422ee2  f2 0f 11 80 b0 02 00 00            movsd      qword ptr [eax + 0x2b0], xmm0
00422eea  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00422eed  c7 81 b8 02 00 00 00 00 00 00      mov        dword ptr [ecx + 0x2b8], 0
00422ef7  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00422efa  c7 82 bc 02 00 00 01 00 00 00      mov        dword ptr [edx + 0x2bc], 1
00422f04  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
00422f07  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
00422f0a  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
00422f11  59                                 pop        ecx
00422f12  8b e5                              mov        esp, ebp
00422f14  5d                                 pop        ebp
00422f15  c3                                 ret        
