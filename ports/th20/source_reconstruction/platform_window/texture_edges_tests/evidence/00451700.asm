00451700  55                                 push       ebp
00451701  8b ec                              mov        ebp, esp
00451703  83 7d 08 00                        cmp        dword ptr [ebp + 8], 0
00451707  75 05                              jne        0x45170e
00451709  e9 a7 00 00 00                     jmp        0x4517b5
0045170e  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
00451711  66 8b 08                           mov        cx, word ptr [eax]
00451714  66 c1 e9 0c                        shr        cx, 0xc
00451718  66 83 e1 0f                        and        cx, 0xf
0045171c  0f b7 d1                           movzx      edx, cx
0045171f  85 d2                              test       edx, edx
00451721  0f 84 8e 00 00 00                  je         0x4517b5
00451727  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0045172a  66 8b 08                           mov        cx, word ptr [eax]
0045172d  66 c1 e9 08                        shr        cx, 8
00451731  66 83 e1 0f                        and        cx, 0xf
00451735  0f b7 d1                           movzx      edx, cx
00451738  b8 04 00 00 00                     mov        eax, 4
0045173d  6b c8 00                           imul       ecx, eax, 0
00451740  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00451743  03 14 08                           add        edx, dword ptr [eax + ecx]
00451746  b9 04 00 00 00                     mov        ecx, 4
0045174b  6b c1 00                           imul       eax, ecx, 0
0045174e  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00451751  89 14 01                           mov        dword ptr [ecx + eax], edx
00451754  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
00451757  66 8b 02                           mov        ax, word ptr [edx]
0045175a  66 c1 e8 04                        shr        ax, 4
0045175e  66 83 e0 0f                        and        ax, 0xf
00451762  0f b7 c8                           movzx      ecx, ax
00451765  ba 04 00 00 00                     mov        edx, 4
0045176a  c1 e2 00                           shl        edx, 0
0045176d  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00451770  03 0c 10                           add        ecx, dword ptr [eax + edx]
00451773  ba 04 00 00 00                     mov        edx, 4
00451778  c1 e2 00                           shl        edx, 0
0045177b  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0045177e  89 0c 10                           mov        dword ptr [eax + edx], ecx
00451781  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
00451784  66 8b 11                           mov        dx, word ptr [ecx]
00451787  66 83 e2 0f                        and        dx, 0xf
0045178b  0f b7 c2                           movzx      eax, dx
0045178e  b9 04 00 00 00                     mov        ecx, 4
00451793  d1 e1                              shl        ecx, 1
00451795  8b 55 08                           mov        edx, dword ptr [ebp + 8]
00451798  03 04 0a                           add        eax, dword ptr [edx + ecx]
0045179b  b9 04 00 00 00                     mov        ecx, 4
004517a0  d1 e1                              shl        ecx, 1
004517a2  8b 55 08                           mov        edx, dword ptr [ebp + 8]
004517a5  89 04 0a                           mov        dword ptr [edx + ecx], eax
004517a8  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
004517ab  8b 08                              mov        ecx, dword ptr [eax]
004517ad  83 c1 01                           add        ecx, 1
004517b0  8b 55 10                           mov        edx, dword ptr [ebp + 0x10]
004517b3  89 0a                              mov        dword ptr [edx], ecx
004517b5  5d                                 pop        ebp
004517b6  c3                                 ret        
