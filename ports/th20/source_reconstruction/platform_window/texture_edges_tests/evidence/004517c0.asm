004517c0  55                                 push       ebp
004517c1  8b ec                              mov        ebp, esp
004517c3  83 7d 08 00                        cmp        dword ptr [ebp + 8], 0
004517c7  75 05                              jne        0x4517ce
004517c9  e9 ab 00 00 00                     jmp        0x451879
004517ce  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
004517d1  66 8b 08                           mov        cx, word ptr [eax]
004517d4  66 c1 e9 08                        shr        cx, 8
004517d8  ba ff 00 00 00                     mov        edx, 0xff
004517dd  66 23 ca                           and        cx, dx
004517e0  0f b7 c1                           movzx      eax, cx
004517e3  85 c0                              test       eax, eax
004517e5  0f 84 8e 00 00 00                  je         0x451879
004517eb  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
004517ee  66 8b 11                           mov        dx, word ptr [ecx]
004517f1  66 c1 ea 05                        shr        dx, 5
004517f5  66 83 e2 07                        and        dx, 7
004517f9  0f b7 c2                           movzx      eax, dx
004517fc  b9 04 00 00 00                     mov        ecx, 4
00451801  6b d1 00                           imul       edx, ecx, 0
00451804  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00451807  03 04 11                           add        eax, dword ptr [ecx + edx]
0045180a  ba 04 00 00 00                     mov        edx, 4
0045180f  6b ca 00                           imul       ecx, edx, 0
00451812  8b 55 08                           mov        edx, dword ptr [ebp + 8]
00451815  89 04 0a                           mov        dword ptr [edx + ecx], eax
00451818  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0045181b  66 8b 08                           mov        cx, word ptr [eax]
0045181e  66 c1 e9 02                        shr        cx, 2
00451822  66 83 e1 07                        and        cx, 7
00451826  0f b7 d1                           movzx      edx, cx
00451829  b8 04 00 00 00                     mov        eax, 4
0045182e  c1 e0 00                           shl        eax, 0
00451831  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00451834  03 14 01                           add        edx, dword ptr [ecx + eax]
00451837  b8 04 00 00 00                     mov        eax, 4
0045183c  c1 e0 00                           shl        eax, 0
0045183f  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00451842  89 14 01                           mov        dword ptr [ecx + eax], edx
00451845  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
00451848  66 8b 02                           mov        ax, word ptr [edx]
0045184b  66 83 e0 03                        and        ax, 3
0045184f  0f b7 c8                           movzx      ecx, ax
00451852  ba 04 00 00 00                     mov        edx, 4
00451857  d1 e2                              shl        edx, 1
00451859  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0045185c  03 0c 10                           add        ecx, dword ptr [eax + edx]
0045185f  ba 04 00 00 00                     mov        edx, 4
00451864  d1 e2                              shl        edx, 1
00451866  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00451869  89 0c 10                           mov        dword ptr [eax + edx], ecx
0045186c  8b 4d 10                           mov        ecx, dword ptr [ebp + 0x10]
0045186f  8b 11                              mov        edx, dword ptr [ecx]
00451871  83 c2 01                           add        edx, 1
00451874  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
00451877  89 10                              mov        dword ptr [eax], edx
00451879  5d                                 pop        ebp
0045187a  c3                                 ret        
