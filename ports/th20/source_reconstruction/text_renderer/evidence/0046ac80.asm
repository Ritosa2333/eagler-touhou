0046ac80  55                                 push       ebp
0046ac81  8b ec                              mov        ebp, esp
0046ac83  83 ec 08                           sub        esp, 8
0046ac86  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0046ac89  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046ac8c  e8 7f dd fa ff                     call       0x418a10
0046ac91  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046ac94  83 c1 14                           add        ecx, 0x14
0046ac97  e8 a4 de fd ff                     call       0x448b40
0046ac9c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046ac9f  81 c1 f8 05 00 00                  add        ecx, 0x5f8
0046aca5  e8 16 d0 fd ff                     call       0x447cc0
0046acaa  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046acad  81 c1 14 06 00 00                  add        ecx, 0x614
0046acb3  e8 58 81 fb ff                     call       0x422e10
0046acb8  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046acbb  c7 80 20 06 00 00 00 00 00 00      mov        dword ptr [eax + 0x620], 0
0046acc5  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046acc8  c7 81 24 06 00 00 00 00 00 00      mov        dword ptr [ecx + 0x624], 0
0046acd2  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046acd5  c7 82 28 06 00 00 00 00 00 00      mov        dword ptr [edx + 0x628], 0
0046acdf  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046ace2  c7 80 2c 06 00 00 00 00 00 00      mov        dword ptr [eax + 0x62c], 0
0046acec  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046acef  c7 81 30 06 00 00 00 00 00 00      mov        dword ptr [ecx + 0x630], 0
0046acf9  33 d2                              xor        edx, edx
0046acfb  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046acfe  05 34 06 00 00                     add        eax, 0x634
0046ad03  89 10                              mov        dword ptr [eax], edx
0046ad05  89 50 04                           mov        dword ptr [eax + 4], edx
0046ad08  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046ad0b  81 c1 3c 06 00 00                  add        ecx, 0x63c
0046ad11  e8 1a 31 fa ff                     call       0x40de30
0046ad16  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046ad19  c7 81 4c 06 00 00 00 00 00 00      mov        dword ptr [ecx + 0x64c], 0
0046ad23  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046ad26  c7 82 50 06 00 00 00 00 00 00      mov        dword ptr [edx + 0x650], 0
0046ad30  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046ad33  c7 80 54 06 00 00 00 00 00 00      mov        dword ptr [eax + 0x654], 0
0046ad3d  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046ad40  0f 57 c0                           xorps      xmm0, xmm0
0046ad43  f3 0f 11 81 58 06 00 00            movss      dword ptr [ecx + 0x658], xmm0
0046ad4b  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046ad4e  0f 57 c0                           xorps      xmm0, xmm0
0046ad51  f3 0f 11 82 5c 06 00 00            movss      dword ptr [edx + 0x65c], xmm0
0046ad59  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046ad5c  c7 80 60 06 00 00 01 00 00 00      mov        dword ptr [eax + 0x660], 1
0046ad66  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046ad69  c7 81 64 06 00 00 01 00 00 00      mov        dword ptr [ecx + 0x664], 1
0046ad73  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046ad76  c7 82 68 06 00 00 00 00 00 00      mov        dword ptr [edx + 0x668], 0
0046ad80  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046ad83  81 c1 6c 06 00 00                  add        ecx, 0x66c
0046ad89  e8 52 d0 fd ff                     call       0x447de0
0046ad8e  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046ad91  c7 80 70 06 00 00 00 00 00 00      mov        dword ptr [eax + 0x670], 0
0046ad9b  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046ad9e  c7 81 74 06 00 00 00 00 00 00      mov        dword ptr [ecx + 0x674], 0
0046ada8  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046adab  c6 82 78 06 00 00 00               mov        byte ptr [edx + 0x678], 0
0046adb2  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046adb5  c7 80 7c 06 00 00 00 00 00 00      mov        dword ptr [eax + 0x67c], 0
0046adbf  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046adc2  81 c1 80 06 00 00                  add        ecx, 0x680
0046adc8  89 4d f8                           mov        dword ptr [ebp - 8], ecx
0046adcb  6a 00                              push       0
0046adcd  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0046add0  e8 3b 09 fa ff                     call       0x40b710
0046add5  90                                 nop        
0046add6  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0046add9  8b e5                              mov        esp, ebp
0046addb  5d                                 pop        ebp
0046addc  c3                                 ret        
