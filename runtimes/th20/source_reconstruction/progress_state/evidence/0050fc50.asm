0050fc50  55                                 push       ebp
0050fc51  8b ec                              mov        ebp, esp
0050fc53  83 ec 0c                           sub        esp, 0xc
0050fc56  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
0050fc59  b9 68 a5 5b 00                     mov        ecx, 0x5ba568
0050fc5e  e8 dd 86 fa ff                     call       0x4b8340
0050fc63  89 45 fc                           mov        dword ptr [ebp - 4], eax
0050fc66  b9 68 a5 5b 00                     mov        ecx, 0x5ba568
0050fc6b  e8 00 83 fa ff                     call       0x4b7f70
0050fc70  89 45 f8                           mov        dword ptr [ebp - 8], eax
0050fc73  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050fc76  50                                 push       eax
0050fc77  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0050fc7a  51                                 push       ecx
0050fc7b  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0050fc7e  e8 dd d7 fa ff                     call       0x4bd460
0050fc83  8b e5                              mov        esp, ebp
0050fc85  5d                                 pop        ebp
0050fc86  c3                                 ret        
