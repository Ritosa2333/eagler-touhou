0046cf00  55                                 push       ebp
0046cf01  8b ec                              mov        ebp, esp
0046cf03  83 ec 10                           sub        esp, 0x10
0046cf06  89 4d f8                           mov        dword ptr [ebp - 8], ecx
0046cf09  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0046cf0c  e8 0f 37 00 00                     call       0x470620
0046cf11  89 45 fc                           mov        dword ptr [ebp - 4], eax
0046cf14  6a 00                              push       0
0046cf16  8d 4d 0c                           lea        ecx, [ebp + 0xc]
0046cf19  e8 72 30 fa ff                     call       0x40ff90
0046cf1e  50                                 push       eax
0046cf1f  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046cf22  50                                 push       eax
0046cf23  8d 4d 0c                           lea        ecx, [ebp + 0xc]
0046cf26  e8 e5 33 00 00                     call       0x470310
0046cf2b  6a 0a                              push       0xa
0046cf2d  8b 4d 14                           mov        ecx, dword ptr [ebp + 0x14]
0046cf30  51                                 push       ecx
0046cf31  ba 01 00 00 00                     mov        edx, 1
0046cf36  69 c2 ff 00 00 00                  imul       eax, edx, 0xff
0046cf3c  03 45 fc                           add        eax, dword ptr [ebp - 4]
0046cf3f  50                                 push       eax
0046cf40  8d 4d 0c                           lea        ecx, [ebp + 0xc]
0046cf43  e8 48 30 fa ff                     call       0x40ff90
0046cf48  03 45 fc                           add        eax, dword ptr [ebp - 4]
0046cf4b  50                                 push       eax
0046cf4c  e8 bf c2 fe ff                     call       0x459210
0046cf51  83 c4 10                           add        esp, 0x10
0046cf54  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0046cf57  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
0046cf5a  6a 00                              push       0
0046cf5c  8d 4d 18                           lea        ecx, [ebp + 0x18]
0046cf5f  e8 2c 30 fa ff                     call       0x40ff90
0046cf64  50                                 push       eax
0046cf65  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046cf68  51                                 push       ecx
0046cf69  8d 4d 18                           lea        ecx, [ebp + 0x18]
0046cf6c  e8 9f 33 00 00                     call       0x470310
0046cf71  8d 4d 18                           lea        ecx, [ebp + 0x18]
0046cf74  e8 17 30 fa ff                     call       0x40ff90
0046cf79  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0046cf7c  c6 04 02 00                        mov        byte ptr [edx + eax], 0
0046cf80  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046cf83  50                                 push       eax
0046cf84  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0046cf87  e8 b4 fa ff ff                     call       0x46ca40
0046cf8c  90                                 nop        
0046cf8d  8b e5                              mov        esp, ebp
0046cf8f  5d                                 pop        ebp
0046cf90  c2 18 00                           ret        0x18
