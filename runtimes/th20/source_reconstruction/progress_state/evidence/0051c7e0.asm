0051c7e0  55                                 push       ebp
0051c7e1  8b ec                              mov        ebp, esp
0051c7e3  6a ff                              push       -1
0051c7e5  68 cd 79 56 00                     push       0x5679cd
0051c7ea  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0051c7f0  50                                 push       eax
0051c7f1  83 ec 10                           sub        esp, 0x10
0051c7f4  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0051c7f9  33 c5                              xor        eax, ebp
0051c7fb  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0051c7fe  50                                 push       eax
0051c7ff  8d 45 f4                           lea        eax, [ebp - 0xc]
0051c802  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0051c808  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
0051c80b  6a 14                              push       0x14
0051c80d  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0051c812  e8 f9 fa ee ff                     call       0x40c310
0051c817  50                                 push       eax
0051c818  8d 4d ec                           lea        ecx, [ebp - 0x14]
0051c81b  e8 10 ef ee ff                     call       0x40b730
0051c820  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0051c827  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0051c82a  e8 b1 78 f4 ff                     call       0x4640e0
0051c82f  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
0051c832  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0051c835  50                                 push       eax
0051c836  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0051c839  51                                 push       ecx
0051c83a  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0051c83d  e8 3e 00 00 00                     call       0x51c880
0051c842  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0051c845  e8 f6 22 fa ff                     call       0x4beb40
0051c84a  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0051c851  8d 4d ec                           lea        ecx, [ebp - 0x14]
0051c854  e8 a7 f0 ee ff                     call       0x40b900
0051c859  90                                 nop        
0051c85a  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0051c85d  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0051c864  59                                 pop        ecx
0051c865  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0051c868  33 cd                              xor        ecx, ebp
0051c86a  e8 7f 61 02 00                     call       0x5429ee
0051c86f  8b e5                              mov        esp, ebp
0051c871  5d                                 pop        ebp
0051c872  c2 08 00                           ret        8
