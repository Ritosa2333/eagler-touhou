00416140  55                                 push       ebp
00416141  8b ec                              mov        ebp, esp
00416143  6a ff                              push       -1
00416145  68 2d 7e 56 00                     push       0x567e2d
0041614a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
00416150  50                                 push       eax
00416151  83 ec 40                           sub        esp, 0x40
00416154  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
00416159  33 c5                              xor        eax, ebp
0041615b  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0041615e  50                                 push       eax
0041615f  8d 45 f4                           lea        eax, [ebp - 0xc]
00416162  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
00416168  b9 e0 66 5b 00                     mov        ecx, 0x5b66e0
0041616d  e8 be c5 ff ff                     call       0x412730
00416172  85 c0                              test       eax, eax
00416174  0f 84 ab 00 00 00                  je         0x416225
0041617a  6a 12                              push       0x12
0041617c  b9 40 02 5c 00                     mov        ecx, 0x5c0240
00416181  e8 8a 61 ff ff                     call       0x40c310
00416186  50                                 push       eax
00416187  8d 4d e8                           lea        ecx, [ebp - 0x18]
0041618a  e8 a1 55 ff ff                     call       0x40b730
0041618f  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
00416196  6a 13                              push       0x13
00416198  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0041619d  e8 6e 61 ff ff                     call       0x40c310
004161a2  50                                 push       eax
004161a3  8d 4d ec                           lea        ecx, [ebp - 0x14]
004161a6  e8 85 55 ff ff                     call       0x40b730
004161ab  c6 45 fc 01                        mov        byte ptr [ebp - 4], 1
004161af  8d 4d c0                           lea        ecx, [ebp - 0x40]
004161b2  e8 99 df ff ff                     call       0x414150
004161b7  c6 45 fc 02                        mov        byte ptr [ebp - 4], 2
004161bb  b9 e0 66 5b 00                     mov        ecx, 0x5b66e0
004161c0  e8 3b f4 ff ff                     call       0x415600
004161c5  50                                 push       eax
004161c6  8d 4d c0                           lea        ecx, [ebp - 0x40]
004161c9  e8 72 e3 ff ff                     call       0x414540
004161ce  90                                 nop        
004161cf  8d 45 b8                           lea        eax, [ebp - 0x48]
004161d2  50                                 push       eax
004161d3  b9 e0 66 5b 00                     mov        ecx, 0x5b66e0
004161d8  e8 d3 ee ff ff                     call       0x4150b0
004161dd  8b 08                              mov        ecx, dword ptr [eax]
004161df  89 4d bc                           mov        dword ptr [ebp - 0x44], ecx
004161e2  8b 55 bc                           mov        edx, dword ptr [ebp - 0x44]
004161e5  52                                 push       edx
004161e6  8d 45 b4                           lea        eax, [ebp - 0x4c]
004161e9  50                                 push       eax
004161ea  b9 e0 66 5b 00                     mov        ecx, 0x5b66e0
004161ef  e8 5c f3 ff ff                     call       0x415550
004161f4  90                                 nop        
004161f5  8d 4d c0                           lea        ecx, [ebp - 0x40]
004161f8  e8 43 e5 ff ff                     call       0x414740
004161fd  c6 45 fc 01                        mov        byte ptr [ebp - 4], 1
00416201  8d 4d c0                           lea        ecx, [ebp - 0x40]
00416204  e8 f7 e1 ff ff                     call       0x414400
00416209  c6 45 fc 00                        mov        byte ptr [ebp - 4], 0
0041620d  8d 4d ec                           lea        ecx, [ebp - 0x14]
00416210  e8 eb 56 ff ff                     call       0x40b900
00416215  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0041621c  8d 4d e8                           lea        ecx, [ebp - 0x18]
0041621f  e8 dc 56 ff ff                     call       0x40b900
00416224  90                                 nop        
00416225  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
00416228  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0041622f  59                                 pop        ecx
00416230  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00416233  33 cd                              xor        ecx, ebp
00416235  e8 b4 c7 12 00                     call       0x5429ee
0041623a  8b e5                              mov        esp, ebp
0041623c  5d                                 pop        ebp
0041623d  c3                                 ret        
