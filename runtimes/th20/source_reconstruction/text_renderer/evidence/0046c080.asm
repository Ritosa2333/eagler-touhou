0046c080  55                                 push       ebp
0046c081  8b ec                              mov        ebp, esp
0046c083  83 ec 14                           sub        esp, 0x14
0046c086  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0046c089  f3 0f 10 05 98 f7 56 00            movss      xmm0, dword ptr [0x56f798]
0046c091  0f 2f 05 18 88 5b 00               comiss     xmm0, dword ptr [0x5b8818]
0046c098  72 09                              jb         0x46c0a3
0046c09a  c7 45 f8 e4 f5 56 00               mov        dword ptr [ebp - 8], 0x56f5e4 ; string='ascii.anm'
0046c0a1  eb 21                              jmp        0x46c0c4
0046c0a3  f3 0f 10 05 9c f7 56 00            movss      xmm0, dword ptr [0x56f79c]
0046c0ab  0f 2f 05 18 88 5b 00               comiss     xmm0, dword ptr [0x5b8818]
0046c0b2  72 09                              jb         0x46c0bd
0046c0b4  c7 45 f8 f0 f5 56 00               mov        dword ptr [ebp - 8], 0x56f5f0 ; string='ascii_960.anm'
0046c0bb  eb 07                              jmp        0x46c0c4
0046c0bd  c7 45 f8 00 f6 56 00               mov        dword ptr [ebp - 8], 0x56f600 ; string='ascii1280.anm'
0046c0c4  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0046c0c7  50                                 push       eax
0046c0c8  6a 02                              push       2
0046c0ca  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0046c0d0  e8 5b 2a fe ff                     call       0x44eb30
0046c0d5  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
0046c0d8  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046c0db  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
0046c0de  89 91 44 a2 01 00                  mov        dword ptr [ecx + 0x1a244], edx
0046c0e4  83 7d f4 00                        cmp        dword ptr [ebp - 0xc], 0
0046c0e8  75 1a                              jne        0x46c104
0046c0ea  68 10 f6 56 00                     push       0x56f610
0046c0ef  68 78 06 5c 00                     push       0x5c0678
0046c0f4  e8 57 80 fe ff                     call       0x454150
0046c0f9  83 c4 08                           add        esp, 8
0046c0fc  83 c8 ff                           or         eax, 0xffffffff
0046c0ff  e9 fb 00 00 00                     jmp        0x46c1ff
0046c104  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046c107  50                                 push       eax
0046c108  68 00 03 47 00                     push       0x470300
0046c10d  6a 08                              push       8
0046c10f  e8 fc 61 fa ff                     call       0x412310
0046c114  83 c4 0c                           add        esp, 0xc
0046c117  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046c11a  89 41 08                           mov        dword ptr [ecx + 8], eax
0046c11d  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046c120  52                                 push       edx
0046c121  68 30 05 47 00                     push       0x470530
0046c126  6a 66                              push       0x66
0046c128  e8 83 62 fa ff                     call       0x4123b0
0046c12d  83 c4 0c                           add        esp, 0xc
0046c130  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046c133  89 41 0c                           mov        dword ptr [ecx + 0xc], eax
0046c136  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046c139  52                                 push       edx
0046c13a  68 a0 04 47 00                     push       0x4704a0
0046c13f  6a 3d                              push       0x3d
0046c141  e8 6a 62 fa ff                     call       0x4123b0
0046c146  83 c4 0c                           add        esp, 0xc
0046c149  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046c14c  89 81 50 a2 01 00                  mov        dword ptr [ecx + 0x1a250], eax
0046c152  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046c155  52                                 push       edx
0046c156  68 b0 04 47 00                     push       0x4704b0
0046c15b  6a 54                              push       0x54
0046c15d  e8 4e 62 fa ff                     call       0x4123b0
0046c162  83 c4 0c                           add        esp, 0xc
0046c165  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046c168  89 81 54 a2 01 00                  mov        dword ptr [ecx + 0x1a254], eax
0046c16e  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046c171  52                                 push       edx
0046c172  68 f0 04 47 00                     push       0x4704f0
0046c177  6a 4b                              push       0x4b
0046c179  e8 32 62 fa ff                     call       0x4123b0
0046c17e  83 c4 0c                           add        esp, 0xc
0046c181  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046c184  89 81 58 a2 01 00                  mov        dword ptr [ecx + 0x1a258], eax
0046c18a  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046c18d  52                                 push       edx
0046c18e  68 10 05 47 00                     push       0x470510
0046c193  6a 55                              push       0x55
0046c195  e8 16 62 fa ff                     call       0x4123b0
0046c19a  83 c4 0c                           add        esp, 0xc
0046c19d  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046c1a0  89 81 5c a2 01 00                  mov        dword ptr [ecx + 0x1a25c], eax
0046c1a6  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046c1a9  8b 82 44 a2 01 00                  mov        eax, dword ptr [edx + 0x1a244]
0046c1af  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0046c1b2  6a 00                              push       0
0046c1b4  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046c1b7  83 c1 10                           add        ecx, 0x10
0046c1ba  51                                 push       ecx
0046c1bb  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046c1be  e8 1d 47 00 00                     call       0x4708e0
0046c1c3  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046c1c6  8b 82 44 a2 01 00                  mov        eax, dword ptr [edx + 0x1a244]
0046c1cc  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
0046c1cf  6a 62                              push       0x62
0046c1d1  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046c1d4  81 c1 f4 05 00 00                  add        ecx, 0x5f4
0046c1da  51                                 push       ecx
0046c1db  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
0046c1de  e8 fd 46 00 00                     call       0x4708e0
0046c1e3  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046c1e6  c7 82 00 a2 01 00 00 08 00 00      mov        dword ptr [edx + 0x1a200], 0x800
0046c1f0  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046c1f3  c7 80 04 a2 01 00 00 08 00 00      mov        dword ptr [eax + 0x1a204], 0x800
0046c1fd  33 c0                              xor        eax, eax
0046c1ff  8b e5                              mov        esp, ebp
0046c201  5d                                 pop        ebp
0046c202  c3                                 ret        
