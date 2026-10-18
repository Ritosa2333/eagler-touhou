004ac080  55                                 push       ebp
004ac081  8b ec                              mov        ebp, esp
004ac083  51                                 push       ecx
004ac084  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004ac087  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004ac08a  50                                 push       eax
004ac08b  68 b0 c0 4a 00                     push       0x4ac0b0
004ac090  6a 60                              push       0x60
004ac092  e8 c9 62 f6 ff                     call       0x412360
004ac097  83 c4 0c                           add        esp, 0xc
004ac09a  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004ac09d  89 41 0c                           mov        dword ptr [ecx + 0xc], eax
004ac0a0  33 c0                              xor        eax, eax
004ac0a2  8b e5                              mov        esp, ebp
004ac0a4  5d                                 pop        ebp
004ac0a5  c3                                 ret        
