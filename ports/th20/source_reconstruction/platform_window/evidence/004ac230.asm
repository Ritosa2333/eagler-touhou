004ac230  55                                 push       ebp
004ac231  8b ec                              mov        ebp, esp
004ac233  83 ec 08                           sub        esp, 8
004ac236  e8 25 00 00 00                     call       0x4ac260
004ac23b  89 45 f8                           mov        dword ptr [ebp - 8], eax
004ac23e  89 55 fc                           mov        dword ptr [ebp - 4], edx
004ac241  8d 45 f8                           lea        eax, [ebp - 8]
004ac244  50                                 push       eax
004ac245  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004ac248  e8 53 ab f9 ff                     call       0x446da0
004ac24d  90                                 nop        
004ac24e  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004ac251  8b e5                              mov        esp, ebp
004ac253  5d                                 pop        ebp
004ac254  c3                                 ret        
