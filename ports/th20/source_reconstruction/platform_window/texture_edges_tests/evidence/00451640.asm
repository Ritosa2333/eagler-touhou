00451640  55                                 push       ebp
00451641  8b ec                              mov        ebp, esp
00451643  83 7d 08 00                        cmp        dword ptr [ebp + 8], 0
00451647  75 05                              jne        0x45164e
00451649  e9 a7 00 00 00                     jmp        0x4516f5
0045164e  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
00451651  66 8b 08                           mov        cx, word ptr [eax]
00451654  66 c1 e9 0f                        shr        cx, 0xf
00451658  66 83 e1 01                        and        cx, 1
0045165c  0f b7 d1                           movzx      edx, cx
0045165f  85 d2                              test       edx, edx
00451661  0f 84 8e 00 00 00                  je         0x4516f5
00451667  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0045166a  66 8b 08                           mov        cx, word ptr [eax]
0045166d  66 c1 e9 0a                        shr        cx, 0xa
00451671  66 83 e1 1f                        and        cx, 0x1f
00451675  0f b7 d1                           movzx      edx, cx
00451678  b8 04 00 00 00                     mov        eax, 4
0045167d  6b c8 00                           imul       ecx, eax, 0
00451680  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00451683  03 14 08                           add        edx, dword ptr [eax + ecx]
00451686  b9 04 00 00 00                     mov        ecx, 4
0045168b  6b c1 00                           imul       eax, ecx, 0
0045168e  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00451691  89 14 01                           mov        dword ptr [ecx + eax], edx
00451694  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
00451697  66 8b 02                           mov        ax, word ptr [edx]
0045169a  66 c1 e8 05                        shr        ax, 5
0045169e  66 83 e0 1f                        and        ax, 0x1f
004516a2  0f b7 c8                           movzx      ecx, ax
004516a5  ba 04 00 00 00                     mov        edx, 4
004516aa  c1 e2 00                           shl        edx, 0
004516ad  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004516b0  03 0c 10                           add        ecx, dword ptr [eax + edx]
004516b3  ba 04 00 00 00                     mov        edx, 4
004516b8  c1 e2 00                           shl        edx, 0
004516bb  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004516be  89 0c 10                           mov        dword ptr [eax + edx], ecx
004516c1  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
004516c4  66 8b 11                           mov        dx, word ptr [ecx]
004516c7  66 83 e2 1f                        and        dx, 0x1f
004516cb  0f b7 c2                           movzx      eax, dx
004516ce  b9 04 00 00 00                     mov        ecx, 4
004516d3  d1 e1                              shl        ecx, 1
004516d5  8b 55 08                           mov        edx, dword ptr [ebp + 8]
004516d8  03 04 0a                           add        eax, dword ptr [edx + ecx]
004516db  b9 04 00 00 00                     mov        ecx, 4
004516e0  d1 e1                              shl        ecx, 1
004516e2  8b 55 08                           mov        edx, dword ptr [ebp + 8]
004516e5  89 04 0a                           mov        dword ptr [edx + ecx], eax
004516e8  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
004516eb  8b 08                              mov        ecx, dword ptr [eax]
004516ed  83 c1 01                           add        ecx, 1
004516f0  8b 55 10                           mov        edx, dword ptr [ebp + 0x10]
004516f3  89 0a                              mov        dword ptr [edx], ecx
004516f5  5d                                 pop        ebp
004516f6  c3                                 ret        
