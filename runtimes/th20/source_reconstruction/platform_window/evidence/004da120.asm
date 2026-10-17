004da120  55                                 push       ebp
004da121  8b ec                              mov        ebp, esp
004da123  83 ec 14                           sub        esp, 0x14
004da126  89 4d ec                           mov        dword ptr [ebp - 0x14], ecx
004da129  83 3d 28 00 5c 00 00               cmp        dword ptr [0x5c0028], 0
004da130  74 0c                              je         0x4da13e
004da132  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
004da138  e8 83 b4 f6 ff                     call       0x4455c0
004da13d  90                                 nop        
004da13e  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004da143  e8 e8 85 f3 ff                     call       0x412730
004da148  89 45 fc                           mov        dword ptr [ebp - 4], eax
004da14b  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004da14e  8b 08                              mov        ecx, dword ptr [eax]
004da150  8b 91 b0 00 00 00                  mov        edx, dword ptr [ecx + 0xb0]
004da156  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
004da159  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004da15c  83 c0 60                           add        eax, 0x60
004da15f  50                                 push       eax
004da160  6a 02                              push       2
004da162  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004da165  51                                 push       ecx
004da166  ff 55 f4                           call       dword ptr [ebp - 0xc]
004da169  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004da16e  e8 bd 85 f3 ff                     call       0x412730
004da173  89 45 f8                           mov        dword ptr [ebp - 8], eax
004da176  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004da179  8b 02                              mov        eax, dword ptr [edx]
004da17b  8b 88 b0 00 00 00                  mov        ecx, dword ptr [eax + 0xb0]
004da181  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
004da184  8b 55 08                           mov        edx, dword ptr [ebp + 8]
004da187  81 c2 a0 00 00 00                  add        edx, 0xa0
004da18d  52                                 push       edx
004da18e  6a 03                              push       3
004da190  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004da193  50                                 push       eax
004da194  ff 55 f0                           call       dword ptr [ebp - 0x10]
004da197  90                                 nop        
004da198  83 3d 28 00 5c 00 00               cmp        dword ptr [0x5c0028], 0
004da19f  74 3b                              je         0x4da1dc
004da1a1  b9 08 00 00 00                     mov        ecx, 8
004da1a6  6b d1 00                           imul       edx, ecx, 0
004da1a9  a1 28 00 5c 00                     mov        eax, dword ptr [0x5c0028]
004da1ae  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004da1b1  8b 94 11 2c 01 00 00               mov        edx, dword ptr [ecx + edx + 0x12c]
004da1b8  89 90 c8 00 00 00                  mov        dword ptr [eax + 0xc8], edx
004da1be  b8 08 00 00 00                     mov        eax, 8
004da1c3  6b c8 00                           imul       ecx, eax, 0
004da1c6  8b 15 28 00 5c 00                  mov        edx, dword ptr [0x5c0028]
004da1cc  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004da1cf  8b 8c 08 30 01 00 00               mov        ecx, dword ptr [eax + ecx + 0x130]
004da1d6  89 8a cc 00 00 00                  mov        dword ptr [edx + 0xcc], ecx
004da1dc  8b e5                              mov        esp, ebp
004da1de  5d                                 pop        ebp
004da1df  c2 04 00                           ret        4
