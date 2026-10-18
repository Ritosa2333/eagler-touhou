0046cfa0  55                                 push       ebp
0046cfa1  8b ec                              mov        ebp, esp
0046cfa3  83 ec 10                           sub        esp, 0x10
0046cfa6  89 4d f8                           mov        dword ptr [ebp - 8], ecx
0046cfa9  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0046cfac  e8 6f 36 00 00                     call       0x470620
0046cfb1  89 45 fc                           mov        dword ptr [ebp - 4], eax
0046cfb4  6a 00                              push       0
0046cfb6  8d 4d 0c                           lea        ecx, [ebp + 0xc]
0046cfb9  e8 d2 2f fa ff                     call       0x40ff90
0046cfbe  50                                 push       eax
0046cfbf  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046cfc2  50                                 push       eax
0046cfc3  8d 4d 0c                           lea        ecx, [ebp + 0xc]
0046cfc6  e8 45 33 00 00                     call       0x470310
0046cfcb  6a 0a                              push       0xa
0046cfcd  8b 4d 14                           mov        ecx, dword ptr [ebp + 0x14]
0046cfd0  51                                 push       ecx
0046cfd1  ba 01 00 00 00                     mov        edx, 1
0046cfd6  69 c2 ff 00 00 00                  imul       eax, edx, 0xff
0046cfdc  03 45 fc                           add        eax, dword ptr [ebp - 4]
0046cfdf  50                                 push       eax
0046cfe0  8d 4d 0c                           lea        ecx, [ebp + 0xc]
0046cfe3  e8 a8 2f fa ff                     call       0x40ff90
0046cfe8  03 45 fc                           add        eax, dword ptr [ebp - 4]
0046cfeb  50                                 push       eax
0046cfec  e8 1f c2 fe ff                     call       0x459210
0046cff1  83 c4 10                           add        esp, 0x10
0046cff4  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0046cff7  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
0046cffa  b9 01 00 00 00                     mov        ecx, 1
0046cfff  6b d1 00                           imul       edx, ecx, 0
0046d002  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0046d005  c6 04 10 00                        mov        byte ptr [eax + edx], 0
0046d009  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046d00c  51                                 push       ecx
0046d00d  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0046d010  e8 2b fa ff ff                     call       0x46ca40
0046d015  90                                 nop        
0046d016  8b e5                              mov        esp, ebp
0046d018  5d                                 pop        ebp
0046d019  c2 10 00                           ret        0x10
