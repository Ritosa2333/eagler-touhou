004ac170  55                                 push       ebp
004ac171  8b ec                              mov        ebp, esp
004ac173  83 ec 10                           sub        esp, 0x10
004ac176  e8 85 57 09 00                     call       0x541900
004ac17b  89 45 f8                           mov        dword ptr [ebp - 8], eax
004ac17e  89 55 fc                           mov        dword ptr [ebp - 4], edx
004ac181  8d 45 f8                           lea        eax, [ebp - 8]
004ac184  50                                 push       eax
004ac185  8d 4d f0                           lea        ecx, [ebp - 0x10]
004ac188  e8 13 ac f9 ff                     call       0x446da0
004ac18d  50                                 push       eax
004ac18e  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004ac191  e8 8a bb f9 ff                     call       0x447d20
004ac196  90                                 nop        
004ac197  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004ac19a  8b e5                              mov        esp, ebp
004ac19c  5d                                 pop        ebp
004ac19d  c3                                 ret        
