004d8160  55                                 push       ebp
004d8161  8b ec                              mov        ebp, esp
004d8163  83 ec 0c                           sub        esp, 0xc
004d8166  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004d8169  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d816c  83 b8 10 06 00 00 01               cmp        dword ptr [eax + 0x610], 1
004d8173  75 43                              jne        0x4d81b8
004d8175  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d8178  8b 91 0c 06 00 00                  mov        edx, dword ptr [ecx + 0x60c]
004d817e  89 55 f8                           mov        dword ptr [ebp - 8], edx
004d8181  6a 00                              push       0
004d8183  6a ff                              push       -1
004d8185  6a 00                              push       0
004d8187  68 0c 19 57 00                     push       0x57190c
004d818c  8d 45 f4                           lea        eax, [ebp - 0xc]
004d818f  50                                 push       eax
004d8190  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004d8193  e8 d8 8a f7 ff                     call       0x450c70
004d8198  8b 08                              mov        ecx, dword ptr [eax]
004d819a  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d819d  89 8a 08 06 00 00                  mov        dword ptr [edx + 0x608], ecx
004d81a3  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d81a6  8b 88 10 06 00 00                  mov        ecx, dword ptr [eax + 0x610]
004d81ac  83 c1 01                           add        ecx, 1
004d81af  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004d81b2  89 8a 10 06 00 00                  mov        dword ptr [edx + 0x610], ecx
004d81b8  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d81bb  83 b8 14 06 00 00 01               cmp        dword ptr [eax + 0x614], 1
004d81c2  75 3c                              jne        0x4d8200
004d81c4  51                                 push       ecx
004d81c5  f3 0f 10 05 88 03 57 00            movss      xmm0, dword ptr [0x570388]
004d81cd  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004d81d2  51                                 push       ecx
004d81d3  f3 0f 10 05 a8 cd 56 00            movss      xmm0, dword ptr [0x56cda8]
004d81db  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004d81e0  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004d81e6  e8 b5 88 fc ff                     call       0x4a0aa0
004d81eb  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d81ee  8b 91 14 06 00 00                  mov        edx, dword ptr [ecx + 0x614]
004d81f4  83 c2 01                           add        edx, 1
004d81f7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d81fa  89 90 14 06 00 00                  mov        dword ptr [eax + 0x614], edx
004d8200  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004d8203  8b 91 18 06 00 00                  mov        edx, dword ptr [ecx + 0x618]
004d8209  83 c2 01                           add        edx, 1
004d820c  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004d820f  89 90 18 06 00 00                  mov        dword ptr [eax + 0x618], edx
004d8215  b8 01 00 00 00                     mov        eax, 1
004d821a  8b e5                              mov        esp, ebp
004d821c  5d                                 pop        ebp
004d821d  c3                                 ret        
