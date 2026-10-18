004abb80  55                                 push       ebp
004abb81  8b ec                              mov        ebp, esp
004abb83  6a ff                              push       -1
004abb85  68 00 76 56 00                     push       0x567600
004abb8a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
004abb90  50                                 push       eax
004abb91  51                                 push       ecx
004abb92  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004abb97  33 c5                              xor        eax, ebp
004abb99  50                                 push       eax
004abb9a  8d 45 f4                           lea        eax, [ebp - 0xc]
004abb9d  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
004abba3  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
004abba6  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004abba9  e8 72 41 f7 ff                     call       0x41fd20
004abbae  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004abbb1  c7 00 a4 05 57 00                  mov        dword ptr [eax], 0x5705a4
004abbb7  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004abbba  c7 41 10 00 00 00 00               mov        dword ptr [ecx + 0x10], 0
004abbc1  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
004abbc4  0f 57 c0                           xorps      xmm0, xmm0
004abbc7  f2 0f 11 42 18                     movsd      qword ptr [edx + 0x18], xmm0
004abbcc  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004abbcf  c7 40 20 00 00 00 00               mov        dword ptr [eax + 0x20], 0
004abbd6  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004abbd9  c7 41 24 00 00 00 00               mov        dword ptr [ecx + 0x24], 0
004abbe0  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
004abbe3  0f 57 c0                           xorps      xmm0, xmm0
004abbe6  f2 0f 11 42 28                     movsd      qword ptr [edx + 0x28], xmm0
004abbeb  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004abbee  0f 57 c0                           xorps      xmm0, xmm0
004abbf1  f2 0f 11 40 30                     movsd      qword ptr [eax + 0x30], xmm0
004abbf6  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004abbf9  0f 57 c0                           xorps      xmm0, xmm0
004abbfc  f3 0f 11 41 38                     movss      dword ptr [ecx + 0x38], xmm0
004abc01  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
004abc04  0f 57 c0                           xorps      xmm0, xmm0
004abc07  f2 0f 11 42 40                     movsd      qword ptr [edx + 0x40], xmm0
004abc0c  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004abc0f  0f 57 c0                           xorps      xmm0, xmm0
004abc12  f2 0f 11 40 48                     movsd      qword ptr [eax + 0x48], xmm0
004abc17  68 60 bb 4a 00                     push       0x4abb60
004abc1c  6a 08                              push       8
004abc1e  6a 08                              push       8
004abc20  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004abc23  83 c1 50                           add        ecx, 0x50
004abc26  51                                 push       ecx
004abc27  e8 f4 ff f5 ff                     call       0x40bc20
004abc2c  68 60 bb 4a 00                     push       0x4abb60
004abc31  6a 08                              push       8
004abc33  6a 08                              push       8
004abc35  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
004abc38  81 c2 90 00 00 00                  add        edx, 0x90
004abc3e  52                                 push       edx
004abc3f  e8 dc ff f5 ff                     call       0x40bc20
004abc44  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004abc47  81 c1 d0 00 00 00                  add        ecx, 0xd0
004abc4d  e8 0e ff ff ff                     call       0x4abb60
004abc52  68 b0 05 57 00                     push       0x5705b0 ; string='initialize FPSInf'
004abc57  e8 54 0a f6 ff                     call       0x40c6b0
004abc5c  83 c4 04                           add        esp, 4
004abc5f  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004abc62  a3 00 4a 5c 00                     mov        dword ptr [0x5c4a00], eax
004abc67  6a 00                              push       0
004abc69  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004abc6c  e8 af 58 fa ff                     call       0x451520
004abc71  90                                 nop        
004abc72  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004abc75  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004abc78  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
004abc7f  59                                 pop        ecx
004abc80  8b e5                              mov        esp, ebp
004abc82  5d                                 pop        ebp
004abc83  c3                                 ret        
