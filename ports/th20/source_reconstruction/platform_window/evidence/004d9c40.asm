004d9c40  55                                 push       ebp
004d9c41  8b ec                              mov        ebp, esp
004d9c43  6a ff                              push       -1
004d9c45  68 40 9d 56 00                     push       0x569d40
004d9c4a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
004d9c50  50                                 push       eax
004d9c51  81 ec 48 02 00 00                  sub        esp, 0x248
004d9c57  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004d9c5c  33 c5                              xor        eax, ebp
004d9c5e  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
004d9c61  50                                 push       eax
004d9c62  8d 45 f4                           lea        eax, [ebp - 0xc]
004d9c65  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
004d9c6b  6a 00                              push       0
004d9c6d  6a 00                              push       0
004d9c6f  6a 00                              push       0
004d9c71  8d 8d cc fd ff ff                  lea        ecx, [ebp - 0x234]
004d9c77  e8 b4 eb ff ff                     call       0x4d8830
004d9c7c  6a 00                              push       0
004d9c7e  8d 85 cc fd ff ff                  lea        eax, [ebp - 0x234]
004d9c84  50                                 push       eax
004d9c85  8d 8d dc fd ff ff                  lea        ecx, [ebp - 0x224]
004d9c8b  51                                 push       ecx
004d9c8c  e8 5b a1 06 00                     call       0x543dec
004d9c91  6a 00                              push       0
004d9c93  8b 55 08                           mov        edx, dword ptr [ebp + 8]
004d9c96  52                                 push       edx
004d9c97  e8 c4 fa ff ff                     call       0x4d9760
004d9c9c  83 c4 08                           add        esp, 8
004d9c9f  89 85 b4 fd ff ff                  mov        dword ptr [ebp - 0x24c], eax
004d9ca5  68 0a 02 00 00                     push       0x20a
004d9caa  6a 00                              push       0
004d9cac  8d 85 e4 fd ff ff                  lea        eax, [ebp - 0x21c]
004d9cb2  50                                 push       eax
004d9cb3  e8 a8 aa 06 00                     call       0x544760
004d9cb8  83 c4 0c                           add        esp, 0xc
004d9cbb  68 04 01 00 00                     push       0x104
004d9cc0  8d 8d e4 fd ff ff                  lea        ecx, [ebp - 0x21c]
004d9cc6  51                                 push       ecx
004d9cc7  6a ff                              push       -1
004d9cc9  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
004d9ccc  52                                 push       edx
004d9ccd  6a 00                              push       0
004d9ccf  68 a4 03 00 00                     push       0x3a4
004d9cd4  ff 15 24 c1 56 00                  call       dword ptr [0x56c124] ; KERNEL32.dll!MultiByteToWideChar
004d9cda  90                                 nop        
004d9cdb  8d 85 bc fd ff ff                  lea        eax, [ebp - 0x244]
004d9ce1  50                                 push       eax
004d9ce2  68 98 1b 57 00                     push       0x571b98 ; string='image/png'
004d9ce7  e8 04 fb ff ff                     call       0x4d97f0
004d9cec  83 c4 08                           add        esp, 8
004d9cef  85 c0                              test       eax, eax
004d9cf1  74 4d                              je         0x4d9d40
004d9cf3  6a 02                              push       2
004d9cf5  b9 40 02 5c 00                     mov        ecx, 0x5c0240
004d9cfa  e8 11 26 f3 ff                     call       0x40c310
004d9cff  50                                 push       eax
004d9d00  8d 8d e0 fd ff ff                  lea        ecx, [ebp - 0x220]
004d9d06  e8 25 1a f3 ff                     call       0x40b730
004d9d0b  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
004d9d12  6a 00                              push       0
004d9d14  8d 8d bc fd ff ff                  lea        ecx, [ebp - 0x244]
004d9d1a  51                                 push       ecx
004d9d1b  8d 95 e4 fd ff ff                  lea        edx, [ebp - 0x21c]
004d9d21  52                                 push       edx
004d9d22  8b 8d b4 fd ff ff                  mov        ecx, dword ptr [ebp - 0x24c]
004d9d28  e8 e3 fe ff ff                     call       0x4d9c10
004d9d2d  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
004d9d34  8d 8d e0 fd ff ff                  lea        ecx, [ebp - 0x220]
004d9d3a  e8 c1 1b f3 ff                     call       0x40b900
004d9d3f  90                                 nop        
004d9d40  8b 85 b4 fd ff ff                  mov        eax, dword ptr [ebp - 0x24c]
004d9d46  89 85 b8 fd ff ff                  mov        dword ptr [ebp - 0x248], eax
004d9d4c  83 bd b8 fd ff ff 00               cmp        dword ptr [ebp - 0x248], 0
004d9d53  74 26                              je         0x4d9d7b
004d9d55  8b 8d b8 fd ff ff                  mov        ecx, dword ptr [ebp - 0x248]
004d9d5b  8b 11                              mov        edx, dword ptr [ecx]
004d9d5d  8b 02                              mov        eax, dword ptr [edx]
004d9d5f  89 85 b0 fd ff ff                  mov        dword ptr [ebp - 0x250], eax
004d9d65  6a 01                              push       1
004d9d67  8b 8d b8 fd ff ff                  mov        ecx, dword ptr [ebp - 0x248]
004d9d6d  ff 95 b0 fd ff ff                  call       dword ptr [ebp - 0x250]
004d9d73  89 85 ac fd ff ff                  mov        dword ptr [ebp - 0x254], eax
004d9d79  eb 0a                              jmp        0x4d9d85
004d9d7b  c7 85 ac fd ff ff 00 00 00 00      mov        dword ptr [ebp - 0x254], 0
004d9d85  8b 8d dc fd ff ff                  mov        ecx, dword ptr [ebp - 0x224]
004d9d8b  51                                 push       ecx
004d9d8c  e8 61 a0 06 00                     call       0x543df2
004d9d91  90                                 nop        
004d9d92  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004d9d95  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
004d9d9c  59                                 pop        ecx
004d9d9d  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004d9da0  33 cd                              xor        ecx, ebp
004d9da2  e8 47 8c 06 00                     call       0x5429ee
004d9da7  8b e5                              mov        esp, ebp
004d9da9  5d                                 pop        ebp
004d9daa  c3                                 ret        
