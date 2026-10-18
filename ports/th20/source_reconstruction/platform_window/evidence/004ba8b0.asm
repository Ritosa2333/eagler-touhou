004ba8b0  55                                 push       ebp
004ba8b1  8b ec                              mov        ebp, esp
004ba8b3  6a ff                              push       -1
004ba8b5  68 2d 76 56 00                     push       0x56762d
004ba8ba  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
004ba8c0  50                                 push       eax
004ba8c1  83 ec 0c                           sub        esp, 0xc
004ba8c4  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004ba8c9  33 c5                              xor        eax, ebp
004ba8cb  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
004ba8ce  50                                 push       eax
004ba8cf  8d 45 f4                           lea        eax, [ebp - 0xc]
004ba8d2  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
004ba8d8  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
004ba8db  6a 06                              push       6
004ba8dd  b9 40 02 5c 00                     mov        ecx, 0x5c0240
004ba8e2  e8 29 1a f5 ff                     call       0x40c310
004ba8e7  50                                 push       eax
004ba8e8  8d 4d ec                           lea        ecx, [ebp - 0x14]
004ba8eb  e8 40 0e f5 ff                     call       0x40b730
004ba8f0  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
004ba8f7  6a 01                              push       1
004ba8f9  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004ba8fc  83 c1 0c                           add        ecx, 0xc
004ba8ff  e8 9c 11 f5 ff                     call       0x40baa0
004ba904  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004ba907  e8 54 13 f5 ff                     call       0x40bc60
004ba90c  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
004ba913  8d 4d ec                           lea        ecx, [ebp - 0x14]
004ba916  e8 e5 0f f5 ff                     call       0x40b900
004ba91b  90                                 nop        
004ba91c  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004ba91f  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
004ba926  59                                 pop        ecx
004ba927  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004ba92a  33 cd                              xor        ecx, ebp
004ba92c  e8 bd 80 08 00                     call       0x5429ee
004ba931  8b e5                              mov        esp, ebp
004ba933  5d                                 pop        ebp
004ba934  c3                                 ret        
