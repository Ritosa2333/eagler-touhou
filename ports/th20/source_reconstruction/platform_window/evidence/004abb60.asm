004abb60  55                                 push       ebp
004abb61  8b ec                              mov        ebp, esp
004abb63  51                                 push       ecx
004abb64  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004abb67  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004abb6a  50                                 push       eax
004abb6b  e8 c0 06 00 00                     call       0x4ac230
004abb70  83 c4 04                           add        esp, 4
004abb73  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004abb76  8b e5                              mov        esp, ebp
004abb78  5d                                 pop        ebp
004abb79  c3                                 ret        
