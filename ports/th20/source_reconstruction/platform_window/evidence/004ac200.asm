004ac200  55                                 push       ebp
004ac201  8b ec                              mov        ebp, esp
004ac203  83 ec 10                           sub        esp, 0x10
004ac206  8d 45 f8                           lea        eax, [ebp - 8]
004ac209  50                                 push       eax
004ac20a  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004ac20d  e8 0e 54 fa ff                     call       0x451620
004ac212  50                                 push       eax
004ac213  8d 4d f0                           lea        ecx, [ebp - 0x10]
004ac216  51                                 push       ecx
004ac217  e8 04 f9 ff ff                     call       0x4abb20
004ac21c  83 c4 08                           add        esp, 8
004ac21f  8b c8                              mov        ecx, eax
004ac221  e8 6a fe f9 ff                     call       0x44c090
004ac226  8b e5                              mov        esp, ebp
004ac228  5d                                 pop        ebp
004ac229  c3                                 ret        
