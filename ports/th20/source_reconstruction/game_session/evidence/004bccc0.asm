004bccc0  55                                 push       ebp
004bccc1  8b ec                              mov        ebp, esp
004bccc3  83 ec 08                           sub        esp, 8
004bccc6  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004bccc9  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004bcccc  05 88 00 00 00                     add        eax, 0x88
004bccd1  89 45 f8                           mov        dword ptr [ebp - 8], eax
004bccd4  6a 01                              push       1
004bccd6  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004bccd9  e8 12 00 00 00                     call       0x4bccf0
004bccde  90                                 nop        
004bccdf  8b e5                              mov        esp, ebp
004bcce1  5d                                 pop        ebp
004bcce2  c3                                 ret        
