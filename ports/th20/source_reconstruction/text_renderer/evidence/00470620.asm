00470620  55                                 push       ebp
00470621  8b ec                              mov        ebp, esp
00470623  83 ec 0c                           sub        esp, 0xc
00470626  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00470629  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0047062c  81 b8 bc a1 01 00 40 01 00 00      cmp        dword ptr [eax + 0x1a1bc], 0x140
00470636  7c 0e                              jl         0x470646
00470638  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0047063b  81 c1 60 a2 01 00                  add        ecx, 0x1a260
00470641  89 4d f8                           mov        dword ptr [ebp - 8], ecx
00470644  eb 21                              jmp        0x470667
00470646  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470649  81 c2 bc 11 00 00                  add        edx, 0x11bc
0047064f  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
00470652  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00470655  8b 88 bc a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1bc]
0047065b  51                                 push       ecx
0047065c  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0047065f  e8 ac ab ff ff                     call       0x46b210
00470664  89 45 f8                           mov        dword ptr [ebp - 8], eax
00470667  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0047066a  8b e5                              mov        esp, ebp
0047066c  5d                                 pop        ebp
0047066d  c3                                 ret        
