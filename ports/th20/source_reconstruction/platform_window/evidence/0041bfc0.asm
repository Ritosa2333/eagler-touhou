0041bfc0  55                                 push       ebp
0041bfc1  8b ec                              mov        ebp, esp
0041bfc3  83 ec 08                           sub        esp, 8
0041bfc6  89 4d f8                           mov        dword ptr [ebp - 8], ecx
0041bfc9  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0041bfd0  eb 09                              jmp        0x41bfdb
0041bfd2  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041bfd5  83 c0 01                           add        eax, 1
0041bfd8  89 45 fc                           mov        dword ptr [ebp - 4], eax
0041bfdb  83 7d fc 04                        cmp        dword ptr [ebp - 4], 4
0041bfdf  7d 33                              jge        0x41c014
0041bfe1  6b 4d fc 28                        imul       ecx, dword ptr [ebp - 4], 0x28
0041bfe5  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0041bfe8  83 7c 0a 18 00                     cmp        dword ptr [edx + ecx + 0x18], 0
0041bfed  7c 23                              jl         0x41c012
0041bfef  6b 45 fc 28                        imul       eax, dword ptr [ebp - 4], 0x28
0041bff3  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0041bff6  8d 54 01 18                        lea        edx, [ecx + eax + 0x18]
0041bffa  52                                 push       edx
0041bffb  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0041bffe  e8 0d fb 02 00                     call       0x44bb10
0041c003  6b 45 fc 28                        imul       eax, dword ptr [ebp - 4], 0x28
0041c007  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0041c00a  c7 44 01 18 ff ff ff ff            mov        dword ptr [ecx + eax + 0x18], 0xffffffff
0041c012  eb be                              jmp        0x41bfd2
0041c014  8b e5                              mov        esp, ebp
0041c016  5d                                 pop        ebp
0041c017  c3                                 ret        
