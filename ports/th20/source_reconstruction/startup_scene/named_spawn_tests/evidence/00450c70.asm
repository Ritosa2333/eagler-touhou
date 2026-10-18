00450c70  55                                 push       ebp
00450c71  8b ec                              mov        ebp, esp
00450c73  51                                 push       ecx
00450c74  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00450c77  8b 45 18                           mov        eax, dword ptr [ebp + 0x18]
00450c7a  50                                 push       eax
00450c7b  6a 00                              push       0
00450c7d  8b 4d 14                           mov        ecx, dword ptr [ebp + 0x14]
00450c80  51                                 push       ecx
00450c81  51                                 push       ecx
00450c82  0f 57 c0                           xorps      xmm0, xmm0
00450c85  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
00450c8a  6a 00                              push       0
00450c8c  8b 55 10                           mov        edx, dword ptr [ebp + 0x10]
00450c8f  52                                 push       edx
00450c90  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
00450c93  50                                 push       eax
00450c94  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00450c97  51                                 push       ecx
00450c98  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00450c9b  e8 10 00 00 00                     call       0x450cb0
00450ca0  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00450ca3  8b e5                              mov        esp, ebp
00450ca5  5d                                 pop        ebp
00450ca6  c2 14 00                           ret        0x14
