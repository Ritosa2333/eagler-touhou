00451520  55                                 push       ebp
00451521  8b ec                              mov        ebp, esp
00451523  83 ec 0c                           sub        esp, 0xc
00451526  56                                 push       esi
00451527  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0045152a  8d 45 f4                           lea        eax, [ebp - 0xc]
0045152d  50                                 push       eax
0045152e  e8 ad c8 ff ff                     call       0x44dde0
00451533  83 c4 04                           add        esp, 4
00451536  8b 08                              mov        ecx, dword ptr [eax]
00451538  8b 50 04                           mov        edx, dword ptr [eax + 4]
0045153b  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0045153e  8b 75 fc                           mov        esi, dword ptr [ebp - 4]
00451541  89 8c c6 90 00 00 00               mov        dword ptr [esi + eax*8 + 0x90], ecx
00451548  89 94 c6 94 00 00 00               mov        dword ptr [esi + eax*8 + 0x94], edx
0045154f  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00451552  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00451555  8b 84 ca 90 00 00 00               mov        eax, dword ptr [edx + ecx*8 + 0x90]
0045155c  8b 8c ca 94 00 00 00               mov        ecx, dword ptr [edx + ecx*8 + 0x94]
00451563  8b 55 08                           mov        edx, dword ptr [ebp + 8]
00451566  8b 75 fc                           mov        esi, dword ptr [ebp - 4]
00451569  89 44 d6 50                        mov        dword ptr [esi + edx*8 + 0x50], eax
0045156d  89 4c d6 54                        mov        dword ptr [esi + edx*8 + 0x54], ecx
00451571  5e                                 pop        esi
00451572  8b e5                              mov        esp, ebp
00451574  5d                                 pop        ebp
00451575  c2 04 00                           ret        4
