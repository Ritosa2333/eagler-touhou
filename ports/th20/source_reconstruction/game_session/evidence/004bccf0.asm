004bccf0  55                                 push       ebp
004bccf1  8b ec                              mov        ebp, esp
004bccf3  83 ec 0c                           sub        esp, 0xc
004bccf6  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004bccf9  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004bccfc  8b 88 e8 01 00 00                  mov        ecx, dword ptr [eax + 0x1e8]
004bcd02  03 4d 08                           add        ecx, dword ptr [ebp + 8]
004bcd05  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004bcd08  89 8a e8 01 00 00                  mov        dword ptr [edx + 0x1e8], ecx
004bcd0e  c7 45 f8 09 00 00 00               mov        dword ptr [ebp - 8], 9
004bcd15  c7 45 f4 00 00 00 00               mov        dword ptr [ebp - 0xc], 0
004bcd1c  8d 45 f8                           lea        eax, [ebp - 8]
004bcd1f  50                                 push       eax
004bcd20  8d 4d f4                           lea        ecx, [ebp - 0xc]
004bcd23  51                                 push       ecx
004bcd24  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004bcd27  81 c2 e8 01 00 00                  add        edx, 0x1e8
004bcd2d  52                                 push       edx
004bcd2e  e8 3d 44 fb ff                     call       0x471170
004bcd33  83 c4 0c                           add        esp, 0xc
004bcd36  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004bcd39  8b 10                              mov        edx, dword ptr [eax]
004bcd3b  89 91 e8 01 00 00                  mov        dword ptr [ecx + 0x1e8], edx
004bcd41  8b e5                              mov        esp, ebp
004bcd43  5d                                 pop        ebp
004bcd44  c2 04 00                           ret        4
