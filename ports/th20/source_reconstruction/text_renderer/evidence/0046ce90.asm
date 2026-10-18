0046ce90  55                                 push       ebp
0046ce91  8b ec                              mov        ebp, esp
0046ce93  83 ec 0c                           sub        esp, 0xc
0046ce96  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0046ce99  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046ce9c  81 b8 bc a1 01 00 40 01 00 00      cmp        dword ptr [eax + 0x1a1bc], 0x140
0046cea6  7c 02                              jl         0x46ceaa
0046cea8  eb 48                              jmp        0x46cef2
0046ceaa  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046cead  81 c1 bc 11 00 00                  add        ecx, 0x11bc
0046ceb3  89 4d f8                           mov        dword ptr [ebp - 8], ecx
0046ceb6  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046ceb9  8b 82 bc a1 01 00                  mov        eax, dword ptr [edx + 0x1a1bc]
0046cebf  50                                 push       eax
0046cec0  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0046cec3  e8 48 e3 ff ff                     call       0x46b210
0046cec8  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
0046cecb  8d 4d 0c                           lea        ecx, [ebp + 0xc]
0046cece  e8 2d f4 f9 ff                     call       0x40c300
0046ced3  50                                 push       eax
0046ced4  68 00 01 00 00                     push       0x100
0046ced9  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0046cedc  51                                 push       ecx
0046cedd  e8 2e b7 0d 00                     call       0x548610
0046cee2  83 c4 0c                           add        esp, 0xc
0046cee5  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046cee8  52                                 push       edx
0046cee9  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046ceec  e8 4f fb ff ff                     call       0x46ca40
0046cef1  90                                 nop        
0046cef2  8b e5                              mov        esp, ebp
0046cef4  5d                                 pop        ebp
0046cef5  c2 0c 00                           ret        0xc
