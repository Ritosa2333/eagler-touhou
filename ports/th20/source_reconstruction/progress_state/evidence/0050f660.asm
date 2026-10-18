0050f660  55                                 push       ebp
0050f661  8b ec                              mov        ebp, esp
0050f663  83 ec 0c                           sub        esp, 0xc
0050f666  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0050f669  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050f66c  81 c1 c4 42 12 00                  add        ecx, 0x1242c4
0050f672  e8 79 c6 ef ff                     call       0x40bcf0
0050f677  90                                 nop        
0050f678  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050f67b  05 c4 42 12 00                     add        eax, 0x1242c4
0050f680  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
0050f683  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
0050f68a  8d 4d f8                           lea        ecx, [ebp - 8]
0050f68d  51                                 push       ecx
0050f68e  68 60 fb 50 00                     push       0x50fb60
0050f693  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050f696  52                                 push       edx
0050f697  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0050f69a  e8 71 b4 ff ff                     call       0x50ab10
0050f69f  90                                 nop        
0050f6a0  33 c0                              xor        eax, eax
0050f6a2  8b e5                              mov        esp, ebp
0050f6a4  5d                                 pop        ebp
0050f6a5  c3                                 ret        
