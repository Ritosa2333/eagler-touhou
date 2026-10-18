00519960 55                       push       ebp
00519961 8bec                     mov        ebp, esp
00519963 6aff                     push       -1
00519965 68bbac5600               push       0x56acbb
0051996a 64a100000000             mov        eax, dword ptr fs:[0]
00519970 50                       push       eax
00519971 81ec24070000             sub        esp, 0x724
00519977 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0051997c 33c5                     xor        eax, ebp
0051997e 8945f0                   mov        dword ptr [ebp - 0x10], eax
00519981 56                       push       esi
00519982 57                       push       edi
00519983 50                       push       eax
00519984 8d45f4                   lea        eax, [ebp - 0xc]
00519987 64a300000000             mov        dword ptr fs:[0], eax
0051998d 898d48fcffff             mov        dword ptr [ebp - 0x3b8], ecx
00519993 682c4e5700               push       0x574e2c
00519998 e8132defff               call       0x40c6b0
0051999d 83c404                   add        esp, 4
005199a0 8b4508                   mov        eax, dword ptr [ebp + 8]
005199a3 50                       push       eax
005199a4 8b8d48fcffff             mov        ecx, dword ptr [ebp - 0x3b8]
005199aa e8f12d0000               call       0x51c7a0
005199af 8b4d08                   mov        ecx, dword ptr [ebp + 8]
005199b2 51                       push       ecx
005199b3 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
005199b9 e8e2dcf5ff               call       0x4776a0
005199be 90                       nop        
005199bf 68484e5700               push       0x574e48
005199c4 6a18                     push       0x18
005199c6 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
005199cc e87f54f3ff               call       0x44ee50
005199d1 898530fcffff             mov        dword ptr [ebp - 0x3d0], eax
005199d7 8b9548fcffff             mov        edx, dword ptr [ebp - 0x3b8]
005199dd 8b8530fcffff             mov        eax, dword ptr [ebp - 0x3d0]
005199e3 894210                   mov        dword ptr [edx + 0x10], eax
005199e6 83bd30fcffff00           cmp        dword ptr [ebp - 0x3d0], 0
005199ed 751a                     jne        0x519a09
005199ef 6810f65600               push       0x56f610
005199f4 6878065c00               push       0x5c0678
005199f9 e852a7f3ff               call       0x454150
005199fe 83c408                   add        esp, 8
00519a01 83c8ff                   or         eax, 0xffffffff
00519a04 e966060000               jmp        0x51a06f
00519a09 8b8d48fcffff             mov        ecx, dword ptr [ebp - 0x3b8]
00519a0f 51                       push       ecx
00519a10 68d0b15100               push       0x51b1d0
00519a15 6a1b                     push       0x1b
00519a17 e8a488efff               call       0x4122c0
00519a1c 83c40c                   add        esp, 0xc
00519a1f 8b9548fcffff             mov        edx, dword ptr [ebp - 0x3b8]
00519a25 894208                   mov        dword ptr [edx + 8], eax
00519a28 8b8548fcffff             mov        eax, dword ptr [ebp - 0x3b8]
00519a2e 50                       push       eax
00519a2f 68b0b45100               push       0x51b4b0
00519a34 6a37                     push       0x37
00519a36 e82589efff               call       0x412360
00519a3b 83c40c                   add        esp, 0xc
00519a3e 8b8d48fcffff             mov        ecx, dword ptr [ebp - 0x3b8]
00519a44 89410c                   mov        dword ptr [ecx + 0xc], eax
00519a47 6a00                     push       0
00519a49 6a00                     push       0
00519a4b 68544e5700               push       0x574e54
00519a50 e84b70efff               call       0x410aa0
00519a55 83c40c                   add        esp, 0xc
00519a58 898534fcffff             mov        dword ptr [ebp - 0x3cc], eax
00519a5e 6a1c                     push       0x1c
00519a60 8d4db4                   lea        ecx, [ebp - 0x4c]
00519a63 e81826efff               call       0x40c080
00519a68 8b9534fcffff             mov        edx, dword ptr [ebp - 0x3cc]
00519a6e 52                       push       edx
00519a6f 8d4db4                   lea        ecx, [ebp - 0x4c]
00519a72 e8b90df5ff               call       0x46a830
00519a77 c745fc00000000           mov        dword ptr [ebp - 4], 0
00519a7e 8d85b8fbffff             lea        eax, [ebp - 0x448]
00519a84 50                       push       eax
00519a85 8d4db4                   lea        ecx, [ebp - 0x4c]
00519a88 e8731a0000               call       0x51b500
00519a8d 8b08                     mov        ecx, dword ptr [eax]
00519a8f 898de4fbffff             mov        dword ptr [ebp - 0x41c], ecx
00519a95 8b95e4fbffff             mov        edx, dword ptr [ebp - 0x41c]
00519a9b 899520fcffff             mov        dword ptr [ebp - 0x3e0], edx
00519aa1 c68543fcffff0d           mov        byte ptr [ebp - 0x3bd], 0xd
00519aa8 8d85b4fbffff             lea        eax, [ebp - 0x44c]
00519aae 50                       push       eax
00519aaf 8d4db4                   lea        ecx, [ebp - 0x4c]
00519ab2 e8491a0000               call       0x51b500
00519ab7 8b08                     mov        ecx, dword ptr [eax]
00519ab9 898de0fbffff             mov        dword ptr [ebp - 0x420], ecx
00519abf 8d95b0fbffff             lea        edx, [ebp - 0x450]
00519ac5 52                       push       edx
00519ac6 8d4db4                   lea        ecx, [ebp - 0x4c]
00519ac9 e83252feff               call       0x4fed00
00519ace 8b00                     mov        eax, dword ptr [eax]
00519ad0 8985dcfbffff             mov        dword ptr [ebp - 0x424], eax
00519ad6 8d8d43fcffff             lea        ecx, [ebp - 0x3bd]
00519adc 51                       push       ecx
00519add 8b95e0fbffff             mov        edx, dword ptr [ebp - 0x420]
00519ae3 52                       push       edx
00519ae4 8b85dcfbffff             mov        eax, dword ptr [ebp - 0x424]
00519aea 50                       push       eax
00519aeb 8d8dacfbffff             lea        ecx, [ebp - 0x454]
00519af1 51                       push       ecx
00519af2 e859baffff               call       0x515550
00519af7 83c410                   add        esp, 0x10
00519afa 8b10                     mov        edx, dword ptr [eax]
00519afc 8995d8fbffff             mov        dword ptr [ebp - 0x428], edx
00519b02 8b85d8fbffff             mov        eax, dword ptr [ebp - 0x428]
00519b08 89851cfcffff             mov        dword ptr [ebp - 0x3e4], eax
00519b0e 8b8d20fcffff             mov        ecx, dword ptr [ebp - 0x3e0]
00519b14 51                       push       ecx
00519b15 8b951cfcffff             mov        edx, dword ptr [ebp - 0x3e4]
00519b1b 52                       push       edx
00519b1c 8d85d4fbffff             lea        eax, [ebp - 0x42c]
00519b22 50                       push       eax
00519b23 8d4db4                   lea        ecx, [ebp - 0x4c]
00519b26 e8b51a0000               call       0x51b5e0
00519b2b 90                       nop        
00519b2c 8d8dd0fbffff             lea        ecx, [ebp - 0x430]
00519b32 51                       push       ecx
00519b33 8d4db4                   lea        ecx, [ebp - 0x4c]
00519b36 e8c5190000               call       0x51b500
00519b3b 8b10                     mov        edx, dword ptr [eax]
00519b3d 899518fcffff             mov        dword ptr [ebp - 0x3e8], edx
00519b43 8b8518fcffff             mov        eax, dword ptr [ebp - 0x3e8]
00519b49 898508fcffff             mov        dword ptr [ebp - 0x3f8], eax
00519b4f c68542fcffff0c           mov        byte ptr [ebp - 0x3be], 0xc
00519b56 8d8dccfbffff             lea        ecx, [ebp - 0x434]
00519b5c 51                       push       ecx
00519b5d 8d4db4                   lea        ecx, [ebp - 0x4c]
00519b60 e89b190000               call       0x51b500
00519b65 8b10                     mov        edx, dword ptr [eax]
00519b67 899514fcffff             mov        dword ptr [ebp - 0x3ec], edx
00519b6d 8d85c8fbffff             lea        eax, [ebp - 0x438]
00519b73 50                       push       eax
00519b74 8d4db4                   lea        ecx, [ebp - 0x4c]
00519b77 e88451feff               call       0x4fed00
00519b7c 8b08                     mov        ecx, dword ptr [eax]
00519b7e 898d10fcffff             mov        dword ptr [ebp - 0x3f0], ecx
00519b84 8d9542fcffff             lea        edx, [ebp - 0x3be]
00519b8a 52                       push       edx
00519b8b 8b8514fcffff             mov        eax, dword ptr [ebp - 0x3ec]
00519b91 50                       push       eax
00519b92 8b8d10fcffff             mov        ecx, dword ptr [ebp - 0x3f0]
00519b98 51                       push       ecx
00519b99 8d95c4fbffff             lea        edx, [ebp - 0x43c]
00519b9f 52                       push       edx
00519ba0 e8abb9ffff               call       0x515550
00519ba5 83c410                   add        esp, 0x10
00519ba8 8b00                     mov        eax, dword ptr [eax]
00519baa 89850cfcffff             mov        dword ptr [ebp - 0x3f4], eax
00519bb0 8b8d0cfcffff             mov        ecx, dword ptr [ebp - 0x3f4]
00519bb6 898d04fcffff             mov        dword ptr [ebp - 0x3fc], ecx
00519bbc 8b9508fcffff             mov        edx, dword ptr [ebp - 0x3f8]
00519bc2 52                       push       edx
00519bc3 8b8504fcffff             mov        eax, dword ptr [ebp - 0x3fc]
00519bc9 50                       push       eax
00519bca 8d8dc0fbffff             lea        ecx, [ebp - 0x440]
00519bd0 51                       push       ecx
00519bd1 8d4db4                   lea        ecx, [ebp - 0x4c]
00519bd4 e8071a0000               call       0x51b5e0
00519bd9 90                       nop        
00519bda 68a8000000               push       0xa8
00519bdf 8d8d0cffffff             lea        ecx, [ebp - 0xf4]
00519be5 e89624efff               call       0x40c080
00519bea 6a01                     push       1
00519bec 6a01                     push       1
00519bee 8d55b4                   lea        edx, [ebp - 0x4c]
00519bf1 52                       push       edx
00519bf2 8d8d0cffffff             lea        ecx, [ebp - 0xf4]
00519bf8 e8b3a6ffff               call       0x5142b0
00519bfd c645fc01                 mov        byte ptr [ebp - 4], 1
00519c01 6a1c                     push       0x1c
00519c03 8d4dd0                   lea        ecx, [ebp - 0x30]
00519c06 e87524efff               call       0x40c080
00519c0b 8d4dd0                   lea        ecx, [ebp - 0x30]
00519c0e e8ade0f2ff               call       0x447cc0
00519c13 c645fc02                 mov        byte ptr [ebp - 4], 2
00519c17 8d45d0                   lea        eax, [ebp - 0x30]
00519c1a 50                       push       eax
00519c1b 8d8d0cffffff             lea        ecx, [ebp - 0xf4]
00519c21 51                       push       ecx
00519c22 e8d9b8ffff               call       0x515500
00519c27 83c408                   add        esp, 8
00519c2a 89852cfcffff             mov        dword ptr [ebp - 0x3d4], eax
00519c30 8b952cfcffff             mov        edx, dword ptr [ebp - 0x3d4]
00519c36 8b02                     mov        eax, dword ptr [edx]
00519c38 8b8d2cfcffff             mov        ecx, dword ptr [ebp - 0x3d4]
00519c3e 034804                   add        ecx, dword ptr [eax + 4]
00519c41 e8dac7ffff               call       0x516420
00519c46 0fb6c8                   movzx      ecx, al
00519c49 85c9                     test       ecx, ecx
00519c4b 0f84cc030000             je         0x51a01d
00519c51 6a00                     push       0
00519c53 8d4dd0                   lea        ecx, [ebp - 0x30]
00519c56 e86555f8ff               call       0x49f1c0
00519c5b 0fbe10                   movsx      edx, byte ptr [eax]
00519c5e 83fa5c                   cmp        edx, 0x5c
00519c61 750a                     jne        0x519c6d
00519c63 e9b5030000               jmp        0x51a01d
00519c68 e9ab030000               jmp        0x51a018
00519c6d 6a00                     push       0
00519c6f 8d4dd0                   lea        ecx, [ebp - 0x30]
00519c72 e84955f8ff               call       0x49f1c0
00519c77 0fbe00                   movsx      eax, byte ptr [eax]
00519c7a 83f840                   cmp        eax, 0x40
00519c7d 0f8595030000             jne        0x51a018
00519c83 68e0425700               push       0x5742e0
00519c88 8d8dd0f8ffff             lea        ecx, [ebp - 0x730]
00519c8e 51                       push       ecx
00519c8f e8fcb9ffff               call       0x515690
00519c94 83c408                   add        esp, 8
00519c97 b9b0000000               mov        ecx, 0xb0
00519c9c 8bf0                     mov        esi, eax
00519c9e 8dbd4cfcffff             lea        edi, [ebp - 0x3b4]
00519ca4 f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
00519ca6 6a04                     push       4
00519ca8 8d4dec                   lea        ecx, [ebp - 0x14]
00519cab e8d023efff               call       0x40c080
00519cb0 33d2                     xor        edx, edx
00519cb2 889541fcffff             mov        byte ptr [ebp - 0x3bf], dl
00519cb8 8a8541fcffff             mov        al, byte ptr [ebp - 0x3bf]
00519cbe 888540fcffff             mov        byte ptr [ebp - 0x3c0], al
00519cc4 6aff                     push       -1
00519cc6 6a01                     push       1
00519cc8 8d8d90fbffff             lea        ecx, [ebp - 0x470]
00519cce 51                       push       ecx
00519ccf 8d4dd0                   lea        ecx, [ebp - 0x30]
00519cd2 e8a92f0000               call       0x51cc80
00519cd7 898500fcffff             mov        dword ptr [ebp - 0x400], eax
00519cdd 8b9500fcffff             mov        edx, dword ptr [ebp - 0x400]
00519ce3 8995fcfbffff             mov        dword ptr [ebp - 0x404], edx
00519ce9 c645fc03                 mov        byte ptr [ebp - 4], 3
00519ced 8b85fcfbffff             mov        eax, dword ptr [ebp - 0x404]
00519cf3 8985f8fbffff             mov        dword ptr [ebp - 0x408], eax
00519cf9 0fb68d40fcffff           movzx      ecx, byte ptr [ebp - 0x3c0]
00519d00 51                       push       ecx
00519d01 8b95f8fbffff             mov        edx, dword ptr [ebp - 0x408]
00519d07 52                       push       edx
00519d08 8d854cfcffff             lea        eax, [ebp - 0x3b4]
00519d0e 50                       push       eax
00519d0f 8d4dec                   lea        ecx, [ebp - 0x14]
00519d12 51                       push       ecx
00519d13 e878a7ffff               call       0x514490
00519d18 83c410                   add        esp, 0x10
00519d1b c645fc02                 mov        byte ptr [ebp - 4], 2
00519d1f 8d8d90fbffff             lea        ecx, [ebp - 0x470]
00519d25 e816f5f2ff               call       0x449240
00519d2a 90                       nop        
00519d2b 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00519d2e 8995f4fbffff             mov        dword ptr [ebp - 0x40c], edx
00519d34 8d85bcfbffff             lea        eax, [ebp - 0x444]
00519d3a 50                       push       eax
00519d3b 8d8d4cfcffff             lea        ecx, [ebp - 0x3b4]
00519d41 e83a60f4ff               call       0x45fd80
00519d46 8b08                     mov        ecx, dword ptr [eax]
00519d48 898df0fbffff             mov        dword ptr [ebp - 0x410], ecx
00519d4e 8b95f4fbffff             mov        edx, dword ptr [ebp - 0x40c]
00519d54 52                       push       edx
00519d55 8b85f0fbffff             mov        eax, dword ptr [ebp - 0x410]
00519d5b 50                       push       eax
00519d5c e87fb5ffff               call       0x5152e0
00519d61 83c408                   add        esp, 8
00519d64 898544fcffff             mov        dword ptr [ebp - 0x3bc], eax
00519d6a 8b8d44fcffff             mov        ecx, dword ptr [ebp - 0x3bc]
00519d70 81e103000080             and        ecx, 0x80000003
00519d76 7905                     jns        0x519d7d
00519d78 49                       dec        ecx
00519d79 83c9fc                   or         ecx, 0xfffffffc
00519d7c 41                       inc        ecx
00519d7d 85c9                     test       ecx, ecx
00519d7f 0f85dd000000             jne        0x519e62
00519d85 83bd44fcffff48           cmp        dword ptr [ebp - 0x3bc], 0x48
00519d8c 0f8dd0000000             jge        0x519e62
00519d92 8d55d0                   lea        edx, [ebp - 0x30]
00519d95 52                       push       edx
00519d96 8d850cffffff             lea        eax, [ebp - 0xf4]
00519d9c 50                       push       eax
00519d9d e85eb7ffff               call       0x515500
00519da2 83c408                   add        esp, 8
00519da5 8d4dd0                   lea        ecx, [ebp - 0x30]
00519da8 e8431df3ff               call       0x44baf0
00519dad 50                       push       eax
00519dae 6800010000               push       0x100
00519db3 8b8d44fcffff             mov        ecx, dword ptr [ebp - 0x3bc]
00519db9 c1e108                   shl        ecx, 8
00519dbc 8b9548fcffff             mov        edx, dword ptr [ebp - 0x3b8]
00519dc2 8d840af4000000           lea        eax, [edx + ecx + 0xf4]
00519dc9 50                       push       eax
00519dca e841e80200               call       0x548610
00519dcf 83c40c                   add        esp, 0xc
00519dd2 8d4dd0                   lea        ecx, [ebp - 0x30]
00519dd5 e8161df3ff               call       0x44baf0
00519dda 50                       push       eax
00519ddb 6800010000               push       0x100
00519de0 8b8d44fcffff             mov        ecx, dword ptr [ebp - 0x3bc]
00519de6 83c101                   add        ecx, 1
00519de9 c1e108                   shl        ecx, 8
00519dec 8b9548fcffff             mov        edx, dword ptr [ebp - 0x3b8]
00519df2 8d840af4000000           lea        eax, [edx + ecx + 0xf4]
00519df9 50                       push       eax
00519dfa e811e80200               call       0x548610
00519dff 83c40c                   add        esp, 0xc
00519e02 8d4dd0                   lea        ecx, [ebp - 0x30]
00519e05 e8e61cf3ff               call       0x44baf0
00519e0a 50                       push       eax
00519e0b 6800010000               push       0x100
00519e10 8b8d44fcffff             mov        ecx, dword ptr [ebp - 0x3bc]
00519e16 83c102                   add        ecx, 2
00519e19 c1e108                   shl        ecx, 8
00519e1c 8b9548fcffff             mov        edx, dword ptr [ebp - 0x3b8]
00519e22 8d840af4000000           lea        eax, [edx + ecx + 0xf4]
00519e29 50                       push       eax
00519e2a e8e1e70200               call       0x548610
00519e2f 83c40c                   add        esp, 0xc
00519e32 8d4dd0                   lea        ecx, [ebp - 0x30]
00519e35 e8b61cf3ff               call       0x44baf0
00519e3a 50                       push       eax
00519e3b 6800010000               push       0x100
00519e40 8b8d44fcffff             mov        ecx, dword ptr [ebp - 0x3bc]
00519e46 83c103                   add        ecx, 3
00519e49 c1e108                   shl        ecx, 8
00519e4c 8b9548fcffff             mov        edx, dword ptr [ebp - 0x3b8]
00519e52 8d840af4000000           lea        eax, [edx + ecx + 0xf4]
00519e59 50                       push       eax
00519e5a e8b1e70200               call       0x548610
00519e5f 83c40c                   add        esp, 0xc
00519e62 83bd44fcffff48           cmp        dword ptr [ebp - 0x3bc], 0x48
00519e69 7c40                     jl         0x519eab
00519e6b 8d4dd0                   lea        ecx, [ebp - 0x30]
00519e6e 51                       push       ecx
00519e6f 8d950cffffff             lea        edx, [ebp - 0xf4]
00519e75 52                       push       edx
00519e76 e885b6ffff               call       0x515500
00519e7b 83c408                   add        esp, 8
00519e7e 8d4dd0                   lea        ecx, [ebp - 0x30]
00519e81 e86a1cf3ff               call       0x44baf0
00519e86 50                       push       eax
00519e87 6800010000               push       0x100
00519e8c 8b8544fcffff             mov        eax, dword ptr [ebp - 0x3bc]
00519e92 c1e008                   shl        eax, 8
00519e95 8b8d48fcffff             mov        ecx, dword ptr [ebp - 0x3b8]
00519e9b 8d9401f4000000           lea        edx, [ecx + eax + 0xf4]
00519ea2 52                       push       edx
00519ea3 e868e70200               call       0x548610
00519ea8 83c40c                   add        esp, 0xc
00519eab 698544fcffff00050000     imul       eax, dword ptr [ebp - 0x3bc], 0x500
00519eb5 8b8d48fcffff             mov        ecx, dword ptr [ebp - 0x3b8]
00519ebb 8d9401f4580000           lea        edx, [ecx + eax + 0x58f4]
00519ec2 899528fcffff             mov        dword ptr [ebp - 0x3d8], edx
00519ec8 8b8528fcffff             mov        eax, dword ptr [ebp - 0x3d8]
00519ece 89853cfcffff             mov        dword ptr [ebp - 0x3c4], eax
00519ed4 8b8d28fcffff             mov        ecx, dword ptr [ebp - 0x3d8]
00519eda 81c100050000             add        ecx, 0x500
00519ee0 898decfbffff             mov        dword ptr [ebp - 0x414], ecx
00519ee6 eb12                     jmp        0x519efa
00519ee8 8b953cfcffff             mov        edx, dword ptr [ebp - 0x3c4]
00519eee 81c200010000             add        edx, 0x100
00519ef4 89953cfcffff             mov        dword ptr [ebp - 0x3c4], edx
00519efa 8b853cfcffff             mov        eax, dword ptr [ebp - 0x3c4]
00519f00 3b85ecfbffff             cmp        eax, dword ptr [ebp - 0x414]
00519f06 7420                     je         0x519f28
00519f08 8b8d3cfcffff             mov        ecx, dword ptr [ebp - 0x3c4]
00519f0e 898de8fbffff             mov        dword ptr [ebp - 0x418], ecx
00519f14 ba01000000               mov        edx, 1
00519f19 6bc200                   imul       eax, edx, 0
00519f1c 8b8de8fbffff             mov        ecx, dword ptr [ebp - 0x418]
00519f22 c6040100                 mov        byte ptr [ecx + eax], 0
00519f26 ebc0                     jmp        0x519ee8
00519f28 c78538fcffff00000000     mov        dword ptr [ebp - 0x3c8], 0
00519f32 8d55d0                   lea        edx, [ebp - 0x30]
00519f35 52                       push       edx
00519f36 8d850cffffff             lea        eax, [ebp - 0xf4]
00519f3c 50                       push       eax
00519f3d e8beb5ffff               call       0x515500
00519f42 83c408                   add        esp, 8
00519f45 898524fcffff             mov        dword ptr [ebp - 0x3dc], eax
00519f4b 8b8d24fcffff             mov        ecx, dword ptr [ebp - 0x3dc]
00519f51 8b11                     mov        edx, dword ptr [ecx]
00519f53 8b8d24fcffff             mov        ecx, dword ptr [ebp - 0x3dc]
00519f59 034a04                   add        ecx, dword ptr [edx + 4]
00519f5c e8bfc4ffff               call       0x516420
00519f61 0fb6c0                   movzx      eax, al
00519f64 85c0                     test       eax, eax
00519f66 0f84ac000000             je         0x51a018
00519f6c 8d4dd0                   lea        ecx, [ebp - 0x30]
00519f6f e81c2bf0ff               call       0x41ca90
00519f74 85c0                     test       eax, eax
00519f76 7412                     je         0x519f8a
00519f78 6a00                     push       0
00519f7a 8d4dd0                   lea        ecx, [ebp - 0x30]
00519f7d e83e52f8ff               call       0x49f1c0
00519f82 0fbe08                   movsx      ecx, byte ptr [eax]
00519f85 83f923                   cmp        ecx, 0x23
00519f88 7507                     jne        0x519f91
00519f8a eba6                     jmp        0x519f32
00519f8c e982000000               jmp        0x51a013
00519f91 6a00                     push       0
00519f93 8d4dd0                   lea        ecx, [ebp - 0x30]
00519f96 e82552f8ff               call       0x49f1c0
00519f9b 0fbe10                   movsx      edx, byte ptr [eax]
00519f9e 83fa5c                   cmp        edx, 0x5c
00519fa1 7504                     jne        0x519fa7
00519fa3 eb78                     jmp        0x51a01d
00519fa5 eb6c                     jmp        0x51a013
00519fa7 6a00                     push       0
00519fa9 8d4dd0                   lea        ecx, [ebp - 0x30]
00519fac e80f52f8ff               call       0x49f1c0
00519fb1 0fbe00                   movsx      eax, byte ptr [eax]
00519fb4 83f840                   cmp        eax, 0x40
00519fb7 7507                     jne        0x519fc0
00519fb9 e9c5fcffff               jmp        0x519c83
00519fbe eb53                     jmp        0x51a013
00519fc0 8d4dd0                   lea        ecx, [ebp - 0x30]
00519fc3 e8281bf3ff               call       0x44baf0
00519fc8 50                       push       eax
00519fc9 6800010000               push       0x100
00519fce 698d44fcffff00050000     imul       ecx, dword ptr [ebp - 0x3bc], 0x500
00519fd8 8b9548fcffff             mov        edx, dword ptr [ebp - 0x3b8]
00519fde 8d840af4580000           lea        eax, [edx + ecx + 0x58f4]
00519fe5 8b8d38fcffff             mov        ecx, dword ptr [ebp - 0x3c8]
00519feb c1e108                   shl        ecx, 8
00519fee 03c1                     add        eax, ecx
00519ff0 50                       push       eax
00519ff1 e81ae60200               call       0x548610
00519ff6 83c40c                   add        esp, 0xc
00519ff9 8b9538fcffff             mov        edx, dword ptr [ebp - 0x3c8]
00519fff 83c201                   add        edx, 1
0051a002 899538fcffff             mov        dword ptr [ebp - 0x3c8], edx
0051a008 83bd38fcffff05           cmp        dword ptr [ebp - 0x3c8], 5
0051a00f 7c02                     jl         0x51a013
0051a011 eb05                     jmp        0x51a018
0051a013 e91affffff               jmp        0x519f32
0051a018 e9fafbffff               jmp        0x519c17
0051a01d 83bd34fcffff00           cmp        dword ptr [ebp - 0x3cc], 0
0051a024 741c                     je         0x51a042
0051a026 8b8534fcffff             mov        eax, dword ptr [ebp - 0x3cc]
0051a02c 50                       push       eax
0051a02d 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0051a033 e83856f0ff               call       0x41f670
0051a038 c78534fcffff00000000     mov        dword ptr [ebp - 0x3cc], 0
0051a042 c645fc01                 mov        byte ptr [ebp - 4], 1
0051a046 8d4dd0                   lea        ecx, [ebp - 0x30]
0051a049 e8f2f1f2ff               call       0x449240
0051a04e c645fc00                 mov        byte ptr [ebp - 4], 0
0051a052 8d8d0cffffff             lea        ecx, [ebp - 0xf4]
0051a058 e8d3c4ffff               call       0x516530
0051a05d c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0051a064 8d4db4                   lea        ecx, [ebp - 0x4c]
0051a067 e8d4f1f2ff               call       0x449240
0051a06c 90                       nop        
0051a06d 33c0                     xor        eax, eax
0051a06f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0051a072 64890d00000000           mov        dword ptr fs:[0], ecx
0051a079 59                       pop        ecx
0051a07a 5f                       pop        edi
0051a07b 5e                       pop        esi
0051a07c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051a07f 33cd                     xor        ecx, ebp
0051a081 e868890200               call       0x5429ee
0051a086 8be5                     mov        esp, ebp
0051a088 5d                       pop        ebp
0051a089 c20400                   ret        4
0051a08c cc                       int3       
0051a08d cc                       int3       
0051a08e cc                       int3       
0051a08f cc                       int3       
