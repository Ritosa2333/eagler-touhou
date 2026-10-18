004bd610  55                                 push       ebp
004bd611  8b ec                              mov        ebp, esp
004bd613  6a ff                              push       -1
004bd615  68 8d 80 56 00                     push       0x56808d
004bd61a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
004bd620  50                                 push       eax
004bd621  83 ec 14                           sub        esp, 0x14
004bd624  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004bd629  33 c5                              xor        eax, ebp
004bd62b  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
004bd62e  50                                 push       eax
004bd62f  8d 45 f4                           lea        eax, [ebp - 0xc]
004bd632  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
004bd638  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
004bd63b  6a 14                              push       0x14
004bd63d  b9 40 02 5c 00                     mov        ecx, 0x5c0240
004bd642  e8 c9 ec f4 ff                     call       0x40c310
004bd647  50                                 push       eax
004bd648  8d 4d ec                           lea        ecx, [ebp - 0x14]
004bd64b  e8 e0 e0 f4 ff                     call       0x40b730
004bd650  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
004bd657  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004bd65a  e8 51 69 fa ff                     call       0x463fb0
004bd65f  90                                 nop        
004bd660  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004bd663  e8 78 6a fa ff                     call       0x4640e0
004bd668  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
004bd66b  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004bd66e  50                                 push       eax
004bd66f  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
004bd672  e8 39 00 00 00                     call       0x4bd6b0
004bd677  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
004bd67a  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
004bd681  8d 4d ec                           lea        ecx, [ebp - 0x14]
004bd684  e8 77 e2 f4 ff                     call       0x40b900
004bd689  8b 45 e0                           mov        eax, dword ptr [ebp - 0x20]
004bd68c  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004bd68f  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
004bd696  59                                 pop        ecx
004bd697  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004bd69a  33 cd                              xor        ecx, ebp
004bd69c  e8 4d 53 08 00                     call       0x5429ee
004bd6a1  8b e5                              mov        esp, ebp
004bd6a3  5d                                 pop        ebp
004bd6a4  c2 04 00                           ret        4
