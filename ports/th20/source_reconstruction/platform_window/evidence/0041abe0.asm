0041abe0  55                                 push       ebp
0041abe1  8b ec                              mov        ebp, esp
0041abe3  83 ec 34                           sub        esp, 0x34
0041abe6  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0041abeb  33 c5                              xor        eax, ebp
0041abed  89 45 fc                           mov        dword ptr [ebp - 4], eax
0041abf0  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0041abf3  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0041abf6  81 7d f0 10 01 00 00               cmp        dword ptr [ebp - 0x10], 0x110
0041abfd  77 25                              ja         0x41ac24
0041abff  81 7d f0 10 01 00 00               cmp        dword ptr [ebp - 0x10], 0x110
0041ac06  74 2e                              je         0x41ac36
0041ac08  83 7d f0 10                        cmp        dword ptr [ebp - 0x10], 0x10
0041ac0c  0f 84 ee 01 00 00                  je         0x41ae00
0041ac12  81 7d f0 ff 00 00 00               cmp        dword ptr [ebp - 0x10], 0xff
0041ac19  0f 84 8f 00 00 00                  je         0x41acae
0041ac1f  e9 1e 02 00 00                     jmp        0x41ae42
0041ac24  81 7d f0 11 01 00 00               cmp        dword ptr [ebp - 0x10], 0x111
0041ac2b  0f 84 62 01 00 00                  je         0x41ad93
0041ac31  e9 0c 02 00 00                     jmp        0x41ae42
0041ac36  8b 0d 8c 4f 5c 00                  mov        ecx, dword ptr [0x5c4f8c]
0041ac3c  c1 e9 07                           shr        ecx, 7
0041ac3f  83 e1 01                           and        ecx, 1
0041ac42  74 20                              je         0x41ac64
0041ac44  6a 00                              push       0
0041ac46  6a 01                              push       1
0041ac48  68 f1 00 00 00                     push       0xf1
0041ac4d  68 ca 00 00 00                     push       0xca
0041ac52  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0041ac55  52                                 push       edx
0041ac56  ff 15 34 c2 56 00                  call       dword ptr [0x56c234] ; USER32.dll!GetDlgItem
0041ac5c  50                                 push       eax
0041ac5d  ff 15 a0 c2 56 00                  call       dword ptr [0x56c2a0] ; USER32.dll!SendMessageW
0041ac63  90                                 nop        
0041ac64  6a 00                              push       0
0041ac66  6a 01                              push       1
0041ac68  68 f1 00 00 00                     push       0xf1
0041ac6d  a1 80 4f 5c 00                     mov        eax, dword ptr [0x5c4f80]
0041ac72  8b 0c 85 28 e1 5a 00               mov        ecx, dword ptr [eax*4 + 0x5ae128]
0041ac79  51                                 push       ecx
0041ac7a  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0041ac7d  52                                 push       edx
0041ac7e  ff 15 34 c2 56 00                  call       dword ptr [0x56c234] ; USER32.dll!GetDlgItem
0041ac84  50                                 push       eax
0041ac85  ff 15 a0 c2 56 00                  call       dword ptr [0x56c2a0] ; USER32.dll!SendMessageW
0041ac8b  a1 e8 87 5b 00                     mov        eax, dword ptr [0x5b87e8]
0041ac90  83 e0 e7                           and        eax, 0xffffffe7
0041ac93  83 c8 10                           or         eax, 0x10
0041ac96  a3 e8 87 5b 00                     mov        dword ptr [0x5b87e8], eax
0041ac9b  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041ac9e  51                                 push       ecx
0041ac9f  e8 0c f5 ff ff                     call       0x41a1b0
0041aca4  83 c4 04                           add        esp, 4
0041aca7  33 c0                              xor        eax, eax
0041aca9  e9 b1 01 00 00                     jmp        0x41ae5f
0041acae  6a 10                              push       0x10
0041acb0  8d 55 f8                           lea        edx, [ebp - 8]
0041acb3  52                                 push       edx
0041acb4  6a 00                              push       0
0041acb6  68 03 00 00 10                     push       0x10000003
0041acbb  8b 45 14                           mov        eax, dword ptr [ebp + 0x14]
0041acbe  50                                 push       eax
0041acbf  ff 15 88 c2 56 00                  call       dword ptr [0x56c288] ; USER32.dll!GetRawInputData
0041acc5  90                                 nop        
0041acc6  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0041acc9  51                                 push       ecx
0041acca  e8 32 7d 12 00                     call       0x542a01
0041accf  83 c4 04                           add        esp, 4
0041acd2  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
0041acd5  8b 55 e0                           mov        edx, dword ptr [ebp - 0x20]
0041acd8  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
0041acdb  6a 10                              push       0x10
0041acdd  8d 45 f8                           lea        eax, [ebp - 8]
0041ace0  50                                 push       eax
0041ace1  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0041ace4  51                                 push       ecx
0041ace5  68 03 00 00 10                     push       0x10000003
0041acea  8b 55 14                           mov        edx, dword ptr [ebp + 0x14]
0041aced  52                                 push       edx
0041acee  ff 15 88 c2 56 00                  call       dword ptr [0x56c288] ; USER32.dll!GetRawInputData
0041acf4  3b 45 f8                           cmp        eax, dword ptr [ebp - 8]
0041acf7  75 63                              jne        0x41ad5c
0041acf9  8b 45 f4                           mov        eax, dword ptr [ebp - 0xc]
0041acfc  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
0041acff  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
0041ad02  83 39 02                           cmp        dword ptr [ecx], 2
0041ad05  75 55                              jne        0x41ad5c
0041ad07  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
0041ad0a  83 c2 18                           add        edx, 0x18
0041ad0d  89 55 dc                           mov        dword ptr [ebp - 0x24], edx
0041ad10  b8 01 00 00 00                     mov        eax, 1
0041ad15  c1 e0 00                           shl        eax, 0
0041ad18  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
0041ad1b  0f b6 14 01                        movzx      edx, byte ptr [ecx + eax]
0041ad1f  89 55 d8                           mov        dword ptr [ebp - 0x28], edx
0041ad22  83 7d d8 00                        cmp        dword ptr [ebp - 0x28], 0
0041ad26  74 34                              je         0x41ad5c
0041ad28  8b 45 f4                           mov        eax, dword ptr [ebp - 0xc]
0041ad2b  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
0041ad2e  6a 01                              push       1
0041ad30  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0041ad33  51                                 push       ecx
0041ad34  e8 54 7a 12 00                     call       0x54278d
0041ad39  83 c4 08                           add        esp, 8
0041ad3c  83 7d e8 00                        cmp        dword ptr [ebp - 0x18], 0
0041ad40  75 09                              jne        0x41ad4b
0041ad42  c7 45 d4 00 00 00 00               mov        dword ptr [ebp - 0x2c], 0
0041ad49  eb 0d                              jmp        0x41ad58
0041ad4b  c7 45 f4 23 81 00 00               mov        dword ptr [ebp - 0xc], 0x8123
0041ad52  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
0041ad55  89 55 d4                           mov        dword ptr [ebp - 0x2c], edx
0041ad58  eb 52                              jmp        0x41adac
0041ad5c  8b 45 f4                           mov        eax, dword ptr [ebp - 0xc]
0041ad5f  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
0041ad62  6a 01                              push       1
0041ad64  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0041ad67  51                                 push       ecx
0041ad68  e8 20 7a 12 00                     call       0x54278d
0041ad6d  83 c4 08                           add        esp, 8
0041ad70  83 7d e4 00                        cmp        dword ptr [ebp - 0x1c], 0
0041ad74  75 09                              jne        0x41ad7f
0041ad76  c7 45 d0 00 00 00 00               mov        dword ptr [ebp - 0x30], 0
0041ad7d  eb 0d                              jmp        0x41ad8c
0041ad7f  c7 45 f4 23 81 00 00               mov        dword ptr [ebp - 0xc], 0x8123
0041ad86  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
0041ad89  89 55 d0                           mov        dword ptr [ebp - 0x30], edx
0041ad8c  33 c0                              xor        eax, eax
0041ad8e  e9 cc 00 00 00                     jmp        0x41ae5f
0041ad93  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
0041ad96  25 ff ff 00 00                     and        eax, 0xffff
0041ad9b  0f b7 c8                           movzx      ecx, ax
0041ad9e  89 4d cc                           mov        dword ptr [ebp - 0x34], ecx
0041ada1  81 7d cc ce 00 00 00               cmp        dword ptr [ebp - 0x34], 0xce
0041ada8  74 02                              je         0x41adac
0041adaa  eb 50                              jmp        0x41adfc
0041adac  e8 7f fd ff ff                     call       0x41ab30
0041adb1  8b 15 e8 87 5b 00                  mov        edx, dword ptr [0x5b87e8]
0041adb7  83 e2 e7                           and        edx, 0xffffffe7
0041adba  89 15 e8 87 5b 00                  mov        dword ptr [0x5b87e8], edx
0041adc0  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041adc5  e8 c6 51 ff ff                     call       0x40ff90
0041adca  50                                 push       eax
0041adcb  ff 15 20 c2 56 00                  call       dword ptr [0x56c220] ; USER32.dll!IsWindow
0041add1  85 c0                              test       eax, eax
0041add3  74 12                              je         0x41ade7
0041add5  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041adda  e8 b1 51 ff ff                     call       0x40ff90
0041addf  50                                 push       eax
0041ade0  ff 15 24 c2 56 00                  call       dword ptr [0x56c224] ; USER32.dll!DestroyWindow
0041ade6  90                                 nop        
0041ade7  6a 00                              push       0
0041ade9  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041adee  e8 7d 67 ff ff                     call       0x411570
0041adf3  b8 01 00 00 00                     mov        eax, 1
0041adf8  eb 65                              jmp        0x41ae5f
0041adfc  33 c0                              xor        eax, eax
0041adfe  eb 5f                              jmp        0x41ae5f
0041ae00  a1 e8 87 5b 00                     mov        eax, dword ptr [0x5b87e8]
0041ae05  c1 e8 03                           shr        eax, 3
0041ae08  83 e0 03                           and        eax, 3
0041ae0b  83 f8 02                           cmp        eax, 2
0041ae0e  75 12                              jne        0x41ae22
0041ae10  8b 0d e8 87 5b 00                  mov        ecx, dword ptr [0x5b87e8]
0041ae16  83 e1 e7                           and        ecx, 0xffffffe7
0041ae19  83 c9 08                           or         ecx, 8
0041ae1c  89 0d e8 87 5b 00                  mov        dword ptr [0x5b87e8], ecx
0041ae22  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041ae27  e8 64 51 ff ff                     call       0x40ff90
0041ae2c  50                                 push       eax
0041ae2d  ff 15 24 c2 56 00                  call       dword ptr [0x56c224] ; USER32.dll!DestroyWindow
0041ae33  6a 00                              push       0
0041ae35  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041ae3a  e8 31 67 ff ff                     call       0x411570
0041ae3f  90                                 nop        
0041ae40  eb 18                              jmp        0x41ae5a
0041ae42  8b 55 14                           mov        edx, dword ptr [ebp + 0x14]
0041ae45  52                                 push       edx
0041ae46  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
0041ae49  50                                 push       eax
0041ae4a  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
0041ae4d  51                                 push       ecx
0041ae4e  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0041ae51  52                                 push       edx
0041ae52  ff 15 9c c2 56 00                  call       dword ptr [0x56c29c] ; USER32.dll!DefWindowProcW
0041ae58  eb 05                              jmp        0x41ae5f
0041ae5a  b8 01 00 00 00                     mov        eax, 1
0041ae5f  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041ae62  33 cd                              xor        ecx, ebp
0041ae64  e8 85 7b 12 00                     call       0x5429ee
0041ae69  8b e5                              mov        esp, ebp
0041ae6b  5d                                 pop        ebp
0041ae6c  c2 10 00                           ret        0x10
