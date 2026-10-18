0046c210  55                                 push       ebp
0046c211  8b ec                              mov        ebp, esp
0046c213  6a ff                              push       -1
0046c215  68 31 8e 56 00                     push       0x568e31
0046c21a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0046c220  50                                 push       eax
0046c221  81 ec b0 01 00 00                  sub        esp, 0x1b0
0046c227  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0046c22c  33 c5                              xor        eax, ebp
0046c22e  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0046c231  56                                 push       esi
0046c232  50                                 push       eax
0046c233  8d 45 f4                           lea        eax, [ebp - 0xc]
0046c236  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0046c23c  6a 12                              push       0x12
0046c23e  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0046c243  e8 c8 00 fa ff                     call       0x40c310
0046c248  50                                 push       eax
0046c249  8d 8d ec fe ff ff                  lea        ecx, [ebp - 0x114]
0046c24f  e8 dc f4 f9 ff                     call       0x40b730
0046c254  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0046c25b  8d 45 18                           lea        eax, [ebp + 0x18]
0046c25e  89 85 ac fe ff ff                  mov        dword ptr [ebp - 0x154], eax
0046c264  8b 8d ac fe ff ff                  mov        ecx, dword ptr [ebp - 0x154]
0046c26a  51                                 push       ecx
0046c26b  8d 4d 10                           lea        ecx, [ebp + 0x10]
0046c26e  e8 8d 00 fa ff                     call       0x40c300
0046c273  50                                 push       eax
0046c274  8d 95 f0 fe ff ff                  lea        edx, [ebp - 0x110]
0046c27a  52                                 push       edx
0046c27b  e8 d0 e2 ff ff                     call       0x46a550
0046c280  83 c4 0c                           add        esp, 0xc
0046c283  c7 85 ac fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x154], 0
0046c28d  6a 1c                              push       0x1c
0046c28f  8d 8d c0 fe ff ff                  lea        ecx, [ebp - 0x140]
0046c295  e8 e6 fd f9 ff                     call       0x40c080
0046c29a  8d 85 f0 fe ff ff                  lea        eax, [ebp - 0x110]
0046c2a0  50                                 push       eax
0046c2a1  8d 8d c0 fe ff ff                  lea        ecx, [ebp - 0x140]
0046c2a7  e8 84 e5 ff ff                     call       0x46a830
0046c2ac  c6 45 fc 01                        mov        byte ptr [ebp - 4], 1
0046c2b0  c7 85 b4 fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x14c], 0
0046c2ba  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046c2bd  81 c1 0c a2 01 00                  add        ecx, 0x1a20c
0046c2c3  89 8d a8 fe ff ff                  mov        dword ptr [ebp - 0x158], ecx
0046c2c9  6a 08                              push       8
0046c2cb  8d 8d e4 fe ff ff                  lea        ecx, [ebp - 0x11c]
0046c2d1  e8 aa fd f9 ff                     call       0x40c080
0046c2d6  8d 95 e4 fe ff ff                  lea        edx, [ebp - 0x11c]
0046c2dc  52                                 push       edx
0046c2dd  8b 8d a8 fe ff ff                  mov        ecx, dword ptr [ebp - 0x158]
0046c2e3  e8 98 5f fa ff                     call       0x412280
0046c2e8  c6 45 fc 02                        mov        byte ptr [ebp - 4], 2
0046c2ec  8b 8d a8 fe ff ff                  mov        ecx, dword ptr [ebp - 0x158]
0046c2f2  e8 49 62 fa ff                     call       0x412540
0046c2f7  89 85 a4 fe ff ff                  mov        dword ptr [ebp - 0x15c], eax
0046c2fd  eb 0c                              jmp        0x46c30b
0046c2ff  8d 8d e4 fe ff ff                  lea        ecx, [ebp - 0x11c]
0046c305  e8 26 59 fa ff                     call       0x411c30
0046c30a  90                                 nop        
0046c30b  8b 85 a4 fe ff ff                  mov        eax, dword ptr [ebp - 0x15c]
0046c311  50                                 push       eax
0046c312  8d 8d e4 fe ff ff                  lea        ecx, [ebp - 0x11c]
0046c318  e8 93 58 fa ff                     call       0x411bb0
0046c31d  0f b6 c8                           movzx      ecx, al
0046c320  85 c9                              test       ecx, ecx
0046c322  0f 84 5d 02 00 00                  je         0x46c585
0046c328  8d 8d e4 fe ff ff                  lea        ecx, [ebp - 0x11c]
0046c32e  e8 cd ff f9 ff                     call       0x40c300
0046c333  89 85 b0 fe ff ff                  mov        dword ptr [ebp - 0x150], eax
0046c339  8d 95 c0 fe ff ff                  lea        edx, [ebp - 0x140]
0046c33f  52                                 push       edx
0046c340  8b 8d b0 fe ff ff                  mov        ecx, dword ptr [ebp - 0x150]
0046c346  e8 15 2e fa ff                     call       0x40f160
0046c34b  8b 00                              mov        eax, dword ptr [eax]
0046c34d  05 f8 05 00 00                     add        eax, 0x5f8
0046c352  50                                 push       eax
0046c353  e8 78 bc ff ff                     call       0x467fd0
0046c358  83 c4 08                           add        esp, 8
0046c35b  0f b6 c8                           movzx      ecx, al
0046c35e  85 c9                              test       ecx, ecx
0046c360  0f 84 1a 02 00 00                  je         0x46c580
0046c366  8b 8d b0 fe ff ff                  mov        ecx, dword ptr [ebp - 0x150]
0046c36c  e8 ef 2d fa ff                     call       0x40f160
0046c371  8b 10                              mov        edx, dword ptr [eax]
0046c373  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046c376  8b 8a 2c 06 00 00                  mov        ecx, dword ptr [edx + 0x62c]
0046c37c  3b 88 e4 a1 01 00                  cmp        ecx, dword ptr [eax + 0x1a1e4]
0046c382  0f 85 f8 01 00 00                  jne        0x46c580
0046c388  8b 8d b0 fe ff ff                  mov        ecx, dword ptr [ebp - 0x150]
0046c38e  e8 cd 2d fa ff                     call       0x40f160
0046c393  8b 10                              mov        edx, dword ptr [eax]
0046c395  89 95 b4 fe ff ff                  mov        dword ptr [ebp - 0x14c], edx
0046c39b  8b 85 b4 fe ff ff                  mov        eax, dword ptr [ebp - 0x14c]
0046c3a1  83 b8 20 06 00 00 00               cmp        dword ptr [eax + 0x620], 0
0046c3a8  0f 85 8f 00 00 00                  jne        0x46c43d
0046c3ae  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
0046c3b1  f3 0f 10 01                        movss      xmm0, dword ptr [ecx]
0046c3b5  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046c3bd  8b 95 b4 fe ff ff                  mov        edx, dword ptr [ebp - 0x14c]
0046c3c3  f3 0f 11 82 14 06 00 00            movss      dword ptr [edx + 0x614], xmm0
0046c3cb  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046c3ce  f3 0f 10 40 04                     movss      xmm0, dword ptr [eax + 4]
0046c3d3  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046c3db  8b 8d b4 fe ff ff                  mov        ecx, dword ptr [ebp - 0x14c]
0046c3e1  f3 0f 11 81 18 06 00 00            movss      dword ptr [ecx + 0x618], xmm0
0046c3e9  8b 95 b4 fe ff ff                  mov        edx, dword ptr [ebp - 0x14c]
0046c3ef  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046c3f2  8b 88 c0 a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1c0]
0046c3f8  89 8a 4c 06 00 00                  mov        dword ptr [edx + 0x64c], ecx
0046c3fe  8b 95 b4 fe ff ff                  mov        edx, dword ptr [ebp - 0x14c]
0046c404  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046c407  8b 88 c4 a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1c4]
0046c40d  89 8a 54 06 00 00                  mov        dword ptr [edx + 0x654], ecx
0046c413  8b 95 b4 fe ff ff                  mov        edx, dword ptr [ebp - 0x14c]
0046c419  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046c41c  8b 88 c8 a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1c8]
0046c422  89 8a 50 06 00 00                  mov        dword ptr [edx + 0x650], ecx
0046c428  8b 95 b4 fe ff ff                  mov        edx, dword ptr [ebp - 0x14c]
0046c42e  c7 82 20 06 00 00 01 00 00 00      mov        dword ptr [edx + 0x620], 1
0046c438  e9 41 01 00 00                     jmp        0x46c57e
0046c43d  68 d0 f6 56 00                     push       0x56f6d0 ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\ascii.cpp:1118 JapaneseStrInf'
0046c442  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0046c448  e8 73 dd ff ff                     call       0x46a1c0
0046c44d  89 85 b8 fe ff ff                  mov        dword ptr [ebp - 0x148], eax
0046c453  8b 85 b8 fe ff ff                  mov        eax, dword ptr [ebp - 0x148]
0046c459  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046c45c  8b 91 e4 a1 01 00                  mov        edx, dword ptr [ecx + 0x1a1e4]
0046c462  89 90 2c 06 00 00                  mov        dword ptr [eax + 0x62c], edx
0046c468  8b 85 b4 fe ff ff                  mov        eax, dword ptr [ebp - 0x14c]
0046c46e  8b 88 34 06 00 00                  mov        ecx, dword ptr [eax + 0x634]
0046c474  8b 90 38 06 00 00                  mov        edx, dword ptr [eax + 0x638]
0046c47a  8b 85 b8 fe ff ff                  mov        eax, dword ptr [ebp - 0x148]
0046c480  89 88 34 06 00 00                  mov        dword ptr [eax + 0x634], ecx
0046c486  89 90 38 06 00 00                  mov        dword ptr [eax + 0x638], edx
0046c48c  8b 8d b4 fe ff ff                  mov        ecx, dword ptr [ebp - 0x14c]
0046c492  81 c1 3c 06 00 00                  add        ecx, 0x63c
0046c498  8b 95 b8 fe ff ff                  mov        edx, dword ptr [ebp - 0x148]
0046c49e  81 c2 3c 06 00 00                  add        edx, 0x63c
0046c4a4  8b 01                              mov        eax, dword ptr [ecx]
0046c4a6  89 02                              mov        dword ptr [edx], eax
0046c4a8  8b 41 04                           mov        eax, dword ptr [ecx + 4]
0046c4ab  89 42 04                           mov        dword ptr [edx + 4], eax
0046c4ae  8b 41 08                           mov        eax, dword ptr [ecx + 8]
0046c4b1  89 42 08                           mov        dword ptr [edx + 8], eax
0046c4b4  8b 49 0c                           mov        ecx, dword ptr [ecx + 0xc]
0046c4b7  89 4a 0c                           mov        dword ptr [edx + 0xc], ecx
0046c4ba  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046c4bd  e8 ae 41 00 00                     call       0x470670
0046c4c2  89 85 a0 fe ff ff                  mov        dword ptr [ebp - 0x160], eax
0046c4c8  6a 34                              push       0x34
0046c4ca  8b 95 b8 fe ff ff                  mov        edx, dword ptr [ebp - 0x148]
0046c4d0  83 c2 14                           add        edx, 0x14
0046c4d3  52                                 push       edx
0046c4d4  8b 8d a0 fe ff ff                  mov        ecx, dword ptr [ebp - 0x160]
0046c4da  e8 01 44 00 00                     call       0x4708e0
0046c4df  8b 85 b8 fe ff ff                  mov        eax, dword ptr [ebp - 0x148]
0046c4e5  83 c0 14                           add        eax, 0x14
0046c4e8  89 85 9c fe ff ff                  mov        dword ptr [ebp - 0x164], eax
0046c4ee  8b 8d b8 fe ff ff                  mov        ecx, dword ptr [ebp - 0x148]
0046c4f4  8b 91 48 06 00 00                  mov        edx, dword ptr [ecx + 0x648]
0046c4fa  52                                 push       edx
0046c4fb  8b 85 b8 fe ff ff                  mov        eax, dword ptr [ebp - 0x148]
0046c501  8b 88 44 06 00 00                  mov        ecx, dword ptr [eax + 0x644]
0046c507  51                                 push       ecx
0046c508  8b 95 b8 fe ff ff                  mov        edx, dword ptr [ebp - 0x148]
0046c50e  8b 82 40 06 00 00                  mov        eax, dword ptr [edx + 0x640]
0046c514  50                                 push       eax
0046c515  8b 8d b8 fe ff ff                  mov        ecx, dword ptr [ebp - 0x148]
0046c51b  8b 91 3c 06 00 00                  mov        edx, dword ptr [ecx + 0x63c]
0046c521  52                                 push       edx
0046c522  8b 8d 9c fe ff ff                  mov        ecx, dword ptr [ebp - 0x164]
0046c528  e8 53 46 00 00                     call       0x470b80
0046c52d  8d 85 c0 fe ff ff                  lea        eax, [ebp - 0x140]
0046c533  50                                 push       eax
0046c534  8b 8d b8 fe ff ff                  mov        ecx, dword ptr [ebp - 0x148]
0046c53a  81 c1 f8 05 00 00                  add        ecx, 0x5f8
0046c540  e8 eb eb ff ff                     call       0x46b130
0046c545  8b 8d b8 fe ff ff                  mov        ecx, dword ptr [ebp - 0x148]
0046c54b  c6 81 78 06 00 00 01               mov        byte ptr [ecx + 0x678], 1
0046c552  8b 95 b4 fe ff ff                  mov        edx, dword ptr [ebp - 0x14c]
0046c558  81 c2 78 06 00 00                  add        edx, 0x678
0046c55e  8b 85 b8 fe ff ff                  mov        eax, dword ptr [ebp - 0x148]
0046c564  89 90 7c 06 00 00                  mov        dword ptr [eax + 0x67c], edx
0046c56a  8b 8d b8 fe ff ff                  mov        ecx, dword ptr [ebp - 0x148]
0046c570  51                                 push       ecx
0046c571  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0046c574  52                                 push       edx
0046c575  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046c578  e8 f3 0b 00 00                     call       0x46d170
0046c57d  90                                 nop        
0046c57e  eb 05                              jmp        0x46c585
0046c580  e9 7a fd ff ff                     jmp        0x46c2ff
0046c585  c6 45 fc 01                        mov        byte ptr [ebp - 4], 1
0046c589  8d 8d e4 fe ff ff                  lea        ecx, [ebp - 0x11c]
0046c58f  e8 6c 55 fa ff                     call       0x411b00
0046c594  90                                 nop        
0046c595  83 bd b4 fe ff ff 00               cmp        dword ptr [ebp - 0x14c], 0
0046c59c  0f 85 51 03 00 00                  jne        0x46c8f3
0046c5a2  68 18 f7 56 00                     push       0x56f718 ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\ascii.cpp:1139 JapaneseStrInf'
0046c5a7  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0046c5ad  e8 0e dc ff ff                     call       0x46a1c0
0046c5b2  89 85 bc fe ff ff                  mov        dword ptr [ebp - 0x144], eax
0046c5b8  8b 85 bc fe ff ff                  mov        eax, dword ptr [ebp - 0x144]
0046c5be  50                                 push       eax
0046c5bf  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
0046c5c2  51                                 push       ecx
0046c5c3  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046c5c6  e8 a5 0b 00 00                     call       0x46d170
0046c5cb  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046c5ce  e8 9d 40 00 00                     call       0x470670
0046c5d3  89 85 94 fe ff ff                  mov        dword ptr [ebp - 0x16c], eax
0046c5d9  8b 95 bc fe ff ff                  mov        edx, dword ptr [ebp - 0x144]
0046c5df  83 c2 14                           add        edx, 0x14
0046c5e2  89 95 98 fe ff ff                  mov        dword ptr [ebp - 0x168], edx
0046c5e8  6a 1b                              push       0x1b
0046c5ea  8b 85 98 fe ff ff                  mov        eax, dword ptr [ebp - 0x168]
0046c5f0  50                                 push       eax
0046c5f1  8b 8d 94 fe ff ff                  mov        ecx, dword ptr [ebp - 0x16c]
0046c5f7  e8 e4 42 00 00                     call       0x4708e0
0046c5fc  8b 8d bc fe ff ff                  mov        ecx, dword ptr [ebp - 0x144]
0046c602  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046c605  8b 82 e4 a1 01 00                  mov        eax, dword ptr [edx + 0x1a1e4]
0046c60b  89 81 2c 06 00 00                  mov        dword ptr [ecx + 0x62c], eax
0046c611  8b 8d bc fe ff ff                  mov        ecx, dword ptr [ebp - 0x144]
0046c617  83 b9 30 06 00 00 00               cmp        dword ptr [ecx + 0x630], 0
0046c61e  0f 85 95 00 00 00                  jne        0x46c6b9
0046c624  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0046c627  8b 82 e4 a1 01 00                  mov        eax, dword ptr [edx + 0x1a1e4]
0046c62d  89 85 90 fe ff ff                  mov        dword ptr [ebp - 0x170], eax
0046c633  8d 8d c0 fe ff ff                  lea        ecx, [ebp - 0x140]
0046c639  e8 b2 f4 fd ff                     call       0x44baf0
0046c63e  89 85 8c fe ff ff                  mov        dword ptr [ebp - 0x174], eax
0046c644  8b 8d 90 fe ff ff                  mov        ecx, dword ptr [ebp - 0x170]
0046c64a  51                                 push       ecx
0046c64b  8b 95 8c fe ff ff                  mov        edx, dword ptr [ebp - 0x174]
0046c651  52                                 push       edx
0046c652  e8 89 90 fa ff                     call       0x4156e0
0046c657  83 c4 08                           add        esp, 8
0046c65a  89 85 70 fe ff ff                  mov        dword ptr [ebp - 0x190], eax
0046c660  89 95 74 fe ff ff                  mov        dword ptr [ebp - 0x18c], edx
0046c666  8b 85 bc fe ff ff                  mov        eax, dword ptr [ebp - 0x144]
0046c66c  8b 8d 70 fe ff ff                  mov        ecx, dword ptr [ebp - 0x190]
0046c672  8b 95 74 fe ff ff                  mov        edx, dword ptr [ebp - 0x18c]
0046c678  89 88 34 06 00 00                  mov        dword ptr [eax + 0x634], ecx
0046c67e  89 90 38 06 00 00                  mov        dword ptr [eax + 0x638], edx
0046c684  8b 85 bc fe ff ff                  mov        eax, dword ptr [ebp - 0x144]
0046c68a  8b 8d bc fe ff ff                  mov        ecx, dword ptr [ebp - 0x144]
0046c690  8b 91 34 06 00 00                  mov        edx, dword ptr [ecx + 0x634]
0046c696  89 90 44 06 00 00                  mov        dword ptr [eax + 0x644], edx
0046c69c  8b 85 bc fe ff ff                  mov        eax, dword ptr [ebp - 0x144]
0046c6a2  8b 8d bc fe ff ff                  mov        ecx, dword ptr [ebp - 0x144]
0046c6a8  8b 91 38 06 00 00                  mov        edx, dword ptr [ecx + 0x638]
0046c6ae  89 90 48 06 00 00                  mov        dword ptr [eax + 0x648], edx
0046c6b4  e9 b9 00 00 00                     jmp        0x46c772
0046c6b9  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046c6bc  8b 88 e4 a1 01 00                  mov        ecx, dword ptr [eax + 0x1a1e4]
0046c6c2  51                                 push       ecx
0046c6c3  68 5c f7 56 00                     push       0x56f75c
0046c6c8  e8 13 90 fa ff                     call       0x4156e0
0046c6cd  83 c4 08                           add        esp, 8
0046c6d0  89 85 68 fe ff ff                  mov        dword ptr [ebp - 0x198], eax
0046c6d6  89 95 6c fe ff ff                  mov        dword ptr [ebp - 0x194], edx
0046c6dc  8b 95 bc fe ff ff                  mov        edx, dword ptr [ebp - 0x144]
0046c6e2  8b 85 68 fe ff ff                  mov        eax, dword ptr [ebp - 0x198]
0046c6e8  8b 8d 6c fe ff ff                  mov        ecx, dword ptr [ebp - 0x194]
0046c6ee  89 82 34 06 00 00                  mov        dword ptr [edx + 0x634], eax
0046c6f4  89 8a 38 06 00 00                  mov        dword ptr [edx + 0x638], ecx
0046c6fa  8d 8d c0 fe ff ff                  lea        ecx, [ebp - 0x140]
0046c700  e8 8b 03 fb ff                     call       0x41ca90
0046c705  8b 95 bc fe ff ff                  mov        edx, dword ptr [ebp - 0x144]
0046c70b  0f af 82 30 06 00 00               imul       eax, dword ptr [edx + 0x630]
0046c712  89 85 84 fe ff ff                  mov        dword ptr [ebp - 0x17c], eax
0046c718  8b 85 bc fe ff ff                  mov        eax, dword ptr [ebp - 0x144]
0046c71e  8b 88 34 06 00 00                  mov        ecx, dword ptr [eax + 0x634]
0046c724  89 8d 88 fe ff ff                  mov        dword ptr [ebp - 0x178], ecx
0046c72a  8b 95 88 fe ff ff                  mov        edx, dword ptr [ebp - 0x178]
0046c730  03 95 84 fe ff ff                  add        edx, dword ptr [ebp - 0x17c]
0046c736  8b 85 bc fe ff ff                  mov        eax, dword ptr [ebp - 0x144]
0046c73c  89 90 34 06 00 00                  mov        dword ptr [eax + 0x634], edx
0046c742  8b 8d bc fe ff ff                  mov        ecx, dword ptr [ebp - 0x144]
0046c748  8b 95 bc fe ff ff                  mov        edx, dword ptr [ebp - 0x144]
0046c74e  8b 82 34 06 00 00                  mov        eax, dword ptr [edx + 0x634]
0046c754  89 81 44 06 00 00                  mov        dword ptr [ecx + 0x644], eax
0046c75a  8b 8d bc fe ff ff                  mov        ecx, dword ptr [ebp - 0x144]
0046c760  8b 95 bc fe ff ff                  mov        edx, dword ptr [ebp - 0x144]
0046c766  8b 82 38 06 00 00                  mov        eax, dword ptr [edx + 0x638]
0046c76c  89 81 48 06 00 00                  mov        dword ptr [ecx + 0x648], eax
0046c772  8b 8d bc fe ff ff                  mov        ecx, dword ptr [ebp - 0x144]
0046c778  8b 91 48 06 00 00                  mov        edx, dword ptr [ecx + 0x648]
0046c77e  52                                 push       edx
0046c77f  8b 85 bc fe ff ff                  mov        eax, dword ptr [ebp - 0x144]
0046c785  8b 88 44 06 00 00                  mov        ecx, dword ptr [eax + 0x644]
0046c78b  51                                 push       ecx
0046c78c  8d 95 dc fe ff ff                  lea        edx, [ebp - 0x124]
0046c792  52                                 push       edx
0046c793  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046c796  e8 e5 3e 00 00                     call       0x470680
0046c79b  90                                 nop        
0046c79c  83 bd dc fe ff ff 00               cmp        dword ptr [ebp - 0x124], 0
0046c7a3  7d 43                              jge        0x46c7e8
0046c7a5  8b 85 bc fe ff ff                  mov        eax, dword ptr [ebp - 0x144]
0046c7ab  50                                 push       eax
0046c7ac  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0046c7b2  e8 49 d9 ff ff                     call       0x46a100
0046c7b7  c7 85 bc fe ff ff 00 00 00 00      mov        dword ptr [ebp - 0x144], 0
0046c7c1  c6 45 fc 00                        mov        byte ptr [ebp - 4], 0
0046c7c5  8d 8d c0 fe ff ff                  lea        ecx, [ebp - 0x140]
0046c7cb  e8 70 ca fd ff                     call       0x449240
0046c7d0  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0046c7d7  8d 8d ec fe ff ff                  lea        ecx, [ebp - 0x114]
0046c7dd  e8 1e f1 f9 ff                     call       0x40b900
0046c7e2  90                                 nop        
0046c7e3  e9 2d 01 00 00                     jmp        0x46c915
0046c7e8  8b 8d bc fe ff ff                  mov        ecx, dword ptr [ebp - 0x144]
0046c7ee  8b 95 dc fe ff ff                  mov        edx, dword ptr [ebp - 0x124]
0046c7f4  89 91 3c 06 00 00                  mov        dword ptr [ecx + 0x63c], edx
0046c7fa  8b 85 bc fe ff ff                  mov        eax, dword ptr [ebp - 0x144]
0046c800  8b 8d e0 fe ff ff                  mov        ecx, dword ptr [ebp - 0x120]
0046c806  89 88 40 06 00 00                  mov        dword ptr [eax + 0x640], ecx
0046c80c  8b 95 bc fe ff ff                  mov        edx, dword ptr [ebp - 0x144]
0046c812  83 c2 14                           add        edx, 0x14
0046c815  89 95 80 fe ff ff                  mov        dword ptr [ebp - 0x180], edx
0046c81b  8b 85 bc fe ff ff                  mov        eax, dword ptr [ebp - 0x144]
0046c821  8b 88 48 06 00 00                  mov        ecx, dword ptr [eax + 0x648]
0046c827  51                                 push       ecx
0046c828  8b 95 bc fe ff ff                  mov        edx, dword ptr [ebp - 0x144]
0046c82e  8b 82 44 06 00 00                  mov        eax, dword ptr [edx + 0x644]
0046c834  50                                 push       eax
0046c835  8b 8d bc fe ff ff                  mov        ecx, dword ptr [ebp - 0x144]
0046c83b  8b 91 40 06 00 00                  mov        edx, dword ptr [ecx + 0x640]
0046c841  52                                 push       edx
0046c842  8b 85 bc fe ff ff                  mov        eax, dword ptr [ebp - 0x144]
0046c848  8b 88 3c 06 00 00                  mov        ecx, dword ptr [eax + 0x63c]
0046c84e  51                                 push       ecx
0046c84f  8b 8d 80 fe ff ff                  mov        ecx, dword ptr [ebp - 0x180]
0046c855  e8 26 43 00 00                     call       0x470b80
0046c85a  8b 95 bc fe ff ff                  mov        edx, dword ptr [ebp - 0x144]
0046c860  c6 82 78 06 00 00 00               mov        byte ptr [edx + 0x678], 0
0046c867  6a 00                              push       0
0046c869  8b 8d bc fe ff ff                  mov        ecx, dword ptr [ebp - 0x144]
0046c86f  81 c1 80 06 00 00                  add        ecx, 0x680
0046c875  e8 26 f2 f9 ff                     call       0x40baa0
0046c87a  8d 85 c0 fe ff ff                  lea        eax, [ebp - 0x140]
0046c880  50                                 push       eax
0046c881  8b 8d bc fe ff ff                  mov        ecx, dword ptr [ebp - 0x144]
0046c887  81 c1 f8 05 00 00                  add        ecx, 0x5f8
0046c88d  e8 9e e8 ff ff                     call       0x46b130
0046c892  90                                 nop        
0046c893  83 ec 28                           sub        esp, 0x28
0046c896  8b f4                              mov        esi, esp
0046c898  89 a5 64 fe ff ff                  mov        dword ptr [ebp - 0x19c], esp
0046c89e  8d 8d c0 fe ff ff                  lea        ecx, [ebp - 0x140]
0046c8a4  51                                 push       ecx
0046c8a5  8d 95 bc fe ff ff                  lea        edx, [ebp - 0x144]
0046c8ab  52                                 push       edx
0046c8ac  8d 8d 44 fe ff ff                  lea        ecx, [ebp - 0x1bc]
0046c8b2  e8 29 dd ff ff                     call       0x46a5e0
0046c8b7  89 85 7c fe ff ff                  mov        dword ptr [ebp - 0x184], eax
0046c8bd  8b 85 7c fe ff ff                  mov        eax, dword ptr [ebp - 0x184]
0046c8c3  89 85 78 fe ff ff                  mov        dword ptr [ebp - 0x188], eax
0046c8c9  c6 45 fc 03                        mov        byte ptr [ebp - 4], 3
0046c8cd  8b 8d 78 fe ff ff                  mov        ecx, dword ptr [ebp - 0x188]
0046c8d3  51                                 push       ecx
0046c8d4  8b ce                              mov        ecx, esi
0046c8d6  e8 05 b6 ff ff                     call       0x467ee0
0046c8db  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046c8de  e8 9d 38 00 00                     call       0x470180
0046c8e3  c6 45 fc 01                        mov        byte ptr [ebp - 4], 1
0046c8e7  8d 8d 44 fe ff ff                  lea        ecx, [ebp - 0x1bc]
0046c8ed  e8 ee e4 ff ff                     call       0x46ade0
0046c8f2  90                                 nop        
0046c8f3  c6 45 fc 00                        mov        byte ptr [ebp - 4], 0
0046c8f7  8d 8d c0 fe ff ff                  lea        ecx, [ebp - 0x140]
0046c8fd  e8 3e c9 fd ff                     call       0x449240
0046c902  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0046c909  8d 8d ec fe ff ff                  lea        ecx, [ebp - 0x114]
0046c90f  e8 ec ef f9 ff                     call       0x40b900
0046c914  90                                 nop        
0046c915  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0046c918  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0046c91f  59                                 pop        ecx
0046c920  5e                                 pop        esi
0046c921  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046c924  33 cd                              xor        ecx, ebp
0046c926  e8 c3 60 0d 00                     call       0x5429ee
0046c92b  8b e5                              mov        esp, ebp
0046c92d  5d                                 pop        ebp
0046c92e  c3                                 ret        
