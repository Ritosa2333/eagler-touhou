00451880  55                                 push       ebp
00451881  8b ec                              mov        ebp, esp
00451883  83 7d 08 00                        cmp        dword ptr [ebp + 8], 0
00451887  75 02                              jne        0x45188b
00451889  eb 7e                              jmp        0x451909
0045188b  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0045188e  0f b6 48 03                        movzx      ecx, byte ptr [eax + 3]
00451892  85 c9                              test       ecx, ecx
00451894  74 73                              je         0x451909
00451896  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
00451899  0f b6 42 02                        movzx      eax, byte ptr [edx + 2]
0045189d  b9 04 00 00 00                     mov        ecx, 4
004518a2  6b d1 00                           imul       edx, ecx, 0
004518a5  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004518a8  03 04 11                           add        eax, dword ptr [ecx + edx]
004518ab  ba 04 00 00 00                     mov        edx, 4
004518b0  6b ca 00                           imul       ecx, edx, 0
004518b3  8b 55 08                           mov        edx, dword ptr [ebp + 8]
004518b6  89 04 0a                           mov        dword ptr [edx + ecx], eax
004518b9  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
004518bc  0f b6 48 01                        movzx      ecx, byte ptr [eax + 1]
004518c0  ba 04 00 00 00                     mov        edx, 4
004518c5  c1 e2 00                           shl        edx, 0
004518c8  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004518cb  03 0c 10                           add        ecx, dword ptr [eax + edx]
004518ce  ba 04 00 00 00                     mov        edx, 4
004518d3  c1 e2 00                           shl        edx, 0
004518d6  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004518d9  89 0c 10                           mov        dword ptr [eax + edx], ecx
004518dc  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
004518df  0f b6 11                           movzx      edx, byte ptr [ecx]
004518e2  b8 04 00 00 00                     mov        eax, 4
004518e7  d1 e0                              shl        eax, 1
004518e9  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004518ec  03 14 01                           add        edx, dword ptr [ecx + eax]
004518ef  b8 04 00 00 00                     mov        eax, 4
004518f4  d1 e0                              shl        eax, 1
004518f6  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004518f9  89 14 01                           mov        dword ptr [ecx + eax], edx
004518fc  8b 55 10                           mov        edx, dword ptr [ebp + 0x10]
004518ff  8b 02                              mov        eax, dword ptr [edx]
00451901  83 c0 01                           add        eax, 1
00451904  8b 4d 10                           mov        ecx, dword ptr [ebp + 0x10]
00451907  89 01                              mov        dword ptr [ecx], eax
00451909  5d                                 pop        ebp
0045190a  c3                                 ret        
