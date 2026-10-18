004bdd60  55                                 push       ebp
004bdd61  8b ec                              mov        ebp, esp
004bdd63  51                                 push       ecx
004bdd64  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004bdd67  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004bdd6a  83 e0 01                           and        eax, 1
004bdd6d  d1 e0                              shl        eax, 1
004bdd6f  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004bdd72  8b 51 6c                           mov        edx, dword ptr [ecx + 0x6c]
004bdd75  83 e2 fd                           and        edx, 0xfffffffd
004bdd78  0b d0                              or         edx, eax
004bdd7a  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004bdd7d  89 50 6c                           mov        dword ptr [eax + 0x6c], edx
004bdd80  8b e5                              mov        esp, ebp
004bdd82  5d                                 pop        ebp
004bdd83  c2 04 00                           ret        4
