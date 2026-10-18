00472d10 55                       push       ebp
00472d11 8bec                     mov        ebp, esp
00472d13 83ec3c                   sub        esp, 0x3c
00472d16 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00472d1b 33c5                     xor        eax, ebp
00472d1d 8945fc                   mov        dword ptr [ebp - 4], eax
00472d20 56                       push       esi
00472d21 57                       push       edi
00472d22 894dd8                   mov        dword ptr [ebp - 0x28], ecx
00472d25 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00472d28 8b8844330000             mov        ecx, dword ptr [eax + 0x3344]
00472d2e c1e903                   shr        ecx, 3
00472d31 83e101                   and        ecx, 1
00472d34 740a                     je         0x472d40
00472d36 b801000000               mov        eax, 1
00472d3b e9e7030000               jmp        0x473127
00472d40 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00472d43 8b8244330000             mov        eax, dword ptr [edx + 0x3344]
00472d49 c1e802                   shr        eax, 2
00472d4c 83e001                   and        eax, 1
00472d4f 7417                     je         0x472d68
00472d51 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00472d54 81c148330000             add        ecx, 0x3348
00472d5a e831d2f9ff               call       0x40ff90
00472d5f 83f83c                   cmp        eax, 0x3c
00472d62 0f8d50020000             jge        0x472fb8
00472d68 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00472d6e e84d28fdff               call       0x4455c0
00472d73 b96c010000               mov        ecx, 0x16c
00472d78 6bd103                   imul       edx, ecx, 3
00472d7b b808000000               mov        eax, 8
00472d80 d1e0                     shl        eax, 1
00472d82 b908000000               mov        ecx, 8
00472d87 6bc900                   imul       ecx, ecx, 0
00472d8a 8b75d8                   mov        esi, dword ptr [ebp - 0x28]
00472d8d 8b9402e4505c00           mov        edx, dword ptr [edx + eax + 0x5c50e4]
00472d94 89940e40030000           mov        dword ptr [esi + ecx + 0x340], edx
00472d9b b86c010000               mov        eax, 0x16c
00472da0 6bc803                   imul       ecx, eax, 3
00472da3 ba08000000               mov        edx, 8
00472da8 d1e2                     shl        edx, 1
00472daa b808000000               mov        eax, 8
00472daf 6bc000                   imul       eax, eax, 0
00472db2 8b75d8                   mov        esi, dword ptr [ebp - 0x28]
00472db5 8b8c11e8505c00           mov        ecx, dword ptr [ecx + edx + 0x5c50e8]
00472dbc 898c0644030000           mov        dword ptr [esi + eax + 0x344], ecx
00472dc3 8b75d8                   mov        esi, dword ptr [ebp - 0x28]
00472dc6 81c614020000             add        esi, 0x214
00472dcc ba6c010000               mov        edx, 0x16c
00472dd1 6bfa03                   imul       edi, edx, 3
00472dd4 81c7b84f5c00             add        edi, 0x5c4fb8
00472dda b95b000000               mov        ecx, 0x5b
00472ddf f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
00472de1 6a03                     push       3
00472de3 b9404d5c00               mov        ecx, 0x5c4d40
00472de8 e833470000               call       0x477520
00472ded b9404d5c00               mov        ecx, 0x5c4d40
00472df2 e8e9b10600               call       0x4ddfe0
00472df7 6a04                     push       4
00472df9 6a17                     push       0x17
00472dfb b9404d5c00               mov        ecx, 0x5c4d40
00472e00 e8ab6f0600               call       0x4d9db0
00472e05 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00472e08 8b887c030000             mov        ecx, dword ptr [eax + 0x37c]
00472e0e 51                       push       ecx
00472e0f 6a22                     push       0x22
00472e11 b9404d5c00               mov        ecx, 0x5c4d40
00472e16 e8956f0600               call       0x4d9db0
00472e1b 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00472e1e 8b8264030000             mov        eax, dword ptr [edx + 0x364]
00472e24 50                       push       eax
00472e25 6a24                     push       0x24
00472e27 b9404d5c00               mov        ecx, 0x5c4d40
00472e2c e87f6f0600               call       0x4d9db0
00472e31 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00472e34 8b9168030000             mov        edx, dword ptr [ecx + 0x368]
00472e3a 52                       push       edx
00472e3b 6a25                     push       0x25
00472e3d b9404d5c00               mov        ecx, 0x5c4d40
00472e42 e8696f0600               call       0x4d9db0
00472e47 90                       nop        
00472e48 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00472e4b 8b8844330000             mov        ecx, dword ptr [eax + 0x3344]
00472e51 c1e902                   shr        ecx, 2
00472e54 83e101                   and        ecx, 1
00472e57 7410                     je         0x472e69
00472e59 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00472e5c 83ba5c33000022           cmp        dword ptr [edx + 0x335c], 0x22
00472e63 0f8caf000000             jl         0x472f18
00472e69 b86c010000               mov        eax, 0x16c
00472e6e 6bc803                   imul       ecx, eax, 3
00472e71 8b9198505c00             mov        edx, dword ptr [ecx + 0x5c5098]
00472e77 8955ec                   mov        dword ptr [ebp - 0x14], edx
00472e7a b86c010000               mov        eax, 0x16c
00472e7f 6bc803                   imul       ecx, eax, 3
00472e82 8b919c505c00             mov        edx, dword ptr [ecx + 0x5c509c]
00472e88 8955f0                   mov        dword ptr [ebp - 0x10], edx
00472e8b b86c010000               mov        eax, 0x16c
00472e90 6bc803                   imul       ecx, eax, 3
00472e93 ba6c010000               mov        edx, 0x16c
00472e98 6bc203                   imul       eax, edx, 3
00472e9b 8b8998505c00             mov        ecx, dword ptr [ecx + 0x5c5098]
00472ea1 0388a0505c00             add        ecx, dword ptr [eax + 0x5c50a0]
00472ea7 894df4                   mov        dword ptr [ebp - 0xc], ecx
00472eaa ba6c010000               mov        edx, 0x16c
00472eaf 6bc203                   imul       eax, edx, 3
00472eb2 b96c010000               mov        ecx, 0x16c
00472eb7 6bd103                   imul       edx, ecx, 3
00472eba 8b809c505c00             mov        eax, dword ptr [eax + 0x5c509c]
00472ec0 0382a4505c00             add        eax, dword ptr [edx + 0x5c50a4]
00472ec6 8945f8                   mov        dword ptr [ebp - 8], eax
00472ec9 b9404d5c00               mov        ecx, 0x5c4d40
00472ece e85df8f9ff               call       0x412730
00472ed3 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00472ed6 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00472ed9 8b11                     mov        edx, dword ptr [ecx]
00472edb 8b82ac000000             mov        eax, dword ptr [edx + 0xac]
00472ee1 8945c8                   mov        dword ptr [ebp - 0x38], eax
00472ee4 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00472ee7 8b917c030000             mov        edx, dword ptr [ecx + 0x37c]
00472eed 8955cc                   mov        dword ptr [ebp - 0x34], edx
00472ef0 6a00                     push       0
00472ef2 51                       push       ecx
00472ef3 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00472efb f30f110424               movss      dword ptr [esp], xmm0
00472f00 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
00472f03 50                       push       eax
00472f04 6a03                     push       3
00472f06 8d4dec                   lea        ecx, [ebp - 0x14]
00472f09 51                       push       ecx
00472f0a 6a01                     push       1
00472f0c 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00472f0f 52                       push       edx
00472f10 ff55c8                   call       dword ptr [ebp - 0x38]
00472f13 e9a0000000               jmp        0x472fb8
00472f18 b86c010000               mov        eax, 0x16c
00472f1d 6bc803                   imul       ecx, eax, 3
00472f20 8b9198505c00             mov        edx, dword ptr [ecx + 0x5c5098]
00472f26 8955dc                   mov        dword ptr [ebp - 0x24], edx
00472f29 b86c010000               mov        eax, 0x16c
00472f2e 6bc803                   imul       ecx, eax, 3
00472f31 8b919c505c00             mov        edx, dword ptr [ecx + 0x5c509c]
00472f37 8955e0                   mov        dword ptr [ebp - 0x20], edx
00472f3a b86c010000               mov        eax, 0x16c
00472f3f 6bc803                   imul       ecx, eax, 3
00472f42 ba6c010000               mov        edx, 0x16c
00472f47 6bc203                   imul       eax, edx, 3
00472f4a 8b8998505c00             mov        ecx, dword ptr [ecx + 0x5c5098]
00472f50 0388a0505c00             add        ecx, dword ptr [eax + 0x5c50a0]
00472f56 894de4                   mov        dword ptr [ebp - 0x1c], ecx
00472f59 ba6c010000               mov        edx, 0x16c
00472f5e 6bc203                   imul       eax, edx, 3
00472f61 b96c010000               mov        ecx, 0x16c
00472f66 6bd103                   imul       edx, ecx, 3
00472f69 8b809c505c00             mov        eax, dword ptr [eax + 0x5c509c]
00472f6f 0382a4505c00             add        eax, dword ptr [edx + 0x5c50a4]
00472f75 8945e8                   mov        dword ptr [ebp - 0x18], eax
00472f78 b9404d5c00               mov        ecx, 0x5c4d40
00472f7d e8aef7f9ff               call       0x412730
00472f82 8945d0                   mov        dword ptr [ebp - 0x30], eax
00472f85 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00472f88 8b11                     mov        edx, dword ptr [ecx]
00472f8a 8b82ac000000             mov        eax, dword ptr [edx + 0xac]
00472f90 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00472f93 6a00                     push       0
00472f95 51                       push       ecx
00472f96 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00472f9e f30f110424               movss      dword ptr [esp], xmm0
00472fa3 68000000ff               push       0xff000000
00472fa8 6a03                     push       3
00472faa 8d4ddc                   lea        ecx, [ebp - 0x24]
00472fad 51                       push       ecx
00472fae 6a01                     push       1
00472fb0 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00472fb3 52                       push       edx
00472fb4 ff55c4                   call       dword ptr [ebp - 0x3c]
00472fb7 90                       nop        
00472fb8 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00472fbb 8b8844330000             mov        ecx, dword ptr [eax + 0x3344]
00472fc1 c1e902                   shr        ecx, 2
00472fc4 83e101                   and        ecx, 1
00472fc7 746e                     je         0x473037
00472fc9 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00472fcc 81c148330000             add        ecx, 0x3348
00472fd2 e8b9cff9ff               call       0x40ff90
00472fd7 83f81e                   cmp        eax, 0x1e
00472fda 7d3c                     jge        0x473018
00472fdc 6a0a                     push       0xa
00472fde 6a00                     push       0
00472fe0 6a00                     push       0
00472fe2 6a00                     push       0
00472fe4 6a1e                     push       0x1e
00472fe6 6a03                     push       3
00472fe8 e8d320fbff               call       0x4250c0
00472fed 83c418                   add        esp, 0x18
00472ff0 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00472ff3 8b8244330000             mov        eax, dword ptr [edx + 0x3344]
00472ff9 83c801                   or         eax, 1
00472ffc 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00472fff 898144330000             mov        dword ptr [ecx + 0x3344], eax
00473005 6a01                     push       1
00473007 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0047300a 81c148330000             add        ecx, 0x3348
00473010 e80b05fbff               call       0x423520
00473015 90                       nop        
00473016 eb1f                     jmp        0x473037
00473018 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
0047301b c6821f33000000           mov        byte ptr [edx + 0x331f], 0
00473022 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00473025 8b8844330000             mov        ecx, dword ptr [eax + 0x3344]
0047302b 83e1fe                   and        ecx, 0xfffffffe
0047302e 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00473031 898a44330000             mov        dword ptr [edx + 0x3344], ecx
00473037 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
0047303a 0fb6881f330000           movzx      ecx, byte ptr [eax + 0x331f]
00473041 85c9                     test       ecx, ecx
00473043 741f                     je         0x473064
00473045 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00473048 8b821c330000             mov        eax, dword ptr [edx + 0x331c]
0047304e 50                       push       eax
0047304f 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00473055 e8e6450000               call       0x477640
0047305a 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0047305d c6811f33000000           mov        byte ptr [ecx + 0x331f], 0
00473064 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00473067 c7823833000000000000     mov        dword ptr [edx + 0x3338], 0
00473071 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00473074 c7803c33000000000000     mov        dword ptr [eax + 0x333c], 0
0047307e 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00473081 c7814033000000000000     mov        dword ptr [ecx + 0x3340], 0
0047308b 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
0047308e 8b8244330000             mov        eax, dword ptr [edx + 0x3344]
00473094 83e001                   and        eax, 1
00473097 7466                     je         0x4730ff
00473099 b9404d5c00               mov        ecx, 0x5c4d40
0047309e e81daa0600               call       0x4ddac0
004730a3 6a00                     push       0
004730a5 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004730a8 e8d3170000               call       0x474880
004730ad 6a01                     push       1
004730af 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004730b2 e8c9170000               call       0x474880
004730b7 6a02                     push       2
004730b9 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004730bc e8bf170000               call       0x474880
004730c1 6a03                     push       3
004730c3 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004730c6 e8b5170000               call       0x474880
004730cb 6a04                     push       4
004730cd 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004730d0 e8ab170000               call       0x474880
004730d5 6a05                     push       5
004730d7 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004730da e8a1170000               call       0x474880
004730df 6a06                     push       6
004730e1 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004730e4 e897170000               call       0x474880
004730e9 6a07                     push       7
004730eb 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004730ee e88d170000               call       0x474880
004730f3 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004730f9 e8c224fdff               call       0x4455c0
004730fe 90                       nop        
004730ff 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00473105 e816200000               call       0x475120
0047310a b9404d5c00               mov        ecx, 0x5c4d40
0047310f e86cae0600               call       0x4ddf80
00473114 6a08                     push       8
00473116 6a17                     push       0x17
00473118 b9404d5c00               mov        ecx, 0x5c4d40
0047311d e88e6c0600               call       0x4d9db0
00473122 b801000000               mov        eax, 1
00473127 5f                       pop        edi
00473128 5e                       pop        esi
00473129 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047312c 33cd                     xor        ecx, ebp
0047312e e8bbf80c00               call       0x5429ee
00473133 8be5                     mov        esp, ebp
00473135 5d                       pop        ebp
00473136 c3                       ret        
00473137 cc                       int3       
00473138 cc                       int3       
00473139 cc                       int3       
0047313a cc                       int3       
0047313b cc                       int3       
0047313c cc                       int3       
0047313d cc                       int3       
0047313e cc                       int3       
0047313f cc                       int3       
