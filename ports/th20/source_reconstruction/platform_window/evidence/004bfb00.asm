004bfb00  55                                 push       ebp
004bfb01  8b ec                              mov        ebp, esp
004bfb03  51                                 push       ecx
004bfb04  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004bfb07  6b 45 08 18                        imul       eax, dword ptr [ebp + 8], 0x18
004bfb0b  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004bfb0e  8b 51 58                           mov        edx, dword ptr [ecx + 0x58]
004bfb11  8b 04 02                           mov        eax, dword ptr [edx + eax]
004bfb14  8b e5                              mov        esp, ebp
004bfb16  5d                                 pop        ebp
004bfb17  c2 04 00                           ret        4
