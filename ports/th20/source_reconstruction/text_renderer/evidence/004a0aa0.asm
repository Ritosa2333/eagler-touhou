004a0aa0  55                                 push       ebp
004a0aa1  8b ec                              mov        ebp, esp
004a0aa3  83 ec 20                           sub        esp, 0x20
004a0aa6  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004a0aab  33 c5                              xor        eax, ebp
004a0aad  89 45 fc                           mov        dword ptr [ebp - 4], eax
004a0ab0  89 4d ec                           mov        dword ptr [ebp - 0x14], ecx
004a0ab3  51                                 push       ecx
004a0ab4  0f 57 c0                           xorps      xmm0, xmm0
004a0ab7  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004a0abc  f3 0f 10 45 0c                     movss      xmm0, dword ptr [ebp + 0xc]
004a0ac1  f3 0f 59 05 d0 c8 56 00            mulss      xmm0, dword ptr [0x56c8d0]
004a0ac9  51                                 push       ecx
004a0aca  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004a0acf  f3 0f 10 45 08                     movss      xmm0, dword ptr [ebp + 8]
004a0ad4  f3 0f 59 05 d0 c8 56 00            mulss      xmm0, dword ptr [0x56c8d0]
004a0adc  51                                 push       ecx
004a0add  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004a0ae2  8d 4d f0                           lea        ecx, [ebp - 0x10]
004a0ae5  e8 e6 22 f8 ff                     call       0x422dd0
004a0aea  90                                 nop        
004a0aeb  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
004a0aee  05 4c a2 01 00                     add        eax, 0x1a24c
004a0af3  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
004a0af6  6a 00                              push       0
004a0af8  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004a0afb  e8 30 89 fa ff                     call       0x449430
004a0b00  0f b6 c8                           movzx      ecx, al
004a0b03  85 c9                              test       ecx, ecx
004a0b05  74 37                              je         0x4a0b3e
004a0b07  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
004a0b0a  e8 41 6e f9 ff                     call       0x437950
004a0b0f  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
004a0b12  6a 00                              push       0
004a0b14  6a ff                              push       -1
004a0b16  51                                 push       ecx
004a0b17  0f 57 c0                           xorps      xmm0, xmm0
004a0b1a  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004a0b1f  8d 55 f0                           lea        edx, [ebp - 0x10]
004a0b22  52                                 push       edx
004a0b23  6a 11                              push       0x11
004a0b25  6a 00                              push       0
004a0b27  8d 45 e0                           lea        eax, [ebp - 0x20]
004a0b2a  50                                 push       eax
004a0b2b  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
004a0b2e  e8 ad 85 fd ff                     call       0x4790e0
004a0b33  8b 08                              mov        ecx, dword ptr [eax]
004a0b35  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
004a0b38  89 8a 4c a2 01 00                  mov        dword ptr [edx + 0x1a24c], ecx
004a0b3e  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004a0b41  33 cd                              xor        ecx, ebp
004a0b43  e8 a6 1e 0a 00                     call       0x5429ee
004a0b48  8b e5                              mov        esp, ebp
004a0b4a  5d                                 pop        ebp
004a0b4b  c2 08 00                           ret        8
