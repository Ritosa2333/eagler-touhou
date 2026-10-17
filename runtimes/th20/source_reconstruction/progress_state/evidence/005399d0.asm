005399d0  55                                 push       ebp
005399d1  8b ec                              mov        ebp, esp
005399d3  b8 0c 00 00 00                     mov        eax, 0xc
005399d8  c1 e0 0d                           shl        eax, 0xd
005399db  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
005399de  89 88 40 8b 5c 00                  mov        dword ptr [eax + 0x5c8b40], ecx
005399e4  6b 55 08 0c                        imul       edx, dword ptr [ebp + 8], 0xc
005399e8  c7 82 38 8b 5c 00 00 20 00 00      mov        dword ptr [edx + 0x5c8b38], 0x2000
005399f2  6b 45 08 0c                        imul       eax, dword ptr [ebp + 8], 0xc
005399f6  c7 80 40 8b 5c 00 00 00 00 00      mov        dword ptr [eax + 0x5c8b40], 0
00539a00  6b 4d 08 0c                        imul       ecx, dword ptr [ebp + 8], 0xc
00539a04  c7 81 3c 8b 5c 00 00 00 00 00      mov        dword ptr [ecx + 0x5c8b3c], 0
00539a0e  5d                                 pop        ebp
00539a0f  c3                                 ret        
