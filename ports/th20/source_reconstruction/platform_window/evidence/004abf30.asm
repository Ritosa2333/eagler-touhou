004abf30  55                                 push       ebp
004abf31  8b ec                              mov        ebp, esp
004abf33  83 ec 20                           sub        esp, 0x20
004abf36  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004abf3b  33 c5                              xor        eax, ebp
004abf3d  89 45 fc                           mov        dword ptr [ebp - 4], eax
004abf40  89 4d e0                           mov        dword ptr [ebp - 0x20], ecx
004abf43  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004abf48  e8 73 01 00 00                     call       0x4ac0c0
004abf4d  83 f8 0f                           cmp        eax, 0xf
004abf50  74 0f                              je         0x4abf61
004abf52  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
004abf57  e8 64 01 00 00                     call       0x4ac0c0
004abf5c  83 f8 04                           cmp        eax, 4
004abf5f  75 17                              jne        0x4abf78
004abf61  e8 da cf fe ff                     call       0x498f40
004abf66  8b c8                              mov        ecx, eax
004abf68  e8 f3 01 00 00                     call       0x4ac160
004abf6d  0f b6 c0                           movzx      eax, al
004abf70  85 c0                              test       eax, eax
004abf72  0f 84 f2 00 00 00                  je         0x4ac06a
004abf78  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
004abf7b  f3 0f 10 41 38                     movss      xmm0, dword ptr [ecx + 0x38]
004abf80  f3 0f 11 45 ec                     movss      dword ptr [ebp - 0x14], xmm0
004abf85  83 3d 98 06 5c 00 00               cmp        dword ptr [0x5c0698], 0
004abf8c  0f 84 d8 00 00 00                  je         0x4ac06a
004abf92  f3 0f 10 05 80 f2 56 00            movss      xmm0, dword ptr [0x56f280]
004abf9a  0f 2f 45 ec                        comiss     xmm0, dword ptr [ebp - 0x14]
004abf9e  76 09                              jbe        0x4abfa9
004abfa0  c7 45 e4 ff 50 50 ff               mov        dword ptr [ebp - 0x1c], 0xff5050ff
004abfa7  eb 24                              jmp        0x4abfcd
004abfa9  f3 0f 10 05 60 05 57 00            movss      xmm0, dword ptr [0x570560]
004abfb1  0f 2f 45 ec                        comiss     xmm0, dword ptr [ebp - 0x14]
004abfb5  76 09                              jbe        0x4abfc0
004abfb7  c7 45 e8 ff a0 a0 ff               mov        dword ptr [ebp - 0x18], 0xffa0a0ff
004abfbe  eb 07                              jmp        0x4abfc7
004abfc0  c7 45 e8 ff ff ff ff               mov        dword ptr [ebp - 0x18], 0xffffffff
004abfc7  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004abfca  89 55 e4                           mov        dword ptr [ebp - 0x1c], edx
004abfcd  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
004abfd0  50                                 push       eax
004abfd1  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004abfd7  e8 f4 4a fc ff                     call       0x470ad0
004abfdc  51                                 push       ecx
004abfdd  0f 57 c0                           xorps      xmm0, xmm0
004abfe0  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004abfe5  51                                 push       ecx
004abfe6  f3 0f 10 05 1c 06 57 00            movss      xmm0, dword ptr [0x57061c]
004abfee  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004abff3  51                                 push       ecx
004abff4  f3 0f 10 05 20 06 57 00            movss      xmm0, dword ptr [0x570620]
004abffc  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004ac001  8d 4d f0                           lea        ecx, [ebp - 0x10]
004ac004  e8 c7 6d f7 ff                     call       0x422dd0
004ac009  6a 01                              push       1
004ac00b  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004ac011  e8 ea ca fd ff                     call       0x488b00
004ac016  90                                 nop        
004ac017  6a 01                              push       1
004ac019  83 ec 08                           sub        esp, 8
004ac01c  8b cc                              mov        ecx, esp
004ac01e  68 10 06 57 00                     push       0x570610
004ac023  e8 a8 1d f6 ff                     call       0x40ddd0
004ac028  f3 0f 5a 45 ec                     cvtss2sd   xmm0, dword ptr [ebp - 0x14]
004ac02d  f2 0f 58 05 80 ff 56 00            addsd      xmm0, qword ptr [0x56ff80]
004ac035  f2 0f 5a c0                        cvtsd2ss   xmm0, xmm0
004ac039  51                                 push       ecx
004ac03a  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
004ac03f  8d 4d f0                           lea        ecx, [ebp - 0x10]
004ac042  51                                 push       ecx
004ac043  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004ac049  e8 b2 0d fc ff                     call       0x46ce00
004ac04e  90                                 nop        
004ac04f  6a ff                              push       -1
004ac051  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004ac057  e8 74 4a fc ff                     call       0x470ad0
004ac05c  6a 00                              push       0
004ac05e  8b 0d 98 06 5c 00                  mov        ecx, dword ptr [0x5c0698]
004ac064  e8 97 ca fd ff                     call       0x488b00
004ac069  90                                 nop        
004ac06a  b8 01 00 00 00                     mov        eax, 1
004ac06f  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004ac072  33 cd                              xor        ecx, ebp
004ac074  e8 75 69 09 00                     call       0x5429ee
004ac079  8b e5                              mov        esp, ebp
004ac07b  5d                                 pop        ebp
004ac07c  c3                                 ret        
