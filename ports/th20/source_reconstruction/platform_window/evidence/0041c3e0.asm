0041c3e0  55                                 push       ebp
0041c3e1  8b ec                              mov        ebp, esp
0041c3e3  83 ec 68                           sub        esp, 0x68
0041c3e6  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0041c3eb  33 c5                              xor        eax, ebp
0041c3ed  89 45 fc                           mov        dword ptr [ebp - 4], eax
0041c3f0  56                                 push       esi
0041c3f1  57                                 push       edi
0041c3f2  89 4d 98                           mov        dword ptr [ebp - 0x68], ecx
0041c3f5  6a 38                              push       0x38
0041c3f7  6a 00                              push       0
0041c3f9  8d 45 b4                           lea        eax, [ebp - 0x4c]
0041c3fc  50                                 push       eax
0041c3fd  e8 5e 83 12 00                     call       0x544760
0041c402  83 c4 0c                           add        esp, 0xc
0041c405  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c40a  e8 81 3b ff ff                     call       0x40ff90
0041c40f  89 45 b0                           mov        dword ptr [ebp - 0x50], eax
0041c412  8b 4d b0                           mov        ecx, dword ptr [ebp - 0x50]
0041c415  8b 11                              mov        edx, dword ptr [ecx]
0041c417  8b 42 20                           mov        eax, dword ptr [edx + 0x20]
0041c41a  89 45 a4                           mov        dword ptr [ebp - 0x5c], eax
0041c41d  8d 4d ec                           lea        ecx, [ebp - 0x14]
0041c420  51                                 push       ecx
0041c421  6a 00                              push       0
0041c423  8b 55 b0                           mov        edx, dword ptr [ebp - 0x50]
0041c426  52                                 push       edx
0041c427  ff 55 a4                           call       dword ptr [ebp - 0x5c]
0041c42a  90                                 nop        
0041c42b  83 7d f8 16                        cmp        dword ptr [ebp - 8], 0x16
0041c42f  75 07                              jne        0x41c438
0041c431  c7 45 f8 15 00 00 00               mov        dword ptr [ebp - 8], 0x15
0041c438  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
0041c43b  a3 cc 4e 5c 00                     mov        dword ptr [0x5c4ecc], eax
0041c440  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0041c443  89 0d d0 4e 5c 00                  mov        dword ptr [0x5c4ed0], ecx
0041c449  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
0041c44c  89 15 d4 4e 5c 00                  mov        dword ptr [0x5c4ed4], edx
0041c452  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0041c455  a3 d8 4e 5c 00                     mov        dword ptr [0x5c4ed8], eax
0041c45a  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c45f  e8 9c 93 ff ff                     call       0x415800
0041c464  85 c0                              test       eax, eax
0041c466  74 15                              je         0x41c47d
0041c468  83 7d f4 3c                        cmp        dword ptr [ebp - 0xc], 0x3c
0041c46c  74 0f                              je         0x41c47d
0041c46e  8b 0d e8 87 5b 00                  mov        ecx, dword ptr [0x5b87e8]
0041c474  83 e1 fb                           and        ecx, 0xfffffffb
0041c477  89 0d e8 87 5b 00                  mov        dword ptr [0x5b87e8], ecx
0041c47d  0f b6 15 e0 67 5b 00               movzx      edx, byte ptr [0x5b67e0]
0041c484  85 d2                              test       edx, edx
0041c486  74 0a                              je         0x41c492
0041c488  c7 05 74 58 5c 00 01 00 00 00      mov        dword ptr [0x5c5874], 1
0041c492  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c497  e8 64 93 ff ff                     call       0x415800
0041c49c  85 c0                              test       eax, eax
0041c49e  0f 85 c6 00 00 00                  jne        0x41c56a
0041c4a4  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c4a9  e8 92 60 ff ff                     call       0x412540
0041c4ae  83 f8 01                           cmp        eax, 1
0041c4b1  75 10                              jne        0x41c4c3
0041c4b3  c7 45 bc 17 00 00 00               mov        dword ptr [ebp - 0x44], 0x17
0041c4ba  c6 05 7c 4f 5c 00 01               mov        byte ptr [0x5c4f7c], 1
0041c4c1  eb 4b                              jmp        0x41c50e
0041c4c3  0f b6 05 7c 4f 5c 00               movzx      eax, byte ptr [0x5c4f7c]
0041c4ca  3d ff 00 00 00                     cmp        eax, 0xff
0041c4cf  75 22                              jne        0x41c4f3
0041c4d1  c7 45 bc 15 00 00 00               mov        dword ptr [ebp - 0x44], 0x15
0041c4d8  c6 05 7c 4f 5c 00 00               mov        byte ptr [0x5c4f7c], 0
0041c4df  68 f8 cb 56 00                     push       0x56cbf8
0041c4e4  68 78 06 5c 00                     push       0x5c0678
0041c4e9  e8 62 7c 03 00                     call       0x454150
0041c4ee  83 c4 08                           add        esp, 8
0041c4f1  eb 1b                              jmp        0x41c50e
0041c4f3  0f b6 0d 7c 4f 5c 00               movzx      ecx, byte ptr [0x5c4f7c]
0041c4fa  85 c9                              test       ecx, ecx
0041c4fc  75 09                              jne        0x41c507
0041c4fe  c7 45 bc 15 00 00 00               mov        dword ptr [ebp - 0x44], 0x15
0041c505  eb 07                              jmp        0x41c50e
0041c507  c7 45 bc 17 00 00 00               mov        dword ptr [ebp - 0x44], 0x17
0041c50e  83 3d 74 58 5c 00 00               cmp        dword ptr [0x5c5874], 0
0041c515  75 3c                              jne        0x41c553
0041c517  8b 15 e8 87 5b 00                  mov        edx, dword ptr [0x5b87e8]
0041c51d  c1 ea 02                           shr        edx, 2
0041c520  83 e2 01                           and        edx, 1
0041c523  74 09                              je         0x41c52e
0041c525  c7 45 e8 01 00 00 00               mov        dword ptr [ebp - 0x18], 1
0041c52c  eb 1c                              jmp        0x41c54a
0041c52e  0f b6 05 89 4f 5c 00               movzx      eax, byte ptr [0x5c4f89]
0041c535  83 f8 03                           cmp        eax, 3
0041c538  75 09                              jne        0x41c543
0041c53a  c7 45 e8 00 00 00 80               mov        dword ptr [ebp - 0x18], 0x80000000
0041c541  eb 07                              jmp        0x41c54a
0041c543  c7 45 e8 01 00 00 00               mov        dword ptr [ebp - 0x18], 1
0041c54a  c7 45 cc 01 00 00 00               mov        dword ptr [ebp - 0x34], 1
0041c551  eb 15                              jmp        0x41c568
0041c553  c7 45 e4 00 00 00 00               mov        dword ptr [ebp - 0x1c], 0
0041c55a  c7 45 cc 01 00 00 00               mov        dword ptr [ebp - 0x34], 1
0041c561  c7 45 e8 00 00 00 80               mov        dword ptr [ebp - 0x18], 0x80000000
0041c568  eb 5e                              jmp        0x41c5c8
0041c56a  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0041c56d  89 4d bc                           mov        dword ptr [ebp - 0x44], ecx
0041c570  83 7d f8 16                        cmp        dword ptr [ebp - 8], 0x16
0041c574  75 07                              jne        0x41c57d
0041c576  c7 45 bc 15 00 00 00               mov        dword ptr [ebp - 0x44], 0x15
0041c57d  c7 45 cc 01 00 00 00               mov        dword ptr [ebp - 0x34], 1
0041c584  0f b6 15 89 4f 5c 00               movzx      edx, byte ptr [0x5c4f89]
0041c58b  83 fa 03                           cmp        edx, 3
0041c58e  74 06                              je         0x41c596
0041c590  83 7d f4 3c                        cmp        dword ptr [ebp - 0xc], 0x3c
0041c594  74 16                              je         0x41c5ac
0041c596  c7 45 e8 00 00 00 80               mov        dword ptr [ebp - 0x18], 0x80000000
0041c59d  a1 e8 87 5b 00                     mov        eax, dword ptr [0x5b87e8]
0041c5a2  83 e0 fb                           and        eax, 0xfffffffb
0041c5a5  a3 e8 87 5b 00                     mov        dword ptr [0x5b87e8], eax
0041c5aa  eb 15                              jmp        0x41c5c1
0041c5ac  8b 0d e8 87 5b 00                  mov        ecx, dword ptr [0x5b87e8]
0041c5b2  c1 e9 02                           shr        ecx, 2
0041c5b5  83 e1 01                           and        ecx, 1
0041c5b8  74 07                              je         0x41c5c1
0041c5ba  c7 45 e8 01 00 00 00               mov        dword ptr [ebp - 0x18], 1
0041c5c1  c7 45 d4 01 00 00 00               mov        dword ptr [ebp - 0x2c], 1
0041c5c8  c7 45 d8 01 00 00 00               mov        dword ptr [ebp - 0x28], 1
0041c5cf  c7 45 dc 50 00 00 00               mov        dword ptr [ebp - 0x24], 0x50
0041c5d6  c7 45 e0 01 00 00 00               mov        dword ptr [ebp - 0x20], 1
0041c5dd  8b 15 88 58 5c 00                  mov        edx, dword ptr [0x5c5888]
0041c5e3  83 ca 02                           or         edx, 2
0041c5e6  89 15 88 58 5c 00                  mov        dword ptr [0x5c5888], edx
0041c5ec  c7 05 78 58 5c 00 01 00 00 00      mov        dword ptr [0x5c5878], 1
0041c5f6  b9 0e 00 00 00                     mov        ecx, 0xe
0041c5fb  8d 75 b4                           lea        esi, [ebp - 0x4c]
0041c5fe  bf 24 4e 5c 00                     mov        edi, 0x5c4e24
0041c603  f3 a5                              rep movsd  dword ptr es:[edi], dword ptr [esi]
0041c605  6a 00                              push       0
0041c607  e8 24 01 00 00                     call       0x41c730
0041c60c  83 c4 04                           add        esp, 4
0041c60f  85 c0                              test       eax, eax
0041c611  74 0a                              je         0x41c61d
0041c613  b8 01 00 00 00                     mov        eax, 1
0041c618  e9 f9 00 00 00                     jmp        0x41c716
0041c61d  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c622  e8 09 61 ff ff                     call       0x412730
0041c627  89 45 ac                           mov        dword ptr [ebp - 0x54], eax
0041c62a  8b 45 ac                           mov        eax, dword ptr [ebp - 0x54]
0041c62d  8b 08                              mov        ecx, dword ptr [eax]
0041c62f  8b 51 1c                           mov        edx, dword ptr [ecx + 0x1c]
0041c632  89 55 a0                           mov        dword ptr [ebp - 0x60], edx
0041c635  68 94 58 5c 00                     push       0x5c5894
0041c63a  8b 45 ac                           mov        eax, dword ptr [ebp - 0x54]
0041c63d  50                                 push       eax
0041c63e  ff 55 a0                           call       dword ptr [ebp - 0x60]
0041c641  90                                 nop        
0041c642  8b 0d 24 59 5c 00                  mov        ecx, dword ptr [0x5c5924]
0041c648  83 e1 40                           and        ecx, 0x40
0041c64b  75 12                              jne        0x41c65f
0041c64d  68 28 cc 56 00                     push       0x56cc28 ; string='D3DTEXOPCAPS_ADD '
0041c652  68 78 06 5c 00                     push       0x5c0678
0041c657  e8 f4 7a 03 00                     call       0x454150
0041c65c  83 c4 08                           add        esp, 8
0041c65f  81 3d ec 58 5c 00 00 01 00 00      cmp        dword ptr [0x5c58ec], 0x100
0041c669  77 12                              ja         0x41c67d
0041c66b  68 78 cc 56 00                     push       0x56cc78
0041c670  68 78 06 5c 00                     push       0x5c0678
0041c675  e8 d6 7a 03 00                     call       0x454150
0041c67a  83 c4 08                           add        esp, 8
0041c67d  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c682  e8 b9 5e ff ff                     call       0x412540
0041c687  85 c0                              test       eax, eax
0041c689  75 70                              jne        0x41c6fb
0041c68b  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041c690  e8 fb 38 ff ff                     call       0x40ff90
0041c695  89 45 a8                           mov        dword ptr [ebp - 0x58], eax
0041c698  8b 55 a8                           mov        edx, dword ptr [ebp - 0x58]
0041c69b  8b 02                              mov        eax, dword ptr [edx]
0041c69d  8b 48 28                           mov        ecx, dword ptr [eax + 0x28]
0041c6a0  89 4d 9c                           mov        dword ptr [ebp - 0x64], ecx
0041c6a3  6a 15                              push       0x15
0041c6a5  6a 03                              push       3
0041c6a7  6a 00                              push       0
0041c6a9  8b 55 bc                           mov        edx, dword ptr [ebp - 0x44]
0041c6ac  52                                 push       edx
0041c6ad  6a 01                              push       1
0041c6af  6a 00                              push       0
0041c6b1  8b 45 a8                           mov        eax, dword ptr [ebp - 0x58]
0041c6b4  50                                 push       eax
0041c6b5  ff 55 9c                           call       dword ptr [ebp - 0x64]
0041c6b8  85 c0                              test       eax, eax
0041c6ba  75 11                              jne        0x41c6cd
0041c6bc  8b 0d 88 58 5c 00                  mov        ecx, dword ptr [0x5c5888]
0041c6c2  83 c9 04                           or         ecx, 4
0041c6c5  89 0d 88 58 5c 00                  mov        dword ptr [0x5c5888], ecx
0041c6cb  eb 2e                              jmp        0x41c6fb
0041c6cd  8b 15 88 58 5c 00                  mov        edx, dword ptr [0x5c5888]
0041c6d3  83 e2 fb                           and        edx, 0xfffffffb
0041c6d6  89 15 88 58 5c 00                  mov        dword ptr [0x5c5888], edx
0041c6dc  a1 8c 4f 5c 00                     mov        eax, dword ptr [0x5c4f8c]
0041c6e1  83 c8 01                           or         eax, 1
0041c6e4  a3 8c 4f 5c 00                     mov        dword ptr [0x5c4f8c], eax
0041c6e9  68 c8 cc 56 00                     push       0x56ccc8 ; string='D3DFMT_A8R8G8B8 '
0041c6ee  68 78 06 5c 00                     push       0x5c0678
0041c6f3  e8 58 7a 03 00                     call       0x454150
0041c6f8  83 c4 08                           add        esp, 8
0041c6fb  c7 05 60 67 5b 00 00 00 00 00      mov        dword ptr [0x5b6760], 0
0041c705  e8 b6 db ff ff                     call       0x41a2c0
0041c70a  c7 05 7c 58 5c 00 00 00 00 00      mov        dword ptr [0x5c587c], 0
0041c714  33 c0                              xor        eax, eax
0041c716  5f                                 pop        edi
0041c717  5e                                 pop        esi
0041c718  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041c71b  33 cd                              xor        ecx, ebp
0041c71d  e8 cc 62 12 00                     call       0x5429ee
0041c722  8b e5                              mov        esp, ebp
0041c724  5d                                 pop        ebp
0041c725  c3                                 ret        
