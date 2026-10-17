0046bb90  55                                 push       ebp
0046bb91  8b ec                              mov        ebp, esp
0046bb93  6a ff                              push       -1
0046bb95  68 d5 8d 56 00                     push       0x568dd5
0046bb9a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0046bba0  50                                 push       eax
0046bba1  81 ec 88 00 00 00                  sub        esp, 0x88
0046bba7  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0046bbac  33 c5                              xor        eax, ebp
0046bbae  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0046bbb1  50                                 push       eax
0046bbb2  8d 45 f4                           lea        eax, [ebp - 0xc]
0046bbb5  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0046bbbb  89 4d c8                           mov        dword ptr [ebp - 0x38], ecx
0046bbbe  6a 12                              push       0x12
0046bbc0  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0046bbc5  e8 46 07 fa ff                     call       0x40c310
0046bbca  50                                 push       eax
0046bbcb  8d 4d d8                           lea        ecx, [ebp - 0x28]
0046bbce  e8 5d fb f9 ff                     call       0x40b730
0046bbd3  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0046bbda  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0046bbdd  05 0c a2 01 00                     add        eax, 0x1a20c
0046bbe2  89 45 d0                           mov        dword ptr [ebp - 0x30], eax
0046bbe5  6a 08                              push       8
0046bbe7  8d 4d e8                           lea        ecx, [ebp - 0x18]
0046bbea  e8 91 04 fa ff                     call       0x40c080
0046bbef  8d 4d e8                           lea        ecx, [ebp - 0x18]
0046bbf2  51                                 push       ecx
0046bbf3  8b 4d d0                           mov        ecx, dword ptr [ebp - 0x30]
0046bbf6  e8 85 66 fa ff                     call       0x412280
0046bbfb  c6 45 fc 01                        mov        byte ptr [ebp - 4], 1
0046bbff  8b 4d d0                           mov        ecx, dword ptr [ebp - 0x30]
0046bc02  e8 39 69 fa ff                     call       0x412540
0046bc07  89 45 c4                           mov        dword ptr [ebp - 0x3c], eax
0046bc0a  eb 09                              jmp        0x46bc15
0046bc0c  8d 4d e8                           lea        ecx, [ebp - 0x18]
0046bc0f  e8 1c 60 fa ff                     call       0x411c30
0046bc14  90                                 nop        
0046bc15  8b 55 c4                           mov        edx, dword ptr [ebp - 0x3c]
0046bc18  52                                 push       edx
0046bc19  8d 4d e8                           lea        ecx, [ebp - 0x18]
0046bc1c  e8 8f 5f fa ff                     call       0x411bb0
0046bc21  0f b6 c0                           movzx      eax, al
0046bc24  85 c0                              test       eax, eax
0046bc26  0f 84 0d 04 00 00                  je         0x46c039
0046bc2c  8d 4d e8                           lea        ecx, [ebp - 0x18]
0046bc2f  e8 cc 06 fa ff                     call       0x40c300
0046bc34  89 45 d4                           mov        dword ptr [ebp - 0x2c], eax
0046bc37  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bc3a  e8 21 35 fa ff                     call       0x40f160
0046bc3f  8b 08                              mov        ecx, dword ptr [eax]
0046bc41  8b 91 24 06 00 00                  mov        edx, dword ptr [ecx + 0x624]
0046bc47  3b 55 08                           cmp        edx, dword ptr [ebp + 8]
0046bc4a  0f 85 e4 03 00 00                  jne        0x46c034
0046bc50  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bc53  e8 08 35 fa ff                     call       0x40f160
0046bc58  8b 00                              mov        eax, dword ptr [eax]
0046bc5a  83 b8 20 06 00 00 00               cmp        dword ptr [eax + 0x620], 0
0046bc61  7f 4f                              jg         0x46bcb2
0046bc63  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bc66  e8 f5 34 fa ff                     call       0x40f160
0046bc6b  8b 08                              mov        ecx, dword ptr [eax]
0046bc6d  0f b6 91 78 06 00 00               movzx      edx, byte ptr [ecx + 0x678]
0046bc74  85 d2                              test       edx, edx
0046bc76  74 1d                              je         0x46bc95
0046bc78  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bc7b  e8 e0 34 fa ff                     call       0x40f160
0046bc80  89 45 c0                           mov        dword ptr [ebp - 0x40], eax
0046bc83  8b 45 c0                           mov        eax, dword ptr [ebp - 0x40]
0046bc86  50                                 push       eax
0046bc87  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0046bc8d  e8 fe e3 ff ff                     call       0x46a090
0046bc92  90                                 nop        
0046bc93  eb 18                              jmp        0x46bcad
0046bc95  6a 01                              push       1
0046bc97  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bc9a  e8 c1 34 fa ff                     call       0x40f160
0046bc9f  8b 08                              mov        ecx, dword ptr [eax]
0046bca1  81 c1 80 06 00 00                  add        ecx, 0x680
0046bca7  e8 f4 fd f9 ff                     call       0x40baa0
0046bcac  90                                 nop        
0046bcad  e9 82 03 00 00                     jmp        0x46c034
0046bcb2  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bcb5  e8 a6 34 fa ff                     call       0x40f160
0046bcba  8b 08                              mov        ecx, dword ptr [eax]
0046bcbc  83 b9 20 06 00 00 00               cmp        dword ptr [ecx + 0x620], 0
0046bcc3  0f 8e 6b 03 00 00                  jle        0x46c034
0046bcc9  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bccc  e8 8f 34 fa ff                     call       0x40f160
0046bcd1  8b 10                              mov        edx, dword ptr [eax]
0046bcd3  0f b6 82 78 06 00 00               movzx      eax, byte ptr [edx + 0x678]
0046bcda  85 c0                              test       eax, eax
0046bcdc  74 13                              je         0x46bcf1
0046bcde  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bce1  e8 7a 34 fa ff                     call       0x40f160
0046bce6  8b 08                              mov        ecx, dword ptr [eax]
0046bce8  83 b9 7c 06 00 00 00               cmp        dword ptr [ecx + 0x67c], 0
0046bcef  74 32                              je         0x46bd23
0046bcf1  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bcf4  e8 67 34 fa ff                     call       0x40f160
0046bcf9  8b 10                              mov        edx, dword ptr [eax]
0046bcfb  83 ba 7c 06 00 00 00               cmp        dword ptr [edx + 0x67c], 0
0046bd02  0f 84 0c 03 00 00                  je         0x46c014
0046bd08  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bd0b  e8 50 34 fa ff                     call       0x40f160
0046bd10  8b 00                              mov        eax, dword ptr [eax]
0046bd12  8b 88 7c 06 00 00                  mov        ecx, dword ptr [eax + 0x67c]
0046bd18  0f b6 11                           movzx      edx, byte ptr [ecx]
0046bd1b  85 d2                              test       edx, edx
0046bd1d  0f 84 f1 02 00 00                  je         0x46c014
0046bd23  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bd26  e8 35 34 fa ff                     call       0x40f160
0046bd2b  8b 00                              mov        eax, dword ptr [eax]
0046bd2d  83 c0 14                           add        eax, 0x14
0046bd30  89 45 b4                           mov        dword ptr [ebp - 0x4c], eax
0046bd33  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bd36  e8 25 34 fa ff                     call       0x40f160
0046bd3b  8b 08                              mov        ecx, dword ptr [eax]
0046bd3d  f3 0f 10 81 5c 06 00 00            movss      xmm0, dword ptr [ecx + 0x65c]
0046bd45  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046bd4d  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046bd55  f3 0f 11 45 bc                     movss      dword ptr [ebp - 0x44], xmm0
0046bd5a  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bd5d  e8 fe 33 fa ff                     call       0x40f160
0046bd62  8b 10                              mov        edx, dword ptr [eax]
0046bd64  f3 0f 10 82 58 06 00 00            movss      xmm0, dword ptr [edx + 0x658]
0046bd6c  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046bd74  f3 0f 5e 05 d0 c8 56 00            divss      xmm0, dword ptr [0x56c8d0]
0046bd7c  f3 0f 11 45 b8                     movss      dword ptr [ebp - 0x48], xmm0
0046bd81  51                                 push       ecx
0046bd82  f3 0f 10 45 bc                     movss      xmm0, dword ptr [ebp - 0x44]
0046bd87  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046bd8c  51                                 push       ecx
0046bd8d  f3 0f 10 45 b8                     movss      xmm0, dword ptr [ebp - 0x48]
0046bd92  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046bd97  8b 4d b4                           mov        ecx, dword ptr [ebp - 0x4c]
0046bd9a  e8 b1 50 00 00                     call       0x470e50
0046bd9f  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bda2  e8 b9 33 fa ff                     call       0x40f160
0046bda7  8b 00                              mov        eax, dword ptr [eax]
0046bda9  83 c0 14                           add        eax, 0x14
0046bdac  89 45 a8                           mov        dword ptr [ebp - 0x58], eax
0046bdaf  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bdb2  e8 a9 33 fa ff                     call       0x40f160
0046bdb7  8b 08                              mov        ecx, dword ptr [eax]
0046bdb9  8b 91 64 06 00 00                  mov        edx, dword ptr [ecx + 0x664]
0046bdbf  89 55 b0                           mov        dword ptr [ebp - 0x50], edx
0046bdc2  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bdc5  e8 96 33 fa ff                     call       0x40f160
0046bdca  8b 00                              mov        eax, dword ptr [eax]
0046bdcc  8b 88 60 06 00 00                  mov        ecx, dword ptr [eax + 0x660]
0046bdd2  89 4d ac                           mov        dword ptr [ebp - 0x54], ecx
0046bdd5  8b 55 b0                           mov        edx, dword ptr [ebp - 0x50]
0046bdd8  52                                 push       edx
0046bdd9  8b 45 ac                           mov        eax, dword ptr [ebp - 0x54]
0046bddc  50                                 push       eax
0046bddd  8b 4d a8                           mov        ecx, dword ptr [ebp - 0x58]
0046bde0  e8 9b 4c 00 00                     call       0x470a80
0046bde5  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bde8  e8 73 33 fa ff                     call       0x40f160
0046bded  8b 08                              mov        ecx, dword ptr [eax]
0046bdef  83 c1 14                           add        ecx, 0x14
0046bdf2  89 4d a0                           mov        dword ptr [ebp - 0x60], ecx
0046bdf5  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bdf8  e8 63 33 fa ff                     call       0x40f160
0046bdfd  8b 08                              mov        ecx, dword ptr [eax]
0046bdff  81 c1 6c 06 00 00                  add        ecx, 0x66c
0046be05  e8 d6 d4 fb ff                     call       0x4292e0 ; candidate_rate_access
0046be0a  d9 5d a4                           fstp       dword ptr [ebp - 0x5c]
0046be0d  51                                 push       ecx
0046be0e  f3 0f 10 45 a4                     movss      xmm0, dword ptr [ebp - 0x5c]
0046be13  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
0046be18  8b 4d a0                           mov        ecx, dword ptr [ebp - 0x60]
0046be1b  e8 70 47 fe ff                     call       0x450590
0046be20  90                                 nop        
0046be21  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046be24  e8 37 33 fa ff                     call       0x40f160
0046be29  8b 10                              mov        edx, dword ptr [eax]
0046be2b  83 ba 70 06 00 00 00               cmp        dword ptr [edx + 0x670], 0
0046be32  0f 84 fa 00 00 00                  je         0x46bf32
0046be38  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046be3b  e8 20 33 fa ff                     call       0x40f160
0046be40  8b 00                              mov        eax, dword ptr [eax]
0046be42  05 14 06 00 00                     add        eax, 0x614
0046be47  8b 08                              mov        ecx, dword ptr [eax]
0046be49  89 4d dc                           mov        dword ptr [ebp - 0x24], ecx
0046be4c  8b 50 04                           mov        edx, dword ptr [eax + 4]
0046be4f  89 55 e0                           mov        dword ptr [ebp - 0x20], edx
0046be52  8b 40 08                           mov        eax, dword ptr [eax + 8]
0046be55  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
0046be58  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
0046be60  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046be68  f3 0f 58 45 dc                     addss      xmm0, dword ptr [ebp - 0x24]
0046be6d  f3 0f 11 45 dc                     movss      dword ptr [ebp - 0x24], xmm0
0046be72  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
0046be7a  f3 0f 59 05 18 88 5b 00            mulss      xmm0, dword ptr [0x5b8818]
0046be82  f3 0f 58 45 e0                     addss      xmm0, dword ptr [ebp - 0x20]
0046be87  f3 0f 11 45 e0                     movss      dword ptr [ebp - 0x20], xmm0
0046be8c  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046be8f  e8 cc 32 fa ff                     call       0x40f160
0046be94  8b 08                              mov        ecx, dword ptr [eax]
0046be96  83 c1 14                           add        ecx, 0x14
0046be99  89 4d 9c                           mov        dword ptr [ebp - 0x64], ecx
0046be9c  8d 55 dc                           lea        edx, [ebp - 0x24]
0046be9f  52                                 push       edx
0046bea0  8b 4d 9c                           mov        ecx, dword ptr [ebp - 0x64]
0046bea3  e8 b8 1e ff ff                     call       0x45dd60
0046bea8  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046beab  e8 b0 32 fa ff                     call       0x40f160
0046beb0  8b 00                              mov        eax, dword ptr [eax]
0046beb2  83 c0 14                           add        eax, 0x14
0046beb5  89 45 98                           mov        dword ptr [ebp - 0x68], eax
0046beb8  68 00 00 00 c0                     push       0xc0000000
0046bebd  8b 4d 98                           mov        ecx, dword ptr [ebp - 0x68]
0046bec0  e8 4b 3b fe ff                     call       0x44fa10
0046bec5  90                                 nop        
0046bec6  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bec9  e8 92 32 fa ff                     call       0x40f160
0046bece  8b 08                              mov        ecx, dword ptr [eax]
0046bed0  81 c1 6c 06 00 00                  add        ecx, 0x66c
0046bed6  e8 05 d4 fb ff                     call       0x4292e0 ; candidate_rate_access
0046bedb  d9 5d 94                           fstp       dword ptr [ebp - 0x6c]
0046bede  f3 0f 10 45 94                     movss      xmm0, dword ptr [ebp - 0x6c]
0046bee3  0f 2e 05 e8 e0 56 00               ucomiss    xmm0, dword ptr [0x56e0e8]
0046beea  9f                                 lahf       
0046beeb  f6 c4 44                           test       ah, 0x44
0046beee  7a 22                              jp         0x46bf12
0046bef0  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bef3  e8 68 32 fa ff                     call       0x40f160
0046bef8  8b 08                              mov        ecx, dword ptr [eax]
0046befa  83 c1 14                           add        ecx, 0x14
0046befd  89 4d 90                           mov        dword ptr [ebp - 0x70], ecx
0046bf00  8b 55 90                           mov        edx, dword ptr [ebp - 0x70]
0046bf03  52                                 push       edx
0046bf04  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0046bf0a  e8 41 36 fd ff                     call       0x43f550
0046bf0f  90                                 nop        
0046bf10  eb 20                              jmp        0x46bf32
0046bf12  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bf15  e8 46 32 fa ff                     call       0x40f160
0046bf1a  8b 00                              mov        eax, dword ptr [eax]
0046bf1c  83 c0 14                           add        eax, 0x14
0046bf1f  89 45 8c                           mov        dword ptr [ebp - 0x74], eax
0046bf22  8b 4d 8c                           mov        ecx, dword ptr [ebp - 0x74]
0046bf25  51                                 push       ecx
0046bf26  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0046bf2c  e8 ff 36 fd ff                     call       0x43f630
0046bf31  90                                 nop        
0046bf32  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bf35  e8 26 32 fa ff                     call       0x40f160
0046bf3a  8b 10                              mov        edx, dword ptr [eax]
0046bf3c  83 c2 14                           add        edx, 0x14
0046bf3f  89 55 84                           mov        dword ptr [ebp - 0x7c], edx
0046bf42  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bf45  e8 16 32 fa ff                     call       0x40f160
0046bf4a  8b 00                              mov        eax, dword ptr [eax]
0046bf4c  05 14 06 00 00                     add        eax, 0x614
0046bf51  89 45 88                           mov        dword ptr [ebp - 0x78], eax
0046bf54  8b 4d 88                           mov        ecx, dword ptr [ebp - 0x78]
0046bf57  51                                 push       ecx
0046bf58  8b 4d 84                           mov        ecx, dword ptr [ebp - 0x7c]
0046bf5b  e8 00 1e ff ff                     call       0x45dd60
0046bf60  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bf63  e8 f8 31 fa ff                     call       0x40f160
0046bf68  8b 10                              mov        edx, dword ptr [eax]
0046bf6a  83 c2 14                           add        edx, 0x14
0046bf6d  89 95 7c ff ff ff                  mov        dword ptr [ebp - 0x84], edx
0046bf73  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bf76  e8 e5 31 fa ff                     call       0x40f160
0046bf7b  8b 00                              mov        eax, dword ptr [eax]
0046bf7d  8b 88 4c 06 00 00                  mov        ecx, dword ptr [eax + 0x64c]
0046bf83  89 4d 80                           mov        dword ptr [ebp - 0x80], ecx
0046bf86  8b 55 80                           mov        edx, dword ptr [ebp - 0x80]
0046bf89  52                                 push       edx
0046bf8a  8b 8d 7c ff ff ff                  mov        ecx, dword ptr [ebp - 0x84]
0046bf90  e8 7b 3a fe ff                     call       0x44fa10
0046bf95  90                                 nop        
0046bf96  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bf99  e8 c2 31 fa ff                     call       0x40f160
0046bf9e  8b 08                              mov        ecx, dword ptr [eax]
0046bfa0  81 c1 6c 06 00 00                  add        ecx, 0x66c
0046bfa6  e8 35 d3 fb ff                     call       0x4292e0 ; candidate_rate_access
0046bfab  d9 9d 78 ff ff ff                  fstp       dword ptr [ebp - 0x88]
0046bfb1  f3 0f 10 85 78 ff ff ff            movss      xmm0, dword ptr [ebp - 0x88]
0046bfb9  0f 2e 05 e8 e0 56 00               ucomiss    xmm0, dword ptr [0x56e0e8]
0046bfc0  9f                                 lahf       
0046bfc1  f6 c4 44                           test       ah, 0x44
0046bfc4  7a 28                              jp         0x46bfee
0046bfc6  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bfc9  e8 92 31 fa ff                     call       0x40f160
0046bfce  8b 00                              mov        eax, dword ptr [eax]
0046bfd0  83 c0 14                           add        eax, 0x14
0046bfd3  89 85 74 ff ff ff                  mov        dword ptr [ebp - 0x8c], eax
0046bfd9  8b 8d 74 ff ff ff                  mov        ecx, dword ptr [ebp - 0x8c]
0046bfdf  51                                 push       ecx
0046bfe0  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0046bfe6  e8 65 35 fd ff                     call       0x43f550
0046bfeb  90                                 nop        
0046bfec  eb 26                              jmp        0x46c014
0046bfee  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046bff1  e8 6a 31 fa ff                     call       0x40f160
0046bff6  8b 10                              mov        edx, dword ptr [eax]
0046bff8  83 c2 14                           add        edx, 0x14
0046bffb  89 95 70 ff ff ff                  mov        dword ptr [ebp - 0x90], edx
0046c001  8b 85 70 ff ff ff                  mov        eax, dword ptr [ebp - 0x90]
0046c007  50                                 push       eax
0046c008  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0046c00e  e8 1d 36 fd ff                     call       0x43f630
0046c013  90                                 nop        
0046c014  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0046c017  e8 44 31 fa ff                     call       0x40f160
0046c01c  8b 08                              mov        ecx, dword ptr [eax]
0046c01e  81 c1 20 06 00 00                  add        ecx, 0x620
0046c024  89 4d cc                           mov        dword ptr [ebp - 0x34], ecx
0046c027  8b 55 cc                           mov        edx, dword ptr [ebp - 0x34]
0046c02a  8b 02                              mov        eax, dword ptr [edx]
0046c02c  83 e8 01                           sub        eax, 1
0046c02f  8b 4d cc                           mov        ecx, dword ptr [ebp - 0x34]
0046c032  89 01                              mov        dword ptr [ecx], eax
0046c034  e9 d3 fb ff ff                     jmp        0x46bc0c
0046c039  c6 45 fc 00                        mov        byte ptr [ebp - 4], 0
0046c03d  8d 4d e8                           lea        ecx, [ebp - 0x18]
0046c040  e8 bb 5a fa ff                     call       0x411b00
0046c045  90                                 nop        
0046c046  c7 85 6c ff ff ff 00 00 00 00      mov        dword ptr [ebp - 0x94], 0
0046c050  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0046c057  8d 4d d8                           lea        ecx, [ebp - 0x28]
0046c05a  e8 a1 f8 f9 ff                     call       0x40b900
0046c05f  8b 85 6c ff ff ff                  mov        eax, dword ptr [ebp - 0x94]
0046c065  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0046c068  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0046c06f  59                                 pop        ecx
0046c070  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046c073  33 cd                              xor        ecx, ebp
0046c075  e8 74 69 0d 00                     call       0x5429ee
0046c07a  8b e5                              mov        esp, ebp
0046c07c  5d                                 pop        ebp
0046c07d  c2 04 00                           ret        4
