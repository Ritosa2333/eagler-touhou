004dc0f0  55                                 push       ebp
004dc0f1  8b ec                              mov        ebp, esp
004dc0f3  51                                 push       ecx
004dc0f4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004dc0f7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dc0fa  c7 80 08 0b 00 00 fe ff ff ff      mov        dword ptr [eax + 0xb08], 0xfffffffe
004dc104  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dc107  c7 81 0c 0b 00 00 00 00 00 00      mov        dword ptr [ecx + 0xb0c], 0
004dc111  8b e5                              mov        esp, ebp
004dc113  5d                                 pop        ebp
004dc114  c3                                 ret        
