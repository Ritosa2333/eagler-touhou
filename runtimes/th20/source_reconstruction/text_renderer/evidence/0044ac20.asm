0044ac20  55                                 push       ebp
0044ac21  8b ec                              mov        ebp, esp
0044ac23  6a ff                              push       -1
0044ac25  68 28 85 56 00                     push       0x568528
0044ac2a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0044ac30  50                                 push       eax
0044ac31  81 ec 2c 05 00 00                  sub        esp, 0x52c
0044ac37  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0044ac3c  33 c5                              xor        eax, ebp
0044ac3e  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0044ac41  50                                 push       eax
0044ac42  8d 45 f4                           lea        eax, [ebp - 0xc]
0044ac45  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0044ac4b  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0044ac52  6a 09                              push       9
0044ac54  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0044ac59  e8 b2 16 fc ff                     call       0x40c310
0044ac5e  50                                 push       eax
0044ac5f  8d 8d ec fa ff ff                  lea        ecx, [ebp - 0x514]
0044ac65  e8 c6 0a fc ff                     call       0x40b730
0044ac6a  c6 45 fc 01                        mov        byte ptr [ebp - 4], 1
0044ac6e  8d 45 58                           lea        eax, [ebp + 0x58]
0044ac71  89 85 d8 fa ff ff                  mov        dword ptr [ebp - 0x528], eax
0044ac77  8b 8d d8 fa ff ff                  mov        ecx, dword ptr [ebp - 0x528]
0044ac7d  51                                 push       ecx
0044ac7e  8b 55 54                           mov        edx, dword ptr [ebp + 0x54]
0044ac81  52                                 push       edx
0044ac82  8d 85 f0 fa ff ff                  lea        eax, [ebp - 0x510]
0044ac88  50                                 push       eax
0044ac89  e8 b2 cc ff ff                     call       0x447940
0044ac8e  83 c4 0c                           add        esp, 0xc
0044ac91  c7 85 d8 fa ff ff 00 00 00 00      mov        dword ptr [ebp - 0x528], 0
0044ac9b  8b 4d 10                           mov        ecx, dword ptr [ebp + 0x10]
0044ac9e  e8 2d d0 fe ff                     call       0x437cd0
0044aca3  89 85 d4 fa ff ff                  mov        dword ptr [ebp - 0x52c], eax
0044aca9  8b 4d 10                           mov        ecx, dword ptr [ebp + 0x10]
0044acac  8b 91 a8 03 00 00                  mov        edx, dword ptr [ecx + 0x3a8]
0044acb2  89 95 dc fa ff ff                  mov        dword ptr [ebp - 0x524], edx
0044acb8  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
0044acbb  8b 88 ac 03 00 00                  mov        ecx, dword ptr [eax + 0x3ac]
0044acc1  89 8d e0 fa ff ff                  mov        dword ptr [ebp - 0x520], ecx
0044acc7  8b 55 10                           mov        edx, dword ptr [ebp + 0x10]
0044acca  8b 82 b0 03 00 00                  mov        eax, dword ptr [edx + 0x3b0]
0044acd0  89 85 e4 fa ff ff                  mov        dword ptr [ebp - 0x51c], eax
0044acd6  8b 4d 10                           mov        ecx, dword ptr [ebp + 0x10]
0044acd9  8b 91 b4 03 00 00                  mov        edx, dword ptr [ecx + 0x3b4]
0044acdf  89 95 e8 fa ff ff                  mov        dword ptr [ebp - 0x518], edx
0044ace5  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
0044ace8  8a 88 9a 04 00 00                  mov        cl, byte ptr [eax + 0x49a]
0044acee  80 c9 01                           or         cl, 1
0044acf1  8b 55 10                           mov        edx, dword ptr [ebp + 0x10]
0044acf4  88 8a 9a 04 00 00                  mov        byte ptr [edx + 0x49a], cl
0044acfa  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
0044acfd  8b 88 9c 04 00 00                  mov        ecx, dword ptr [eax + 0x49c]
0044ad03  c1 e9 0a                           shr        ecx, 0xa
0044ad06  83 e1 01                           and        ecx, 1
0044ad09  89 8d d0 fa ff ff                  mov        dword ptr [ebp - 0x530], ecx
0044ad0f  8b 95 d4 fa ff ff                  mov        edx, dword ptr [ebp - 0x52c]
0044ad15  8b 42 08                           mov        eax, dword ptr [edx + 8]
0044ad18  50                                 push       eax
0044ad19  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0044ad1c  e8 df ac ff ff                     call       0x445a00
0044ad21  89 85 cc fa ff ff                  mov        dword ptr [ebp - 0x534], eax
0044ad27  83 ec 28                           sub        esp, 0x28
0044ad2a  8b cc                              mov        ecx, esp
0044ad2c  89 a5 c8 fa ff ff                  mov        dword ptr [ebp - 0x538], esp
0044ad32  8d 55 2c                           lea        edx, [ebp + 0x2c]
0044ad35  52                                 push       edx
0044ad36  e8 b5 93 fc ff                     call       0x4140f0
0044ad3b  8b 45 28                           mov        eax, dword ptr [ebp + 0x28]
0044ad3e  50                                 push       eax
0044ad3f  8d 8d dc fa ff ff                  lea        ecx, [ebp - 0x524]
0044ad45  51                                 push       ecx
0044ad46  8b 55 24                           mov        edx, dword ptr [ebp + 0x24]
0044ad49  d1 e2                              shl        edx, 1
0044ad4b  52                                 push       edx
0044ad4c  8b 45 1c                           mov        eax, dword ptr [ebp + 0x1c]
0044ad4f  50                                 push       eax
0044ad50  8b 8d d0 fa ff ff                  mov        ecx, dword ptr [ebp - 0x530]
0044ad56  51                                 push       ecx
0044ad57  8d 95 f0 fa ff ff                  lea        edx, [ebp - 0x510]
0044ad5d  52                                 push       edx
0044ad5e  8b 45 18                           mov        eax, dword ptr [ebp + 0x18]
0044ad61  50                                 push       eax
0044ad62  8b 4d 14                           mov        ecx, dword ptr [ebp + 0x14]
0044ad65  51                                 push       ecx
0044ad66  8b 55 20                           mov        edx, dword ptr [ebp + 0x20]
0044ad69  d1 e2                              shl        edx, 1
0044ad6b  52                                 push       edx
0044ad6c  8b 85 d4 fa ff ff                  mov        eax, dword ptr [ebp - 0x52c]
0044ad72  50                                 push       eax
0044ad73  8b 8d cc fa ff ff                  mov        ecx, dword ptr [ebp - 0x534]
0044ad79  51                                 push       ecx
0044ad7a  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0044ad7d  52                                 push       edx
0044ad7e  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0044ad81  e8 6a 52 00 00                     call       0x44fff0
0044ad86  c6 45 fc 00                        mov        byte ptr [ebp - 4], 0
0044ad8a  8d 8d ec fa ff ff                  lea        ecx, [ebp - 0x514]
0044ad90  e8 6b 0b fc ff                     call       0x40b900
0044ad95  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0044ad9c  8d 4d 2c                           lea        ecx, [ebp + 0x2c]
0044ad9f  e8 5c 96 fc ff                     call       0x414400
0044ada4  90                                 nop        
0044ada5  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0044ada8  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0044adab  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0044adb2  59                                 pop        ecx
0044adb3  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0044adb6  33 cd                              xor        ecx, ebp
0044adb8  e8 31 7c 0f 00                     call       0x5429ee
0044adbd  8b e5                              mov        esp, ebp
0044adbf  5d                                 pop        ebp
0044adc0  c3                                 ret        
