004abc90  55                                 push       ebp
004abc91  8b ec                              mov        ebp, esp
004abc93  6a ff                              push       -1
004abc95  68 00 76 56 00                     push       0x567600
004abc9a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
004abca0  50                                 push       eax
004abca1  51                                 push       ecx
004abca2  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004abca7  33 c5                              xor        eax, ebp
004abca9  50                                 push       eax
004abcaa  8d 45 f4                           lea        eax, [ebp - 0xc]
004abcad  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
004abcb3  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
004abcb6  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004abcb9  c7 00 a4 05 57 00                  mov        dword ptr [eax], 0x5705a4
004abcbf  68 c4 05 57 00                     push       0x5705c4 ; string='shutdown FPSInf'
004abcc4  e8 e7 09 f6 ff                     call       0x40c6b0
004abcc9  83 c4 04                           add        esp, 4
004abccc  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004abccf  8b 51 0c                           mov        edx, dword ptr [ecx + 0xc]
004abcd2  52                                 push       edx
004abcd3  8b 0d d8 66 5b 00                  mov        ecx, dword ptr [0x5b66d8]
004abcd9  e8 d2 67 f6 ff                     call       0x4124b0
004abcde  c7 05 00 4a 5c 00 00 00 00 00      mov        dword ptr [0x5c4a00], 0
004abce8  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004abceb  e8 00 41 f7 ff                     call       0x41fdf0
004abcf0  90                                 nop        
004abcf1  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004abcf4  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
004abcfb  59                                 pop        ecx
004abcfc  8b e5                              mov        esp, ebp
004abcfe  5d                                 pop        ebp
004abcff  c3                                 ret        
