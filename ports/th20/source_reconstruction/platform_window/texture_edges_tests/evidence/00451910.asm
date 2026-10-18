00451910  55                                 push       ebp
00451911  8b ec                              mov        ebp, esp
00451913  81 ec b4 00 00 00                  sub        esp, 0xb4
00451919  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0045191e  33 c5                              xor        eax, ebp
00451920  89 45 fc                           mov        dword ptr [ebp - 4], eax
00451923  56                                 push       esi
00451924  89 8d 4c ff ff ff                  mov        dword ptr [ebp - 0xb4], ecx
0045192a  c7 45 98 00 00 00 00               mov        dword ptr [ebp - 0x68], 0
00451931  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00451934  8b 08                              mov        ecx, dword ptr [eax]
00451936  8b 51 48                           mov        edx, dword ptr [ecx + 0x48]
00451939  89 95 58 ff ff ff                  mov        dword ptr [ebp - 0xa8], edx
0045193f  8d 45 98                           lea        eax, [ebp - 0x68]
00451942  50                                 push       eax
00451943  6a 00                              push       0
00451945  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00451948  51                                 push       ecx
00451949  ff 95 58 ff ff ff                  call       dword ptr [ebp - 0xa8]
0045194f  90                                 nop        
00451950  83 7d 98 00                        cmp        dword ptr [ebp - 0x68], 0
00451954  75 05                              jne        0x45195b
00451956  e9 5d 0a 00 00                     jmp        0x4523b8
0045195b  8b 55 98                           mov        edx, dword ptr [ebp - 0x68]
0045195e  8b 02                              mov        eax, dword ptr [edx]
00451960  8b 48 30                           mov        ecx, dword ptr [eax + 0x30]
00451963  89 8d 54 ff ff ff                  mov        dword ptr [ebp - 0xac], ecx
00451969  8d 55 9c                           lea        edx, [ebp - 0x64]
0045196c  52                                 push       edx
0045196d  8b 45 98                           mov        eax, dword ptr [ebp - 0x68]
00451970  50                                 push       eax
00451971  ff 95 54 ff ff ff                  call       dword ptr [ebp - 0xac]
00451977  8b 4d 98                           mov        ecx, dword ptr [ebp - 0x68]
0045197a  8b 11                              mov        edx, dword ptr [ecx]
0045197c  8b 42 34                           mov        eax, dword ptr [edx + 0x34]
0045197f  89 85 50 ff ff ff                  mov        dword ptr [ebp - 0xb0], eax
00451985  6a 00                              push       0
00451987  6a 00                              push       0
00451989  8d 4d 90                           lea        ecx, [ebp - 0x70]
0045198c  51                                 push       ecx
0045198d  8b 55 98                           mov        edx, dword ptr [ebp - 0x68]
00451990  52                                 push       edx
00451991  ff 95 50 ff ff ff                  call       dword ptr [ebp - 0xb0]
00451997  90                                 nop        
00451998  8b 45 9c                           mov        eax, dword ptr [ebp - 0x64]
0045199b  89 85 5c ff ff ff                  mov        dword ptr [ebp - 0xa4], eax
004519a1  83 bd 5c ff ff ff 1d               cmp        dword ptr [ebp - 0xa4], 0x1d
004519a8  0f 87 ed 09 00 00                  ja         0x45239b
004519ae  8b 8d 5c ff ff ff                  mov        ecx, dword ptr [ebp - 0xa4]
004519b4  0f b6 91 e0 23 45 00               movzx      edx, byte ptr [ecx + 0x4523e0]
004519bb  ff 24 95 cc 23 45 00               jmp        dword ptr [edx*4 + 0x4523cc]
004519c2  33 c0                              xor        eax, eax
004519c4  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
004519c7  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
004519ca  89 45 f8                           mov        dword ptr [ebp - 8], eax
004519cd  c7 85 7c ff ff ff 00 00 00 00      mov        dword ptr [ebp - 0x84], 0
004519d7  eb 0f                              jmp        0x4519e8
004519d9  8b 8d 7c ff ff ff                  mov        ecx, dword ptr [ebp - 0x84]
004519df  83 c1 01                           add        ecx, 1
004519e2  89 8d 7c ff ff ff                  mov        dword ptr [ebp - 0x84], ecx
004519e8  8b 95 7c ff ff ff                  mov        edx, dword ptr [ebp - 0x84]
004519ee  3b 55 b8                           cmp        edx, dword ptr [ebp - 0x48]
004519f1  0f 83 f3 01 00 00                  jae        0x451bea
004519f7  8b 45 90                           mov        eax, dword ptr [ebp - 0x70]
004519fa  0f af 85 7c ff ff ff               imul       eax, dword ptr [ebp - 0x84]
00451a01  03 45 94                           add        eax, dword ptr [ebp - 0x6c]
00451a04  89 45 80                           mov        dword ptr [ebp - 0x80], eax
00451a07  c7 85 6c ff ff ff 00 00 00 00      mov        dword ptr [ebp - 0x94], 0
00451a11  eb 0f                              jmp        0x451a22
00451a13  8b 8d 6c ff ff ff                  mov        ecx, dword ptr [ebp - 0x94]
00451a19  83 c1 01                           add        ecx, 1
00451a1c  89 8d 6c ff ff ff                  mov        dword ptr [ebp - 0x94], ecx
00451a22  8b 95 6c ff ff ff                  mov        edx, dword ptr [ebp - 0x94]
00451a28  3b 55 b4                           cmp        edx, dword ptr [ebp - 0x4c]
00451a2b  0f 83 b4 01 00 00                  jae        0x451be5
00451a31  b8 04 00 00 00                     mov        eax, 4
00451a36  6b c8 00                           imul       ecx, eax, 0
00451a39  8b 55 80                           mov        edx, dword ptr [ebp - 0x80]
00451a3c  0f b6 44 0a 03                     movzx      eax, byte ptr [edx + ecx + 3]
00451a41  85 c0                              test       eax, eax
00451a43  0f 85 8e 01 00 00                  jne        0x451bd7
00451a49  b9 04 00 00 00                     mov        ecx, 4
00451a4e  d1 e1                              shl        ecx, 1
00451a50  c7 44 0d f0 00 00 00 00            mov        dword ptr [ebp + ecx - 0x10], 0
00451a58  ba 04 00 00 00                     mov        edx, 4
00451a5d  c1 e2 00                           shl        edx, 0
00451a60  c7 44 15 f0 00 00 00 00            mov        dword ptr [ebp + edx - 0x10], 0
00451a68  b8 04 00 00 00                     mov        eax, 4
00451a6d  6b c8 00                           imul       ecx, eax, 0
00451a70  c7 44 0d f0 00 00 00 00            mov        dword ptr [ebp + ecx - 0x10], 0
00451a78  c7 45 c8 00 00 00 00               mov        dword ptr [ebp - 0x38], 0
00451a7f  83 bd 6c ff ff ff 00               cmp        dword ptr [ebp - 0x94], 0
00451a86  76 1c                              jbe        0x451aa4
00451a88  8d 55 c8                           lea        edx, [ebp - 0x38]
00451a8b  52                                 push       edx
00451a8c  b8 04 00 00 00                     mov        eax, 4
00451a91  6b c8 ff                           imul       ecx, eax, -1
00451a94  03 4d 80                           add        ecx, dword ptr [ebp - 0x80]
00451a97  51                                 push       ecx
00451a98  8d 55 f0                           lea        edx, [ebp - 0x10]
00451a9b  52                                 push       edx
00451a9c  e8 df fd ff ff                     call       0x451880
00451aa1  83 c4 0c                           add        esp, 0xc
00451aa4  8b 45 b4                           mov        eax, dword ptr [ebp - 0x4c]
00451aa7  83 e8 01                           sub        eax, 1
00451aaa  39 85 6c ff ff ff                  cmp        dword ptr [ebp - 0x94], eax
00451ab0  73 1c                              jae        0x451ace
00451ab2  8d 4d c8                           lea        ecx, [ebp - 0x38]
00451ab5  51                                 push       ecx
00451ab6  ba 04 00 00 00                     mov        edx, 4
00451abb  c1 e2 00                           shl        edx, 0
00451abe  03 55 80                           add        edx, dword ptr [ebp - 0x80]
00451ac1  52                                 push       edx
00451ac2  8d 45 f0                           lea        eax, [ebp - 0x10]
00451ac5  50                                 push       eax
00451ac6  e8 b5 fd ff ff                     call       0x451880
00451acb  83 c4 0c                           add        esp, 0xc
00451ace  83 bd 7c ff ff ff 00               cmp        dword ptr [ebp - 0x84], 0
00451ad5  76 25                              jbe        0x451afc
00451ad7  8d 4d c8                           lea        ecx, [ebp - 0x38]
00451ada  51                                 push       ecx
00451adb  8b 45 90                           mov        eax, dword ptr [ebp - 0x70]
00451ade  f7 d8                              neg        eax
00451ae0  99                                 cdq        
00451ae1  83 e2 03                           and        edx, 3
00451ae4  03 c2                              add        eax, edx
00451ae6  c1 f8 02                           sar        eax, 2
00451ae9  8b 55 80                           mov        edx, dword ptr [ebp - 0x80]
00451aec  8d 04 82                           lea        eax, [edx + eax*4]
00451aef  50                                 push       eax
00451af0  8d 4d f0                           lea        ecx, [ebp - 0x10]
00451af3  51                                 push       ecx
00451af4  e8 87 fd ff ff                     call       0x451880
00451af9  83 c4 0c                           add        esp, 0xc
00451afc  8b 55 b8                           mov        edx, dword ptr [ebp - 0x48]
00451aff  83 ea 01                           sub        edx, 1
00451b02  39 95 7c ff ff ff                  cmp        dword ptr [ebp - 0x84], edx
00451b08  73 23                              jae        0x451b2d
00451b0a  8d 45 c8                           lea        eax, [ebp - 0x38]
00451b0d  50                                 push       eax
00451b0e  8b 45 90                           mov        eax, dword ptr [ebp - 0x70]
00451b11  99                                 cdq        
00451b12  83 e2 03                           and        edx, 3
00451b15  03 c2                              add        eax, edx
00451b17  c1 f8 02                           sar        eax, 2
00451b1a  8b 4d 80                           mov        ecx, dword ptr [ebp - 0x80]
00451b1d  8d 14 81                           lea        edx, [ecx + eax*4]
00451b20  52                                 push       edx
00451b21  8d 45 f0                           lea        eax, [ebp - 0x10]
00451b24  50                                 push       eax
00451b25  e8 56 fd ff ff                     call       0x451880
00451b2a  83 c4 0c                           add        esp, 0xc
00451b2d  83 7d c8 01                        cmp        dword ptr [ebp - 0x38], 1
00451b31  76 55                              jbe        0x451b88
00451b33  b9 04 00 00 00                     mov        ecx, 4
00451b38  6b d1 00                           imul       edx, ecx, 0
00451b3b  8b 44 15 f0                        mov        eax, dword ptr [ebp + edx - 0x10]
00451b3f  33 d2                              xor        edx, edx
00451b41  f7 75 c8                           div        dword ptr [ebp - 0x38]
00451b44  b9 04 00 00 00                     mov        ecx, 4
00451b49  6b d1 00                           imul       edx, ecx, 0
00451b4c  89 44 15 f0                        mov        dword ptr [ebp + edx - 0x10], eax
00451b50  b8 04 00 00 00                     mov        eax, 4
00451b55  c1 e0 00                           shl        eax, 0
00451b58  8b 44 05 f0                        mov        eax, dword ptr [ebp + eax - 0x10]
00451b5c  33 d2                              xor        edx, edx
00451b5e  f7 75 c8                           div        dword ptr [ebp - 0x38]
00451b61  b9 04 00 00 00                     mov        ecx, 4
00451b66  c1 e1 00                           shl        ecx, 0
00451b69  89 44 0d f0                        mov        dword ptr [ebp + ecx - 0x10], eax
00451b6d  ba 04 00 00 00                     mov        edx, 4
00451b72  d1 e2                              shl        edx, 1
00451b74  8b 44 15 f0                        mov        eax, dword ptr [ebp + edx - 0x10]
00451b78  33 d2                              xor        edx, edx
00451b7a  f7 75 c8                           div        dword ptr [ebp - 0x38]
00451b7d  b9 04 00 00 00                     mov        ecx, 4
00451b82  d1 e1                              shl        ecx, 1
00451b84  89 44 0d f0                        mov        dword ptr [ebp + ecx - 0x10], eax
00451b88  ba 04 00 00 00                     mov        edx, 4
00451b8d  6b c2 00                           imul       eax, edx, 0
00451b90  b9 04 00 00 00                     mov        ecx, 4
00451b95  6b d1 00                           imul       edx, ecx, 0
00451b98  8b 4d 80                           mov        ecx, dword ptr [ebp - 0x80]
00451b9b  8a 44 05 f0                        mov        al, byte ptr [ebp + eax - 0x10]
00451b9f  88 44 11 02                        mov        byte ptr [ecx + edx + 2], al
00451ba3  b9 04 00 00 00                     mov        ecx, 4
00451ba8  c1 e1 00                           shl        ecx, 0
00451bab  ba 04 00 00 00                     mov        edx, 4
00451bb0  6b c2 00                           imul       eax, edx, 0
00451bb3  8b 55 80                           mov        edx, dword ptr [ebp - 0x80]
00451bb6  8a 4c 0d f0                        mov        cl, byte ptr [ebp + ecx - 0x10]
00451bba  88 4c 02 01                        mov        byte ptr [edx + eax + 1], cl
00451bbe  ba 04 00 00 00                     mov        edx, 4
00451bc3  d1 e2                              shl        edx, 1
00451bc5  b8 04 00 00 00                     mov        eax, 4
00451bca  6b c8 00                           imul       ecx, eax, 0
00451bcd  8b 45 80                           mov        eax, dword ptr [ebp - 0x80]
00451bd0  8a 54 15 f0                        mov        dl, byte ptr [ebp + edx - 0x10]
00451bd4  88 14 08                           mov        byte ptr [eax + ecx], dl
00451bd7  8b 45 80                           mov        eax, dword ptr [ebp - 0x80]
00451bda  83 c0 04                           add        eax, 4
00451bdd  89 45 80                           mov        dword ptr [ebp - 0x80], eax
00451be0  e9 2e fe ff ff                     jmp        0x451a13
00451be5  e9 ef fd ff ff                     jmp        0x4519d9
00451bea  e9 ac 07 00 00                     jmp        0x45239b
00451bef  33 c9                              xor        ecx, ecx
00451bf1  89 4d e4                           mov        dword ptr [ebp - 0x1c], ecx
00451bf4  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
00451bf7  89 4d ec                           mov        dword ptr [ebp - 0x14], ecx
00451bfa  c7 85 78 ff ff ff 00 00 00 00      mov        dword ptr [ebp - 0x88], 0
00451c04  eb 0f                              jmp        0x451c15
00451c06  8b 95 78 ff ff ff                  mov        edx, dword ptr [ebp - 0x88]
00451c0c  83 c2 01                           add        edx, 1
00451c0f  89 95 78 ff ff ff                  mov        dword ptr [ebp - 0x88], edx
00451c15  8b 85 78 ff ff ff                  mov        eax, dword ptr [ebp - 0x88]
00451c1b  3b 45 b8                           cmp        eax, dword ptr [ebp - 0x48]
00451c1e  0f 83 55 02 00 00                  jae        0x451e79
00451c24  8b 4d 90                           mov        ecx, dword ptr [ebp - 0x70]
00451c27  0f af 8d 78 ff ff ff               imul       ecx, dword ptr [ebp - 0x88]
00451c2e  03 4d 94                           add        ecx, dword ptr [ebp - 0x6c]
00451c31  89 4d 8c                           mov        dword ptr [ebp - 0x74], ecx
00451c34  c7 85 68 ff ff ff 00 00 00 00      mov        dword ptr [ebp - 0x98], 0
00451c3e  eb 0f                              jmp        0x451c4f
00451c40  8b 95 68 ff ff ff                  mov        edx, dword ptr [ebp - 0x98]
00451c46  83 c2 01                           add        edx, 1
00451c49  89 95 68 ff ff ff                  mov        dword ptr [ebp - 0x98], edx
00451c4f  8b 85 68 ff ff ff                  mov        eax, dword ptr [ebp - 0x98]
00451c55  3b 45 b4                           cmp        eax, dword ptr [ebp - 0x4c]
00451c58  0f 83 16 02 00 00                  jae        0x451e74
00451c5e  b9 02 00 00 00                     mov        ecx, 2
00451c63  6b d1 00                           imul       edx, ecx, 0
00451c66  8b 45 8c                           mov        eax, dword ptr [ebp - 0x74]
00451c69  66 8b 0c 10                        mov        cx, word ptr [eax + edx]
00451c6d  66 c1 e9 0f                        shr        cx, 0xf
00451c71  66 83 e1 01                        and        cx, 1
00451c75  0f b7 d1                           movzx      edx, cx
00451c78  85 d2                              test       edx, edx
00451c7a  0f 85 e6 01 00 00                  jne        0x451e66
00451c80  b8 04 00 00 00                     mov        eax, 4
00451c85  d1 e0                              shl        eax, 1
00451c87  c7 44 05 e4 00 00 00 00            mov        dword ptr [ebp + eax - 0x1c], 0
00451c8f  b9 04 00 00 00                     mov        ecx, 4
00451c94  c1 e1 00                           shl        ecx, 0
00451c97  c7 44 0d e4 00 00 00 00            mov        dword ptr [ebp + ecx - 0x1c], 0
00451c9f  ba 04 00 00 00                     mov        edx, 4
00451ca4  6b c2 00                           imul       eax, edx, 0
00451ca7  c7 44 05 e4 00 00 00 00            mov        dword ptr [ebp + eax - 0x1c], 0
00451caf  c7 45 c4 00 00 00 00               mov        dword ptr [ebp - 0x3c], 0
00451cb6  83 bd 68 ff ff ff 00               cmp        dword ptr [ebp - 0x98], 0
00451cbd  76 1c                              jbe        0x451cdb
00451cbf  8d 4d c4                           lea        ecx, [ebp - 0x3c]
00451cc2  51                                 push       ecx
00451cc3  ba 02 00 00 00                     mov        edx, 2
00451cc8  6b c2 ff                           imul       eax, edx, -1
00451ccb  03 45 8c                           add        eax, dword ptr [ebp - 0x74]
00451cce  50                                 push       eax
00451ccf  8d 4d e4                           lea        ecx, [ebp - 0x1c]
00451cd2  51                                 push       ecx
00451cd3  e8 68 f9 ff ff                     call       0x451640
00451cd8  83 c4 0c                           add        esp, 0xc
00451cdb  8b 55 b4                           mov        edx, dword ptr [ebp - 0x4c]
00451cde  83 ea 01                           sub        edx, 1
00451ce1  39 95 68 ff ff ff                  cmp        dword ptr [ebp - 0x98], edx
00451ce7  73 1c                              jae        0x451d05
00451ce9  8d 45 c4                           lea        eax, [ebp - 0x3c]
00451cec  50                                 push       eax
00451ced  b9 02 00 00 00                     mov        ecx, 2
00451cf2  c1 e1 00                           shl        ecx, 0
00451cf5  03 4d 8c                           add        ecx, dword ptr [ebp - 0x74]
00451cf8  51                                 push       ecx
00451cf9  8d 55 e4                           lea        edx, [ebp - 0x1c]
00451cfc  52                                 push       edx
00451cfd  e8 3e f9 ff ff                     call       0x451640
00451d02  83 c4 0c                           add        esp, 0xc
00451d05  83 bd 78 ff ff ff 00               cmp        dword ptr [ebp - 0x88], 0
00451d0c  76 21                              jbe        0x451d2f
00451d0e  8d 45 c4                           lea        eax, [ebp - 0x3c]
00451d11  50                                 push       eax
00451d12  8b 45 90                           mov        eax, dword ptr [ebp - 0x70]
00451d15  f7 d8                              neg        eax
00451d17  99                                 cdq        
00451d18  2b c2                              sub        eax, edx
00451d1a  d1 f8                              sar        eax, 1
00451d1c  8b 4d 8c                           mov        ecx, dword ptr [ebp - 0x74]
00451d1f  8d 14 41                           lea        edx, [ecx + eax*2]
00451d22  52                                 push       edx
00451d23  8d 45 e4                           lea        eax, [ebp - 0x1c]
00451d26  50                                 push       eax
00451d27  e8 14 f9 ff ff                     call       0x451640
00451d2c  83 c4 0c                           add        esp, 0xc
00451d2f  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
00451d32  83 e9 01                           sub        ecx, 1
00451d35  39 8d 78 ff ff ff                  cmp        dword ptr [ebp - 0x88], ecx
00451d3b  73 1f                              jae        0x451d5c
00451d3d  8d 55 c4                           lea        edx, [ebp - 0x3c]
00451d40  52                                 push       edx
00451d41  8b 45 90                           mov        eax, dword ptr [ebp - 0x70]
00451d44  99                                 cdq        
00451d45  2b c2                              sub        eax, edx
00451d47  d1 f8                              sar        eax, 1
00451d49  8b 4d 8c                           mov        ecx, dword ptr [ebp - 0x74]
00451d4c  8d 14 41                           lea        edx, [ecx + eax*2]
00451d4f  52                                 push       edx
00451d50  8d 45 e4                           lea        eax, [ebp - 0x1c]
00451d53  50                                 push       eax
00451d54  e8 e7 f8 ff ff                     call       0x451640
00451d59  83 c4 0c                           add        esp, 0xc
00451d5c  83 7d c4 01                        cmp        dword ptr [ebp - 0x3c], 1
00451d60  76 55                              jbe        0x451db7
00451d62  b9 04 00 00 00                     mov        ecx, 4
00451d67  6b d1 00                           imul       edx, ecx, 0
00451d6a  8b 44 15 e4                        mov        eax, dword ptr [ebp + edx - 0x1c]
00451d6e  33 d2                              xor        edx, edx
00451d70  f7 75 c4                           div        dword ptr [ebp - 0x3c]
00451d73  b9 04 00 00 00                     mov        ecx, 4
00451d78  6b d1 00                           imul       edx, ecx, 0
00451d7b  89 44 15 e4                        mov        dword ptr [ebp + edx - 0x1c], eax
00451d7f  b8 04 00 00 00                     mov        eax, 4
00451d84  c1 e0 00                           shl        eax, 0
00451d87  8b 44 05 e4                        mov        eax, dword ptr [ebp + eax - 0x1c]
00451d8b  33 d2                              xor        edx, edx
00451d8d  f7 75 c4                           div        dword ptr [ebp - 0x3c]
00451d90  b9 04 00 00 00                     mov        ecx, 4
00451d95  c1 e1 00                           shl        ecx, 0
00451d98  89 44 0d e4                        mov        dword ptr [ebp + ecx - 0x1c], eax
00451d9c  ba 04 00 00 00                     mov        edx, 4
00451da1  d1 e2                              shl        edx, 1
00451da3  8b 44 15 e4                        mov        eax, dword ptr [ebp + edx - 0x1c]
00451da7  33 d2                              xor        edx, edx
00451da9  f7 75 c4                           div        dword ptr [ebp - 0x3c]
00451dac  b9 04 00 00 00                     mov        ecx, 4
00451db1  d1 e1                              shl        ecx, 1
00451db3  89 44 0d e4                        mov        dword ptr [ebp + ecx - 0x1c], eax
00451db7  ba 04 00 00 00                     mov        edx, 4
00451dbc  6b c2 00                           imul       eax, edx, 0
00451dbf  66 0f b6 4c 05 e4                  movzx      cx, byte ptr [ebp + eax - 0x1c]
00451dc5  ba 02 00 00 00                     mov        edx, 2
00451dca  6b c2 00                           imul       eax, edx, 0
00451dcd  66 83 e1 1f                        and        cx, 0x1f
00451dd1  66 c1 e1 0a                        shl        cx, 0xa
00451dd5  ba ff 83 00 00                     mov        edx, 0x83ff
00451dda  8b 75 8c                           mov        esi, dword ptr [ebp - 0x74]
00451ddd  66 23 14 06                        and        dx, word ptr [esi + eax]
00451de1  66 0b d1                           or         dx, cx
00451de4  b8 02 00 00 00                     mov        eax, 2
00451de9  6b c8 00                           imul       ecx, eax, 0
00451dec  8b 45 8c                           mov        eax, dword ptr [ebp - 0x74]
00451def  66 89 14 08                        mov        word ptr [eax + ecx], dx
00451df3  b9 04 00 00 00                     mov        ecx, 4
00451df8  c1 e1 00                           shl        ecx, 0
00451dfb  66 0f b6 54 0d e4                  movzx      dx, byte ptr [ebp + ecx - 0x1c]
00451e01  b8 02 00 00 00                     mov        eax, 2
00451e06  6b c8 00                           imul       ecx, eax, 0
00451e09  66 83 e2 1f                        and        dx, 0x1f
00451e0d  66 c1 e2 05                        shl        dx, 5
00451e11  b8 1f fc 00 00                     mov        eax, 0xfc1f
00451e16  8b 75 8c                           mov        esi, dword ptr [ebp - 0x74]
00451e19  66 23 04 0e                        and        ax, word ptr [esi + ecx]
00451e1d  66 0b c2                           or         ax, dx
00451e20  b9 02 00 00 00                     mov        ecx, 2
00451e25  6b d1 00                           imul       edx, ecx, 0
00451e28  8b 4d 8c                           mov        ecx, dword ptr [ebp - 0x74]
00451e2b  66 89 04 11                        mov        word ptr [ecx + edx], ax
00451e2f  ba 04 00 00 00                     mov        edx, 4
00451e34  d1 e2                              shl        edx, 1
00451e36  66 0f b6 44 15 e4                  movzx      ax, byte ptr [ebp + edx - 0x1c]
00451e3c  b9 02 00 00 00                     mov        ecx, 2
00451e41  6b d1 00                           imul       edx, ecx, 0
00451e44  66 83 e0 1f                        and        ax, 0x1f
00451e48  b9 e0 ff 00 00                     mov        ecx, 0xffe0
00451e4d  8b 75 8c                           mov        esi, dword ptr [ebp - 0x74]
00451e50  66 23 0c 16                        and        cx, word ptr [esi + edx]
00451e54  66 0b c8                           or         cx, ax
00451e57  ba 02 00 00 00                     mov        edx, 2
00451e5c  6b c2 00                           imul       eax, edx, 0
00451e5f  8b 55 8c                           mov        edx, dword ptr [ebp - 0x74]
00451e62  66 89 0c 02                        mov        word ptr [edx + eax], cx
00451e66  8b 45 8c                           mov        eax, dword ptr [ebp - 0x74]
00451e69  83 c0 02                           add        eax, 2
00451e6c  89 45 8c                           mov        dword ptr [ebp - 0x74], eax
00451e6f  e9 cc fd ff ff                     jmp        0x451c40
00451e74  e9 8d fd ff ff                     jmp        0x451c06
00451e79  e9 1d 05 00 00                     jmp        0x45239b
00451e7e  33 c9                              xor        ecx, ecx
00451e80  89 4d d8                           mov        dword ptr [ebp - 0x28], ecx
00451e83  89 4d dc                           mov        dword ptr [ebp - 0x24], ecx
00451e86  89 4d e0                           mov        dword ptr [ebp - 0x20], ecx
00451e89  c7 85 74 ff ff ff 00 00 00 00      mov        dword ptr [ebp - 0x8c], 0
00451e93  eb 0f                              jmp        0x451ea4
00451e95  8b 95 74 ff ff ff                  mov        edx, dword ptr [ebp - 0x8c]
00451e9b  83 c2 01                           add        edx, 1
00451e9e  89 95 74 ff ff ff                  mov        dword ptr [ebp - 0x8c], edx
00451ea4  8b 85 74 ff ff ff                  mov        eax, dword ptr [ebp - 0x8c]
00451eaa  3b 45 b8                           cmp        eax, dword ptr [ebp - 0x48]
00451ead  0f 83 55 02 00 00                  jae        0x452108
00451eb3  8b 4d 90                           mov        ecx, dword ptr [ebp - 0x70]
00451eb6  0f af 8d 74 ff ff ff               imul       ecx, dword ptr [ebp - 0x8c]
00451ebd  03 4d 94                           add        ecx, dword ptr [ebp - 0x6c]
00451ec0  89 4d 88                           mov        dword ptr [ebp - 0x78], ecx
00451ec3  c7 85 64 ff ff ff 00 00 00 00      mov        dword ptr [ebp - 0x9c], 0
00451ecd  eb 0f                              jmp        0x451ede
00451ecf  8b 95 64 ff ff ff                  mov        edx, dword ptr [ebp - 0x9c]
00451ed5  83 c2 01                           add        edx, 1
00451ed8  89 95 64 ff ff ff                  mov        dword ptr [ebp - 0x9c], edx
00451ede  8b 85 64 ff ff ff                  mov        eax, dword ptr [ebp - 0x9c]
00451ee4  3b 45 b4                           cmp        eax, dword ptr [ebp - 0x4c]
00451ee7  0f 83 16 02 00 00                  jae        0x452103
00451eed  b9 02 00 00 00                     mov        ecx, 2
00451ef2  6b d1 00                           imul       edx, ecx, 0
00451ef5  8b 45 88                           mov        eax, dword ptr [ebp - 0x78]
00451ef8  66 8b 0c 10                        mov        cx, word ptr [eax + edx]
00451efc  66 c1 e9 0c                        shr        cx, 0xc
00451f00  66 83 e1 0f                        and        cx, 0xf
00451f04  0f b7 d1                           movzx      edx, cx
00451f07  85 d2                              test       edx, edx
00451f09  0f 85 e6 01 00 00                  jne        0x4520f5
00451f0f  b8 04 00 00 00                     mov        eax, 4
00451f14  d1 e0                              shl        eax, 1
00451f16  c7 44 05 d8 00 00 00 00            mov        dword ptr [ebp + eax - 0x28], 0
00451f1e  b9 04 00 00 00                     mov        ecx, 4
00451f23  c1 e1 00                           shl        ecx, 0
00451f26  c7 44 0d d8 00 00 00 00            mov        dword ptr [ebp + ecx - 0x28], 0
00451f2e  ba 04 00 00 00                     mov        edx, 4
00451f33  6b c2 00                           imul       eax, edx, 0
00451f36  c7 44 05 d8 00 00 00 00            mov        dword ptr [ebp + eax - 0x28], 0
00451f3e  c7 45 c0 00 00 00 00               mov        dword ptr [ebp - 0x40], 0
00451f45  83 bd 64 ff ff ff 00               cmp        dword ptr [ebp - 0x9c], 0
00451f4c  76 1c                              jbe        0x451f6a
00451f4e  8d 4d c0                           lea        ecx, [ebp - 0x40]
00451f51  51                                 push       ecx
00451f52  ba 02 00 00 00                     mov        edx, 2
00451f57  6b c2 ff                           imul       eax, edx, -1
00451f5a  03 45 88                           add        eax, dword ptr [ebp - 0x78]
00451f5d  50                                 push       eax
00451f5e  8d 4d d8                           lea        ecx, [ebp - 0x28]
00451f61  51                                 push       ecx
00451f62  e8 99 f7 ff ff                     call       0x451700
00451f67  83 c4 0c                           add        esp, 0xc
00451f6a  8b 55 b4                           mov        edx, dword ptr [ebp - 0x4c]
00451f6d  83 ea 01                           sub        edx, 1
00451f70  39 95 64 ff ff ff                  cmp        dword ptr [ebp - 0x9c], edx
00451f76  73 1c                              jae        0x451f94
00451f78  8d 45 c0                           lea        eax, [ebp - 0x40]
00451f7b  50                                 push       eax
00451f7c  b9 02 00 00 00                     mov        ecx, 2
00451f81  c1 e1 00                           shl        ecx, 0
00451f84  03 4d 88                           add        ecx, dword ptr [ebp - 0x78]
00451f87  51                                 push       ecx
00451f88  8d 55 d8                           lea        edx, [ebp - 0x28]
00451f8b  52                                 push       edx
00451f8c  e8 6f f7 ff ff                     call       0x451700
00451f91  83 c4 0c                           add        esp, 0xc
00451f94  83 bd 74 ff ff ff 00               cmp        dword ptr [ebp - 0x8c], 0
00451f9b  76 21                              jbe        0x451fbe
00451f9d  8d 45 c0                           lea        eax, [ebp - 0x40]
00451fa0  50                                 push       eax
00451fa1  8b 45 90                           mov        eax, dword ptr [ebp - 0x70]
00451fa4  f7 d8                              neg        eax
00451fa6  99                                 cdq        
00451fa7  2b c2                              sub        eax, edx
00451fa9  d1 f8                              sar        eax, 1
00451fab  8b 4d 88                           mov        ecx, dword ptr [ebp - 0x78]
00451fae  8d 14 41                           lea        edx, [ecx + eax*2]
00451fb1  52                                 push       edx
00451fb2  8d 45 d8                           lea        eax, [ebp - 0x28]
00451fb5  50                                 push       eax
00451fb6  e8 45 f7 ff ff                     call       0x451700
00451fbb  83 c4 0c                           add        esp, 0xc
00451fbe  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
00451fc1  83 e9 01                           sub        ecx, 1
00451fc4  39 8d 74 ff ff ff                  cmp        dword ptr [ebp - 0x8c], ecx
00451fca  73 1f                              jae        0x451feb
00451fcc  8d 55 c0                           lea        edx, [ebp - 0x40]
00451fcf  52                                 push       edx
00451fd0  8b 45 90                           mov        eax, dword ptr [ebp - 0x70]
00451fd3  99                                 cdq        
00451fd4  2b c2                              sub        eax, edx
00451fd6  d1 f8                              sar        eax, 1
00451fd8  8b 4d 88                           mov        ecx, dword ptr [ebp - 0x78]
00451fdb  8d 14 41                           lea        edx, [ecx + eax*2]
00451fde  52                                 push       edx
00451fdf  8d 45 d8                           lea        eax, [ebp - 0x28]
00451fe2  50                                 push       eax
00451fe3  e8 18 f7 ff ff                     call       0x451700
00451fe8  83 c4 0c                           add        esp, 0xc
00451feb  83 7d c0 01                        cmp        dword ptr [ebp - 0x40], 1
00451fef  76 55                              jbe        0x452046
00451ff1  b9 04 00 00 00                     mov        ecx, 4
00451ff6  6b d1 00                           imul       edx, ecx, 0
00451ff9  8b 44 15 d8                        mov        eax, dword ptr [ebp + edx - 0x28]
00451ffd  33 d2                              xor        edx, edx
00451fff  f7 75 c0                           div        dword ptr [ebp - 0x40]
00452002  b9 04 00 00 00                     mov        ecx, 4
00452007  6b d1 00                           imul       edx, ecx, 0
0045200a  89 44 15 d8                        mov        dword ptr [ebp + edx - 0x28], eax
0045200e  b8 04 00 00 00                     mov        eax, 4
00452013  c1 e0 00                           shl        eax, 0
00452016  8b 44 05 d8                        mov        eax, dword ptr [ebp + eax - 0x28]
0045201a  33 d2                              xor        edx, edx
0045201c  f7 75 c0                           div        dword ptr [ebp - 0x40]
0045201f  b9 04 00 00 00                     mov        ecx, 4
00452024  c1 e1 00                           shl        ecx, 0
00452027  89 44 0d d8                        mov        dword ptr [ebp + ecx - 0x28], eax
0045202b  ba 04 00 00 00                     mov        edx, 4
00452030  d1 e2                              shl        edx, 1
00452032  8b 44 15 d8                        mov        eax, dword ptr [ebp + edx - 0x28]
00452036  33 d2                              xor        edx, edx
00452038  f7 75 c0                           div        dword ptr [ebp - 0x40]
0045203b  b9 04 00 00 00                     mov        ecx, 4
00452040  d1 e1                              shl        ecx, 1
00452042  89 44 0d d8                        mov        dword ptr [ebp + ecx - 0x28], eax
00452046  ba 04 00 00 00                     mov        edx, 4
0045204b  6b c2 00                           imul       eax, edx, 0
0045204e  66 0f b6 4c 05 d8                  movzx      cx, byte ptr [ebp + eax - 0x28]
00452054  ba 02 00 00 00                     mov        edx, 2
00452059  6b c2 00                           imul       eax, edx, 0
0045205c  66 83 e1 0f                        and        cx, 0xf
00452060  66 c1 e1 08                        shl        cx, 8
00452064  ba ff f0 00 00                     mov        edx, 0xf0ff
00452069  8b 75 88                           mov        esi, dword ptr [ebp - 0x78]
0045206c  66 23 14 06                        and        dx, word ptr [esi + eax]
00452070  66 0b d1                           or         dx, cx
00452073  b8 02 00 00 00                     mov        eax, 2
00452078  6b c8 00                           imul       ecx, eax, 0
0045207b  8b 45 88                           mov        eax, dword ptr [ebp - 0x78]
0045207e  66 89 14 08                        mov        word ptr [eax + ecx], dx
00452082  b9 04 00 00 00                     mov        ecx, 4
00452087  c1 e1 00                           shl        ecx, 0
0045208a  66 0f b6 54 0d d8                  movzx      dx, byte ptr [ebp + ecx - 0x28]
00452090  b8 02 00 00 00                     mov        eax, 2
00452095  6b c8 00                           imul       ecx, eax, 0
00452098  66 83 e2 0f                        and        dx, 0xf
0045209c  66 c1 e2 04                        shl        dx, 4
004520a0  b8 0f ff 00 00                     mov        eax, 0xff0f
004520a5  8b 75 88                           mov        esi, dword ptr [ebp - 0x78]
004520a8  66 23 04 0e                        and        ax, word ptr [esi + ecx]
004520ac  66 0b c2                           or         ax, dx
004520af  b9 02 00 00 00                     mov        ecx, 2
004520b4  6b d1 00                           imul       edx, ecx, 0
004520b7  8b 4d 88                           mov        ecx, dword ptr [ebp - 0x78]
004520ba  66 89 04 11                        mov        word ptr [ecx + edx], ax
004520be  ba 04 00 00 00                     mov        edx, 4
004520c3  d1 e2                              shl        edx, 1
004520c5  66 0f b6 44 15 d8                  movzx      ax, byte ptr [ebp + edx - 0x28]
004520cb  b9 02 00 00 00                     mov        ecx, 2
004520d0  6b d1 00                           imul       edx, ecx, 0
004520d3  66 83 e0 0f                        and        ax, 0xf
004520d7  b9 f0 ff 00 00                     mov        ecx, 0xfff0
004520dc  8b 75 88                           mov        esi, dword ptr [ebp - 0x78]
004520df  66 23 0c 16                        and        cx, word ptr [esi + edx]
004520e3  66 0b c8                           or         cx, ax
004520e6  ba 02 00 00 00                     mov        edx, 2
004520eb  6b c2 00                           imul       eax, edx, 0
004520ee  8b 55 88                           mov        edx, dword ptr [ebp - 0x78]
004520f1  66 89 0c 02                        mov        word ptr [edx + eax], cx
004520f5  8b 45 88                           mov        eax, dword ptr [ebp - 0x78]
004520f8  83 c0 02                           add        eax, 2
004520fb  89 45 88                           mov        dword ptr [ebp - 0x78], eax
004520fe  e9 cc fd ff ff                     jmp        0x451ecf
00452103  e9 8d fd ff ff                     jmp        0x451e95
00452108  e9 8e 02 00 00                     jmp        0x45239b
0045210d  33 c9                              xor        ecx, ecx
0045210f  89 4d cc                           mov        dword ptr [ebp - 0x34], ecx
00452112  89 4d d0                           mov        dword ptr [ebp - 0x30], ecx
00452115  89 4d d4                           mov        dword ptr [ebp - 0x2c], ecx
00452118  c7 85 70 ff ff ff 00 00 00 00      mov        dword ptr [ebp - 0x90], 0
00452122  eb 0f                              jmp        0x452133
00452124  8b 95 70 ff ff ff                  mov        edx, dword ptr [ebp - 0x90]
0045212a  83 c2 01                           add        edx, 1
0045212d  89 95 70 ff ff ff                  mov        dword ptr [ebp - 0x90], edx
00452133  8b 85 70 ff ff ff                  mov        eax, dword ptr [ebp - 0x90]
00452139  3b 45 b8                           cmp        eax, dword ptr [ebp - 0x48]
0045213c  0f 83 59 02 00 00                  jae        0x45239b
00452142  8b 4d 90                           mov        ecx, dword ptr [ebp - 0x70]
00452145  0f af 8d 70 ff ff ff               imul       ecx, dword ptr [ebp - 0x90]
0045214c  03 4d 94                           add        ecx, dword ptr [ebp - 0x6c]
0045214f  89 4d 84                           mov        dword ptr [ebp - 0x7c], ecx
00452152  c7 85 60 ff ff ff 00 00 00 00      mov        dword ptr [ebp - 0xa0], 0
0045215c  eb 0f                              jmp        0x45216d
0045215e  8b 95 60 ff ff ff                  mov        edx, dword ptr [ebp - 0xa0]
00452164  83 c2 01                           add        edx, 1
00452167  89 95 60 ff ff ff                  mov        dword ptr [ebp - 0xa0], edx
0045216d  8b 85 60 ff ff ff                  mov        eax, dword ptr [ebp - 0xa0]
00452173  3b 45 b4                           cmp        eax, dword ptr [ebp - 0x4c]
00452176  0f 83 1a 02 00 00                  jae        0x452396
0045217c  b9 02 00 00 00                     mov        ecx, 2
00452181  6b d1 00                           imul       edx, ecx, 0
00452184  8b 45 84                           mov        eax, dword ptr [ebp - 0x7c]
00452187  66 8b 0c 10                        mov        cx, word ptr [eax + edx]
0045218b  66 c1 e9 08                        shr        cx, 8
0045218f  ba ff 00 00 00                     mov        edx, 0xff
00452194  66 23 ca                           and        cx, dx
00452197  0f b7 c1                           movzx      eax, cx
0045219a  85 c0                              test       eax, eax
0045219c  0f 85 e6 01 00 00                  jne        0x452388
004521a2  b9 04 00 00 00                     mov        ecx, 4
004521a7  d1 e1                              shl        ecx, 1
004521a9  c7 44 0d cc 00 00 00 00            mov        dword ptr [ebp + ecx - 0x34], 0
004521b1  ba 04 00 00 00                     mov        edx, 4
004521b6  c1 e2 00                           shl        edx, 0
004521b9  c7 44 15 cc 00 00 00 00            mov        dword ptr [ebp + edx - 0x34], 0
004521c1  b8 04 00 00 00                     mov        eax, 4
004521c6  6b c8 00                           imul       ecx, eax, 0
004521c9  c7 44 0d cc 00 00 00 00            mov        dword ptr [ebp + ecx - 0x34], 0
004521d1  c7 45 bc 00 00 00 00               mov        dword ptr [ebp - 0x44], 0
004521d8  83 bd 60 ff ff ff 00               cmp        dword ptr [ebp - 0xa0], 0
004521df  76 1c                              jbe        0x4521fd
004521e1  8d 55 bc                           lea        edx, [ebp - 0x44]
004521e4  52                                 push       edx
004521e5  b8 02 00 00 00                     mov        eax, 2
004521ea  6b c8 ff                           imul       ecx, eax, -1
004521ed  03 4d 84                           add        ecx, dword ptr [ebp - 0x7c]
004521f0  51                                 push       ecx
004521f1  8d 55 cc                           lea        edx, [ebp - 0x34]
004521f4  52                                 push       edx
004521f5  e8 c6 f5 ff ff                     call       0x4517c0
004521fa  83 c4 0c                           add        esp, 0xc
004521fd  8b 45 b4                           mov        eax, dword ptr [ebp - 0x4c]
00452200  83 e8 01                           sub        eax, 1
00452203  39 85 60 ff ff ff                  cmp        dword ptr [ebp - 0xa0], eax
00452209  73 1c                              jae        0x452227
0045220b  8d 4d bc                           lea        ecx, [ebp - 0x44]
0045220e  51                                 push       ecx
0045220f  ba 02 00 00 00                     mov        edx, 2
00452214  c1 e2 00                           shl        edx, 0
00452217  03 55 84                           add        edx, dword ptr [ebp - 0x7c]
0045221a  52                                 push       edx
0045221b  8d 45 cc                           lea        eax, [ebp - 0x34]
0045221e  50                                 push       eax
0045221f  e8 9c f5 ff ff                     call       0x4517c0
00452224  83 c4 0c                           add        esp, 0xc
00452227  83 bd 70 ff ff ff 00               cmp        dword ptr [ebp - 0x90], 0
0045222e  76 21                              jbe        0x452251
00452230  8d 4d bc                           lea        ecx, [ebp - 0x44]
00452233  51                                 push       ecx
00452234  8b 45 90                           mov        eax, dword ptr [ebp - 0x70]
00452237  f7 d8                              neg        eax
00452239  99                                 cdq        
0045223a  2b c2                              sub        eax, edx
0045223c  d1 f8                              sar        eax, 1
0045223e  8b 55 84                           mov        edx, dword ptr [ebp - 0x7c]
00452241  8d 04 42                           lea        eax, [edx + eax*2]
00452244  50                                 push       eax
00452245  8d 4d cc                           lea        ecx, [ebp - 0x34]
00452248  51                                 push       ecx
00452249  e8 72 f5 ff ff                     call       0x4517c0
0045224e  83 c4 0c                           add        esp, 0xc
00452251  8b 55 b8                           mov        edx, dword ptr [ebp - 0x48]
00452254  83 ea 01                           sub        edx, 1
00452257  39 95 70 ff ff ff                  cmp        dword ptr [ebp - 0x90], edx
0045225d  73 1f                              jae        0x45227e
0045225f  8d 45 bc                           lea        eax, [ebp - 0x44]
00452262  50                                 push       eax
00452263  8b 45 90                           mov        eax, dword ptr [ebp - 0x70]
00452266  99                                 cdq        
00452267  2b c2                              sub        eax, edx
00452269  d1 f8                              sar        eax, 1
0045226b  8b 4d 84                           mov        ecx, dword ptr [ebp - 0x7c]
0045226e  8d 14 41                           lea        edx, [ecx + eax*2]
00452271  52                                 push       edx
00452272  8d 45 cc                           lea        eax, [ebp - 0x34]
00452275  50                                 push       eax
00452276  e8 45 f5 ff ff                     call       0x4517c0
0045227b  83 c4 0c                           add        esp, 0xc
0045227e  83 7d bc 01                        cmp        dword ptr [ebp - 0x44], 1
00452282  76 55                              jbe        0x4522d9
00452284  b9 04 00 00 00                     mov        ecx, 4
00452289  6b d1 00                           imul       edx, ecx, 0
0045228c  8b 44 15 cc                        mov        eax, dword ptr [ebp + edx - 0x34]
00452290  33 d2                              xor        edx, edx
00452292  f7 75 bc                           div        dword ptr [ebp - 0x44]
00452295  b9 04 00 00 00                     mov        ecx, 4
0045229a  6b d1 00                           imul       edx, ecx, 0
0045229d  89 44 15 cc                        mov        dword ptr [ebp + edx - 0x34], eax
004522a1  b8 04 00 00 00                     mov        eax, 4
004522a6  c1 e0 00                           shl        eax, 0
004522a9  8b 44 05 cc                        mov        eax, dword ptr [ebp + eax - 0x34]
004522ad  33 d2                              xor        edx, edx
004522af  f7 75 bc                           div        dword ptr [ebp - 0x44]
004522b2  b9 04 00 00 00                     mov        ecx, 4
004522b7  c1 e1 00                           shl        ecx, 0
004522ba  89 44 0d cc                        mov        dword ptr [ebp + ecx - 0x34], eax
004522be  ba 04 00 00 00                     mov        edx, 4
004522c3  d1 e2                              shl        edx, 1
004522c5  8b 44 15 cc                        mov        eax, dword ptr [ebp + edx - 0x34]
004522c9  33 d2                              xor        edx, edx
004522cb  f7 75 bc                           div        dword ptr [ebp - 0x44]
004522ce  b9 04 00 00 00                     mov        ecx, 4
004522d3  d1 e1                              shl        ecx, 1
004522d5  89 44 0d cc                        mov        dword ptr [ebp + ecx - 0x34], eax
004522d9  ba 04 00 00 00                     mov        edx, 4
004522de  6b c2 00                           imul       eax, edx, 0
004522e1  66 0f b6 4c 05 cc                  movzx      cx, byte ptr [ebp + eax - 0x34]
004522e7  ba 02 00 00 00                     mov        edx, 2
004522ec  6b c2 00                           imul       eax, edx, 0
004522ef  66 83 e1 07                        and        cx, 7
004522f3  66 c1 e1 05                        shl        cx, 5
004522f7  ba 1f ff 00 00                     mov        edx, 0xff1f
004522fc  8b 75 84                           mov        esi, dword ptr [ebp - 0x7c]
004522ff  66 23 14 06                        and        dx, word ptr [esi + eax]
00452303  66 0b d1                           or         dx, cx
00452306  b8 02 00 00 00                     mov        eax, 2
0045230b  6b c8 00                           imul       ecx, eax, 0
0045230e  8b 45 84                           mov        eax, dword ptr [ebp - 0x7c]
00452311  66 89 14 08                        mov        word ptr [eax + ecx], dx
00452315  b9 04 00 00 00                     mov        ecx, 4
0045231a  c1 e1 00                           shl        ecx, 0
0045231d  66 0f b6 54 0d cc                  movzx      dx, byte ptr [ebp + ecx - 0x34]
00452323  b8 02 00 00 00                     mov        eax, 2
00452328  6b c8 00                           imul       ecx, eax, 0
0045232b  66 83 e2 07                        and        dx, 7
0045232f  66 c1 e2 02                        shl        dx, 2
00452333  b8 e3 ff 00 00                     mov        eax, 0xffe3
00452338  8b 75 84                           mov        esi, dword ptr [ebp - 0x7c]
0045233b  66 23 04 0e                        and        ax, word ptr [esi + ecx]
0045233f  66 0b c2                           or         ax, dx
00452342  b9 02 00 00 00                     mov        ecx, 2
00452347  6b d1 00                           imul       edx, ecx, 0
0045234a  8b 4d 84                           mov        ecx, dword ptr [ebp - 0x7c]
0045234d  66 89 04 11                        mov        word ptr [ecx + edx], ax
00452351  ba 04 00 00 00                     mov        edx, 4
00452356  d1 e2                              shl        edx, 1
00452358  66 0f b6 44 15 cc                  movzx      ax, byte ptr [ebp + edx - 0x34]
0045235e  b9 02 00 00 00                     mov        ecx, 2
00452363  6b d1 00                           imul       edx, ecx, 0
00452366  66 83 e0 03                        and        ax, 3
0045236a  b9 fc ff 00 00                     mov        ecx, 0xfffc
0045236f  8b 75 84                           mov        esi, dword ptr [ebp - 0x7c]
00452372  66 23 0c 16                        and        cx, word ptr [esi + edx]
00452376  66 0b c8                           or         cx, ax
00452379  ba 02 00 00 00                     mov        edx, 2
0045237e  6b c2 00                           imul       eax, edx, 0
00452381  8b 55 84                           mov        edx, dword ptr [ebp - 0x7c]
00452384  66 89 0c 02                        mov        word ptr [edx + eax], cx
00452388  8b 45 84                           mov        eax, dword ptr [ebp - 0x7c]
0045238b  83 c0 02                           add        eax, 2
0045238e  89 45 84                           mov        dword ptr [ebp - 0x7c], eax
00452391  e9 c8 fd ff ff                     jmp        0x45215e
00452396  e9 89 fd ff ff                     jmp        0x452124
0045239b  8b 4d 98                           mov        ecx, dword ptr [ebp - 0x68]
0045239e  8b 11                              mov        edx, dword ptr [ecx]
004523a0  8b 45 98                           mov        eax, dword ptr [ebp - 0x68]
004523a3  50                                 push       eax
004523a4  8b 4a 38                           mov        ecx, dword ptr [edx + 0x38]
004523a7  ff d1                              call       ecx
004523a9  8b 55 98                           mov        edx, dword ptr [ebp - 0x68]
004523ac  8b 02                              mov        eax, dword ptr [edx]
004523ae  8b 4d 98                           mov        ecx, dword ptr [ebp - 0x68]
004523b1  51                                 push       ecx
004523b2  8b 50 08                           mov        edx, dword ptr [eax + 8]
004523b5  ff d2                              call       edx
004523b7  90                                 nop        
004523b8  5e                                 pop        esi
004523b9  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004523bc  33 cd                              xor        ecx, ebp
004523be  e8 2b 06 0f 00                     call       0x5429ee
004523c3  8b e5                              mov        esp, ebp
004523c5  5d                                 pop        ebp
004523c6  c2 04 00                           ret        4
