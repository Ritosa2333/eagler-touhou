0041ccf0  55                                 push       ebp
0041ccf1  8b ec                              mov        ebp, esp
0041ccf3  83 ec 6c                           sub        esp, 0x6c
0041ccf6  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0041ccfb  33 c5                              xor        eax, ebp
0041ccfd  89 45 fc                           mov        dword ptr [ebp - 4], eax
0041cd00  89 4d c0                           mov        dword ptr [ebp - 0x40], ecx
0041cd03  e8 b8 03 00 00                     call       0x41d0c0
0041cd08  85 c0                              test       eax, eax
0041cd0a  74 09                              je         0x41cd15
0041cd0c  c7 45 b8 00 cb 56 00               mov        dword ptr [ebp - 0x48], 0x56cb00
0041cd13  eb 07                              jmp        0x41cd1c
0041cd15  c7 45 b8 50 cb 56 00               mov        dword ptr [ebp - 0x48], 0x56cb50 ; string='TH20 - Fossilized Wonder. ver 1.00c'
0041cd1c  8b 45 b8                           mov        eax, dword ptr [ebp - 0x48]
0041cd1f  89 45 bc                           mov        dword ptr [ebp - 0x44], eax
0041cd22  6a 28                              push       0x28
0041cd24  6a 00                              push       0
0041cd26  8d 4d c4                           lea        ecx, [ebp - 0x3c]
0041cd29  51                                 push       ecx
0041cd2a  e8 31 7a 12 00                     call       0x544760
0041cd2f  83 c4 0c                           add        esp, 0xc
0041cd32  6a 04                              push       4
0041cd34  ff 15 10 c0 56 00                  call       dword ptr [0x56c010] ; GDI32.dll!GetStockObject
0041cd3a  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
0041cd3d  68 00 7f 00 00                     push       0x7f00
0041cd42  6a 00                              push       0
0041cd44  ff 15 78 c2 56 00                  call       dword ptr [0x56c278] ; USER32.dll!LoadCursorW
0041cd4a  89 45 dc                           mov        dword ptr [ebp - 0x24], eax
0041cd4d  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0041cd50  89 55 d4                           mov        dword ptr [ebp - 0x2c], edx
0041cd53  c7 45 c8 50 d3 41 00               mov        dword ptr [ebp - 0x38], 0x41d350
0041cd5a  8b 45 c0                           mov        eax, dword ptr [ebp - 0x40]
0041cd5d  c6 40 68 01                        mov        byte ptr [eax + 0x68], 1
0041cd61  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0041cd64  c6 41 69 00                        mov        byte ptr [ecx + 0x69], 0
0041cd68  c7 45 e8 98 cb 56 00               mov        dword ptr [ebp - 0x18], 0x56cb98
0041cd6f  8d 55 c4                           lea        edx, [ebp - 0x3c]
0041cd72  52                                 push       edx
0041cd73  ff 15 58 c2 56 00                  call       dword ptr [0x56c258] ; USER32.dll!RegisterClassW
0041cd79  a1 80 4f 5c 00                     mov        eax, dword ptr [0x5c4f80]
0041cd7e  50                                 push       eax
0041cd7f  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0041cd82  e8 a9 12 00 00                     call       0x41e030
0041cd87  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0041cd8a  e8 e1 fe ff ff                     call       0x41cc70
0041cd8f  83 f8 03                           cmp        eax, 3
0041cd92  7c 09                              jl         0x41cd9d
0041cd94  c7 45 b4 01 00 00 00               mov        dword ptr [ebp - 0x4c], 1
0041cd9b  eb 07                              jmp        0x41cda4
0041cd9d  c7 45 b4 00 00 00 00               mov        dword ptr [ebp - 0x4c], 0
0041cda4  8b 4d b4                           mov        ecx, dword ptr [ebp - 0x4c]
0041cda7  89 0d 44 4e 5c 00                  mov        dword ptr [0x5c4e44], ecx
0041cdad  0f b6 15 84 4f 5c 00               movzx      edx, byte ptr [0x5c4f84]
0041cdb4  85 d2                              test       edx, edx
0041cdb6  75 23                              jne        0x41cddb
0041cdb8  0f b6 05 89 4f 5c 00               movzx      eax, byte ptr [0x5c4f89]
0041cdbf  83 f8 02                           cmp        eax, 2
0041cdc2  75 17                              jne        0x41cddb
0041cdc4  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0041cdc7  8b 91 90 20 00 00                  mov        edx, dword ptr [ecx + 0x2090]
0041cdcd  83 ca 04                           or         edx, 4
0041cdd0  8b 45 c0                           mov        eax, dword ptr [ebp - 0x40]
0041cdd3  89 90 90 20 00 00                  mov        dword ptr [eax + 0x2090], edx
0041cdd9  eb 15                              jmp        0x41cdf0
0041cddb  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0041cdde  8b 91 90 20 00 00                  mov        edx, dword ptr [ecx + 0x2090]
0041cde4  83 e2 fb                           and        edx, 0xfffffffb
0041cde7  8b 45 c0                           mov        eax, dword ptr [ebp - 0x40]
0041cdea  89 90 90 20 00 00                  mov        dword ptr [eax + 0x2090], edx
0041cdf0  b9 0c 00 00 00                     mov        ecx, 0xc
0041cdf5  6b d1 00                           imul       edx, ecx, 0
0041cdf8  8b 45 c0                           mov        eax, dword ptr [ebp - 0x40]
0041cdfb  8d 8c 10 04 21 00 00               lea        ecx, [eax + edx + 0x2104]
0041ce02  89 4d b0                           mov        dword ptr [ebp - 0x50], ecx
0041ce05  6a 0f                              push       0xf
0041ce07  8b 4d b0                           mov        ecx, dword ptr [ebp - 0x50]
0041ce0a  e8 81 fe ff ff                     call       0x41cc90
0041ce0f  ba 0c 00 00 00                     mov        edx, 0xc
0041ce14  c1 e2 00                           shl        edx, 0
0041ce17  8b 45 c0                           mov        eax, dword ptr [ebp - 0x40]
0041ce1a  8d 8c 10 04 21 00 00               lea        ecx, [eax + edx + 0x2104]
0041ce21  89 4d ac                           mov        dword ptr [ebp - 0x54], ecx
0041ce24  6a 0c                              push       0xc
0041ce26  8b 4d ac                           mov        ecx, dword ptr [ebp - 0x54]
0041ce29  e8 62 fe ff ff                     call       0x41cc90
0041ce2e  ba 0c 00 00 00                     mov        edx, 0xc
0041ce33  d1 e2                              shl        edx, 1
0041ce35  8b 45 c0                           mov        eax, dword ptr [ebp - 0x40]
0041ce38  8d 8c 10 04 21 00 00               lea        ecx, [eax + edx + 0x2104]
0041ce3f  89 4d a8                           mov        dword ptr [ebp - 0x58], ecx
0041ce42  6a 0c                              push       0xc
0041ce44  8b 4d a8                           mov        ecx, dword ptr [ebp - 0x58]
0041ce47  e8 44 fe ff ff                     call       0x41cc90
0041ce4c  ba 0c 00 00 00                     mov        edx, 0xc
0041ce51  6b c2 03                           imul       eax, edx, 3
0041ce54  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0041ce57  8d 94 01 04 21 00 00               lea        edx, [ecx + eax + 0x2104]
0041ce5e  89 55 a4                           mov        dword ptr [ebp - 0x5c], edx
0041ce61  6a 08                              push       8
0041ce63  8b 4d a4                           mov        ecx, dword ptr [ebp - 0x5c]
0041ce66  e8 25 fe ff ff                     call       0x41cc90
0041ce6b  8b 45 c0                           mov        eax, dword ptr [ebp - 0x40]
0041ce6e  c7 80 00 21 00 00 00 00 00 00      mov        dword ptr [eax + 0x2100], 0
0041ce78  6a 01                              push       1
0041ce7a  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0041ce7d  e8 ce 11 00 00                     call       0x41e050
0041ce82  90                                 nop        
0041ce83  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041ce88  e8 73 89 ff ff                     call       0x415800
0041ce8d  85 c0                              test       eax, eax
0041ce8f  75 53                              jne        0x41cee4
0041ce91  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0041ce94  8b 91 a8 20 00 00                  mov        edx, dword ptr [ecx + 0x20a8]
0041ce9a  89 55 9c                           mov        dword ptr [ebp - 0x64], edx
0041ce9d  8b 45 c0                           mov        eax, dword ptr [ebp - 0x40]
0041cea0  8b 88 ac 20 00 00                  mov        ecx, dword ptr [eax + 0x20ac]
0041cea6  89 4d a0                           mov        dword ptr [ebp - 0x60], ecx
0041cea9  6a 00                              push       0
0041ceab  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0041ceae  52                                 push       edx
0041ceaf  6a 00                              push       0
0041ceb1  6a 00                              push       0
0041ceb3  8b 45 a0                           mov        eax, dword ptr [ebp - 0x60]
0041ceb6  50                                 push       eax
0041ceb7  8b 4d 9c                           mov        ecx, dword ptr [ebp - 0x64]
0041ceba  51                                 push       ecx
0041cebb  6a 00                              push       0
0041cebd  6a 00                              push       0
0041cebf  68 00 00 00 90                     push       0x90000000
0041cec4  8b 55 bc                           mov        edx, dword ptr [ebp - 0x44]
0041cec7  52                                 push       edx
0041cec8  68 98 cb 56 00                     push       0x56cb98
0041cecd  6a 00                              push       0
0041cecf  ff 15 98 c2 56 00                  call       dword ptr [0x56c298] ; USER32.dll!CreateWindowExW
0041ced5  50                                 push       eax
0041ced6  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0041ced9  e8 c2 48 ff ff                     call       0x4117a0
0041cede  90                                 nop        
0041cedf  e9 58 01 00 00                     jmp        0x41d03c
0041cee4  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041cee9  e8 82 fd ff ff                     call       0x41cc70
0041ceee  83 f8 08                           cmp        eax, 8
0041cef1  0f 84 c3 00 00 00                  je         0x41cfba
0041cef7  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041cefc  e8 6f fd ff ff                     call       0x41cc70
0041cf01  83 f8 09                           cmp        eax, 9
0041cf04  0f 84 b0 00 00 00                  je         0x41cfba
0041cf0a  c7 45 ec 00 00 00 00               mov        dword ptr [ebp - 0x14], 0
0041cf11  c7 45 f0 00 00 00 00               mov        dword ptr [ebp - 0x10], 0
0041cf18  8b 45 c0                           mov        eax, dword ptr [ebp - 0x40]
0041cf1b  8b 88 a8 20 00 00                  mov        ecx, dword ptr [eax + 0x20a8]
0041cf21  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
0041cf24  8b 55 c0                           mov        edx, dword ptr [ebp - 0x40]
0041cf27  8b 82 ac 20 00 00                  mov        eax, dword ptr [edx + 0x20ac]
0041cf2d  89 45 f8                           mov        dword ptr [ebp - 8], eax
0041cf30  c7 45 98 00 00 cb 10               mov        dword ptr [ebp - 0x68], 0x10cb0000
0041cf37  6a 00                              push       0
0041cf39  6a 00                              push       0
0041cf3b  68 00 00 cb 10                     push       0x10cb0000
0041cf40  8d 4d ec                           lea        ecx, [ebp - 0x14]
0041cf43  51                                 push       ecx
0041cf44  ff 15 64 c2 56 00                  call       dword ptr [0x56c264] ; USER32.dll!AdjustWindowRectEx
0041cf4a  6a 00                              push       0
0041cf4c  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0041cf4f  52                                 push       edx
0041cf50  6a 00                              push       0
0041cf52  6a 00                              push       0
0041cf54  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0041cf57  2b 45 f0                           sub        eax, dword ptr [ebp - 0x10]
0041cf5a  50                                 push       eax
0041cf5b  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0041cf5e  2b 4d ec                           sub        ecx, dword ptr [ebp - 0x14]
0041cf61  51                                 push       ecx
0041cf62  8b 15 94 4f 5c 00                  mov        edx, dword ptr [0x5c4f94]
0041cf68  52                                 push       edx
0041cf69  a1 90 4f 5c 00                     mov        eax, dword ptr [0x5c4f90]
0041cf6e  50                                 push       eax
0041cf6f  68 00 00 cb 10                     push       0x10cb0000
0041cf74  8b 4d bc                           mov        ecx, dword ptr [ebp - 0x44]
0041cf77  51                                 push       ecx
0041cf78  68 98 cb 56 00                     push       0x56cb98
0041cf7d  6a 00                              push       0
0041cf7f  ff 15 98 c2 56 00                  call       dword ptr [0x56c298] ; USER32.dll!CreateWindowExW
0041cf85  8b 55 c0                           mov        edx, dword ptr [ebp - 0x40]
0041cf88  89 02                              mov        dword ptr [edx], eax
0041cf8a  6a 05                              push       5
0041cf8c  8b 45 c0                           mov        eax, dword ptr [ebp - 0x40]
0041cf8f  8b 08                              mov        ecx, dword ptr [eax]
0041cf91  51                                 push       ecx
0041cf92  ff 15 28 c2 56 00                  call       dword ptr [0x56c228] ; USER32.dll!ShowWindow
0041cf98  6a 01                              push       1
0041cf9a  6a 00                              push       0
0041cf9c  8b 55 c0                           mov        edx, dword ptr [ebp - 0x40]
0041cf9f  8b 02                              mov        eax, dword ptr [edx]
0041cfa1  50                                 push       eax
0041cfa2  ff 15 5c c2 56 00                  call       dword ptr [0x56c25c] ; USER32.dll!InvalidateRect
0041cfa8  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0041cfab  8b 11                              mov        edx, dword ptr [ecx]
0041cfad  52                                 push       edx
0041cfae  ff 15 4c c2 56 00                  call       dword ptr [0x56c24c] ; USER32.dll!UpdateWindow
0041cfb4  90                                 nop        
0041cfb5  e9 82 00 00 00                     jmp        0x41d03c
0041cfba  c7 45 94 00 00 00 80               mov        dword ptr [ebp - 0x6c], 0x80000000
0041cfc1  6a 00                              push       0
0041cfc3  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041cfc6  50                                 push       eax
0041cfc7  6a 00                              push       0
0041cfc9  6a 00                              push       0
0041cfcb  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0041cfce  8b 91 ac 20 00 00                  mov        edx, dword ptr [ecx + 0x20ac]
0041cfd4  52                                 push       edx
0041cfd5  8b 45 c0                           mov        eax, dword ptr [ebp - 0x40]
0041cfd8  8b 88 a8 20 00 00                  mov        ecx, dword ptr [eax + 0x20a8]
0041cfde  51                                 push       ecx
0041cfdf  6a 00                              push       0
0041cfe1  6a 00                              push       0
0041cfe3  68 00 00 00 80                     push       0x80000000
0041cfe8  8b 55 bc                           mov        edx, dword ptr [ebp - 0x44]
0041cfeb  52                                 push       edx
0041cfec  68 98 cb 56 00                     push       0x56cb98
0041cff1  68 00 00 04 00                     push       0x40000
0041cff6  ff 15 98 c2 56 00                  call       dword ptr [0x56c298] ; USER32.dll!CreateWindowExW
0041cffc  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0041cfff  89 01                              mov        dword ptr [ecx], eax
0041d001  6a 00                              push       0
0041d003  6a f0                              push       -0x10
0041d005  8b 55 c0                           mov        edx, dword ptr [ebp - 0x40]
0041d008  8b 02                              mov        eax, dword ptr [edx]
0041d00a  50                                 push       eax
0041d00b  ff 15 74 c2 56 00                  call       dword ptr [0x56c274] ; USER32.dll!SetWindowLongW
0041d011  6a 05                              push       5
0041d013  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0041d016  8b 11                              mov        edx, dword ptr [ecx]
0041d018  52                                 push       edx
0041d019  ff 15 28 c2 56 00                  call       dword ptr [0x56c228] ; USER32.dll!ShowWindow
0041d01f  6a 01                              push       1
0041d021  6a 00                              push       0
0041d023  8b 45 c0                           mov        eax, dword ptr [ebp - 0x40]
0041d026  8b 08                              mov        ecx, dword ptr [eax]
0041d028  51                                 push       ecx
0041d029  ff 15 5c c2 56 00                  call       dword ptr [0x56c25c] ; USER32.dll!InvalidateRect
0041d02f  8b 55 c0                           mov        edx, dword ptr [ebp - 0x40]
0041d032  8b 02                              mov        eax, dword ptr [edx]
0041d034  50                                 push       eax
0041d035  ff 15 4c c2 56 00                  call       dword ptr [0x56c24c] ; USER32.dll!UpdateWindow
0041d03b  90                                 nop        
0041d03c  68 4c 4d 5c 00                     push       0x5c4d4c
0041d041  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
0041d044  8b 11                              mov        edx, dword ptr [ecx]
0041d046  52                                 push       edx
0041d047  ff 15 60 c2 56 00                  call       dword ptr [0x56c260] ; USER32.dll!GetWindowRect
0041d04d  8b 45 c0                           mov        eax, dword ptr [ebp - 0x40]
0041d050  8b 08                              mov        ecx, dword ptr [eax]
0041d052  89 0d 88 4d 5c 00                  mov        dword ptr [0x5c4d88], ecx
0041d058  8b 55 c0                           mov        edx, dword ptr [ebp - 0x40]
0041d05b  83 3a 00                           cmp        dword ptr [edx], 0
0041d05e  75 07                              jne        0x41d067
0041d060  b8 01 00 00 00                     mov        eax, 1
0041d065  eb 02                              jmp        0x41d069
0041d067  33 c0                              xor        eax, eax
0041d069  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041d06c  33 cd                              xor        ecx, ebp
0041d06e  e8 7b 59 12 00                     call       0x5429ee
0041d073  8b e5                              mov        esp, ebp
0041d075  5d                                 pop        ebp
0041d076  c2 04 00                           ret        4
