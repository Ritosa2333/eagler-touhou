0046cd80  55                                 push       ebp
0046cd81  8b ec                              mov        ebp, esp
0046cd83  83 ec 10                           sub        esp, 0x10
0046cd86  89 4d f8                           mov        dword ptr [ebp - 8], ecx
0046cd89  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0046cd8c  e8 8f 38 00 00                     call       0x470620
0046cd91  89 45 fc                           mov        dword ptr [ebp - 4], eax
0046cd94  6a 0a                              push       0xa
0046cd96  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046cd99  50                                 push       eax
0046cd9a  b9 01 00 00 00                     mov        ecx, 1
0046cd9f  69 d1 ff 00 00 00                  imul       edx, ecx, 0xff
0046cda5  03 55 fc                           add        edx, dword ptr [ebp - 4]
0046cda8  52                                 push       edx
0046cda9  b8 01 00 00 00                     mov        eax, 1
0046cdae  6b c8 00                           imul       ecx, eax, 0
0046cdb1  03 4d fc                           add        ecx, dword ptr [ebp - 4]
0046cdb4  51                                 push       ecx
0046cdb5  e8 56 c4 fe ff                     call       0x459210
0046cdba  83 c4 10                           add        esp, 0x10
0046cdbd  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0046cdc0  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
0046cdc3  6a 00                              push       0
0046cdc5  8d 4d 10                           lea        ecx, [ebp + 0x10]
0046cdc8  e8 c3 31 fa ff                     call       0x40ff90
0046cdcd  50                                 push       eax
0046cdce  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0046cdd1  52                                 push       edx
0046cdd2  8d 4d 10                           lea        ecx, [ebp + 0x10]
0046cdd5  e8 36 35 00 00                     call       0x470310
0046cdda  8d 4d 10                           lea        ecx, [ebp + 0x10]
0046cddd  e8 ae 31 fa ff                     call       0x40ff90
0046cde2  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046cde5  c6 04 01 00                        mov        byte ptr [ecx + eax], 0
0046cde9  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046cdec  52                                 push       edx
0046cded  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0046cdf0  e8 4b fc ff ff                     call       0x46ca40
0046cdf5  90                                 nop        
0046cdf6  8b e5                              mov        esp, ebp
0046cdf8  5d                                 pop        ebp
0046cdf9  c2 10 00                           ret        0x10
