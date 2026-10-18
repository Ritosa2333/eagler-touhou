004710f0  55                                 push       ebp
004710f1  8b ec                              mov        ebp, esp
004710f3  83 ec 08                           sub        esp, 8
004710f6  8b 45 18                           mov        eax, dword ptr [ebp + 0x18]
004710f9  50                                 push       eax
004710fa  8b 4d 14                           mov        ecx, dword ptr [ebp + 0x14]
004710fd  51                                 push       ecx
004710fe  51                                 push       ecx
004710ff  f3 0f 10 45 10                     movss      xmm0, dword ptr [ebp + 0x10]
00471104  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
00471109  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0047110c  52                                 push       edx
0047110d  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00471110  50                                 push       eax
00471111  e8 5a 73 ff ff                     call       0x468470
00471116  83 c4 14                           add        esp, 0x14
00471119  89 45 f8                           mov        dword ptr [ebp - 8], eax
0047111c  89 55 fc                           mov        dword ptr [ebp - 4], edx
0047111f  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
00471122  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00471125  8b e5                              mov        esp, ebp
00471127  5d                                 pop        ebp
00471128  c3                                 ret        
