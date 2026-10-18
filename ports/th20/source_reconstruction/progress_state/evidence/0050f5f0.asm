0050f5f0  55                                 push       ebp
0050f5f1  8b ec                              mov        ebp, esp
0050f5f3  83 ec 0c                           sub        esp, 0xc
0050f5f6  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0050f5f9  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050f5fc  89 45 f8                           mov        dword ptr [ebp - 8], eax
0050f5ff  68 38 1f 09 00                     push       0x91f38
0050f604  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050f607  81 c1 50 21 09 00                  add        ecx, 0x92150
0050f60d  51                                 push       ecx
0050f60e  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0050f611  83 c2 10                           add        edx, 0x10
0050f614  52                                 push       edx
0050f615  e8 06 48 03 00                     call       0x543e20
0050f61a  83 c4 0c                           add        esp, 0xc
0050f61d  68 f8 01 00 00                     push       0x1f8
0050f622  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050f625  05 88 40 12 00                     add        eax, 0x124088
0050f62a  50                                 push       eax
0050f62b  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0050f62e  81 c1 48 1f 09 00                  add        ecx, 0x91f48
0050f634  51                                 push       ecx
0050f635  e8 e6 47 03 00                     call       0x543e20
0050f63a  83 c4 0c                           add        esp, 0xc
0050f63d  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0050f640  52                                 push       edx
0050f641  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050f644  e8 27 f5 ff ff                     call       0x50eb70
0050f649  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
0050f64c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050f64f  e8 5c f8 ff ff                     call       0x50eeb0
0050f654  8b 45 f4                           mov        eax, dword ptr [ebp - 0xc]
0050f657  8b e5                              mov        esp, ebp
0050f659  5d                                 pop        ebp
0050f65a  c3                                 ret        
