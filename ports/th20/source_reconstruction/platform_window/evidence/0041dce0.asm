0041dce0  55                                 push       ebp
0041dce1  8b ec                              mov        ebp, esp
0041dce3  83 ec 0c                           sub        esp, 0xc
0041dce6  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041dce9  69 45 08 6c 01 00 00               imul       eax, dword ptr [ebp + 8], 0x16c
0041dcf0  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041dcf3  8d 94 01 78 02 00 00               lea        edx, [ecx + eax + 0x278]
0041dcfa  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041dcfd  89 90 00 0b 00 00                  mov        dword ptr [eax + 0xb00], edx
0041dd03  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041dd06  8b 91 00 0b 00 00                  mov        edx, dword ptr [ecx + 0xb00]
0041dd0c  81 c2 e0 00 00 00                  add        edx, 0xe0
0041dd12  52                                 push       edx
0041dd13  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041dd16  8b 88 00 0b 00 00                  mov        ecx, dword ptr [eax + 0xb00]
0041dd1c  51                                 push       ecx
0041dd1d  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041dd20  e8 cb c4 0b 00                     call       0x4da1f0
0041dd25  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0041dd28  8b 82 00 0b 00 00                  mov        eax, dword ptr [edx + 0xb00]
0041dd2e  50                                 push       eax
0041dd2f  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041dd32  e8 e9 c3 0b 00                     call       0x4da120
0041dd37  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041dd3a  8b 51 08                           mov        edx, dword ptr [ecx + 8]
0041dd3d  89 55 f8                           mov        dword ptr [ebp - 8], edx
0041dd40  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041dd43  8b 48 08                           mov        ecx, dword ptr [eax + 8]
0041dd46  8b 11                              mov        edx, dword ptr [ecx]
0041dd48  8b 82 bc 00 00 00                  mov        eax, dword ptr [edx + 0xbc]
0041dd4e  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
0041dd51  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041dd54  8b 91 00 0b 00 00                  mov        edx, dword ptr [ecx + 0xb00]
0041dd5a  81 c2 04 01 00 00                  add        edx, 0x104
0041dd60  52                                 push       edx
0041dd61  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0041dd64  50                                 push       eax
0041dd65  ff 55 f4                           call       dword ptr [ebp - 0xc]
0041dd68  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041dd6b  8b 91 00 0b 00 00                  mov        edx, dword ptr [ecx + 0xb00]
0041dd71  f3 0f 2a 82 fc 00 00 00            cvtsi2ss   xmm0, dword ptr [edx + 0xfc]
0041dd79  a1 28 00 5c 00                     mov        eax, dword ptr [0x5c0028]
0041dd7e  f3 0f 11 80 d0 00 00 00            movss      dword ptr [eax + 0xd0], xmm0
0041dd86  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041dd89  8b 91 00 0b 00 00                  mov        edx, dword ptr [ecx + 0xb00]
0041dd8f  f3 0f 2a 82 00 01 00 00            cvtsi2ss   xmm0, dword ptr [edx + 0x100]
0041dd97  a1 28 00 5c 00                     mov        eax, dword ptr [0x5c0028]
0041dd9c  f3 0f 11 80 d4 00 00 00            movss      dword ptr [eax + 0xd4], xmm0
0041dda4  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041dda7  8b 91 00 0b 00 00                  mov        edx, dword ptr [ecx + 0xb00]
0041ddad  b8 08 00 00 00                     mov        eax, 8
0041ddb2  d1 e0                              shl        eax, 1
0041ddb4  8b 0d 28 00 5c 00                  mov        ecx, dword ptr [0x5c0028]
0041ddba  8b 94 02 2c 01 00 00               mov        edx, dword ptr [edx + eax + 0x12c]
0041ddc1  89 91 c8 00 00 00                  mov        dword ptr [ecx + 0xc8], edx
0041ddc7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041ddca  8b 88 00 0b 00 00                  mov        ecx, dword ptr [eax + 0xb00]
0041ddd0  ba 08 00 00 00                     mov        edx, 8
0041ddd5  d1 e2                              shl        edx, 1
0041ddd7  a1 28 00 5c 00                     mov        eax, dword ptr [0x5c0028]
0041dddc  8b 8c 11 30 01 00 00               mov        ecx, dword ptr [ecx + edx + 0x130]
0041dde3  89 88 cc 00 00 00                  mov        dword ptr [eax + 0xcc], ecx
0041dde9  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0041ddec  c7 82 04 0b 00 00 00 00 00 00      mov        dword ptr [edx + 0xb04], 0
0041ddf6  8b e5                              mov        esp, ebp
0041ddf8  5d                                 pop        ebp
0041ddf9  c2 04 00                           ret        4
