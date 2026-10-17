00559d30 8bff                     mov        edi, edi
00559d32 55                       push       ebp
00559d33 8bec                     mov        ebp, esp
00559d35 83ec24                   sub        esp, 0x24
00559d38 53                       push       ebx
00559d39 56                       push       esi
00559d3a 57                       push       edi
00559d3b 8b7d08                   mov        edi, dword ptr [ebp + 8]
00559d3e 8bc7                     mov        eax, edi
00559d40 c1e80e                   shr        eax, 0xe
00559d43 8bd7                     mov        edx, edi
00559d45 c1ea10                   shr        edx, 0x10
00559d48 2500030000               and        eax, 0x300
00559d4d 83e23f                   and        edx, 0x3f
00559d50 0bd0                     or         edx, eax
00559d52 8bc7                     mov        eax, edi
00559d54 2500400000               and        eax, 0x4000
00559d59 7411                     je         0x559d6c
00559d5b 33c9                     xor        ecx, ecx
00559d5d bb00100000               mov        ebx, 0x1000
00559d62 3d00400000               cmp        eax, 0x4000
00559d67 0f45d9                   cmovne     ebx, ecx
00559d6a eb02                     jmp        0x559d6e
00559d6c 33db                     xor        ebx, ebx
00559d6e 8bc7                     mov        eax, edi
00559d70 2500300000               and        eax, 0x3000
00559d75 7419                     je         0x559d90
00559d77 3d00100000               cmp        eax, 0x1000
00559d7c 7409                     je         0x559d87
00559d7e c745fc00000000           mov        dword ptr [ebp - 4], 0
00559d85 eb10                     jmp        0x559d97
00559d87 c745fc00020000           mov        dword ptr [ebp - 4], 0x200
00559d8e eb07                     jmp        0x559d97
00559d90 c745fc00030000           mov        dword ptr [ebp - 4], 0x300
00559d97 8bc2                     mov        eax, edx
00559d99 2500030000               and        eax, 0x300
00559d9e 3d00020000               cmp        eax, 0x200
00559da3 771b                     ja         0x559dc0
00559da5 7412                     je         0x559db9
00559da7 85c0                     test       eax, eax
00559da9 741c                     je         0x559dc7
00559dab 3d00010000               cmp        eax, 0x100
00559db0 7515                     jne        0x559dc7
00559db2 be00040000               mov        esi, 0x400
00559db7 eb17                     jmp        0x559dd0
00559db9 be00080000               mov        esi, 0x800
00559dbe eb10                     jmp        0x559dd0
00559dc0 3d00030000               cmp        eax, 0x300
00559dc5 7404                     je         0x559dcb
00559dc7 33f6                     xor        esi, esi
00559dc9 eb05                     jmp        0x559dd0
00559dcb be000c0000               mov        esi, 0xc00
00559dd0 8bc2                     mov        eax, edx
00559dd2 c745f400000000           mov        dword ptr [ebp - 0xc], 0
00559dd9 83e001                   and        eax, 1
00559ddc 0f57c0                   xorps      xmm0, xmm0
00559ddf f7d8                     neg        eax
00559de1 660fd645ec               movq       qword ptr [ebp - 0x14], xmm0
00559de6 0f1145dc                 movups     xmmword ptr [ebp - 0x24], xmm0
00559dea 1bc0                     sbb        eax, eax
00559dec 83e020                   and        eax, 0x20
00559def 8bc8                     mov        ecx, eax
00559df1 8bc2                     mov        eax, edx
00559df3 83e002                   and        eax, 2
00559df6 f7d8                     neg        eax
00559df8 1bc0                     sbb        eax, eax
00559dfa 83e010                   and        eax, 0x10
00559dfd 0bc8                     or         ecx, eax
00559dff 8bc2                     mov        eax, edx
00559e01 83e004                   and        eax, 4
00559e04 f7d8                     neg        eax
00559e06 1bc0                     sbb        eax, eax
00559e08 83e008                   and        eax, 8
00559e0b 0bc8                     or         ecx, eax
00559e0d 8bc2                     mov        eax, edx
00559e0f 83e008                   and        eax, 8
00559e12 f7d8                     neg        eax
00559e14 1bc0                     sbb        eax, eax
00559e16 83e004                   and        eax, 4
00559e19 0bc8                     or         ecx, eax
00559e1b f7c210000000             test       edx, 0x10
00559e21 b800000000               mov        eax, 0
00559e26 0f95c0                   setne      al
00559e29 83e220                   and        edx, 0x20
00559e2c 0bc8                     or         ecx, eax
00559e2e f7da                     neg        edx
00559e30 1bd2                     sbb        edx, edx
00559e32 83e202                   and        edx, 2
00559e35 0bca                     or         ecx, edx
00559e37 0bf1                     or         esi, ecx
00559e39 0b75fc                   or         esi, dword ptr [ebp - 4]
00559e3c 0bf3                     or         esi, ebx
00559e3e d975dc                   fnstenv    [ebp - 0x24]
00559e41 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00559e44 81e63f1f0000             and        esi, 0x1f3f
00559e4a 81e1c0e0ffff             and        ecx, 0xffffe0c0
00559e50 0bce                     or         ecx, esi
00559e52 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00559e55 d965dc                   fldenv     [ebp - 0x24]
00559e58 8bc7                     mov        eax, edi
00559e5a 8bf7                     mov        esi, edi
00559e5c c1ee18                   shr        esi, 0x18
00559e5f 81e7000c0000             and        edi, 0xc00
00559e65 c1e816                   shr        eax, 0x16
00559e68 83e63f                   and        esi, 0x3f
00559e6b 2500030000               and        eax, 0x300
00559e70 0bf0                     or         esi, eax
00559e72 81ff00040000             cmp        edi, 0x400
00559e78 7421                     je         0x559e9b
00559e7a 81ff00080000             cmp        edi, 0x800
00559e80 7412                     je         0x559e94
00559e82 33c0                     xor        eax, eax
00559e84 bb00800000               mov        ebx, 0x8000
00559e89 81ff000c0000             cmp        edi, 0xc00
00559e8f 0f45d8                   cmovne     ebx, eax
00559e92 eb13                     jmp        0x559ea7
00559e94 bb40000000               mov        ebx, 0x40
00559e99 eb0c                     jmp        0x559ea7
00559e9b c745f840800000           mov        dword ptr [ebp - 8], 0x8040
00559ea2 bb40800000               mov        ebx, 0x8040
00559ea7 8bc6                     mov        eax, esi
00559ea9 2500030000               and        eax, 0x300
00559eae 3d00020000               cmp        eax, 0x200
00559eb3 771b                     ja         0x559ed0
00559eb5 7412                     je         0x559ec9
00559eb7 85c0                     test       eax, eax
00559eb9 741c                     je         0x559ed7
00559ebb 3d00010000               cmp        eax, 0x100
00559ec0 7515                     jne        0x559ed7
00559ec2 ba00200000               mov        edx, 0x2000
00559ec7 eb17                     jmp        0x559ee0
00559ec9 ba00400000               mov        edx, 0x4000
00559ece eb10                     jmp        0x559ee0
00559ed0 3d00030000               cmp        eax, 0x300
00559ed5 7404                     je         0x559edb
00559ed7 33d2                     xor        edx, edx
00559ed9 eb05                     jmp        0x559ee0
00559edb ba00600000               mov        edx, 0x6000
00559ee0 8bc6                     mov        eax, esi
00559ee2 83e001                   and        eax, 1
00559ee5 f7d8                     neg        eax
00559ee7 5f                       pop        edi
00559ee8 1bc0                     sbb        eax, eax
00559eea 2500100000               and        eax, 0x1000
00559eef 8bc8                     mov        ecx, eax
00559ef1 8bc6                     mov        eax, esi
00559ef3 83e002                   and        eax, 2
00559ef6 f7d8                     neg        eax
00559ef8 1bc0                     sbb        eax, eax
00559efa 2500080000               and        eax, 0x800
00559eff 0bc8                     or         ecx, eax
00559f01 8bc6                     mov        eax, esi
00559f03 83e004                   and        eax, 4
00559f06 f7d8                     neg        eax
00559f08 1bc0                     sbb        eax, eax
00559f0a 2500040000               and        eax, 0x400
00559f0f 0bc8                     or         ecx, eax
00559f11 8bc6                     mov        eax, esi
00559f13 83e008                   and        eax, 8
00559f16 f7d8                     neg        eax
00559f18 1bc0                     sbb        eax, eax
00559f1a 2500020000               and        eax, 0x200
00559f1f 0bc8                     or         ecx, eax
00559f21 8bc6                     mov        eax, esi
00559f23 83e010                   and        eax, 0x10
00559f26 f7d8                     neg        eax
00559f28 1bc0                     sbb        eax, eax
00559f2a 83e620                   and        esi, 0x20
00559f2d 2580000000               and        eax, 0x80
00559f32 0bc8                     or         ecx, eax
00559f34 f7de                     neg        esi
00559f36 1bf6                     sbb        esi, esi
00559f38 81e600010000             and        esi, 0x100
00559f3e 0bce                     or         ecx, esi
00559f40 0bd1                     or         edx, ecx
00559f42 0bd3                     or         edx, ebx
00559f44 833d88525e0001           cmp        dword ptr [0x5e5288], 1
00559f4b 5e                       pop        esi
00559f4c 5b                       pop        ebx
00559f4d 7c1f                     jl         0x559f6e
00559f4f 0fae5dfc                 stmxcsr    dword ptr [ebp - 4]
00559f53 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00559f56 81e2c0ff0000             and        edx, 0xffc0
00559f5c 81e13f00ffff             and        ecx, 0xffff003f
00559f62 0fb7c2                   movzx      eax, dx
00559f65 0bc8                     or         ecx, eax
00559f67 894dfc                   mov        dword ptr [ebp - 4], ecx
00559f6a 0fae55fc                 ldmxcsr    dword ptr [ebp - 4]
00559f6e 8be5                     mov        esp, ebp
00559f70 5d                       pop        ebp
00559f71 c3                       ret        
