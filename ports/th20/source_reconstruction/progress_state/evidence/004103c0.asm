004103c0  55                                 push       ebp
004103c1  8b ec                              mov        ebp, esp
004103c3  83 ec 24                           sub        esp, 0x24
004103c6  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
004103c9  99                                 cdq        
004103ca  f7 7d 18                           idiv       dword ptr [ebp + 0x18]
004103cd  8b ca                              mov        ecx, edx
004103cf  8b 45 18                           mov        eax, dword ptr [ebp + 0x18]
004103d2  99                                 cdq        
004103d3  83 e2 03                           and        edx, 3
004103d6  03 c2                              add        eax, edx
004103d8  c1 f8 02                           sar        eax, 2
004103db  3b c8                              cmp        ecx, eax
004103dd  7d 0c                              jge        0x4103eb
004103df  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
004103e2  99                                 cdq        
004103e3  f7 7d 18                           idiv       dword ptr [ebp + 0x18]
004103e6  89 55 ec                           mov        dword ptr [ebp - 0x14], edx
004103e9  eb 07                              jmp        0x4103f2
004103eb  c7 45 ec 00 00 00 00               mov        dword ptr [ebp - 0x14], 0
004103f2  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
004103f5  89 55 f0                           mov        dword ptr [ebp - 0x10], edx
004103f8  8b 45 1c                           mov        eax, dword ptr [ebp + 0x1c]
004103fb  3b 45 0c                           cmp        eax, dword ptr [ebp + 0xc]
004103fe  7e 08                              jle        0x410408
00410400  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
00410403  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
00410406  eb 06                              jmp        0x41040e
00410408  8b 55 1c                           mov        edx, dword ptr [ebp + 0x1c]
0041040b  89 55 e8                           mov        dword ptr [ebp - 0x18], edx
0041040e  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
00410411  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
00410414  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00410417  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
0041041a  8b 55 e4                           mov        edx, dword ptr [ebp - 0x1c]
0041041d  52                                 push       edx
0041041e  68 b8 c5 56 00                     push       0x56c5b8 ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\core\\filectrl.cpp:131 u_char'
00410423  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
00410429  e8 92 d4 ff ff                     call       0x40d8c0
0041042e  89 45 fc                           mov        dword ptr [ebp - 4], eax
00410431  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00410434  89 45 dc                           mov        dword ptr [ebp - 0x24], eax
00410437  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
0041043a  83 e1 01                           and        ecx, 1
0041043d  03 4d f0                           add        ecx, dword ptr [ebp - 0x10]
00410440  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
00410443  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
00410446  2b 55 f0                           sub        edx, dword ptr [ebp - 0x10]
00410449  89 55 0c                           mov        dword ptr [ebp + 0xc], edx
0041044c  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
0041044f  50                                 push       eax
00410450  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00410453  51                                 push       ecx
00410454  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00410457  52                                 push       edx
00410458  e8 c3 39 13 00                     call       0x543e20
0041045d  83 c4 0c                           add        esp, 0xc
00410460  83 7d 0c 00                        cmp        dword ptr [ebp + 0xc], 0
00410464  0f 8e f7 00 00 00                  jle        0x410561
0041046a  83 7d 1c 00                        cmp        dword ptr [ebp + 0x1c], 0
0041046e  0f 8e ed 00 00 00                  jle        0x410561
00410474  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
00410477  3b 45 18                           cmp        eax, dword ptr [ebp + 0x18]
0041047a  7d 06                              jge        0x410482
0041047c  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
0041047f  89 4d 18                           mov        dword ptr [ebp + 0x18], ecx
00410482  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00410485  89 55 e0                           mov        dword ptr [ebp - 0x20], edx
00410488  8b 45 18                           mov        eax, dword ptr [ebp + 0x18]
0041048b  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041048e  8d 54 01 ff                        lea        edx, [ecx + eax - 1]
00410492  89 55 fc                           mov        dword ptr [ebp - 4], edx
00410495  8b 45 18                           mov        eax, dword ptr [ebp + 0x18]
00410498  83 c0 01                           add        eax, 1
0041049b  99                                 cdq        
0041049c  2b c2                              sub        eax, edx
0041049e  d1 f8                              sar        eax, 1
004104a0  89 45 f8                           mov        dword ptr [ebp - 8], eax
004104a3  eb 12                              jmp        0x4104b7
004104a5  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004104a8  83 e8 01                           sub        eax, 1
004104ab  89 45 f8                           mov        dword ptr [ebp - 8], eax
004104ae  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004104b1  83 c1 01                           add        ecx, 1
004104b4  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
004104b7  83 7d f8 00                        cmp        dword ptr [ebp - 8], 0
004104bb  7e 29                              jle        0x4104e6
004104bd  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004104c0  0f b6 02                           movzx      eax, byte ptr [edx]
004104c3  0f b6 4d 10                        movzx      ecx, byte ptr [ebp + 0x10]
004104c7  33 c1                              xor        eax, ecx
004104c9  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
004104cc  88 02                              mov        byte ptr [edx], al
004104ce  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004104d1  83 e8 02                           sub        eax, 2
004104d4  89 45 fc                           mov        dword ptr [ebp - 4], eax
004104d7  0f b6 4d 10                        movzx      ecx, byte ptr [ebp + 0x10]
004104db  0f b6 55 14                        movzx      edx, byte ptr [ebp + 0x14]
004104df  03 ca                              add        ecx, edx
004104e1  88 4d 10                           mov        byte ptr [ebp + 0x10], cl
004104e4  eb bf                              jmp        0x4104a5
004104e6  8b 45 18                           mov        eax, dword ptr [ebp + 0x18]
004104e9  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
004104ec  8d 54 01 fe                        lea        edx, [ecx + eax - 2]
004104f0  89 55 fc                           mov        dword ptr [ebp - 4], edx
004104f3  8b 45 18                           mov        eax, dword ptr [ebp + 0x18]
004104f6  99                                 cdq        
004104f7  2b c2                              sub        eax, edx
004104f9  d1 f8                              sar        eax, 1
004104fb  89 45 f8                           mov        dword ptr [ebp - 8], eax
004104fe  eb 12                              jmp        0x410512
00410500  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
00410503  83 e8 01                           sub        eax, 1
00410506  89 45 f8                           mov        dword ptr [ebp - 8], eax
00410509  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0041050c  83 c1 01                           add        ecx, 1
0041050f  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
00410512  83 7d f8 00                        cmp        dword ptr [ebp - 8], 0
00410516  7e 29                              jle        0x410541
00410518  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0041051b  0f b6 02                           movzx      eax, byte ptr [edx]
0041051e  0f b6 4d 10                        movzx      ecx, byte ptr [ebp + 0x10]
00410522  33 c1                              xor        eax, ecx
00410524  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
00410527  88 02                              mov        byte ptr [edx], al
00410529  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041052c  83 e8 02                           sub        eax, 2
0041052f  89 45 fc                           mov        dword ptr [ebp - 4], eax
00410532  0f b6 4d 10                        movzx      ecx, byte ptr [ebp + 0x10]
00410536  0f b6 55 14                        movzx      edx, byte ptr [ebp + 0x14]
0041053a  03 ca                              add        ecx, edx
0041053c  88 4d 10                           mov        byte ptr [ebp + 0x10], cl
0041053f  eb bf                              jmp        0x410500
00410541  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
00410544  2b 45 18                           sub        eax, dword ptr [ebp + 0x18]
00410547  89 45 0c                           mov        dword ptr [ebp + 0xc], eax
0041054a  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
0041054d  03 4d 18                           add        ecx, dword ptr [ebp + 0x18]
00410550  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00410553  8b 55 1c                           mov        edx, dword ptr [ebp + 0x1c]
00410556  2b 55 18                           sub        edx, dword ptr [ebp + 0x18]
00410559  89 55 1c                           mov        dword ptr [ebp + 0x1c], edx
0041055c  e9 ff fe ff ff                     jmp        0x410460
00410561  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
00410564  50                                 push       eax
00410565  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0041056b  e8 d0 d2 ff ff                     call       0x40d840
00410570  c7 45 dc 00 00 00 00               mov        dword ptr [ebp - 0x24], 0
00410577  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041057a  8b e5                              mov        esp, ebp
0041057c  5d                                 pop        ebp
0041057d  c3                                 ret        
