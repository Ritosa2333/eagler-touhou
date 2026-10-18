0044bb10  55                                 push       ebp
0044bb11  8b ec                              mov        ebp, esp
0044bb13  83 ec 60                           sub        esp, 0x60
0044bb16  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0044bb1b  33 c5                              xor        eax, ebp
0044bb1d  89 45 fc                           mov        dword ptr [ebp - 4], eax
0044bb20  89 4d d4                           mov        dword ptr [ebp - 0x2c], ecx
0044bb23  8b 45 d4                           mov        eax, dword ptr [ebp - 0x2c]
0044bb26  05 30 07 00 06                     add        eax, 0x6000730
0044bb2b  89 45 c0                           mov        dword ptr [ebp - 0x40], eax
0044bb2e  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0044bb31  8b 11                              mov        edx, dword ptr [ecx]
0044bb33  52                                 push       edx
0044bb34  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0044bb37  e8 44 8a fc ff                     call       0x414580
0044bb3c  8b 00                              mov        eax, dword ptr [eax]
0044bb3e  8b 48 58                           mov        ecx, dword ptr [eax + 0x58]
0044bb41  89 4d bc                           mov        dword ptr [ebp - 0x44], ecx
0044bb44  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0044bb47  6b 42 04 18                        imul       eax, dword ptr [edx + 4], 0x18
0044bb4b  89 45 b8                           mov        dword ptr [ebp - 0x48], eax
0044bb4e  8b 4d bc                           mov        ecx, dword ptr [ebp - 0x44]
0044bb51  03 4d b8                           add        ecx, dword ptr [ebp - 0x48]
0044bb54  83 39 00                           cmp        dword ptr [ecx], 0
0044bb57  75 05                              jne        0x44bb5e
0044bb59  e9 59 01 00 00                     jmp        0x44bcb7
0044bb5e  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
0044bb61  e8 5a 9a ff ff                     call       0x4455c0
0044bb66  90                                 nop        
0044bb67  8b 55 d4                           mov        edx, dword ptr [ebp - 0x2c]
0044bb6a  81 c2 30 07 00 06                  add        edx, 0x6000730
0044bb70  89 55 b4                           mov        dword ptr [ebp - 0x4c], edx
0044bb73  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0044bb76  8b 08                              mov        ecx, dword ptr [eax]
0044bb78  51                                 push       ecx
0044bb79  8b 4d b4                           mov        ecx, dword ptr [ebp - 0x4c]
0044bb7c  e8 ff 89 fc ff                     call       0x414580
0044bb81  8b 10                              mov        edx, dword ptr [eax]
0044bb83  8b 42 58                           mov        eax, dword ptr [edx + 0x58]
0044bb86  89 45 d0                           mov        dword ptr [ebp - 0x30], eax
0044bb89  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0044bb8c  6b 51 04 18                        imul       edx, dword ptr [ecx + 4], 0x18
0044bb90  89 55 cc                           mov        dword ptr [ebp - 0x34], edx
0044bb93  8b 45 d0                           mov        eax, dword ptr [ebp - 0x30]
0044bb96  03 45 cc                           add        eax, dword ptr [ebp - 0x34]
0044bb99  8b 08                              mov        ecx, dword ptr [eax]
0044bb9b  89 4d b0                           mov        dword ptr [ebp - 0x50], ecx
0044bb9e  8b 55 d0                           mov        edx, dword ptr [ebp - 0x30]
0044bba1  03 55 cc                           add        edx, dword ptr [ebp - 0x34]
0044bba4  8b 02                              mov        eax, dword ptr [edx]
0044bba6  8b 08                              mov        ecx, dword ptr [eax]
0044bba8  8b 51 48                           mov        edx, dword ptr [ecx + 0x48]
0044bbab  89 55 ac                           mov        dword ptr [ebp - 0x54], edx
0044bbae  8d 45 d8                           lea        eax, [ebp - 0x28]
0044bbb1  50                                 push       eax
0044bbb2  6a 00                              push       0
0044bbb4  8b 4d b0                           mov        ecx, dword ptr [ebp - 0x50]
0044bbb7  51                                 push       ecx
0044bbb8  ff 55 ac                           call       dword ptr [ebp - 0x54]
0044bbbb  85 c0                              test       eax, eax
0044bbbd  74 05                              je         0x44bbc4
0044bbbf  e9 f3 00 00 00                     jmp        0x44bcb7
0044bbc4  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0044bbc7  8b 42 08                           mov        eax, dword ptr [edx + 8]
0044bbca  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
0044bbcd  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0044bbd0  8b 51 0c                           mov        edx, dword ptr [ecx + 0xc]
0044bbd3  89 55 f0                           mov        dword ptr [ebp - 0x10], edx
0044bbd6  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0044bbd9  8b 48 08                           mov        ecx, dword ptr [eax + 8]
0044bbdc  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0044bbdf  03 4a 10                           add        ecx, dword ptr [edx + 0x10]
0044bbe2  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
0044bbe5  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0044bbe8  8b 48 0c                           mov        ecx, dword ptr [eax + 0xc]
0044bbeb  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0044bbee  03 4a 14                           add        ecx, dword ptr [edx + 0x14]
0044bbf1  89 4d f8                           mov        dword ptr [ebp - 8], ecx
0044bbf4  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0044bbf7  8b 48 18                           mov        ecx, dword ptr [eax + 0x18]
0044bbfa  89 4d dc                           mov        dword ptr [ebp - 0x24], ecx
0044bbfd  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0044bc00  8b 42 1c                           mov        eax, dword ptr [edx + 0x1c]
0044bc03  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
0044bc06  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0044bc09  8b 51 18                           mov        edx, dword ptr [ecx + 0x18]
0044bc0c  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0044bc0f  03 50 20                           add        edx, dword ptr [eax + 0x20]
0044bc12  89 55 e4                           mov        dword ptr [ebp - 0x1c], edx
0044bc15  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0044bc18  8b 51 1c                           mov        edx, dword ptr [ecx + 0x1c]
0044bc1b  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0044bc1e  03 50 24                           add        edx, dword ptr [eax + 0x24]
0044bc21  89 55 e8                           mov        dword ptr [ebp - 0x18], edx
0044bc24  6a 00                              push       0
0044bc26  6a 02                              push       2
0044bc28  8d 4d ec                           lea        ecx, [ebp - 0x14]
0044bc2b  51                                 push       ecx
0044bc2c  6a 00                              push       0
0044bc2e  8b 15 e4 4e 5c 00                  mov        edx, dword ptr [0x5c4ee4]
0044bc34  52                                 push       edx
0044bc35  8d 45 dc                           lea        eax, [ebp - 0x24]
0044bc38  50                                 push       eax
0044bc39  6a 00                              push       0
0044bc3b  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
0044bc3e  51                                 push       ecx
0044bc3f  e8 58 4a 0f 00                     call       0x54069c
0044bc44  85 c0                              test       eax, eax
0044bc46  74 11                              je         0x44bc59
0044bc48  8b 55 d8                           mov        edx, dword ptr [ebp - 0x28]
0044bc4b  8b 02                              mov        eax, dword ptr [edx]
0044bc4d  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
0044bc50  51                                 push       ecx
0044bc51  8b 50 08                           mov        edx, dword ptr [eax + 8]
0044bc54  ff d2                              call       edx
0044bc56  90                                 nop        
0044bc57  eb 5e                              jmp        0x44bcb7
0044bc59  8b 45 d4                           mov        eax, dword ptr [ebp - 0x2c]
0044bc5c  05 30 07 00 06                     add        eax, 0x6000730
0044bc61  89 45 a8                           mov        dword ptr [ebp - 0x58], eax
0044bc64  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0044bc67  8b 11                              mov        edx, dword ptr [ecx]
0044bc69  52                                 push       edx
0044bc6a  8b 4d a8                           mov        ecx, dword ptr [ebp - 0x58]
0044bc6d  e8 0e 89 fc ff                     call       0x414580
0044bc72  8b 00                              mov        eax, dword ptr [eax]
0044bc74  8b 48 58                           mov        ecx, dword ptr [eax + 0x58]
0044bc77  89 4d c8                           mov        dword ptr [ebp - 0x38], ecx
0044bc7a  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0044bc7d  6b 42 04 18                        imul       eax, dword ptr [edx + 4], 0x18
0044bc81  89 45 c4                           mov        dword ptr [ebp - 0x3c], eax
0044bc84  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
0044bc87  03 4d c4                           add        ecx, dword ptr [ebp - 0x3c]
0044bc8a  8b 11                              mov        edx, dword ptr [ecx]
0044bc8c  89 55 a4                           mov        dword ptr [ebp - 0x5c], edx
0044bc8f  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0044bc92  03 45 c4                           add        eax, dword ptr [ebp - 0x3c]
0044bc95  8b 08                              mov        ecx, dword ptr [eax]
0044bc97  8b 11                              mov        edx, dword ptr [ecx]
0044bc99  8b 42 54                           mov        eax, dword ptr [edx + 0x54]
0044bc9c  89 45 a0                           mov        dword ptr [ebp - 0x60], eax
0044bc9f  6a 00                              push       0
0044bca1  8b 4d a4                           mov        ecx, dword ptr [ebp - 0x5c]
0044bca4  51                                 push       ecx
0044bca5  ff 55 a0                           call       dword ptr [ebp - 0x60]
0044bca8  8b 55 d8                           mov        edx, dword ptr [ebp - 0x28]
0044bcab  8b 02                              mov        eax, dword ptr [edx]
0044bcad  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
0044bcb0  51                                 push       ecx
0044bcb1  8b 50 08                           mov        edx, dword ptr [eax + 8]
0044bcb4  ff d2                              call       edx
0044bcb6  90                                 nop        
0044bcb7  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0044bcba  33 cd                              xor        ecx, ebp
0044bcbc  e8 2d 6d 0f 00                     call       0x5429ee
0044bcc1  8b e5                              mov        esp, ebp
0044bcc3  5d                                 pop        ebp
0044bcc4  c2 04 00                           ret        4
