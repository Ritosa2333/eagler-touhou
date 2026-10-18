0050eeb0  55                                 push       ebp
0050eeb1  8b ec                              mov        ebp, esp
0050eeb3  51                                 push       ecx
0050eeb4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0050eeb7  68 38 1f 09 00                     push       0x91f38
0050eebc  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050eebf  83 c0 10                           add        eax, 0x10
0050eec2  50                                 push       eax
0050eec3  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050eec6  81 c1 50 21 09 00                  add        ecx, 0x92150
0050eecc  51                                 push       ecx
0050eecd  e8 4e 4f 03 00                     call       0x543e20
0050eed2  83 c4 0c                           add        esp, 0xc
0050eed5  68 f8 01 00 00                     push       0x1f8
0050eeda  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050eedd  81 c2 48 1f 09 00                  add        edx, 0x91f48
0050eee3  52                                 push       edx
0050eee4  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050eee7  05 88 40 12 00                     add        eax, 0x124088
0050eeec  50                                 push       eax
0050eeed  e8 2e 4f 03 00                     call       0x543e20
0050eef2  83 c4 0c                           add        esp, 0xc
0050eef5  33 c0                              xor        eax, eax
0050eef7  8b e5                              mov        esp, ebp
0050eef9  5d                                 pop        ebp
0050eefa  c3                                 ret        
