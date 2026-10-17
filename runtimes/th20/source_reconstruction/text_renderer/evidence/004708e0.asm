004708e0  55                                 push       ebp
004708e1  8b ec                              mov        ebp, esp
004708e3  51                                 push       ecx
004708e4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004708e7  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004708ea  e8 e1 90 fb ff                     call       0x4299d0
004708ef  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004708f2  e8 39 95 fb ff                     call       0x429e30
004708f7  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004708fa  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004708fd  8b 11                              mov        edx, dword ptr [ecx]
004708ff  89 50 1c                           mov        dword ptr [eax + 0x1c], edx
00470902  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
00470905  50                                 push       eax
00470906  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00470909  51                                 push       ecx
0047090a  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0047090d  e8 0e 7d fc ff                     call       0x438620
00470912  8b e5                              mov        esp, ebp
00470914  5d                                 pop        ebp
00470915  c2 08 00                           ret        8
