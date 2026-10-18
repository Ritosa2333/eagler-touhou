00416d20  55                                 push       ebp
00416d21  8b ec                              mov        ebp, esp
00416d23  81 ec 10 01 00 00                  sub        esp, 0x110
00416d29  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
00416d2e  33 c5                              xor        eax, ebp
00416d30  89 45 fc                           mov        dword ptr [ebp - 4], eax
00416d33  6a 00                              push       0
00416d35  ff 15 ac c2 56 00                  call       dword ptr [0x56c2ac] ; USER32.dll!GetDC
00416d3b  89 45 94                           mov        dword ptr [ebp - 0x6c], eax
00416d3e  6a 5c                              push       0x5c
00416d40  6a 00                              push       0
00416d42  8d 45 a0                           lea        eax, [ebp - 0x60]
00416d45  50                                 push       eax
00416d46  e8 15 da 12 00                     call       0x544760
00416d4b  83 c4 0c                           add        esp, 0xc
00416d4e  c7 05 48 67 5b 00 ec 66 5b 00      mov        dword ptr [0x5b6748], 0x5b66ec
00416d58  68 20 c8 56 00                     push       0x56c820
00416d5d  6a 20                              push       0x20
00416d5f  8d 4d bc                           lea        ecx, [ebp - 0x44]
00416d62  51                                 push       ecx
00416d63  e8 98 13 13 00                     call       0x548100
00416d68  83 c4 0c                           add        esp, 0xc
00416d6b  6a 00                              push       0
00416d6d  6a 00                              push       0
00416d6f  68 20 48 41 00                     push       0x414820
00416d74  8d 55 a0                           lea        edx, [ebp - 0x60]
00416d77  52                                 push       edx
00416d78  8b 45 94                           mov        eax, dword ptr [ebp - 0x6c]
00416d7b  50                                 push       eax
00416d7c  ff 15 34 c0 56 00                  call       dword ptr [0x56c034] ; GDI32.dll!EnumFontFamiliesExW
00416d82  c7 05 48 67 5b 00 ee 66 5b 00      mov        dword ptr [0x5b6748], 0x5b66ee
00416d8c  68 2c c8 56 00                     push       0x56c82c
00416d91  6a 20                              push       0x20
00416d93  8d 4d bc                           lea        ecx, [ebp - 0x44]
00416d96  51                                 push       ecx
00416d97  e8 64 13 13 00                     call       0x548100
00416d9c  83 c4 0c                           add        esp, 0xc
00416d9f  6a 00                              push       0
00416da1  6a 00                              push       0
00416da3  68 20 48 41 00                     push       0x414820
00416da8  8d 55 a0                           lea        edx, [ebp - 0x60]
00416dab  52                                 push       edx
00416dac  8b 45 94                           mov        eax, dword ptr [ebp - 0x6c]
00416daf  50                                 push       eax
00416db0  ff 15 34 c0 56 00                  call       dword ptr [0x56c034] ; GDI32.dll!EnumFontFamiliesExW
00416db6  90                                 nop        
00416db7  0f b6 0d ed 66 5b 00               movzx      ecx, byte ptr [0x5b66ed]
00416dbe  85 c9                              test       ecx, ecx
00416dc0  0f 84 93 03 00 00                  je         0x417159
00416dc6  ba 04 00 00 00                     mov        edx, 4
00416dcb  6b c2 00                           imul       eax, edx, 0
00416dce  c7 44 05 98 34 c8 56 00            mov        dword ptr [ebp + eax - 0x68], 0x56c834
00416dd6  b9 04 00 00 00                     mov        ecx, 4
00416ddb  6b d1 00                           imul       edx, ecx, 0
00416dde  8b 44 15 98                        mov        eax, dword ptr [ebp + edx - 0x68]
00416de2  50                                 push       eax
00416de3  6a 11                              push       0x11
00416de5  6a 02                              push       2
00416de7  6a 00                              push       0
00416de9  6a 00                              push       0
00416deb  68 80 00 00 00                     push       0x80
00416df0  6a 00                              push       0
00416df2  6a 00                              push       0
00416df4  6a 00                              push       0
00416df6  68 90 01 00 00                     push       0x190
00416dfb  6a 00                              push       0
00416dfd  6a 00                              push       0
00416dff  6a 00                              push       0
00416e01  6a 18                              push       0x18
00416e03  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00416e09  89 85 18 ff ff ff                  mov        dword ptr [ebp - 0xe8], eax
00416e0f  6a 00                              push       0
00416e11  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00416e16  e8 65 d7 ff ff                     call       0x414580
00416e1b  8b 8d 18 ff ff ff                  mov        ecx, dword ptr [ebp - 0xe8]
00416e21  89 08                              mov        dword ptr [eax], ecx
00416e23  ba 04 00 00 00                     mov        edx, 4
00416e28  6b c2 00                           imul       eax, edx, 0
00416e2b  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
00416e2f  51                                 push       ecx
00416e30  6a 11                              push       0x11
00416e32  6a 02                              push       2
00416e34  6a 00                              push       0
00416e36  6a 00                              push       0
00416e38  68 80 00 00 00                     push       0x80
00416e3d  6a 00                              push       0
00416e3f  6a 00                              push       0
00416e41  6a 00                              push       0
00416e43  68 90 01 00 00                     push       0x190
00416e48  6a 00                              push       0
00416e4a  6a 00                              push       0
00416e4c  6a 00                              push       0
00416e4e  6a 1e                              push       0x1e
00416e50  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00416e56  89 85 f4 fe ff ff                  mov        dword ptr [ebp - 0x10c], eax
00416e5c  6a 01                              push       1
00416e5e  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00416e63  e8 18 d7 ff ff                     call       0x414580
00416e68  8b 95 f4 fe ff ff                  mov        edx, dword ptr [ebp - 0x10c]
00416e6e  89 10                              mov        dword ptr [eax], edx
00416e70  b8 04 00 00 00                     mov        eax, 4
00416e75  6b c8 00                           imul       ecx, eax, 0
00416e78  8b 54 0d 98                        mov        edx, dword ptr [ebp + ecx - 0x68]
00416e7c  52                                 push       edx
00416e7d  6a 11                              push       0x11
00416e7f  6a 02                              push       2
00416e81  6a 00                              push       0
00416e83  6a 00                              push       0
00416e85  68 80 00 00 00                     push       0x80
00416e8a  6a 00                              push       0
00416e8c  6a 00                              push       0
00416e8e  6a 00                              push       0
00416e90  68 90 01 00 00                     push       0x190
00416e95  6a 00                              push       0
00416e97  6a 00                              push       0
00416e99  6a 00                              push       0
00416e9b  6a 24                              push       0x24
00416e9d  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00416ea3  89 45 90                           mov        dword ptr [ebp - 0x70], eax
00416ea6  6a 02                              push       2
00416ea8  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00416ead  e8 ce d6 ff ff                     call       0x414580
00416eb2  8b 4d 90                           mov        ecx, dword ptr [ebp - 0x70]
00416eb5  89 08                              mov        dword ptr [eax], ecx
00416eb7  ba 04 00 00 00                     mov        edx, 4
00416ebc  6b c2 00                           imul       eax, edx, 0
00416ebf  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
00416ec3  51                                 push       ecx
00416ec4  6a 11                              push       0x11
00416ec6  6a 02                              push       2
00416ec8  6a 00                              push       0
00416eca  6a 00                              push       0
00416ecc  68 80 00 00 00                     push       0x80
00416ed1  6a 00                              push       0
00416ed3  6a 00                              push       0
00416ed5  6a 00                              push       0
00416ed7  68 90 01 00 00                     push       0x190
00416edc  6a 00                              push       0
00416ede  6a 00                              push       0
00416ee0  6a 00                              push       0
00416ee2  6a 2a                              push       0x2a
00416ee4  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00416eea  89 45 8c                           mov        dword ptr [ebp - 0x74], eax
00416eed  6a 03                              push       3
00416eef  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00416ef4  e8 87 d6 ff ff                     call       0x414580
00416ef9  8b 55 8c                           mov        edx, dword ptr [ebp - 0x74]
00416efc  89 10                              mov        dword ptr [eax], edx
00416efe  b8 04 00 00 00                     mov        eax, 4
00416f03  6b c8 00                           imul       ecx, eax, 0
00416f06  8b 54 0d 98                        mov        edx, dword ptr [ebp + ecx - 0x68]
00416f0a  52                                 push       edx
00416f0b  6a 11                              push       0x11
00416f0d  6a 02                              push       2
00416f0f  6a 00                              push       0
00416f11  6a 00                              push       0
00416f13  68 80 00 00 00                     push       0x80
00416f18  6a 00                              push       0
00416f1a  6a 00                              push       0
00416f1c  6a 00                              push       0
00416f1e  68 90 01 00 00                     push       0x190
00416f23  6a 00                              push       0
00416f25  6a 00                              push       0
00416f27  6a 00                              push       0
00416f29  6a 30                              push       0x30
00416f2b  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00416f31  89 45 88                           mov        dword ptr [ebp - 0x78], eax
00416f34  6a 04                              push       4
00416f36  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00416f3b  e8 40 d6 ff ff                     call       0x414580
00416f40  8b 4d 88                           mov        ecx, dword ptr [ebp - 0x78]
00416f43  89 08                              mov        dword ptr [eax], ecx
00416f45  ba 04 00 00 00                     mov        edx, 4
00416f4a  6b c2 00                           imul       eax, edx, 0
00416f4d  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
00416f51  51                                 push       ecx
00416f52  6a 11                              push       0x11
00416f54  6a 02                              push       2
00416f56  6a 00                              push       0
00416f58  6a 00                              push       0
00416f5a  68 80 00 00 00                     push       0x80
00416f5f  6a 00                              push       0
00416f61  6a 00                              push       0
00416f63  6a 00                              push       0
00416f65  68 90 01 00 00                     push       0x190
00416f6a  6a 00                              push       0
00416f6c  6a 00                              push       0
00416f6e  6a 00                              push       0
00416f70  6a 36                              push       0x36
00416f72  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00416f78  89 45 84                           mov        dword ptr [ebp - 0x7c], eax
00416f7b  6a 05                              push       5
00416f7d  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00416f82  e8 f9 d5 ff ff                     call       0x414580
00416f87  8b 55 84                           mov        edx, dword ptr [ebp - 0x7c]
00416f8a  89 10                              mov        dword ptr [eax], edx
00416f8c  b8 04 00 00 00                     mov        eax, 4
00416f91  6b c8 00                           imul       ecx, eax, 0
00416f94  8b 54 0d 98                        mov        edx, dword ptr [ebp + ecx - 0x68]
00416f98  52                                 push       edx
00416f99  6a 11                              push       0x11
00416f9b  6a 02                              push       2
00416f9d  6a 00                              push       0
00416f9f  6a 00                              push       0
00416fa1  68 80 00 00 00                     push       0x80
00416fa6  6a 00                              push       0
00416fa8  6a 00                              push       0
00416faa  6a 00                              push       0
00416fac  68 90 01 00 00                     push       0x190
00416fb1  6a 00                              push       0
00416fb3  6a 00                              push       0
00416fb5  6a 00                              push       0
00416fb7  6a 3c                              push       0x3c
00416fb9  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00416fbf  89 45 80                           mov        dword ptr [ebp - 0x80], eax
00416fc2  6a 06                              push       6
00416fc4  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00416fc9  e8 b2 d5 ff ff                     call       0x414580
00416fce  8b 4d 80                           mov        ecx, dword ptr [ebp - 0x80]
00416fd1  89 08                              mov        dword ptr [eax], ecx
00416fd3  ba 04 00 00 00                     mov        edx, 4
00416fd8  6b c2 00                           imul       eax, edx, 0
00416fdb  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
00416fdf  51                                 push       ecx
00416fe0  6a 11                              push       0x11
00416fe2  6a 02                              push       2
00416fe4  6a 00                              push       0
00416fe6  6a 00                              push       0
00416fe8  68 80 00 00 00                     push       0x80
00416fed  6a 00                              push       0
00416fef  6a 00                              push       0
00416ff1  6a 00                              push       0
00416ff3  68 90 01 00 00                     push       0x190
00416ff8  6a 00                              push       0
00416ffa  6a 00                              push       0
00416ffc  6a 00                              push       0
00416ffe  6a 42                              push       0x42
00417000  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00417006  89 85 7c ff ff ff                  mov        dword ptr [ebp - 0x84], eax
0041700c  6a 07                              push       7
0041700e  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00417013  e8 68 d5 ff ff                     call       0x414580
00417018  8b 95 7c ff ff ff                  mov        edx, dword ptr [ebp - 0x84]
0041701e  89 10                              mov        dword ptr [eax], edx
00417020  b8 04 00 00 00                     mov        eax, 4
00417025  6b c8 00                           imul       ecx, eax, 0
00417028  8b 54 0d 98                        mov        edx, dword ptr [ebp + ecx - 0x68]
0041702c  52                                 push       edx
0041702d  6a 11                              push       0x11
0041702f  6a 02                              push       2
00417031  6a 00                              push       0
00417033  6a 00                              push       0
00417035  68 80 00 00 00                     push       0x80
0041703a  6a 00                              push       0
0041703c  6a 00                              push       0
0041703e  6a 00                              push       0
00417040  68 90 01 00 00                     push       0x190
00417045  6a 00                              push       0
00417047  6a 00                              push       0
00417049  6a 00                              push       0
0041704b  6a 48                              push       0x48
0041704d  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00417053  89 85 78 ff ff ff                  mov        dword ptr [ebp - 0x88], eax
00417059  6a 08                              push       8
0041705b  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00417060  e8 1b d5 ff ff                     call       0x414580
00417065  8b 8d 78 ff ff ff                  mov        ecx, dword ptr [ebp - 0x88]
0041706b  89 08                              mov        dword ptr [eax], ecx
0041706d  ba 04 00 00 00                     mov        edx, 4
00417072  6b c2 00                           imul       eax, edx, 0
00417075  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
00417079  51                                 push       ecx
0041707a  6a 11                              push       0x11
0041707c  6a 02                              push       2
0041707e  6a 00                              push       0
00417080  6a 00                              push       0
00417082  68 80 00 00 00                     push       0x80
00417087  6a 00                              push       0
00417089  6a 00                              push       0
0041708b  6a 00                              push       0
0041708d  68 90 01 00 00                     push       0x190
00417092  6a 00                              push       0
00417094  6a 00                              push       0
00417096  6a 00                              push       0
00417098  6a 5a                              push       0x5a
0041709a  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
004170a0  89 85 74 ff ff ff                  mov        dword ptr [ebp - 0x8c], eax
004170a6  6a 09                              push       9
004170a8  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
004170ad  e8 ce d4 ff ff                     call       0x414580
004170b2  8b 95 74 ff ff ff                  mov        edx, dword ptr [ebp - 0x8c]
004170b8  89 10                              mov        dword ptr [eax], edx
004170ba  b8 04 00 00 00                     mov        eax, 4
004170bf  6b c8 00                           imul       ecx, eax, 0
004170c2  8b 54 0d 98                        mov        edx, dword ptr [ebp + ecx - 0x68]
004170c6  52                                 push       edx
004170c7  6a 11                              push       0x11
004170c9  6a 02                              push       2
004170cb  6a 00                              push       0
004170cd  6a 00                              push       0
004170cf  68 80 00 00 00                     push       0x80
004170d4  6a 00                              push       0
004170d6  6a 00                              push       0
004170d8  6a 00                              push       0
004170da  68 90 01 00 00                     push       0x190
004170df  6a 00                              push       0
004170e1  6a 00                              push       0
004170e3  6a 00                              push       0
004170e5  6a 60                              push       0x60
004170e7  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
004170ed  89 85 70 ff ff ff                  mov        dword ptr [ebp - 0x90], eax
004170f3  6a 0a                              push       0xa
004170f5  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
004170fa  e8 81 d4 ff ff                     call       0x414580
004170ff  8b 8d 70 ff ff ff                  mov        ecx, dword ptr [ebp - 0x90]
00417105  89 08                              mov        dword ptr [eax], ecx
00417107  ba 04 00 00 00                     mov        edx, 4
0041710c  6b c2 00                           imul       eax, edx, 0
0041710f  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
00417113  51                                 push       ecx
00417114  6a 11                              push       0x11
00417116  6a 02                              push       2
00417118  6a 00                              push       0
0041711a  6a 00                              push       0
0041711c  68 80 00 00 00                     push       0x80
00417121  6a 00                              push       0
00417123  6a 00                              push       0
00417125  6a 00                              push       0
00417127  68 58 02 00 00                     push       0x258
0041712c  6a 00                              push       0
0041712e  6a 00                              push       0
00417130  6a 00                              push       0
00417132  6a 30                              push       0x30
00417134  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
0041713a  89 85 6c ff ff ff                  mov        dword ptr [ebp - 0x94], eax
00417140  6a 0b                              push       0xb
00417142  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00417147  e8 34 d4 ff ff                     call       0x414580
0041714c  8b 95 6c ff ff ff                  mov        edx, dword ptr [ebp - 0x94]
00417152  89 10                              mov        dword ptr [eax], edx
00417154  e9 38 06 00 00                     jmp        0x417791
00417159  0f b6 05 ec 66 5b 00               movzx      eax, byte ptr [0x5b66ec]
00417160  85 c0                              test       eax, eax
00417162  0f 84 17 03 00 00                  je         0x41747f
00417168  b9 04 00 00 00                     mov        ecx, 4
0041716d  6b d1 00                           imul       edx, ecx, 0
00417170  c7 44 15 98 20 c8 56 00            mov        dword ptr [ebp + edx - 0x68], 0x56c820
00417178  b8 04 00 00 00                     mov        eax, 4
0041717d  6b c8 00                           imul       ecx, eax, 0
00417180  8b 54 0d 98                        mov        edx, dword ptr [ebp + ecx - 0x68]
00417184  52                                 push       edx
00417185  6a 11                              push       0x11
00417187  6a 02                              push       2
00417189  6a 00                              push       0
0041718b  6a 00                              push       0
0041718d  68 80 00 00 00                     push       0x80
00417192  6a 00                              push       0
00417194  6a 00                              push       0
00417196  6a 00                              push       0
00417198  68 90 01 00 00                     push       0x190
0041719d  6a 00                              push       0
0041719f  6a 00                              push       0
004171a1  6a 00                              push       0
004171a3  6a 24                              push       0x24
004171a5  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
004171ab  89 85 68 ff ff ff                  mov        dword ptr [ebp - 0x98], eax
004171b1  6a 02                              push       2
004171b3  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
004171b8  e8 c3 d3 ff ff                     call       0x414580
004171bd  8b 8d 68 ff ff ff                  mov        ecx, dword ptr [ebp - 0x98]
004171c3  89 08                              mov        dword ptr [eax], ecx
004171c5  ba 04 00 00 00                     mov        edx, 4
004171ca  6b c2 00                           imul       eax, edx, 0
004171cd  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
004171d1  51                                 push       ecx
004171d2  6a 11                              push       0x11
004171d4  6a 02                              push       2
004171d6  6a 00                              push       0
004171d8  6a 00                              push       0
004171da  68 80 00 00 00                     push       0x80
004171df  6a 00                              push       0
004171e1  6a 00                              push       0
004171e3  6a 00                              push       0
004171e5  68 90 01 00 00                     push       0x190
004171ea  6a 00                              push       0
004171ec  6a 00                              push       0
004171ee  6a 00                              push       0
004171f0  6a 2a                              push       0x2a
004171f2  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
004171f8  89 85 64 ff ff ff                  mov        dword ptr [ebp - 0x9c], eax
004171fe  6a 03                              push       3
00417200  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00417205  e8 76 d3 ff ff                     call       0x414580
0041720a  8b 95 64 ff ff ff                  mov        edx, dword ptr [ebp - 0x9c]
00417210  89 10                              mov        dword ptr [eax], edx
00417212  b8 04 00 00 00                     mov        eax, 4
00417217  6b c8 00                           imul       ecx, eax, 0
0041721a  8b 54 0d 98                        mov        edx, dword ptr [ebp + ecx - 0x68]
0041721e  52                                 push       edx
0041721f  6a 11                              push       0x11
00417221  6a 02                              push       2
00417223  6a 00                              push       0
00417225  6a 00                              push       0
00417227  68 80 00 00 00                     push       0x80
0041722c  6a 00                              push       0
0041722e  6a 00                              push       0
00417230  6a 00                              push       0
00417232  68 90 01 00 00                     push       0x190
00417237  6a 00                              push       0
00417239  6a 00                              push       0
0041723b  6a 00                              push       0
0041723d  6a 30                              push       0x30
0041723f  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00417245  89 85 60 ff ff ff                  mov        dword ptr [ebp - 0xa0], eax
0041724b  6a 04                              push       4
0041724d  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00417252  e8 29 d3 ff ff                     call       0x414580
00417257  8b 8d 60 ff ff ff                  mov        ecx, dword ptr [ebp - 0xa0]
0041725d  89 08                              mov        dword ptr [eax], ecx
0041725f  ba 04 00 00 00                     mov        edx, 4
00417264  6b c2 00                           imul       eax, edx, 0
00417267  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
0041726b  51                                 push       ecx
0041726c  6a 11                              push       0x11
0041726e  6a 02                              push       2
00417270  6a 00                              push       0
00417272  6a 00                              push       0
00417274  68 80 00 00 00                     push       0x80
00417279  6a 00                              push       0
0041727b  6a 00                              push       0
0041727d  6a 00                              push       0
0041727f  68 90 01 00 00                     push       0x190
00417284  6a 00                              push       0
00417286  6a 00                              push       0
00417288  6a 00                              push       0
0041728a  6a 36                              push       0x36
0041728c  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00417292  89 85 5c ff ff ff                  mov        dword ptr [ebp - 0xa4], eax
00417298  6a 05                              push       5
0041729a  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
0041729f  e8 dc d2 ff ff                     call       0x414580
004172a4  8b 95 5c ff ff ff                  mov        edx, dword ptr [ebp - 0xa4]
004172aa  89 10                              mov        dword ptr [eax], edx
004172ac  b8 04 00 00 00                     mov        eax, 4
004172b1  6b c8 00                           imul       ecx, eax, 0
004172b4  8b 54 0d 98                        mov        edx, dword ptr [ebp + ecx - 0x68]
004172b8  52                                 push       edx
004172b9  6a 11                              push       0x11
004172bb  6a 02                              push       2
004172bd  6a 00                              push       0
004172bf  6a 00                              push       0
004172c1  68 80 00 00 00                     push       0x80
004172c6  6a 00                              push       0
004172c8  6a 00                              push       0
004172ca  6a 00                              push       0
004172cc  68 90 01 00 00                     push       0x190
004172d1  6a 00                              push       0
004172d3  6a 00                              push       0
004172d5  6a 00                              push       0
004172d7  6a 3c                              push       0x3c
004172d9  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
004172df  89 85 58 ff ff ff                  mov        dword ptr [ebp - 0xa8], eax
004172e5  6a 06                              push       6
004172e7  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
004172ec  e8 8f d2 ff ff                     call       0x414580
004172f1  8b 8d 58 ff ff ff                  mov        ecx, dword ptr [ebp - 0xa8]
004172f7  89 08                              mov        dword ptr [eax], ecx
004172f9  ba 04 00 00 00                     mov        edx, 4
004172fe  6b c2 00                           imul       eax, edx, 0
00417301  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
00417305  51                                 push       ecx
00417306  6a 11                              push       0x11
00417308  6a 02                              push       2
0041730a  6a 00                              push       0
0041730c  6a 00                              push       0
0041730e  68 80 00 00 00                     push       0x80
00417313  6a 00                              push       0
00417315  6a 00                              push       0
00417317  6a 00                              push       0
00417319  68 90 01 00 00                     push       0x190
0041731e  6a 00                              push       0
00417320  6a 00                              push       0
00417322  6a 00                              push       0
00417324  6a 42                              push       0x42
00417326  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
0041732c  89 85 54 ff ff ff                  mov        dword ptr [ebp - 0xac], eax
00417332  6a 07                              push       7
00417334  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00417339  e8 42 d2 ff ff                     call       0x414580
0041733e  8b 95 54 ff ff ff                  mov        edx, dword ptr [ebp - 0xac]
00417344  89 10                              mov        dword ptr [eax], edx
00417346  b8 04 00 00 00                     mov        eax, 4
0041734b  6b c8 00                           imul       ecx, eax, 0
0041734e  8b 54 0d 98                        mov        edx, dword ptr [ebp + ecx - 0x68]
00417352  52                                 push       edx
00417353  6a 11                              push       0x11
00417355  6a 02                              push       2
00417357  6a 00                              push       0
00417359  6a 00                              push       0
0041735b  68 80 00 00 00                     push       0x80
00417360  6a 00                              push       0
00417362  6a 00                              push       0
00417364  6a 00                              push       0
00417366  68 90 01 00 00                     push       0x190
0041736b  6a 00                              push       0
0041736d  6a 00                              push       0
0041736f  6a 00                              push       0
00417371  6a 48                              push       0x48
00417373  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00417379  89 85 50 ff ff ff                  mov        dword ptr [ebp - 0xb0], eax
0041737f  6a 08                              push       8
00417381  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00417386  e8 f5 d1 ff ff                     call       0x414580
0041738b  8b 8d 50 ff ff ff                  mov        ecx, dword ptr [ebp - 0xb0]
00417391  89 08                              mov        dword ptr [eax], ecx
00417393  ba 04 00 00 00                     mov        edx, 4
00417398  6b c2 00                           imul       eax, edx, 0
0041739b  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
0041739f  51                                 push       ecx
004173a0  6a 11                              push       0x11
004173a2  6a 02                              push       2
004173a4  6a 00                              push       0
004173a6  6a 00                              push       0
004173a8  68 80 00 00 00                     push       0x80
004173ad  6a 00                              push       0
004173af  6a 00                              push       0
004173b1  6a 00                              push       0
004173b3  68 90 01 00 00                     push       0x190
004173b8  6a 00                              push       0
004173ba  6a 00                              push       0
004173bc  6a 00                              push       0
004173be  6a 5a                              push       0x5a
004173c0  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
004173c6  89 85 4c ff ff ff                  mov        dword ptr [ebp - 0xb4], eax
004173cc  6a 09                              push       9
004173ce  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
004173d3  e8 a8 d1 ff ff                     call       0x414580
004173d8  8b 95 4c ff ff ff                  mov        edx, dword ptr [ebp - 0xb4]
004173de  89 10                              mov        dword ptr [eax], edx
004173e0  b8 04 00 00 00                     mov        eax, 4
004173e5  6b c8 00                           imul       ecx, eax, 0
004173e8  8b 54 0d 98                        mov        edx, dword ptr [ebp + ecx - 0x68]
004173ec  52                                 push       edx
004173ed  6a 11                              push       0x11
004173ef  6a 02                              push       2
004173f1  6a 00                              push       0
004173f3  6a 00                              push       0
004173f5  68 80 00 00 00                     push       0x80
004173fa  6a 00                              push       0
004173fc  6a 00                              push       0
004173fe  6a 00                              push       0
00417400  68 90 01 00 00                     push       0x190
00417405  6a 00                              push       0
00417407  6a 00                              push       0
00417409  6a 00                              push       0
0041740b  6a 60                              push       0x60
0041740d  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00417413  89 85 48 ff ff ff                  mov        dword ptr [ebp - 0xb8], eax
00417419  6a 0a                              push       0xa
0041741b  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00417420  e8 5b d1 ff ff                     call       0x414580
00417425  8b 8d 48 ff ff ff                  mov        ecx, dword ptr [ebp - 0xb8]
0041742b  89 08                              mov        dword ptr [eax], ecx
0041742d  ba 04 00 00 00                     mov        edx, 4
00417432  6b c2 00                           imul       eax, edx, 0
00417435  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
00417439  51                                 push       ecx
0041743a  6a 11                              push       0x11
0041743c  6a 02                              push       2
0041743e  6a 00                              push       0
00417440  6a 00                              push       0
00417442  68 80 00 00 00                     push       0x80
00417447  6a 00                              push       0
00417449  6a 00                              push       0
0041744b  6a 00                              push       0
0041744d  68 58 02 00 00                     push       0x258
00417452  6a 00                              push       0
00417454  6a 00                              push       0
00417456  6a 00                              push       0
00417458  6a 30                              push       0x30
0041745a  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00417460  89 85 44 ff ff ff                  mov        dword ptr [ebp - 0xbc], eax
00417466  6a 0b                              push       0xb
00417468  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
0041746d  e8 0e d1 ff ff                     call       0x414580
00417472  8b 95 44 ff ff ff                  mov        edx, dword ptr [ebp - 0xbc]
00417478  89 10                              mov        dword ptr [eax], edx
0041747a  e9 12 03 00 00                     jmp        0x417791
0041747f  b8 04 00 00 00                     mov        eax, 4
00417484  6b c8 00                           imul       ecx, eax, 0
00417487  c7 44 0d 98 d0 c7 56 00            mov        dword ptr [ebp + ecx - 0x68], 0x56c7d0
0041748f  ba 04 00 00 00                     mov        edx, 4
00417494  6b c2 00                           imul       eax, edx, 0
00417497  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
0041749b  51                                 push       ecx
0041749c  6a 11                              push       0x11
0041749e  6a 02                              push       2
004174a0  6a 00                              push       0
004174a2  6a 00                              push       0
004174a4  68 80 00 00 00                     push       0x80
004174a9  6a 00                              push       0
004174ab  6a 00                              push       0
004174ad  6a 00                              push       0
004174af  68 90 01 00 00                     push       0x190
004174b4  6a 00                              push       0
004174b6  6a 00                              push       0
004174b8  6a 00                              push       0
004174ba  6a 18                              push       0x18
004174bc  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
004174c2  89 85 40 ff ff ff                  mov        dword ptr [ebp - 0xc0], eax
004174c8  6a 02                              push       2
004174ca  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
004174cf  e8 ac d0 ff ff                     call       0x414580
004174d4  8b 95 40 ff ff ff                  mov        edx, dword ptr [ebp - 0xc0]
004174da  89 10                              mov        dword ptr [eax], edx
004174dc  b8 04 00 00 00                     mov        eax, 4
004174e1  6b c8 00                           imul       ecx, eax, 0
004174e4  8b 54 0d 98                        mov        edx, dword ptr [ebp + ecx - 0x68]
004174e8  52                                 push       edx
004174e9  6a 11                              push       0x11
004174eb  6a 02                              push       2
004174ed  6a 00                              push       0
004174ef  6a 00                              push       0
004174f1  68 80 00 00 00                     push       0x80
004174f6  6a 00                              push       0
004174f8  6a 00                              push       0
004174fa  6a 00                              push       0
004174fc  68 90 01 00 00                     push       0x190
00417501  6a 00                              push       0
00417503  6a 00                              push       0
00417505  6a 00                              push       0
00417507  6a 1c                              push       0x1c
00417509  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
0041750f  89 85 3c ff ff ff                  mov        dword ptr [ebp - 0xc4], eax
00417515  6a 03                              push       3
00417517  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
0041751c  e8 5f d0 ff ff                     call       0x414580
00417521  8b 8d 3c ff ff ff                  mov        ecx, dword ptr [ebp - 0xc4]
00417527  89 08                              mov        dword ptr [eax], ecx
00417529  ba 04 00 00 00                     mov        edx, 4
0041752e  6b c2 00                           imul       eax, edx, 0
00417531  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
00417535  51                                 push       ecx
00417536  6a 11                              push       0x11
00417538  6a 02                              push       2
0041753a  6a 00                              push       0
0041753c  6a 00                              push       0
0041753e  68 80 00 00 00                     push       0x80
00417543  6a 00                              push       0
00417545  6a 00                              push       0
00417547  6a 00                              push       0
00417549  68 90 01 00 00                     push       0x190
0041754e  6a 00                              push       0
00417550  6a 00                              push       0
00417552  6a 00                              push       0
00417554  6a 20                              push       0x20
00417556  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
0041755c  89 85 38 ff ff ff                  mov        dword ptr [ebp - 0xc8], eax
00417562  6a 04                              push       4
00417564  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00417569  e8 12 d0 ff ff                     call       0x414580
0041756e  8b 95 38 ff ff ff                  mov        edx, dword ptr [ebp - 0xc8]
00417574  89 10                              mov        dword ptr [eax], edx
00417576  b8 04 00 00 00                     mov        eax, 4
0041757b  6b c8 00                           imul       ecx, eax, 0
0041757e  8b 54 0d 98                        mov        edx, dword ptr [ebp + ecx - 0x68]
00417582  52                                 push       edx
00417583  6a 11                              push       0x11
00417585  6a 02                              push       2
00417587  6a 00                              push       0
00417589  6a 00                              push       0
0041758b  68 80 00 00 00                     push       0x80
00417590  6a 00                              push       0
00417592  6a 00                              push       0
00417594  6a 00                              push       0
00417596  68 90 01 00 00                     push       0x190
0041759b  6a 00                              push       0
0041759d  6a 00                              push       0
0041759f  6a 00                              push       0
004175a1  6a 24                              push       0x24
004175a3  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
004175a9  89 85 34 ff ff ff                  mov        dword ptr [ebp - 0xcc], eax
004175af  6a 05                              push       5
004175b1  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
004175b6  e8 c5 cf ff ff                     call       0x414580
004175bb  8b 8d 34 ff ff ff                  mov        ecx, dword ptr [ebp - 0xcc]
004175c1  89 08                              mov        dword ptr [eax], ecx
004175c3  ba 04 00 00 00                     mov        edx, 4
004175c8  6b c2 00                           imul       eax, edx, 0
004175cb  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
004175cf  51                                 push       ecx
004175d0  6a 11                              push       0x11
004175d2  6a 02                              push       2
004175d4  6a 00                              push       0
004175d6  6a 00                              push       0
004175d8  68 80 00 00 00                     push       0x80
004175dd  6a 00                              push       0
004175df  6a 00                              push       0
004175e1  6a 00                              push       0
004175e3  68 90 01 00 00                     push       0x190
004175e8  6a 00                              push       0
004175ea  6a 00                              push       0
004175ec  6a 00                              push       0
004175ee  6a 28                              push       0x28
004175f0  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
004175f6  89 85 30 ff ff ff                  mov        dword ptr [ebp - 0xd0], eax
004175fc  6a 06                              push       6
004175fe  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00417603  e8 78 cf ff ff                     call       0x414580
00417608  8b 95 30 ff ff ff                  mov        edx, dword ptr [ebp - 0xd0]
0041760e  89 10                              mov        dword ptr [eax], edx
00417610  b8 04 00 00 00                     mov        eax, 4
00417615  6b c8 00                           imul       ecx, eax, 0
00417618  8b 54 0d 98                        mov        edx, dword ptr [ebp + ecx - 0x68]
0041761c  52                                 push       edx
0041761d  6a 11                              push       0x11
0041761f  6a 02                              push       2
00417621  6a 00                              push       0
00417623  6a 00                              push       0
00417625  68 80 00 00 00                     push       0x80
0041762a  6a 00                              push       0
0041762c  6a 00                              push       0
0041762e  6a 00                              push       0
00417630  68 90 01 00 00                     push       0x190
00417635  6a 00                              push       0
00417637  6a 00                              push       0
00417639  6a 00                              push       0
0041763b  6a 2c                              push       0x2c
0041763d  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00417643  89 85 2c ff ff ff                  mov        dword ptr [ebp - 0xd4], eax
00417649  6a 07                              push       7
0041764b  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00417650  e8 2b cf ff ff                     call       0x414580
00417655  8b 8d 2c ff ff ff                  mov        ecx, dword ptr [ebp - 0xd4]
0041765b  89 08                              mov        dword ptr [eax], ecx
0041765d  ba 04 00 00 00                     mov        edx, 4
00417662  6b c2 00                           imul       eax, edx, 0
00417665  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
00417669  51                                 push       ecx
0041766a  6a 11                              push       0x11
0041766c  6a 02                              push       2
0041766e  6a 00                              push       0
00417670  6a 00                              push       0
00417672  68 80 00 00 00                     push       0x80
00417677  6a 00                              push       0
00417679  6a 00                              push       0
0041767b  6a 00                              push       0
0041767d  68 90 01 00 00                     push       0x190
00417682  6a 00                              push       0
00417684  6a 00                              push       0
00417686  6a 00                              push       0
00417688  6a 30                              push       0x30
0041768a  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00417690  89 85 28 ff ff ff                  mov        dword ptr [ebp - 0xd8], eax
00417696  6a 08                              push       8
00417698  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
0041769d  e8 de ce ff ff                     call       0x414580
004176a2  8b 95 28 ff ff ff                  mov        edx, dword ptr [ebp - 0xd8]
004176a8  89 10                              mov        dword ptr [eax], edx
004176aa  b8 04 00 00 00                     mov        eax, 4
004176af  6b c8 00                           imul       ecx, eax, 0
004176b2  8b 54 0d 98                        mov        edx, dword ptr [ebp + ecx - 0x68]
004176b6  52                                 push       edx
004176b7  6a 11                              push       0x11
004176b9  6a 02                              push       2
004176bb  6a 00                              push       0
004176bd  6a 00                              push       0
004176bf  68 80 00 00 00                     push       0x80
004176c4  6a 00                              push       0
004176c6  6a 00                              push       0
004176c8  6a 00                              push       0
004176ca  68 90 01 00 00                     push       0x190
004176cf  6a 00                              push       0
004176d1  6a 00                              push       0
004176d3  6a 00                              push       0
004176d5  6a 3c                              push       0x3c
004176d7  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
004176dd  89 85 24 ff ff ff                  mov        dword ptr [ebp - 0xdc], eax
004176e3  6a 09                              push       9
004176e5  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
004176ea  e8 91 ce ff ff                     call       0x414580
004176ef  8b 8d 24 ff ff ff                  mov        ecx, dword ptr [ebp - 0xdc]
004176f5  89 08                              mov        dword ptr [eax], ecx
004176f7  ba 04 00 00 00                     mov        edx, 4
004176fc  6b c2 00                           imul       eax, edx, 0
004176ff  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
00417703  51                                 push       ecx
00417704  6a 11                              push       0x11
00417706  6a 02                              push       2
00417708  6a 00                              push       0
0041770a  6a 00                              push       0
0041770c  68 80 00 00 00                     push       0x80
00417711  6a 00                              push       0
00417713  6a 00                              push       0
00417715  6a 00                              push       0
00417717  68 90 01 00 00                     push       0x190
0041771c  6a 00                              push       0
0041771e  6a 00                              push       0
00417720  6a 00                              push       0
00417722  6a 40                              push       0x40
00417724  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
0041772a  89 85 20 ff ff ff                  mov        dword ptr [ebp - 0xe0], eax
00417730  6a 0a                              push       0xa
00417732  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00417737  e8 44 ce ff ff                     call       0x414580
0041773c  8b 95 20 ff ff ff                  mov        edx, dword ptr [ebp - 0xe0]
00417742  89 10                              mov        dword ptr [eax], edx
00417744  b8 04 00 00 00                     mov        eax, 4
00417749  6b c8 00                           imul       ecx, eax, 0
0041774c  8b 54 0d 98                        mov        edx, dword ptr [ebp + ecx - 0x68]
00417750  52                                 push       edx
00417751  6a 11                              push       0x11
00417753  6a 02                              push       2
00417755  6a 00                              push       0
00417757  6a 00                              push       0
00417759  68 80 00 00 00                     push       0x80
0041775e  6a 00                              push       0
00417760  6a 00                              push       0
00417762  6a 00                              push       0
00417764  68 58 02 00 00                     push       0x258
00417769  6a 00                              push       0
0041776b  6a 00                              push       0
0041776d  6a 00                              push       0
0041776f  6a 20                              push       0x20
00417771  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00417777  89 85 1c ff ff ff                  mov        dword ptr [ebp - 0xe4], eax
0041777d  6a 0b                              push       0xb
0041777f  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00417784  e8 f7 cd ff ff                     call       0x414580
00417789  8b 8d 1c ff ff ff                  mov        ecx, dword ptr [ebp - 0xe4]
0041778f  89 08                              mov        dword ptr [eax], ecx
00417791  0f b6 15 ee 66 5b 00               movzx      edx, byte ptr [0x5b66ee]
00417798  85 d2                              test       edx, edx
0041779a  74 12                              je         0x4177ae
0041779c  b8 04 00 00 00                     mov        eax, 4
004177a1  c1 e0 00                           shl        eax, 0
004177a4  c7 44 05 98 40 c8 56 00            mov        dword ptr [ebp + eax - 0x68], 0x56c840 ; string='Yu Mincho'
004177ac  eb 10                              jmp        0x4177be
004177ae  b9 04 00 00 00                     mov        ecx, 4
004177b3  c1 e1 00                           shl        ecx, 0
004177b6  c7 44 0d 98 54 c8 56 00            mov        dword ptr [ebp + ecx - 0x68], 0x56c854
004177be  8b 55 94                           mov        edx, dword ptr [ebp - 0x6c]
004177c1  52                                 push       edx
004177c2  6a 00                              push       0
004177c4  ff 15 b0 c2 56 00                  call       dword ptr [0x56c2b0] ; USER32.dll!ReleaseDC
004177ca  b8 04 00 00 00                     mov        eax, 4
004177cf  c1 e0 00                           shl        eax, 0
004177d2  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
004177d6  51                                 push       ecx
004177d7  6a 11                              push       0x11
004177d9  6a 02                              push       2
004177db  6a 00                              push       0
004177dd  6a 00                              push       0
004177df  68 80 00 00 00                     push       0x80
004177e4  6a 00                              push       0
004177e6  6a 00                              push       0
004177e8  6a 00                              push       0
004177ea  68 bc 02 00 00                     push       0x2bc
004177ef  6a 00                              push       0
004177f1  6a 00                              push       0
004177f3  6a 00                              push       0
004177f5  6a 20                              push       0x20
004177f7  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
004177fd  89 85 04 ff ff ff                  mov        dword ptr [ebp - 0xfc], eax
00417803  6a 0d                              push       0xd
00417805  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
0041780a  e8 71 cd ff ff                     call       0x414580
0041780f  8b 95 04 ff ff ff                  mov        edx, dword ptr [ebp - 0xfc]
00417815  89 10                              mov        dword ptr [eax], edx
00417817  b8 04 00 00 00                     mov        eax, 4
0041781c  c1 e0 00                           shl        eax, 0
0041781f  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
00417823  51                                 push       ecx
00417824  6a 11                              push       0x11
00417826  6a 02                              push       2
00417828  6a 00                              push       0
0041782a  6a 00                              push       0
0041782c  68 80 00 00 00                     push       0x80
00417831  6a 00                              push       0
00417833  6a 00                              push       0
00417835  6a 00                              push       0
00417837  68 bc 02 00 00                     push       0x2bc
0041783c  6a 00                              push       0
0041783e  6a 00                              push       0
00417840  6a 00                              push       0
00417842  6a 28                              push       0x28
00417844  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
0041784a  89 85 14 ff ff ff                  mov        dword ptr [ebp - 0xec], eax
00417850  6a 0e                              push       0xe
00417852  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00417857  e8 24 cd ff ff                     call       0x414580
0041785c  8b 95 14 ff ff ff                  mov        edx, dword ptr [ebp - 0xec]
00417862  89 10                              mov        dword ptr [eax], edx
00417864  b8 04 00 00 00                     mov        eax, 4
00417869  c1 e0 00                           shl        eax, 0
0041786c  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
00417870  51                                 push       ecx
00417871  6a 11                              push       0x11
00417873  6a 02                              push       2
00417875  6a 00                              push       0
00417877  6a 00                              push       0
00417879  68 80 00 00 00                     push       0x80
0041787e  6a 00                              push       0
00417880  6a 00                              push       0
00417882  6a 00                              push       0
00417884  68 bc 02 00 00                     push       0x2bc
00417889  6a 00                              push       0
0041788b  6a 00                              push       0
0041788d  6a 00                              push       0
0041788f  6a 30                              push       0x30
00417891  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00417897  89 85 10 ff ff ff                  mov        dword ptr [ebp - 0xf0], eax
0041789d  6a 0f                              push       0xf
0041789f  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
004178a4  e8 d7 cc ff ff                     call       0x414580
004178a9  8b 95 10 ff ff ff                  mov        edx, dword ptr [ebp - 0xf0]
004178af  89 10                              mov        dword ptr [eax], edx
004178b1  b8 04 00 00 00                     mov        eax, 4
004178b6  c1 e0 00                           shl        eax, 0
004178b9  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
004178bd  51                                 push       ecx
004178be  6a 11                              push       0x11
004178c0  6a 02                              push       2
004178c2  6a 00                              push       0
004178c4  6a 00                              push       0
004178c6  68 80 00 00 00                     push       0x80
004178cb  6a 00                              push       0
004178cd  6a 00                              push       0
004178cf  6a 00                              push       0
004178d1  68 bc 02 00 00                     push       0x2bc
004178d6  6a 00                              push       0
004178d8  6a 00                              push       0
004178da  6a 00                              push       0
004178dc  6a 3c                              push       0x3c
004178de  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
004178e4  89 85 0c ff ff ff                  mov        dword ptr [ebp - 0xf4], eax
004178ea  6a 10                              push       0x10
004178ec  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
004178f1  e8 8a cc ff ff                     call       0x414580
004178f6  8b 95 0c ff ff ff                  mov        edx, dword ptr [ebp - 0xf4]
004178fc  89 10                              mov        dword ptr [eax], edx
004178fe  b8 04 00 00 00                     mov        eax, 4
00417903  c1 e0 00                           shl        eax, 0
00417906  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
0041790a  51                                 push       ecx
0041790b  6a 11                              push       0x11
0041790d  6a 02                              push       2
0041790f  6a 00                              push       0
00417911  6a 00                              push       0
00417913  68 80 00 00 00                     push       0x80
00417918  6a 00                              push       0
0041791a  6a 00                              push       0
0041791c  6a 00                              push       0
0041791e  68 bc 02 00 00                     push       0x2bc
00417923  6a 00                              push       0
00417925  6a 00                              push       0
00417927  6a 00                              push       0
00417929  6a 40                              push       0x40
0041792b  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00417931  89 85 08 ff ff ff                  mov        dword ptr [ebp - 0xf8], eax
00417937  6a 11                              push       0x11
00417939  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
0041793e  e8 3d cc ff ff                     call       0x414580
00417943  8b 95 08 ff ff ff                  mov        edx, dword ptr [ebp - 0xf8]
00417949  89 10                              mov        dword ptr [eax], edx
0041794b  b8 04 00 00 00                     mov        eax, 4
00417950  c1 e0 00                           shl        eax, 0
00417953  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
00417957  51                                 push       ecx
00417958  6a 11                              push       0x11
0041795a  6a 02                              push       2
0041795c  6a 00                              push       0
0041795e  6a 00                              push       0
00417960  68 80 00 00 00                     push       0x80
00417965  6a 00                              push       0
00417967  6a 00                              push       0
00417969  6a 00                              push       0
0041796b  68 bc 02 00 00                     push       0x2bc
00417970  6a 00                              push       0
00417972  6a 00                              push       0
00417974  6a 00                              push       0
00417976  6a 0f                              push       0xf
00417978  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
0041797e  89 85 f0 fe ff ff                  mov        dword ptr [ebp - 0x110], eax
00417984  6a 14                              push       0x14
00417986  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
0041798b  e8 f0 cb ff ff                     call       0x414580
00417990  8b 95 f0 fe ff ff                  mov        edx, dword ptr [ebp - 0x110]
00417996  89 10                              mov        dword ptr [eax], edx
00417998  b8 04 00 00 00                     mov        eax, 4
0041799d  c1 e0 00                           shl        eax, 0
004179a0  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
004179a4  51                                 push       ecx
004179a5  6a 11                              push       0x11
004179a7  6a 02                              push       2
004179a9  6a 00                              push       0
004179ab  6a 00                              push       0
004179ad  68 80 00 00 00                     push       0x80
004179b2  6a 00                              push       0
004179b4  6a 00                              push       0
004179b6  6a 00                              push       0
004179b8  68 bc 02 00 00                     push       0x2bc
004179bd  6a 00                              push       0
004179bf  6a 00                              push       0
004179c1  6a 00                              push       0
004179c3  6a 0f                              push       0xf
004179c5  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
004179cb  89 85 00 ff ff ff                  mov        dword ptr [ebp - 0x100], eax
004179d1  6a 15                              push       0x15
004179d3  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
004179d8  e8 a3 cb ff ff                     call       0x414580
004179dd  8b 95 00 ff ff ff                  mov        edx, dword ptr [ebp - 0x100]
004179e3  89 10                              mov        dword ptr [eax], edx
004179e5  b8 04 00 00 00                     mov        eax, 4
004179ea  6b c8 00                           imul       ecx, eax, 0
004179ed  8b 54 0d 98                        mov        edx, dword ptr [ebp + ecx - 0x68]
004179f1  52                                 push       edx
004179f2  6a 11                              push       0x11
004179f4  6a 02                              push       2
004179f6  6a 00                              push       0
004179f8  6a 00                              push       0
004179fa  68 80 00 00 00                     push       0x80
004179ff  6a 00                              push       0
00417a01  6a 00                              push       0
00417a03  6a 00                              push       0
00417a05  68 90 01 00 00                     push       0x190
00417a0a  6a 00                              push       0
00417a0c  6a 00                              push       0
00417a0e  6a 00                              push       0
00417a10  6a 0f                              push       0xf
00417a12  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00417a18  89 85 fc fe ff ff                  mov        dword ptr [ebp - 0x104], eax
00417a1e  6a 12                              push       0x12
00417a20  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00417a25  e8 56 cb ff ff                     call       0x414580
00417a2a  8b 8d fc fe ff ff                  mov        ecx, dword ptr [ebp - 0x104]
00417a30  89 08                              mov        dword ptr [eax], ecx
00417a32  ba 04 00 00 00                     mov        edx, 4
00417a37  6b c2 00                           imul       eax, edx, 0
00417a3a  8b 4c 05 98                        mov        ecx, dword ptr [ebp + eax - 0x68]
00417a3e  51                                 push       ecx
00417a3f  6a 11                              push       0x11
00417a41  6a 02                              push       2
00417a43  6a 00                              push       0
00417a45  6a 00                              push       0
00417a47  68 80 00 00 00                     push       0x80
00417a4c  6a 00                              push       0
00417a4e  6a 00                              push       0
00417a50  6a 00                              push       0
00417a52  68 90 01 00 00                     push       0x190
00417a57  6a 00                              push       0
00417a59  6a 00                              push       0
00417a5b  6a 00                              push       0
00417a5d  6a 0f                              push       0xf
00417a5f  ff 15 40 c0 56 00                  call       dword ptr [0x56c040] ; GDI32.dll!CreateFontW
00417a65  89 85 f8 fe ff ff                  mov        dword ptr [ebp - 0x108], eax
00417a6b  6a 13                              push       0x13
00417a6d  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00417a72  e8 09 cb ff ff                     call       0x414580
00417a77  8b 95 f8 fe ff ff                  mov        edx, dword ptr [ebp - 0x108]
00417a7d  89 10                              mov        dword ptr [eax], edx
00417a7f  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00417a82  33 cd                              xor        ecx, ebp
00417a84  e8 65 af 12 00                     call       0x5429ee
00417a89  8b e5                              mov        esp, ebp
00417a8b  5d                                 pop        ebp
00417a8c  c3                                 ret        
