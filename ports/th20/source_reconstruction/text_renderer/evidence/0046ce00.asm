0046ce00  55                                 push       ebp
0046ce01  8b ec                              mov        ebp, esp
0046ce03  83 ec 10                           sub        esp, 0x10
0046ce06  89 4d f8                           mov        dword ptr [ebp - 8], ecx
0046ce09  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0046ce0c  e8 0f 38 00 00                     call       0x470620
0046ce11  89 45 fc                           mov        dword ptr [ebp - 4], eax
0046ce14  8b 45 18                           mov        eax, dword ptr [ebp + 0x18]
0046ce17  50                                 push       eax
0046ce18  6a 02                              push       2
0046ce1a  51                                 push       ecx
0046ce1b  f3 0f 10 45 0c                     movss      xmm0, dword ptr [ebp + 0xc]
0046ce20  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046ce25  b9 01 00 00 00                     mov        ecx, 1
0046ce2a  69 d1 ff 00 00 00                  imul       edx, ecx, 0xff
0046ce30  03 55 fc                           add        edx, dword ptr [ebp - 4]
0046ce33  52                                 push       edx
0046ce34  b8 01 00 00 00                     mov        eax, 1
0046ce39  6b c8 00                           imul       ecx, eax, 0
0046ce3c  03 4d fc                           add        ecx, dword ptr [ebp - 4]
0046ce3f  51                                 push       ecx
0046ce40  e8 ab 42 00 00                     call       0x4710f0
0046ce45  83 c4 14                           add        esp, 0x14
0046ce48  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0046ce4b  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
0046ce4e  6a 00                              push       0
0046ce50  8d 4d 10                           lea        ecx, [ebp + 0x10]
0046ce53  e8 38 31 fa ff                     call       0x40ff90
0046ce58  50                                 push       eax
0046ce59  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0046ce5c  52                                 push       edx
0046ce5d  8d 4d 10                           lea        ecx, [ebp + 0x10]
0046ce60  e8 ab 34 00 00                     call       0x470310
0046ce65  8d 4d 10                           lea        ecx, [ebp + 0x10]
0046ce68  e8 23 31 fa ff                     call       0x40ff90
0046ce6d  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046ce70  c6 04 01 00                        mov        byte ptr [ecx + eax], 0
0046ce74  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046ce77  52                                 push       edx
0046ce78  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0046ce7b  e8 c0 fb ff ff                     call       0x46ca40
0046ce80  90                                 nop        
0046ce81  8b e5                              mov        esp, ebp
0046ce83  5d                                 pop        ebp
0046ce84  c2 14 00                           ret        0x14
