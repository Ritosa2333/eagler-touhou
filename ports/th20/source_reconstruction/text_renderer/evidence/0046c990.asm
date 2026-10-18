0046c990  55                                 push       ebp
0046c991  8b ec                              mov        ebp, esp
0046c993  6a ff                              push       -1
0046c995  68 cd 79 56 00                     push       0x5679cd
0046c99a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0046c9a0  50                                 push       eax
0046c9a1  83 ec 10                           sub        esp, 0x10
0046c9a4  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0046c9a9  33 c5                              xor        eax, ebp
0046c9ab  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0046c9ae  50                                 push       eax
0046c9af  8d 45 f4                           lea        eax, [ebp - 0xc]
0046c9b2  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0046c9b8  6a 12                              push       0x12
0046c9ba  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0046c9bf  e8 4c f9 f9 ff                     call       0x40c310
0046c9c4  50                                 push       eax
0046c9c5  8d 4d ec                           lea        ecx, [ebp - 0x14]
0046c9c8  e8 63 ed f9 ff                     call       0x40b730
0046c9cd  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0046c9d4  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046c9d7  e8 44 3c 00 00                     call       0x470620
0046c9dc  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
0046c9df  8d 45 14                           lea        eax, [ebp + 0x14]
0046c9e2  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
0046c9e5  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0046c9e8  51                                 push       ecx
0046c9e9  8b 55 10                           mov        edx, dword ptr [ebp + 0x10]
0046c9ec  52                                 push       edx
0046c9ed  68 00 01 00 00                     push       0x100
0046c9f2  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
0046c9f5  50                                 push       eax
0046c9f6  e8 85 ce fc ff                     call       0x439880
0046c9fb  83 c4 10                           add        esp, 0x10
0046c9fe  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
0046ca01  51                                 push       ecx
0046ca02  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046ca05  e8 36 00 00 00                     call       0x46ca40
0046ca0a  c7 45 e8 00 00 00 00               mov        dword ptr [ebp - 0x18], 0
0046ca11  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0046ca18  8d 4d ec                           lea        ecx, [ebp - 0x14]
0046ca1b  e8 e0 ee f9 ff                     call       0x40b900
0046ca20  90                                 nop        
0046ca21  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0046ca24  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0046ca2b  59                                 pop        ecx
0046ca2c  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046ca2f  33 cd                              xor        ecx, ebp
0046ca31  e8 b8 5f 0d 00                     call       0x5429ee
0046ca36  8b e5                              mov        esp, ebp
0046ca38  5d                                 pop        ebp
0046ca39  c3                                 ret        
