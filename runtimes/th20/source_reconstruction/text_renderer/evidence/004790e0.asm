004790e0  55                                 push       ebp
004790e1  8b ec                              mov        ebp, esp
004790e3  51                                 push       ecx
004790e4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004790e7  8b 45 20                           mov        eax, dword ptr [ebp + 0x20]
004790ea  50                                 push       eax
004790eb  6a 00                              push       0
004790ed  8b 4d 1c                           mov        ecx, dword ptr [ebp + 0x1c]
004790f0  51                                 push       ecx
004790f1  51                                 push       ecx
004790f2  f3 0f 10 45 18                     movss      xmm0, dword ptr [ebp + 0x18]
004790f7  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004790fc  8b 55 14                           mov        edx, dword ptr [ebp + 0x14]
004790ff  52                                 push       edx
00479100  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
00479103  50                                 push       eax
00479104  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
00479107  51                                 push       ecx
00479108  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0047910b  52                                 push       edx
0047910c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0047910f  e8 9c 7b fd ff                     call       0x450cb0
00479114  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00479117  8b e5                              mov        esp, ebp
00479119  5d                                 pop        ebp
0047911a  c2 1c 00                           ret        0x1c
