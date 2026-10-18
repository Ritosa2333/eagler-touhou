0050eb70  55                                 push       ebp
0050eb71  8b ec                              mov        ebp, esp
0050eb73  83 ec 14                           sub        esp, 0x14
0050eb76  56                                 push       esi
0050eb77  57                                 push       edi
0050eb78  89 4d ec                           mov        dword ptr [ebp - 0x14], ecx
0050eb7b  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0050eb7e  83 78 04 00                        cmp        dword ptr [eax + 4], 0
0050eb82  0f 85 c5 00 00 00                  jne        0x50ec4d
0050eb88  68 b0 37 57 00                     push       0x5737b0 ; string='Init ScoreFile'
0050eb8d  e8 1e db ef ff                     call       0x40c6b0
0050eb92  83 c4 04                           add        esp, 4
0050eb95  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0050eb98  83 79 04 00                        cmp        dword ptr [ecx + 4], 0
0050eb9c  74 25                              je         0x50ebc3
0050eb9e  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0050eba1  83 7a 04 00                        cmp        dword ptr [edx + 4], 0
0050eba5  74 1c                              je         0x50ebc3
0050eba7  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0050ebaa  8b 48 04                           mov        ecx, dword ptr [eax + 4]
0050ebad  51                                 push       ecx
0050ebae  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0050ebb4  e8 b7 0a f1 ff                     call       0x41f670
0050ebb9  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0050ebbc  c7 42 04 00 00 00 00               mov        dword ptr [edx + 4], 0
0050ebc3  68 c0 37 57 00                     push       0x5737c0 ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\score.cpp:175 ScoreFileHeaderInf'
0050ebc8  6a 2c                              push       0x2c
0050ebca  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0050ebd0  e8 3b 0a f1 ff                     call       0x41f610
0050ebd5  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0050ebd8  89 41 04                           mov        dword ptr [ecx + 4], eax
0050ebdb  6a 2c                              push       0x2c
0050ebdd  6a 00                              push       0
0050ebdf  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0050ebe2  8b 42 04                           mov        eax, dword ptr [edx + 4]
0050ebe5  50                                 push       eax
0050ebe6  e8 75 5b 03 00                     call       0x544760
0050ebeb  83 c4 0c                           add        esp, 0xc
0050ebee  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0050ebf1  8b 51 04                           mov        edx, dword ptr [ecx + 4]
0050ebf4  c7 02 54 48 30 32                  mov        dword ptr [edx], 0x32304854
0050ebfa  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0050ebfd  8b 48 04                           mov        ecx, dword ptr [eax + 4]
0050ec00  c7 41 08 04 00 00 00               mov        dword ptr [ecx + 8], 4
0050ec07  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0050ec0a  8b 42 04                           mov        eax, dword ptr [edx + 4]
0050ec0d  c7 40 20 00 01 00 00               mov        dword ptr [eax + 0x20], 0x100
0050ec14  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0050ec17  8b 51 04                           mov        edx, dword ptr [ecx + 4]
0050ec1a  c7 42 0c e0 00 00 00               mov        dword ptr [edx + 0xc], 0xe0
0050ec21  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0050ec24  8b 48 04                           mov        ecx, dword ptr [eax + 4]
0050ec27  c7 41 10 e8 7a 00 00               mov        dword ptr [ecx + 0x10], 0x7ae8
0050ec2e  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0050ec31  8b 42 04                           mov        eax, dword ptr [edx + 4]
0050ec34  c7 40 14 f8 01 00 00               mov        dword ptr [eax + 0x14], 0x1f8
0050ec3b  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0050ec3e  8b 51 04                           mov        edx, dword ptr [ecx + 4]
0050ec41  c7 42 18 40 21 09 00               mov        dword ptr [edx + 0x18], 0x92140
0050ec48  e9 51 02 00 00                     jmp        0x50ee9e
0050ec4d  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0050ec50  8b 48 04                           mov        ecx, dword ptr [eax + 4]
0050ec53  81 39 54 48 30 32                  cmp        dword ptr [ecx], 0x32304854
0050ec59  75 5b                              jne        0x50ecb6
0050ec5b  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0050ec5e  8b 42 04                           mov        eax, dword ptr [edx + 4]
0050ec61  83 78 08 04                        cmp        dword ptr [eax + 8], 4
0050ec65  75 4f                              jne        0x50ecb6
0050ec67  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0050ec6a  8b 51 04                           mov        edx, dword ptr [ecx + 4]
0050ec6d  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0050ec70  8b 08                              mov        ecx, dword ptr [eax]
0050ec72  83 e9 2c                           sub        ecx, 0x2c
0050ec75  39 4a 24                           cmp        dword ptr [edx + 0x24], ecx
0050ec78  75 3c                              jne        0x50ecb6
0050ec7a  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0050ec7d  8b 42 04                           mov        eax, dword ptr [edx + 4]
0050ec80  81 78 0c e0 00 00 00               cmp        dword ptr [eax + 0xc], 0xe0
0050ec87  75 2d                              jne        0x50ecb6
0050ec89  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0050ec8c  8b 51 04                           mov        edx, dword ptr [ecx + 4]
0050ec8f  81 7a 10 e8 7a 00 00               cmp        dword ptr [edx + 0x10], 0x7ae8
0050ec96  75 1e                              jne        0x50ecb6
0050ec98  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0050ec9b  8b 48 04                           mov        ecx, dword ptr [eax + 4]
0050ec9e  81 79 14 f8 01 00 00               cmp        dword ptr [ecx + 0x14], 0x1f8
0050eca5  75 0f                              jne        0x50ecb6
0050eca7  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0050ecaa  8b 42 04                           mov        eax, dword ptr [edx + 4]
0050ecad  81 78 18 40 21 09 00               cmp        dword ptr [eax + 0x18], 0x92140
0050ecb4  74 12                              je         0x50ecc8
0050ecb6  68 08 38 57 00                     push       0x573808 ; string='Attention : ScoreFile '
0050ecbb  e8 f0 d9 ef ff                     call       0x40c6b0
0050ecc0  83 c4 04                           add        esp, 4
0050ecc3  e9 c0 fe ff ff                     jmp        0x50eb88
0050ecc8  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0050eccb  8b 51 04                           mov        edx, dword ptr [ecx + 4]
0050ecce  8b 42 24                           mov        eax, dword ptr [edx + 0x24]
0050ecd1  50                                 push       eax
0050ecd2  6a 10                              push       0x10
0050ecd4  6a 35                              push       0x35
0050ecd6  68 ac 00 00 00                     push       0xac
0050ecdb  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0050ecde  8b 51 04                           mov        edx, dword ptr [ecx + 4]
0050ece1  8b 42 24                           mov        eax, dword ptr [edx + 0x24]
0050ece4  50                                 push       eax
0050ece5  b9 2c 00 00 00                     mov        ecx, 0x2c
0050ecea  c1 e1 00                           shl        ecx, 0
0050eced  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0050ecf0  03 4a 04                           add        ecx, dword ptr [edx + 4]
0050ecf3  51                                 push       ecx
0050ecf4  e8 e7 13 f0 ff                     call       0x4100e0
0050ecf9  83 c4 18                           add        esp, 0x18
0050ecfc  b8 2c 00 00 00                     mov        eax, 0x2c
0050ed01  c1 e0 00                           shl        eax, 0
0050ed04  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0050ed07  03 41 04                           add        eax, dword ptr [ecx + 4]
0050ed0a  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0050ed0d  68 48 38 57 00                     push       0x573848 ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\score.cpp:201 ScoreChunkInf'
0050ed12  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0050ed15  8b 42 04                           mov        eax, dword ptr [edx + 4]
0050ed18  8b 48 28                           mov        ecx, dword ptr [eax + 0x28]
0050ed1b  c1 e1 02                           shl        ecx, 2
0050ed1e  51                                 push       ecx
0050ed1f  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0050ed25  e8 e6 08 f1 ff                     call       0x41f610
0050ed2a  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0050ed2d  89 42 08                           mov        dword ptr [edx + 8], eax
0050ed30  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0050ed33  8b 48 04                           mov        ecx, dword ptr [eax + 4]
0050ed36  8b 51 28                           mov        edx, dword ptr [ecx + 0x28]
0050ed39  52                                 push       edx
0050ed3a  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0050ed3d  8b 48 08                           mov        ecx, dword ptr [eax + 8]
0050ed40  51                                 push       ecx
0050ed41  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0050ed44  8b 42 04                           mov        eax, dword ptr [edx + 4]
0050ed47  8b 48 24                           mov        ecx, dword ptr [eax + 0x24]
0050ed4a  51                                 push       ecx
0050ed4b  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
0050ed4e  52                                 push       edx
0050ed4f  e8 9c a4 02 00                     call       0x5391f0
0050ed54  83 c4 10                           add        esp, 0x10
0050ed57  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0050ed5a  8b 48 04                           mov        ecx, dword ptr [eax + 4]
0050ed5d  8b 51 28                           mov        edx, dword ptr [ecx + 0x28]
0050ed60  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
0050ed63  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0050ed66  8b 48 08                           mov        ecx, dword ptr [eax + 8]
0050ed69  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0050ed6c  83 7d f4 00                        cmp        dword ptr [ebp - 0xc], 0
0050ed70  0f 8e 28 01 00 00                  jle        0x50ee9e
0050ed76  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050ed79  0f b7 02                           movzx      eax, word ptr [edx]
0050ed7c  3d 43 52 00 00                     cmp        eax, 0x5243
0050ed81  0f 85 8a 00 00 00                  jne        0x50ee11
0050ed87  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050ed8a  0f b7 51 02                        movzx      edx, word ptr [ecx + 2]
0050ed8e  83 fa 01                           cmp        edx, 1
0050ed91  75 7c                              jne        0x50ee0f
0050ed93  68 e8 7a 00 00                     push       0x7ae8
0050ed98  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050ed9b  e8 f0 0e 00 00                     call       0x50fc90
0050eda0  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050eda3  3b 41 04                           cmp        eax, dword ptr [ecx + 4]
0050eda6  75 67                              jne        0x50ee0f
0050eda8  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050edab  81 7a 08 e8 7a 00 00               cmp        dword ptr [edx + 8], 0x7ae8
0050edb2  75 5b                              jne        0x50ee0f
0050edb4  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050edb7  89 45 f8                           mov        dword ptr [ebp - 8], eax
0050edba  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0050edbd  83 79 0c 02                        cmp        dword ptr [ecx + 0xc], 2
0050edc1  74 31                              je         0x50edf4
0050edc3  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0050edc6  69 42 0c 28 52 04 00               imul       eax, dword ptr [edx + 0xc], 0x45228
0050edcd  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0050edd0  8d 54 01 10                        lea        edx, [ecx + eax + 0x10]
0050edd4  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0050edd7  69 48 10 e8 7a 00 00               imul       ecx, dword ptr [eax + 0x10], 0x7ae8
0050edde  03 d1                              add        edx, ecx
0050ede0  68 e8 7a 00 00                     push       0x7ae8
0050ede5  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0050ede8  50                                 push       eax
0050ede9  52                                 push       edx
0050edea  e8 31 50 03 00                     call       0x543e20
0050edef  83 c4 0c                           add        esp, 0xc
0050edf2  eb 1b                              jmp        0x50ee0f
0050edf4  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0050edf7  81 c1 60 a4 08 00                  add        ecx, 0x8a460
0050edfd  68 e8 7a 00 00                     push       0x7ae8
0050ee02  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0050ee05  52                                 push       edx
0050ee06  51                                 push       ecx
0050ee07  e8 14 50 03 00                     call       0x543e20
0050ee0c  83 c4 0c                           add        esp, 0xc
0050ee0f  eb 5f                              jmp        0x50ee70
0050ee11  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050ee14  0f b7 08                           movzx      ecx, word ptr [eax]
0050ee17  81 f9 53 54 00 00                  cmp        ecx, 0x5453
0050ee1d  75 42                              jne        0x50ee61
0050ee1f  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050ee22  0f b7 42 02                        movzx      eax, word ptr [edx + 2]
0050ee26  83 f8 02                           cmp        eax, 2
0050ee29  75 34                              jne        0x50ee5f
0050ee2b  68 f8 01 00 00                     push       0x1f8
0050ee30  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050ee33  e8 58 0e 00 00                     call       0x50fc90
0050ee38  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050ee3b  3b 41 04                           cmp        eax, dword ptr [ecx + 4]
0050ee3e  75 1f                              jne        0x50ee5f
0050ee40  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050ee43  81 7a 08 f8 01 00 00               cmp        dword ptr [edx + 8], 0x1f8
0050ee4a  75 13                              jne        0x50ee5f
0050ee4c  8b 7d 08                           mov        edi, dword ptr [ebp + 8]
0050ee4f  81 c7 48 1f 09 00                  add        edi, 0x91f48
0050ee55  b9 7e 00 00 00                     mov        ecx, 0x7e
0050ee5a  8b 75 fc                           mov        esi, dword ptr [ebp - 4]
0050ee5d  f3 a5                              rep movsd  dword ptr es:[edi], dword ptr [esi]
0050ee5f  eb 0f                              jmp        0x50ee70
0050ee61  68 8c 38 57 00                     push       0x57388c ; string='error ScoreFile Data Error'
0050ee66  e8 45 d8 ef ff                     call       0x40c6b0
0050ee6b  83 c4 04                           add        esp, 4
0050ee6e  eb 2e                              jmp        0x50ee9e
0050ee70  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050ee73  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0050ee76  2b 48 08                           sub        ecx, dword ptr [eax + 8]
0050ee79  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
0050ee7c  79 0f                              jns        0x50ee8d
0050ee7e  68 8c 38 57 00                     push       0x57388c ; string='error ScoreFile Data Error'
0050ee83  e8 28 d8 ef ff                     call       0x40c6b0
0050ee88  83 c4 04                           add        esp, 4
0050ee8b  eb 11                              jmp        0x50ee9e
0050ee8d  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050ee90  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050ee93  03 42 08                           add        eax, dword ptr [edx + 8]
0050ee96  89 45 fc                           mov        dword ptr [ebp - 4], eax
0050ee99  e9 ce fe ff ff                     jmp        0x50ed6c
0050ee9e  33 c0                              xor        eax, eax
0050eea0  5f                                 pop        edi
0050eea1  5e                                 pop        esi
0050eea2  8b e5                              mov        esp, ebp
0050eea4  5d                                 pop        ebp
0050eea5  c2 04 00                           ret        4
