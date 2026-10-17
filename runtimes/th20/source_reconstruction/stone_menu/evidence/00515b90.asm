00515b90 55                       push       ebp
00515b91 8bec                     mov        ebp, esp
00515b93 51                       push       ecx
00515b94 894dfc                   mov        dword ptr [ebp - 4], ecx
00515b97 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515b9a e881a1f0ff               call       0x41fd20
00515b9f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00515ba2 c700784d5700             mov        dword ptr [eax], 0x574d78
00515ba8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515bab c7411000000000           mov        dword ptr [ecx + 0x10], 0
00515bb2 8b55fc                   mov        edx, dword ptr [ebp - 4]
00515bb5 c7421400000000           mov        dword ptr [edx + 0x14], 0
00515bbc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515bbf 83c118                   add        ecx, 0x18
00515bc2 e8c9d1f0ff               call       0x422d90
00515bc7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515bca 83c128                   add        ecx, 0x28
00515bcd e82e93f9ff               call       0x4aef00
00515bd2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515bd5 83c174                   add        ecx, 0x74
00515bd8 e82393f9ff               call       0x4aef00
00515bdd 8b45fc                   mov        eax, dword ptr [ebp - 4]
00515be0 c780c000000000000000     mov        dword ptr [eax + 0xc0], 0
00515bea 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515bed 81c1c4000000             add        ecx, 0xc4
00515bf3 e8c800f1ff               call       0x425cc0
00515bf8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515bfb 81c1c8000000             add        ecx, 0xc8
00515c01 e8ba00f1ff               call       0x425cc0
00515c06 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515c09 81c1cc000000             add        ecx, 0xcc
00515c0f e8ac00f1ff               call       0x425cc0
00515c14 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515c17 81c1d0000000             add        ecx, 0xd0
00515c1d e89e00f1ff               call       0x425cc0
00515c22 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515c25 81c1d4000000             add        ecx, 0xd4
00515c2b e89000f1ff               call       0x425cc0
00515c30 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515c33 81c1d8000000             add        ecx, 0xd8
00515c39 e88200f1ff               call       0x425cc0
00515c3e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515c41 81c1dc000000             add        ecx, 0xdc
00515c47 e87400f1ff               call       0x425cc0
00515c4c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515c4f 81c1e0000000             add        ecx, 0xe0
00515c55 e86600f1ff               call       0x425cc0
00515c5a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515c5d 81c1e4000000             add        ecx, 0xe4
00515c63 e85800f1ff               call       0x425cc0
00515c68 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515c6b 81c1e8000000             add        ecx, 0xe8
00515c71 e89ad1f0ff               call       0x422e10
00515c76 6800580000               push       0x5800
00515c7b 6a00                     push       0
00515c7d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515c80 81c1f4000000             add        ecx, 0xf4
00515c86 51                       push       ecx
00515c87 e8d4ea0200               call       0x544760
00515c8c 83c40c                   add        esp, 0xc
00515c8f 6800b80100               push       0x1b800
00515c94 6a00                     push       0
00515c96 8b55fc                   mov        edx, dword ptr [ebp - 4]
00515c99 81c2f4580000             add        edx, 0x58f4
00515c9f 52                       push       edx
00515ca0 e8bbea0200               call       0x544760
00515ca5 83c40c                   add        esp, 0xc
00515ca8 8b45fc                   mov        eax, dword ptr [ebp - 4]
00515cab c780f410020000000000     mov        dword ptr [eax + 0x210f4], 0
00515cb5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00515cb8 c681f810020000           mov        byte ptr [ecx + 0x210f8], 0
00515cbf 8b55fc                   mov        edx, dword ptr [ebp - 4]
00515cc2 c782fc10020000000000     mov        dword ptr [edx + 0x210fc], 0
00515ccc 8b45fc                   mov        eax, dword ptr [ebp - 4]
00515ccf c7800011020000000000     mov        dword ptr [eax + 0x21100], 0
00515cd9 8b45fc                   mov        eax, dword ptr [ebp - 4]
00515cdc 8be5                     mov        esp, ebp
00515cde 5d                       pop        ebp
00515cdf c3                       ret        
