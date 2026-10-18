0044fff0  55                                 push       ebp
0044fff1  8b ec                              mov        ebp, esp
0044fff3  6a ff                              push       -1
0044fff5  68 fd 86 56 00                     push       0x5686fd
0044fffa  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
00450000  50                                 push       eax
00450001  83 ec 1c                           sub        esp, 0x1c
00450004  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
00450009  33 c5                              xor        eax, ebp
0045000b  50                                 push       eax
0045000c  8d 45 f4                           lea        eax, [ebp - 0xc]
0045000f  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
00450015  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
00450018  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0045001f  83 7d 24 00                        cmp        dword ptr [ebp + 0x24], 0
00450023  75 6a                              jne        0x45008f
00450025  83 ec 28                           sub        esp, 0x28
00450028  8b cc                              mov        ecx, esp
0045002a  89 65 ec                           mov        dword ptr [ebp - 0x14], esp
0045002d  8d 45 38                           lea        eax, [ebp + 0x38]
00450030  50                                 push       eax
00450031  e8 ba 40 fc ff                     call       0x4140f0
00450036  8b 4d 34                           mov        ecx, dword ptr [ebp + 0x34]
00450039  51                                 push       ecx
0045003a  6a 01                              push       1
0045003c  8b 55 2c                           mov        edx, dword ptr [ebp + 0x2c]
0045003f  52                                 push       edx
00450040  8b 45 28                           mov        eax, dword ptr [ebp + 0x28]
00450043  50                                 push       eax
00450044  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
00450047  51                                 push       ecx
00450048  8b 55 20                           mov        edx, dword ptr [ebp + 0x20]
0045004b  52                                 push       edx
0045004c  8b 45 1c                           mov        eax, dword ptr [ebp + 0x1c]
0045004f  50                                 push       eax
00450050  8b 4d 18                           mov        ecx, dword ptr [ebp + 0x18]
00450053  51                                 push       ecx
00450054  8b 55 14                           mov        edx, dword ptr [ebp + 0x14]
00450057  52                                 push       edx
00450058  8b 45 30                           mov        eax, dword ptr [ebp + 0x30]
0045005b  50                                 push       eax
0045005c  e8 cf 57 fc ff                     call       0x415830
00450061  83 c4 50                           add        esp, 0x50
00450064  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
00450067  89 55 e4                           mov        dword ptr [ebp - 0x1c], edx
0045006a  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0045006d  8b 55 e0                           mov        edx, dword ptr [ebp - 0x20]
00450070  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
00450073  89 11                              mov        dword ptr [ecx], edx
00450075  89 41 04                           mov        dword ptr [ecx + 4], eax
00450078  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0045007f  8d 4d 38                           lea        ecx, [ebp + 0x38]
00450082  e8 79 43 fc ff                     call       0x414400
00450087  90                                 nop        
00450088  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0045008b  eb 78                              jmp        0x450105
0045008f  83 ec 28                           sub        esp, 0x28
00450092  8b cc                              mov        ecx, esp
00450094  89 65 e8                           mov        dword ptr [ebp - 0x18], esp
00450097  8d 55 38                           lea        edx, [ebp + 0x38]
0045009a  52                                 push       edx
0045009b  e8 50 40 fc ff                     call       0x4140f0
004500a0  8b 45 34                           mov        eax, dword ptr [ebp + 0x34]
004500a3  50                                 push       eax
004500a4  6a 00                              push       0
004500a6  8b 4d 2c                           mov        ecx, dword ptr [ebp + 0x2c]
004500a9  51                                 push       ecx
004500aa  8b 55 28                           mov        edx, dword ptr [ebp + 0x28]
004500ad  52                                 push       edx
004500ae  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
004500b1  50                                 push       eax
004500b2  8b 4d 20                           mov        ecx, dword ptr [ebp + 0x20]
004500b5  51                                 push       ecx
004500b6  6a 00                              push       0
004500b8  8b 55 18                           mov        edx, dword ptr [ebp + 0x18]
004500bb  52                                 push       edx
004500bc  8b 45 14                           mov        eax, dword ptr [ebp + 0x14]
004500bf  50                                 push       eax
004500c0  8b 4d 30                           mov        ecx, dword ptr [ebp + 0x30]
004500c3  51                                 push       ecx
004500c4  e8 67 57 fc ff                     call       0x415830
004500c9  83 c4 50                           add        esp, 0x50
004500cc  89 45 d8                           mov        dword ptr [ebp - 0x28], eax
004500cf  89 55 dc                           mov        dword ptr [ebp - 0x24], edx
004500d2  8b 55 08                           mov        edx, dword ptr [ebp + 8]
004500d5  8b 45 d8                           mov        eax, dword ptr [ebp - 0x28]
004500d8  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
004500db  89 02                              mov        dword ptr [edx], eax
004500dd  89 4a 04                           mov        dword ptr [edx + 4], ecx
004500e0  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
004500e7  8d 4d 38                           lea        ecx, [ebp + 0x38]
004500ea  e8 11 43 fc ff                     call       0x414400
004500ef  90                                 nop        
004500f0  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004500f3  eb 10                              jmp        0x450105
00450105  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
00450108  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0045010f  59                                 pop        ecx
00450110  8b e5                              mov        esp, ebp
00450112  5d                                 pop        ebp
00450113  c2 58 00                           ret        0x58
