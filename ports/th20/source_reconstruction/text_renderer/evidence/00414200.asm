00414200  55                                 push       ebp
00414201  8b ec                              mov        ebp, esp
00414203  51                                 push       ecx
00414204  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00414207  68 24 01 00 00                     push       0x124
0041420c  6a 00                              push       0
0041420e  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00414211  50                                 push       eax
00414212  e8 49 05 13 00                     call       0x544760
00414217  83 c4 0c                           add        esp, 0xc
0041421a  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041421d  c7 81 00 01 00 00 ff ff ff ff      mov        dword ptr [ecx + 0x100], 0xffffffff
00414227  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0041422a  c7 82 04 01 00 00 00 00 00 00      mov        dword ptr [edx + 0x104], 0
00414234  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00414237  c7 80 08 01 00 00 00 00 00 00      mov        dword ptr [eax + 0x108], 0
00414241  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00414244  c7 81 14 01 00 00 00 00 00 00      mov        dword ptr [ecx + 0x114], 0
0041424e  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00414251  c7 82 1c 01 00 00 00 00 00 00      mov        dword ptr [edx + 0x11c], 0
0041425b  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041425e  c7 80 18 01 00 00 00 00 00 00      mov        dword ptr [eax + 0x118], 0
00414268  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041426b  c7 81 20 01 00 00 00 00 00 00      mov        dword ptr [ecx + 0x120], 0
00414275  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00414278  8b e5                              mov        esp, ebp
0041427a  5d                                 pop        ebp
0041427b  c3                                 ret        
