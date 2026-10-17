00415830  55                                 push       ebp
00415831  8b ec                              mov        ebp, esp
00415833  6a ff                              push       -1
00415835  68 d9 7d 56 00                     push       0x567dd9
0041583a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
00415840  50                                 push       eax
00415841  81 ec 10 03 00 00                  sub        esp, 0x310
00415847  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0041584c  33 c5                              xor        eax, ebp
0041584e  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
00415851  50                                 push       eax
00415852  8d 45 f4                           lea        eax, [ebp - 0xc]
00415855  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0041585b  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
00415862  6a 12                              push       0x12
00415864  b9 40 02 5c 00                     mov        ecx, 0x5c0240
00415869  e8 a2 6a ff ff                     call       0x40c310
0041586e  50                                 push       eax
0041586f  8d 8d c0 fd ff ff                  lea        ecx, [ebp - 0x240]
00415875  e8 b6 5e ff ff                     call       0x40b730
0041587a  c6 45 fc 01                        mov        byte ptr [ebp - 4], 1
0041587e  8b 45 20                           mov        eax, dword ptr [ebp + 0x20]
00415881  50                                 push       eax
00415882  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
00415887  e8 f4 ec ff ff                     call       0x414580
0041588c  8b 08                              mov        ecx, dword ptr [eax]
0041588e  89 8d 74 fd ff ff                  mov        dword ptr [ebp - 0x28c], ecx
00415894  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
0041589c  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
004158a4  c7 85 ac fd ff ff 02 00 00 00      mov        dword ptr [ebp - 0x254], 2
004158ae  68 60 c8 56 00                     push       0x56c860 ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\core\\grpfont.cpp:920 AflDIB'
004158b3  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
004158b9  e8 92 e3 ff ff                     call       0x413c50
004158be  89 85 b4 fd ff ff                  mov        dword ptr [ebp - 0x24c], eax
004158c4  8b 55 20                           mov        edx, dword ptr [ebp + 0x20]
004158c7  89 95 88 fd ff ff                  mov        dword ptr [ebp - 0x278], edx
004158cd  83 bd 88 fd ff ff 15               cmp        dword ptr [ebp - 0x278], 0x15
004158d4  77 0d                              ja         0x4158e3
004158d6  8b 85 88 fd ff ff                  mov        eax, dword ptr [ebp - 0x278]
004158dc  ff 24 85 e4 60 41 00               jmp        dword ptr [eax*4 + 0x4160e4]
004158e3  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
004158eb  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
004158f3  c7 85 ac fd ff ff 03 00 00 00      mov        dword ptr [ebp - 0x254], 3
004158fd  e9 df 01 00 00                     jmp        0x415ae1
00415902  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
0041590a  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
00415912  c7 85 ac fd ff ff 03 00 00 00      mov        dword ptr [ebp - 0x254], 3
0041591c  e9 c0 01 00 00                     jmp        0x415ae1
00415921  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
00415929  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
00415931  c7 85 ac fd ff ff 03 00 00 00      mov        dword ptr [ebp - 0x254], 3
0041593b  e9 a1 01 00 00                     jmp        0x415ae1
00415940  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
00415948  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
00415950  c7 85 ac fd ff ff 03 00 00 00      mov        dword ptr [ebp - 0x254], 3
0041595a  e9 82 01 00 00                     jmp        0x415ae1
0041595f  f3 0f 10 05 d4 c8 56 00            movss      xmm0, dword ptr [0x56c8d4]
00415967  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
0041596f  c7 85 ac fd ff ff 03 00 00 00      mov        dword ptr [ebp - 0x254], 3
00415979  e9 63 01 00 00                     jmp        0x415ae1
0041597e  f3 0f 10 05 d4 c8 56 00            movss      xmm0, dword ptr [0x56c8d4]
00415986  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
0041598e  c7 85 ac fd ff ff 03 00 00 00      mov        dword ptr [ebp - 0x254], 3
00415998  e9 44 01 00 00                     jmp        0x415ae1
0041599d  f3 0f 10 05 d8 c8 56 00            movss      xmm0, dword ptr [0x56c8d8]
004159a5  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
004159ad  c7 85 ac fd ff ff 03 00 00 00      mov        dword ptr [ebp - 0x254], 3
004159b7  e9 25 01 00 00                     jmp        0x415ae1
004159bc  f3 0f 10 05 dc c8 56 00            movss      xmm0, dword ptr [0x56c8dc]
004159c4  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
004159cc  c7 85 ac fd ff ff 03 00 00 00      mov        dword ptr [ebp - 0x254], 3
004159d6  e9 06 01 00 00                     jmp        0x415ae1
004159db  f3 0f 10 05 e0 c8 56 00            movss      xmm0, dword ptr [0x56c8e0]
004159e3  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
004159eb  c7 85 ac fd ff ff 04 00 00 00      mov        dword ptr [ebp - 0x254], 4
004159f5  e9 e7 00 00 00                     jmp        0x415ae1
004159fa  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
00415a02  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
00415a0a  c7 85 ac fd ff ff 06 00 00 00      mov        dword ptr [ebp - 0x254], 6
00415a14  e9 c8 00 00 00                     jmp        0x415ae1
00415a19  f3 0f 10 05 d8 c8 56 00            movss      xmm0, dword ptr [0x56c8d8]
00415a21  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
00415a29  c7 85 ac fd ff ff 06 00 00 00      mov        dword ptr [ebp - 0x254], 6
00415a33  e9 a9 00 00 00                     jmp        0x415ae1
00415a38  f3 0f 10 05 d8 c8 56 00            movss      xmm0, dword ptr [0x56c8d8]
00415a40  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
00415a48  c7 85 ac fd ff ff 06 00 00 00      mov        dword ptr [ebp - 0x254], 6
00415a52  e9 8a 00 00 00                     jmp        0x415ae1
00415a57  f3 0f 10 05 e0 c8 56 00            movss      xmm0, dword ptr [0x56c8e0]
00415a5f  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
00415a67  c7 85 ac fd ff ff 06 00 00 00      mov        dword ptr [ebp - 0x254], 6
00415a71  eb 6e                              jmp        0x415ae1
00415a73  f3 0f 10 05 e0 c8 56 00            movss      xmm0, dword ptr [0x56c8e0]
00415a7b  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
00415a83  c7 85 ac fd ff ff 06 00 00 00      mov        dword ptr [ebp - 0x254], 6
00415a8d  eb 52                              jmp        0x415ae1
00415a8f  f3 0f 10 05 d0 c8 56 00            movss      xmm0, dword ptr [0x56c8d0]
00415a97  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
00415a9f  c7 85 ac fd ff ff 02 00 00 00      mov        dword ptr [ebp - 0x254], 2
00415aa9  eb 36                              jmp        0x415ae1
00415aab  f3 0f 10 05 d8 c8 56 00            movss      xmm0, dword ptr [0x56c8d8]
00415ab3  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
00415abb  c7 85 ac fd ff ff 07 00 00 00      mov        dword ptr [ebp - 0x254], 7
00415ac5  eb 1a                              jmp        0x415ae1
00415ac7  f3 0f 10 05 d8 c8 56 00            movss      xmm0, dword ptr [0x56c8d8]
00415acf  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
00415ad7  c7 85 ac fd ff ff 07 00 00 00      mov        dword ptr [ebp - 0x254], 7
00415ae1  f3 0f 10 85 b0 fd ff ff            movss      xmm0, dword ptr [ebp - 0x250]
00415ae9  f3 0f 59 05 20 e1 5a 00            mulss      xmm0, dword ptr [0x5ae120]
00415af1  f3 0f 11 85 b0 fd ff ff            movss      dword ptr [ebp - 0x250], xmm0
00415af9  f3 0f 10 05 cc c8 56 00            movss      xmm0, dword ptr [0x56c8cc]
00415b01  f3 0f 11 05 20 e1 5a 00            movss      dword ptr [0x5ae120], xmm0
00415b09  68 02 02 00 00                     push       0x202
00415b0e  6a 00                              push       0
00415b10  8d 8d ec fd ff ff                  lea        ecx, [ebp - 0x214]
00415b16  51                                 push       ecx
00415b17  e8 44 ec 12 00                     call       0x544760
00415b1c  83 c4 0c                           add        esp, 0xc
00415b1f  68 00 01 00 00                     push       0x100
00415b24  8d 95 ec fd ff ff                  lea        edx, [ebp - 0x214]
00415b2a  52                                 push       edx
00415b2b  6a ff                              push       -1
00415b2d  8b 45 18                           mov        eax, dword ptr [ebp + 0x18]
00415b30  50                                 push       eax
00415b31  6a 00                              push       0
00415b33  68 a4 03 00 00                     push       0x3a4
00415b38  ff 15 24 c1 56 00                  call       dword ptr [0x56c124] ; KERNEL32.dll!MultiByteToWideChar
00415b3e  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00415b41  8b 55 08                           mov        edx, dword ptr [ebp + 8]
00415b44  8b 41 08                           mov        eax, dword ptr [ecx + 8]
00415b47  2b 02                              sub        eax, dword ptr [edx]
00415b49  f3 0f 2c 8d b0 fd ff ff            cvttss2si  ecx, dword ptr [ebp - 0x250]
00415b51  8d 54 08 03                        lea        edx, [eax + ecx + 3]
00415b55  89 95 90 fd ff ff                  mov        dword ptr [ebp - 0x270], edx
00415b5b  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00415b5e  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00415b61  8b 50 0c                           mov        edx, dword ptr [eax + 0xc]
00415b64  2b 51 04                           sub        edx, dword ptr [ecx + 4]
00415b67  f3 0f 2c 85 b0 fd ff ff            cvttss2si  eax, dword ptr [ebp - 0x250]
00415b6f  8d 4c 02 03                        lea        ecx, [edx + eax + 3]
00415b73  89 8d 8c fd ff ff                  mov        dword ptr [ebp - 0x274], ecx
00415b79  6a 15                              push       0x15
00415b7b  8b 95 8c fd ff ff                  mov        edx, dword ptr [ebp - 0x274]
00415b81  83 c2 06                           add        edx, 6
00415b84  52                                 push       edx
00415b85  8b 85 90 fd ff ff                  mov        eax, dword ptr [ebp - 0x270]
00415b8b  50                                 push       eax
00415b8c  8b 8d b4 fd ff ff                  mov        ecx, dword ptr [ebp - 0x24c]
00415b92  e8 39 f8 ff ff                     call       0x4153d0
00415b97  8b 8d b4 fd ff ff                  mov        ecx, dword ptr [ebp - 0x24c]
00415b9d  e8 7e fa ff ff                     call       0x415620
00415ba2  89 85 a8 fd ff ff                  mov        dword ptr [ebp - 0x258], eax
00415ba8  8b 8d 74 fd ff ff                  mov        ecx, dword ptr [ebp - 0x28c]
00415bae  51                                 push       ecx
00415baf  8b 95 a8 fd ff ff                  mov        edx, dword ptr [ebp - 0x258]
00415bb5  52                                 push       edx
00415bb6  ff 15 2c c0 56 00                  call       dword ptr [0x56c02c] ; GDI32.dll!SelectObject
00415bbc  89 85 78 fd ff ff                  mov        dword ptr [ebp - 0x288], eax
00415bc2  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
00415bc5  c1 e8 10                           shr        eax, 0x10
00415bc8  25 ff 00 00 00                     and        eax, 0xff
00415bcd  88 85 a7 fd ff ff                  mov        byte ptr [ebp - 0x259], al
00415bd3  8b 4d 10                           mov        ecx, dword ptr [ebp + 0x10]
00415bd6  c1 e9 08                           shr        ecx, 8
00415bd9  81 e1 ff 00 00 00                  and        ecx, 0xff
00415bdf  88 8d a6 fd ff ff                  mov        byte ptr [ebp - 0x25a], cl
00415be5  8b 55 10                           mov        edx, dword ptr [ebp + 0x10]
00415be8  81 e2 ff 00 00 00                  and        edx, 0xff
00415bee  88 95 a5 fd ff ff                  mov        byte ptr [ebp - 0x25b], dl
00415bf4  0f b6 85 a7 fd ff ff               movzx      eax, byte ptr [ebp - 0x259]
00415bfb  0f b6 8d a6 fd ff ff               movzx      ecx, byte ptr [ebp - 0x25a]
00415c02  c1 e1 08                           shl        ecx, 8
00415c05  0b c1                              or         eax, ecx
00415c07  0f b6 95 a5 fd ff ff               movzx      edx, byte ptr [ebp - 0x25b]
00415c0e  c1 e2 10                           shl        edx, 0x10
00415c11  0b c2                              or         eax, edx
00415c13  89 85 84 fd ff ff                  mov        dword ptr [ebp - 0x27c], eax
00415c19  6a 01                              push       1
00415c1b  8b 85 a8 fd ff ff                  mov        eax, dword ptr [ebp - 0x258]
00415c21  50                                 push       eax
00415c22  ff 15 28 c0 56 00                  call       dword ptr [0x56c028] ; GDI32.dll!SetBkMode
00415c28  8d 8d ec fd ff ff                  lea        ecx, [ebp - 0x214]
00415c2e  51                                 push       ecx
00415c2f  e8 cc 26 13 00                     call       0x548300
00415c34  83 c4 04                           add        esp, 4
00415c37  89 85 9c fd ff ff                  mov        dword ptr [ebp - 0x264], eax
00415c3d  8d 95 b8 fd ff ff                  lea        edx, [ebp - 0x248]
00415c43  52                                 push       edx
00415c44  8b 85 9c fd ff ff                  mov        eax, dword ptr [ebp - 0x264]
00415c4a  50                                 push       eax
00415c4b  8d 8d ec fd ff ff                  lea        ecx, [ebp - 0x214]
00415c51  51                                 push       ecx
00415c52  8b 95 a8 fd ff ff                  mov        edx, dword ptr [ebp - 0x258]
00415c58  52                                 push       edx
00415c59  ff 15 30 c0 56 00                  call       dword ptr [0x56c030] ; GDI32.dll!GetTextExtentPoint32W
00415c5f  90                                 nop        
00415c60  83 7d 24 00                        cmp        dword ptr [ebp + 0x24], 0
00415c64  0f 85 a7 00 00 00                  jne        0x415d11
00415c6a  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
00415c6d  83 c0 02                           add        eax, 2
00415c70  89 45 0c                           mov        dword ptr [ebp + 0xc], eax
00415c73  8b 8d 84 fd ff ff                  mov        ecx, dword ptr [ebp - 0x27c]
00415c79  51                                 push       ecx
00415c7a  8b 95 a8 fd ff ff                  mov        edx, dword ptr [ebp - 0x258]
00415c80  52                                 push       edx
00415c81  ff 15 24 c0 56 00                  call       dword ptr [0x56c024] ; GDI32.dll!SetTextColor
00415c87  90                                 nop        
00415c88  83 7d 28 00                        cmp        dword ptr [ebp + 0x28], 0
00415c8c  74 57                              je         0x415ce5
00415c8e  8b 85 9c fd ff ff                  mov        eax, dword ptr [ebp - 0x264]
00415c94  50                                 push       eax
00415c95  8d 8d ec fd ff ff                  lea        ecx, [ebp - 0x214]
00415c9b  51                                 push       ecx
00415c9c  8b 95 ac fd ff ff                  mov        edx, dword ptr [ebp - 0x254]
00415ca2  52                                 push       edx
00415ca3  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
00415ca6  50                                 push       eax
00415ca7  8b 8d a8 fd ff ff                  mov        ecx, dword ptr [ebp - 0x258]
00415cad  51                                 push       ecx
00415cae  ff 15 1c c0 56 00                  call       dword ptr [0x56c01c] ; GDI32.dll!TextOutW
00415cb4  51                                 push       ecx
00415cb5  f3 0f 10 85 b0 fd ff ff            movss      xmm0, dword ptr [ebp - 0x250]
00415cbd  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
00415cc2  8b 55 14                           mov        edx, dword ptr [ebp + 0x14]
00415cc5  52                                 push       edx
00415cc6  8b 85 90 fd ff ff                  mov        eax, dword ptr [ebp - 0x270]
00415ccc  50                                 push       eax
00415ccd  6a 00                              push       0
00415ccf  8b 8d 8c fd ff ff                  mov        ecx, dword ptr [ebp - 0x274]
00415cd5  83 c1 06                           add        ecx, 6
00415cd8  51                                 push       ecx
00415cd9  8b 8d b4 fd ff ff                  mov        ecx, dword ptr [ebp - 0x24c]
00415cdf  e8 3c 07 00 00                     call       0x416420
00415ce4  90                                 nop        
00415ce5  8b 95 9c fd ff ff                  mov        edx, dword ptr [ebp - 0x264]
00415ceb  52                                 push       edx
00415cec  8d 85 ec fd ff ff                  lea        eax, [ebp - 0x214]
00415cf2  50                                 push       eax
00415cf3  8b 8d ac fd ff ff                  mov        ecx, dword ptr [ebp - 0x254]
00415cf9  51                                 push       ecx
00415cfa  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
00415cfd  52                                 push       edx
00415cfe  8b 85 a8 fd ff ff                  mov        eax, dword ptr [ebp - 0x258]
00415d04  50                                 push       eax
00415d05  ff 15 1c c0 56 00                  call       dword ptr [0x56c01c] ; GDI32.dll!TextOutW
00415d0b  90                                 nop        
00415d0c  e9 c6 01 00 00                     jmp        0x415ed7
00415d11  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
00415d14  89 8d a0 fd ff ff                  mov        dword ptr [ebp - 0x260], ecx
00415d1a  33 d2                              xor        edx, edx
00415d1c  89 95 e8 fd ff ff                  mov        dword ptr [ebp - 0x218], edx
00415d22  8b 85 84 fd ff ff                  mov        eax, dword ptr [ebp - 0x27c]
00415d28  50                                 push       eax
00415d29  8b 8d a8 fd ff ff                  mov        ecx, dword ptr [ebp - 0x258]
00415d2f  51                                 push       ecx
00415d30  ff 15 24 c0 56 00                  call       dword ptr [0x56c024] ; GDI32.dll!SetTextColor
00415d36  90                                 nop        
00415d37  83 7d 28 00                        cmp        dword ptr [ebp + 0x28], 0
00415d3b  0f 84 df 00 00 00                  je         0x415e20
00415d41  c7 85 98 fd ff ff 00 00 00 00      mov        dword ptr [ebp - 0x268], 0
00415d4b  eb 0f                              jmp        0x415d5c
00415d4d  8b 95 98 fd ff ff                  mov        edx, dword ptr [ebp - 0x268]
00415d53  83 c2 01                           add        edx, 1
00415d56  89 95 98 fd ff ff                  mov        dword ptr [ebp - 0x268], edx
00415d5c  8b 85 98 fd ff ff                  mov        eax, dword ptr [ebp - 0x268]
00415d62  3b 85 9c fd ff ff                  cmp        eax, dword ptr [ebp - 0x264]
00415d68  0f 8d 81 00 00 00                  jge        0x415def
00415d6e  b9 02 00 00 00                     mov        ecx, 2
00415d73  6b d1 00                           imul       edx, ecx, 0
00415d76  8b 85 98 fd ff ff                  mov        eax, dword ptr [ebp - 0x268]
00415d7c  66 8b 8c 45 ec fd ff ff            mov        cx, word ptr [ebp + eax*2 - 0x214]
00415d84  66 89 8c 15 e8 fd ff ff            mov        word ptr [ebp + edx - 0x218], cx
00415d8c  c7 85 80 fd ff ff 02 00 00 00      mov        dword ptr [ebp - 0x280], 2
00415d96  83 bd 80 fd ff ff 04               cmp        dword ptr [ebp - 0x280], 4
00415d9d  73 02                              jae        0x415da1
00415d9f  eb 06                              jmp        0x415da7
00415da1  e8 af cd 12 00                     call       0x542b55
00415da6  90                                 nop        
00415da7  33 d2                              xor        edx, edx
00415da9  8b 85 80 fd ff ff                  mov        eax, dword ptr [ebp - 0x280]
00415daf  66 89 94 05 e8 fd ff ff            mov        word ptr [ebp + eax - 0x218], dx
00415db7  6a 01                              push       1
00415db9  8d 8d e8 fd ff ff                  lea        ecx, [ebp - 0x218]
00415dbf  51                                 push       ecx
00415dc0  8b 95 ac fd ff ff                  mov        edx, dword ptr [ebp - 0x254]
00415dc6  52                                 push       edx
00415dc7  8b 85 a0 fd ff ff                  mov        eax, dword ptr [ebp - 0x260]
00415dcd  50                                 push       eax
00415dce  8b 8d a8 fd ff ff                  mov        ecx, dword ptr [ebp - 0x258]
00415dd4  51                                 push       ecx
00415dd5  ff 15 1c c0 56 00                  call       dword ptr [0x56c01c] ; GDI32.dll!TextOutW
00415ddb  8b 95 a0 fd ff ff                  mov        edx, dword ptr [ebp - 0x260]
00415de1  03 55 24                           add        edx, dword ptr [ebp + 0x24]
00415de4  89 95 a0 fd ff ff                  mov        dword ptr [ebp - 0x260], edx
00415dea  e9 5e ff ff ff                     jmp        0x415d4d
00415def  51                                 push       ecx
00415df0  f3 0f 10 85 b0 fd ff ff            movss      xmm0, dword ptr [ebp - 0x250]
00415df8  f3 0f 11 04 24                     movss      dword ptr [esp], xmm0
00415dfd  8b 45 14                           mov        eax, dword ptr [ebp + 0x14]
00415e00  50                                 push       eax
00415e01  8b 8d 90 fd ff ff                  mov        ecx, dword ptr [ebp - 0x270]
00415e07  51                                 push       ecx
00415e08  6a 00                              push       0
00415e0a  8b 95 8c fd ff ff                  mov        edx, dword ptr [ebp - 0x274]
00415e10  83 c2 06                           add        edx, 6
00415e13  52                                 push       edx
00415e14  8b 8d b4 fd ff ff                  mov        ecx, dword ptr [ebp - 0x24c]
00415e1a  e8 01 06 00 00                     call       0x416420
00415e1f  90                                 nop        
00415e20  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
00415e23  89 85 a0 fd ff ff                  mov        dword ptr [ebp - 0x260], eax
00415e29  c7 85 94 fd ff ff 00 00 00 00      mov        dword ptr [ebp - 0x26c], 0
00415e33  eb 0f                              jmp        0x415e44
00415e35  8b 8d 94 fd ff ff                  mov        ecx, dword ptr [ebp - 0x26c]
00415e3b  83 c1 01                           add        ecx, 1
00415e3e  89 8d 94 fd ff ff                  mov        dword ptr [ebp - 0x26c], ecx
00415e44  8b 95 94 fd ff ff                  mov        edx, dword ptr [ebp - 0x26c]
00415e4a  3b 95 9c fd ff ff                  cmp        edx, dword ptr [ebp - 0x264]
00415e50  0f 8d 81 00 00 00                  jge        0x415ed7
00415e56  b8 02 00 00 00                     mov        eax, 2
00415e5b  6b c8 00                           imul       ecx, eax, 0
00415e5e  8b 95 94 fd ff ff                  mov        edx, dword ptr [ebp - 0x26c]
00415e64  66 8b 84 55 ec fd ff ff            mov        ax, word ptr [ebp + edx*2 - 0x214]
00415e6c  66 89 84 0d e8 fd ff ff            mov        word ptr [ebp + ecx - 0x218], ax
00415e74  c7 85 7c fd ff ff 02 00 00 00      mov        dword ptr [ebp - 0x284], 2
00415e7e  83 bd 7c fd ff ff 04               cmp        dword ptr [ebp - 0x284], 4
00415e85  73 02                              jae        0x415e89
00415e87  eb 06                              jmp        0x415e8f
00415e89  e8 c7 cc 12 00                     call       0x542b55
00415e8e  90                                 nop        
00415e8f  33 c9                              xor        ecx, ecx
00415e91  8b 95 7c fd ff ff                  mov        edx, dword ptr [ebp - 0x284]
00415e97  66 89 8c 15 e8 fd ff ff            mov        word ptr [ebp + edx - 0x218], cx
00415e9f  6a 01                              push       1
00415ea1  8d 85 e8 fd ff ff                  lea        eax, [ebp - 0x218]
00415ea7  50                                 push       eax
00415ea8  8b 8d ac fd ff ff                  mov        ecx, dword ptr [ebp - 0x254]
00415eae  51                                 push       ecx
00415eaf  8b 95 a0 fd ff ff                  mov        edx, dword ptr [ebp - 0x260]
00415eb5  52                                 push       edx
00415eb6  8b 85 a8 fd ff ff                  mov        eax, dword ptr [ebp - 0x258]
00415ebc  50                                 push       eax
00415ebd  ff 15 1c c0 56 00                  call       dword ptr [0x56c01c] ; GDI32.dll!TextOutW
00415ec3  8b 8d a0 fd ff ff                  mov        ecx, dword ptr [ebp - 0x260]
00415ec9  03 4d 24                           add        ecx, dword ptr [ebp + 0x24]
00415ecc  89 8d a0 fd ff ff                  mov        dword ptr [ebp - 0x260], ecx
00415ed2  e9 5e ff ff ff                     jmp        0x415e35
00415ed7  8b 95 78 fd ff ff                  mov        edx, dword ptr [ebp - 0x288]
00415edd  52                                 push       edx
00415ede  8b 85 a8 fd ff ff                  mov        eax, dword ptr [ebp - 0x258]
00415ee4  50                                 push       eax
00415ee5  ff 15 2c c0 56 00                  call       dword ptr [0x56c02c] ; GDI32.dll!SelectObject
00415eeb  6a 00                              push       0
00415eed  8b 8d 90 fd ff ff                  mov        ecx, dword ptr [ebp - 0x270]
00415ef3  51                                 push       ecx
00415ef4  6a 00                              push       0
00415ef6  8b 95 8c fd ff ff                  mov        edx, dword ptr [ebp - 0x274]
00415efc  83 c2 06                           add        edx, 6
00415eff  52                                 push       edx
00415f00  8b 8d b4 fd ff ff                  mov        ecx, dword ptr [ebp - 0x24c]
00415f06  e8 a5 1b 00 00                     call       0x417ab0
00415f0b  8b 85 78 fd ff ff                  mov        eax, dword ptr [ebp - 0x288]
00415f11  50                                 push       eax
00415f12  8b 8d a8 fd ff ff                  mov        ecx, dword ptr [ebp - 0x258]
00415f18  51                                 push       ecx
00415f19  ff 15 2c c0 56 00                  call       dword ptr [0x56c02c] ; GDI32.dll!SelectObject
00415f1f  90                                 nop        
00415f20  6a 13                              push       0x13
00415f22  b9 40 02 5c 00                     mov        ecx, 0x5c0240
00415f27  e8 e4 63 ff ff                     call       0x40c310
00415f2c  50                                 push       eax
00415f2d  8d 8d c4 fd ff ff                  lea        ecx, [ebp - 0x23c]
00415f33  e8 f8 57 ff ff                     call       0x40b730
00415f38  c6 45 fc 02                        mov        byte ptr [ebp - 4], 2
00415f3c  8b 55 08                           mov        edx, dword ptr [ebp + 8]
00415f3f  8b 02                              mov        eax, dword ptr [edx]
00415f41  89 85 c8 fd ff ff                  mov        dword ptr [ebp - 0x238], eax
00415f47  8b 4a 04                           mov        ecx, dword ptr [edx + 4]
00415f4a  89 8d cc fd ff ff                  mov        dword ptr [ebp - 0x234], ecx
00415f50  8b 42 08                           mov        eax, dword ptr [edx + 8]
00415f53  89 85 d0 fd ff ff                  mov        dword ptr [ebp - 0x230], eax
00415f59  8b 4a 0c                           mov        ecx, dword ptr [edx + 0xc]
00415f5c  89 8d d4 fd ff ff                  mov        dword ptr [ebp - 0x22c], ecx
00415f62  c7 85 d8 fd ff ff 00 00 00 00      mov        dword ptr [ebp - 0x228], 0
00415f6c  c7 85 dc fd ff ff 00 00 00 00      mov        dword ptr [ebp - 0x224], 0
00415f76  8b 95 d0 fd ff ff                  mov        edx, dword ptr [ebp - 0x230]
00415f7c  2b 95 c8 fd ff ff                  sub        edx, dword ptr [ebp - 0x238]
00415f82  89 95 e0 fd ff ff                  mov        dword ptr [ebp - 0x220], edx
00415f88  8b 85 d4 fd ff ff                  mov        eax, dword ptr [ebp - 0x22c]
00415f8e  2b 85 cc fd ff ff                  sub        eax, dword ptr [ebp - 0x234]
00415f94  89 85 e4 fd ff ff                  mov        dword ptr [ebp - 0x21c], eax
00415f9a  81 bd e0 fd ff ff 00 04 00 00      cmp        dword ptr [ebp - 0x220], 0x400
00415fa4  7e 0a                              jle        0x415fb0
00415fa6  c7 85 e0 fd ff ff 00 04 00 00      mov        dword ptr [ebp - 0x220], 0x400
00415fb0  0f b6 0d ec 66 5b 00               movzx      ecx, byte ptr [0x5b66ec]
00415fb7  85 c9                              test       ecx, ecx
00415fb9  74 2a                              je         0x415fe5
00415fbb  83 7d 20 13                        cmp        dword ptr [ebp + 0x20], 0x13
00415fbf  74 24                              je         0x415fe5
00415fc1  83 7d 20 12                        cmp        dword ptr [ebp + 0x20], 0x12
00415fc5  74 1e                              je         0x415fe5
00415fc7  8b 95 dc fd ff ff                  mov        edx, dword ptr [ebp - 0x224]
00415fcd  83 c2 06                           add        edx, 6
00415fd0  89 95 dc fd ff ff                  mov        dword ptr [ebp - 0x224], edx
00415fd6  8b 85 e4 fd ff ff                  mov        eax, dword ptr [ebp - 0x21c]
00415fdc  83 c0 06                           add        eax, 6
00415fdf  89 85 e4 fd ff ff                  mov        dword ptr [ebp - 0x21c], eax
00415fe5  8d 8d b4 fd ff ff                  lea        ecx, [ebp - 0x24c]
00415feb  51                                 push       ecx
00415fec  e8 8f 52 ff ff                     call       0x40b280
00415ff1  83 c4 04                           add        esp, 4
00415ff4  8d 55 30                           lea        edx, [ebp + 0x30]
00415ff7  52                                 push       edx
00415ff8  8d 4d 2c                           lea        ecx, [ebp + 0x2c]
00415ffb  51                                 push       ecx
00415ffc  8d 95 d8 fd ff ff                  lea        edx, [ebp - 0x228]
00416002  52                                 push       edx
00416003  8d 8d c8 fd ff ff                  lea        ecx, [ebp - 0x238]
00416009  51                                 push       ecx
0041600a  8d 55 1c                           lea        edx, [ebp + 0x1c]
0041600d  52                                 push       edx
0041600e  8b 00                              mov        eax, dword ptr [eax]
00416010  50                                 push       eax
00416011  8d 8d e4 fc ff ff                  lea        ecx, [ebp - 0x31c]
00416017  e8 a4 de ff ff                     call       0x413ec0
0041601c  89 85 70 fd ff ff                  mov        dword ptr [ebp - 0x290], eax
00416022  8b 8d 70 fd ff ff                  mov        ecx, dword ptr [ebp - 0x290]
00416028  89 8d 6c fd ff ff                  mov        dword ptr [ebp - 0x294], ecx
0041602e  c6 45 fc 03                        mov        byte ptr [ebp - 4], 3
00416032  8b 95 6c fd ff ff                  mov        edx, dword ptr [ebp - 0x294]
00416038  52                                 push       edx
00416039  8d 8d 3c fd ff ff                  lea        ecx, [ebp - 0x2c4]
0041603f  e8 dc ce ff ff                     call       0x412f20
00416044  c6 45 fc 04                        mov        byte ptr [ebp - 4], 4
00416048  8d 85 3c fd ff ff                  lea        eax, [ebp - 0x2c4]
0041604e  50                                 push       eax
0041604f  b9 e0 66 5b 00                     mov        ecx, 0x5b66e0
00416054  e8 97 02 00 00                     call       0x4162f0
00416059  c6 45 fc 03                        mov        byte ptr [ebp - 4], 3
0041605d  8d 8d 3c fd ff ff                  lea        ecx, [ebp - 0x2c4]
00416063  e8 98 e3 ff ff                     call       0x414400
00416068  c6 45 fc 02                        mov        byte ptr [ebp - 4], 2
0041606c  8d 8d e4 fc ff ff                  lea        ecx, [ebp - 0x31c]
00416072  e8 49 e2 ff ff                     call       0x4142c0
00416077  90                                 nop        
00416078  c6 45 fc 01                        mov        byte ptr [ebp - 4], 1
0041607c  8d 8d c4 fd ff ff                  lea        ecx, [ebp - 0x23c]
00416082  e8 79 58 ff ff                     call       0x40b900
00416087  90                                 nop        
00416088  8b 8d b8 fd ff ff                  mov        ecx, dword ptr [ebp - 0x248]
0041608e  8b 95 bc fd ff ff                  mov        edx, dword ptr [ebp - 0x244]
00416094  89 8d 64 fd ff ff                  mov        dword ptr [ebp - 0x29c], ecx
0041609a  89 95 68 fd ff ff                  mov        dword ptr [ebp - 0x298], edx
004160a0  c6 45 fc 00                        mov        byte ptr [ebp - 4], 0
004160a4  8d 8d c0 fd ff ff                  lea        ecx, [ebp - 0x240]
004160aa  e8 51 58 ff ff                     call       0x40b900
004160af  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
004160b6  8d 4d 30                           lea        ecx, [ebp + 0x30]
004160b9  e8 42 e3 ff ff                     call       0x414400
004160be  8b 85 64 fd ff ff                  mov        eax, dword ptr [ebp - 0x29c]
004160c4  8b 95 68 fd ff ff                  mov        edx, dword ptr [ebp - 0x298]
004160ca  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004160cd  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
004160d4  59                                 pop        ecx
004160d5  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004160d8  33 cd                              xor        ecx, ebp
004160da  e8 0f c9 12 00                     call       0x5429ee
004160df  8b e5                              mov        esp, ebp
004160e1  5d                                 pop        ebp
004160e2  c3                                 ret        
