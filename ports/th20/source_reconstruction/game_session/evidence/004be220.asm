004be220  55                                 push       ebp
004be221  8b ec                              mov        ebp, esp
004be223  51                                 push       ecx
004be224  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004be227  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004be22a  83 e0 01                           and        eax, 1
004be22d  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004be230  8b 51 6c                           mov        edx, dword ptr [ecx + 0x6c]
004be233  83 e2 fe                           and        edx, 0xfffffffe
004be236  0b d0                              or         edx, eax
004be238  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004be23b  89 50 6c                           mov        dword ptr [eax + 0x6c], edx
004be23e  8b e5                              mov        esp, ebp
004be240  5d                                 pop        ebp
004be241  c2 04 00                           ret        4
