00470180  55                                 push       ebp
00470181  8b ec                              mov        ebp, esp
00470183  6a ff                              push       -1
00470185  68 75 8e 56 00                     push       0x568e75
0047018a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
00470190  50                                 push       eax
00470191  83 ec 10                           sub        esp, 0x10
00470194  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
00470199  33 c5                              xor        eax, ebp
0047019b  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0047019e  50                                 push       eax
0047019f  8d 45 f4                           lea        eax, [ebp - 0xc]
004701a2  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
004701a8  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
004701ab  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
004701b2  6a 12                              push       0x12
004701b4  b9 40 02 5c 00                     mov        ecx, 0x5c0240
004701b9  e8 52 c1 f9 ff                     call       0x40c310
004701be  50                                 push       eax
004701bf  8d 4d ec                           lea        ecx, [ebp - 0x14]
004701c2  e8 69 b5 f9 ff                     call       0x40b730
004701c7  c6 45 fc 01                        mov        byte ptr [ebp - 4], 1
004701cb  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004701ce  05 34 a2 01 00                     add        eax, 0x1a234
004701d3  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
004701d6  8d 4d 08                           lea        ecx, [ebp + 8]
004701d9  51                                 push       ecx
004701da  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
004701dd  e8 3e 08 00 00                     call       0x470a20
004701e2  c6 45 fc 00                        mov        byte ptr [ebp - 4], 0
004701e6  8d 4d ec                           lea        ecx, [ebp - 0x14]
004701e9  e8 12 b7 f9 ff                     call       0x40b900
004701ee  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
004701f5  8d 4d 08                           lea        ecx, [ebp + 8]
004701f8  e8 03 42 fa ff                     call       0x414400
004701fd  90                                 nop        
004701fe  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
00470201  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
00470208  59                                 pop        ecx
00470209  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0047020c  33 cd                              xor        ecx, ebp
0047020e  e8 db 27 0d 00                     call       0x5429ee
00470213  8b e5                              mov        esp, ebp
00470215  5d                                 pop        ebp
00470216  c2 28 00                           ret        0x28
