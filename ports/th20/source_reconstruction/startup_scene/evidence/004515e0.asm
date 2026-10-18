004515e0  55                                 push       ebp
004515e1  8b ec                              mov        ebp, esp
004515e3  83 ec 0c                           sub        esp, 0xc
004515e6  56                                 push       esi
004515e7  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004515ea  8d 45 f4                           lea        eax, [ebp - 0xc]
004515ed  50                                 push       eax
004515ee  e8 ed c7 ff ff                     call       0x44dde0
004515f3  83 c4 04                           add        esp, 4
004515f6  8b 08                              mov        ecx, dword ptr [eax]
004515f8  8b 50 04                           mov        edx, dword ptr [eax + 4]
004515fb  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004515fe  8b 75 fc                           mov        esi, dword ptr [ebp - 4]
00451601  89 8c c6 90 00 00 00               mov        dword ptr [esi + eax*8 + 0x90], ecx
00451608  89 94 c6 94 00 00 00               mov        dword ptr [esi + eax*8 + 0x94], edx
0045160f  5e                                 pop        esi
00451610  8b e5                              mov        esp, ebp
00451612  5d                                 pop        ebp
00451613  c2 04 00                           ret        4
