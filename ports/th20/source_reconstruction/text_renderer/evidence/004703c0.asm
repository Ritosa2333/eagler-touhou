004703c0  55                                 push       ebp
004703c1  8b ec                              mov        ebp, esp
004703c3  83 ec 18                           sub        esp, 0x18
004703c6  56                                 push       esi
004703c7  6a 0a                              push       0xa
004703c9  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
004703cc  50                                 push       eax
004703cd  b9 01 00 00 00                     mov        ecx, 1
004703d2  69 d1 ff 00 00 00                  imul       edx, ecx, 0xff
004703d8  03 55 08                           add        edx, dword ptr [ebp + 8]
004703db  52                                 push       edx
004703dc  b8 01 00 00 00                     mov        eax, 1
004703e1  6b c8 00                           imul       ecx, eax, 0
004703e4  03 4d 08                           add        ecx, dword ptr [ebp + 8]
004703e7  51                                 push       ecx
004703e8  e8 23 8e fe ff                     call       0x459210
004703ed  83 c4 10                           add        esp, 0x10
004703f0  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
004703f3  89 55 ec                           mov        dword ptr [ebp - 0x14], edx
004703f6  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004703f9  2b 55 08                           sub        edx, dword ptr [ebp + 8]
004703fc  89 55 fc                           mov        dword ptr [ebp - 4], edx
004703ff  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
00470402  2b 45 fc                           sub        eax, dword ptr [ebp - 4]
00470405  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
00470408  83 7d f4 00                        cmp        dword ptr [ebp - 0xc], 0
0047040c  7e 74                              jle        0x470482
0047040e  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
00470415  eb 09                              jmp        0x470420
00470417  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0047041a  83 c1 01                           add        ecx, 1
0047041d  89 4d f8                           mov        dword ptr [ebp - 8], ecx
00470420  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
00470423  3b 55 fc                           cmp        edx, dword ptr [ebp - 4]
00470426  7d 24                              jge        0x47044c
00470428  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0047042b  83 e8 01                           sub        eax, 1
0047042e  2b 45 f8                           sub        eax, dword ptr [ebp - 8]
00470431  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
00470434  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470437  8d 4c 0a ff                        lea        ecx, [edx + ecx - 1]
0047043b  2b 4d f8                           sub        ecx, dword ptr [ebp - 8]
0047043e  8b 55 08                           mov        edx, dword ptr [ebp + 8]
00470441  8b 75 08                           mov        esi, dword ptr [ebp + 8]
00470444  8a 04 06                           mov        al, byte ptr [esi + eax]
00470447  88 04 0a                           mov        byte ptr [edx + ecx], al
0047044a  eb cb                              jmp        0x470417
0047044c  c7 45 f0 00 00 00 00               mov        dword ptr [ebp - 0x10], 0
00470453  eb 09                              jmp        0x47045e
00470455  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00470458  83 c1 01                           add        ecx, 1
0047045b  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
0047045e  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
00470461  3b 55 f4                           cmp        edx, dword ptr [ebp - 0xc]
00470464  7d 0d                              jge        0x470473
00470466  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00470469  03 45 f0                           add        eax, dword ptr [ebp - 0x10]
0047046c  8a 4d 14                           mov        cl, byte ptr [ebp + 0x14]
0047046f  88 08                              mov        byte ptr [eax], cl
00470471  eb e2                              jmp        0x470455
00470473  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00470476  03 55 f4                           add        edx, dword ptr [ebp - 0xc]
00470479  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0047047c  c6 04 10 00                        mov        byte ptr [eax + edx], 0
00470480  eb 09                              jmp        0x47048b
00470482  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00470485  03 4d fc                           add        ecx, dword ptr [ebp - 4]
00470488  c6 01 00                           mov        byte ptr [ecx], 0
0047048b  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0047048e  03 45 f4                           add        eax, dword ptr [ebp - 0xc]
00470491  5e                                 pop        esi
00470492  8b e5                              mov        esp, ebp
00470494  5d                                 pop        ebp
00470495  c3                                 ret        
