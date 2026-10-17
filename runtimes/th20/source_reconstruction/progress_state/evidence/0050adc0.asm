0050adc0  55                                 push       ebp
0050adc1  8b ec                              mov        ebp, esp
0050adc3  6a ff                              push       -1
0050adc5  68 e7 a8 56 00                     push       0x56a8e7
0050adca  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0050add0  50                                 push       eax
0050add1  83 ec 10                           sub        esp, 0x10
0050add4  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0050add9  33 c5                              xor        eax, ebp
0050addb  50                                 push       eax
0050addc  8d 45 f4                           lea        eax, [ebp - 0xc]
0050addf  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0050ade5  89 4d e4                           mov        dword ptr [ebp - 0x1c], ecx
0050ade8  68 d8 42 12 00                     push       0x1242d8
0050aded  e8 6b 79 03 00                     call       0x54275d
0050adf2  83 c4 04                           add        esp, 4
0050adf5  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0050adf8  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0050adff  83 7d f0 00                        cmp        dword ptr [ebp - 0x10], 0
0050ae03  74 1a                              je         0x50ae1f
0050ae05  68 d8 42 12 00                     push       0x1242d8
0050ae0a  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050ae0d  e8 6e 12 f0 ff                     call       0x40c080
0050ae12  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0050ae15  e8 b6 37 00 00                     call       0x50e5d0
0050ae1a  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
0050ae1d  eb 07                              jmp        0x50ae26
0050ae1f  c7 45 ec 00 00 00 00               mov        dword ptr [ebp - 0x14], 0
0050ae26  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
0050ae29  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
0050ae2c  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0050ae33  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
0050ae36  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0050ae39  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0050ae40  59                                 pop        ecx
0050ae41  8b e5                              mov        esp, ebp
0050ae43  5d                                 pop        ebp
0050ae44  c2 04 00                           ret        4
