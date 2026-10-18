004ac270  55                                 push       ebp
004ac271  8b ec                              mov        ebp, esp
004ac273  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
004ac276  50                                 push       eax
004ac277  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004ac27a  51                                 push       ecx
004ac27b  e8 1c 51 0a 00                     call       0x55139c
004ac280  83 c4 08                           add        esp, 8
004ac283  5d                                 pop        ebp
004ac284  c3                                 ret        
