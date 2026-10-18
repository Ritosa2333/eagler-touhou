004640e0  55                                 push       ebp
004640e1  8b ec                              mov        ebp, esp
004640e3  51                                 push       ecx
004640e4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004640e7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004640ea  05 48 1f 09 00                     add        eax, 0x91f48
004640ef  8b e5                              mov        esp, ebp
004640f1  5d                                 pop        ebp
004640f2  c3                                 ret        
