004d60a0 55                       push       ebp
004d60a1 8bec                     mov        ebp, esp
004d60a3 83ec2c                   sub        esp, 0x2c
004d60a6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004d60ab 33c5                     xor        eax, ebp
004d60ad 8945fc                   mov        dword ptr [ebp - 4], eax
004d60b0 56                       push       esi
004d60b1 57                       push       edi
004d60b2 894dec                   mov        dword ptr [ebp - 0x14], ecx
004d60b5 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d60b8 8b88d8060000             mov        ecx, dword ptr [eax + 0x6d8]
004d60be 894de8                   mov        dword ptr [ebp - 0x18], ecx
004d60c1 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
004d60c8 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d60cb 8b421c                   mov        eax, dword ptr [edx + 0x1c]
004d60ce 83c840                   or         eax, 0x40
004d60d1 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d60d4 89411c                   mov        dword ptr [ecx + 0x1c], eax
004d60d7 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d60da 8b4238                   mov        eax, dword ptr [edx + 0x38]
004d60dd 83e0f7                   and        eax, 0xfffffff7
004d60e0 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004d60e3 894138                   mov        dword ptr [ecx + 0x38], eax
004d60e6 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d60e9 837a2004                 cmp        dword ptr [edx + 0x20], 4
004d60ed 740d                     je         0x4d60fc
004d60ef 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d60f2 83782002                 cmp        dword ptr [eax + 0x20], 2
004d60f6 0f85cc010000             jne        0x4d62c8
004d60fc 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d60ff f30f104174               movss      xmm0, dword ptr [ecx + 0x74]
004d6104 0f2f0590cd5600           comiss     xmm0, dword ptr [0x56cd90]
004d610b 0f86b7010000             jbe        0x4d62c8
004d6111 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d6114 f30f104278               movss      xmm0, dword ptr [edx + 0x78]
004d6119 0f2f05d8c85600           comiss     xmm0, dword ptr [0x56c8d8]
004d6120 0f86a2010000             jbe        0x4d62c8
004d6126 8d4df0                   lea        ecx, [ebp - 0x10]
004d6129 e8e2ccf4ff               call       0x422e10
004d612e 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d6131 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
004d6139 0f2f4078                 comiss     xmm0, dword ptr [eax + 0x78]
004d613d 7617                     jbe        0x4d6156
004d613f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d6142 f30f104178               movss      xmm0, dword ptr [ecx + 0x78]
004d6147 f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
004d614f f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
004d6154 eb29                     jmp        0x4d617f
004d6156 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d6159 f30f104278               movss      xmm0, dword ptr [edx + 0x78]
004d615e f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
004d6166 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004d616e 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d6171 f30f104878               movss      xmm1, dword ptr [eax + 0x78]
004d6176 f30f5cc8                 subss      xmm1, xmm0
004d617a f30f114de0               movss      dword ptr [ebp - 0x20], xmm1
004d617f f30f1045e0               movss      xmm0, dword ptr [ebp - 0x20]
004d6184 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
004d6189 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d618c 8b912c070000             mov        edx, dword ptr [ecx + 0x72c]
004d6192 d1ea                     shr        edx, 1
004d6194 83e201                   and        edx, 1
004d6197 740f                     je         0x4d61a8
004d6199 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d619c f30f104074               movss      xmm0, dword ptr [eax + 0x74]
004d61a1 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
004d61a6 eb1d                     jmp        0x4d61c5
004d61a8 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d61ab f30f104174               movss      xmm0, dword ptr [ecx + 0x74]
004d61b0 f30f5905e0c85600         mulss      xmm0, dword ptr [0x56c8e0]
004d61b8 f30f5e05d4ed5600         divss      xmm0, dword ptr [0x56edd4]
004d61c0 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
004d61c5 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
004d61ca f30f1145d8               movss      dword ptr [ebp - 0x28], xmm0
004d61cf 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d61d2 f30f104274               movss      xmm0, dword ptr [edx + 0x74]
004d61d7 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004d61df 51                       push       ecx
004d61e0 f30f110424               movss      dword ptr [esp], xmm0
004d61e5 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d61e8 51                       push       ecx
004d61e9 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004d61ee f30f110424               movss      dword ptr [esp], xmm0
004d61f3 8d4df0                   lea        ecx, [ebp - 0x10]
004d61f6 51                       push       ecx
004d61f7 e83431f6ff               call       0x439330
004d61fc 83c40c                   add        esp, 0xc
004d61ff 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d6202 83c258                   add        edx, 0x58
004d6205 52                       push       edx
004d6206 8d4df0                   lea        ecx, [ebp - 0x10]
004d6209 e8d234f5ff               call       0x4296e0
004d620e 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004d6211 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004d6214 8908                     mov        dword ptr [eax], ecx
004d6216 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004d6219 895004                   mov        dword ptr [eax + 4], edx
004d621c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004d621f 894808                   mov        dword ptr [eax + 8], ecx
004d6222 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d6225 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d6228 8b4870                   mov        ecx, dword ptr [eax + 0x70]
004d622b 894a24                   mov        dword ptr [edx + 0x24], ecx
004d622e 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d6231 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d6234 8b4870                   mov        ecx, dword ptr [eax + 0x70]
004d6237 894a28                   mov        dword ptr [edx + 0x28], ecx
004d623a 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d623d 8b4238                   mov        eax, dword ptr [edx + 0x38]
004d6240 83e0fe                   and        eax, 0xfffffffe
004d6243 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004d6246 894138                   mov        dword ptr [ecx + 0x38], eax
004d6249 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d624c 8b4238                   mov        eax, dword ptr [edx + 0x38]
004d624f 83e0fd                   and        eax, 0xfffffffd
004d6252 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004d6255 894138                   mov        dword ptr [ecx + 0x38], eax
004d6258 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d625b 8b4238                   mov        eax, dword ptr [edx + 0x38]
004d625e 83c804                   or         eax, 4
004d6261 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004d6264 894138                   mov        dword ptr [ecx + 0x38], eax
004d6267 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d626a 8b4238                   mov        eax, dword ptr [edx + 0x38]
004d626d 83c808                   or         eax, 8
004d6270 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004d6273 894138                   mov        dword ptr [ecx + 0x38], eax
004d6276 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d6279 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d627c 8b487c                   mov        ecx, dword ptr [eax + 0x7c]
004d627f 894a2c                   mov        dword ptr [edx + 0x2c], ecx
004d6282 f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
004d6287 f30f5905ccc85600         mulss      xmm0, dword ptr [0x56c8cc]
004d628f 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d6292 f30f114218               movss      dword ptr [edx + 0x18], xmm0
004d6297 f30f1045d4               movss      xmm0, dword ptr [ebp - 0x2c]
004d629c f30f5905ccc85600         mulss      xmm0, dword ptr [0x56c8cc]
004d62a4 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004d62a7 f30f11401c               movss      dword ptr [eax + 0x1c], xmm0
004d62ac 837d0800                 cmp        dword ptr [ebp + 8], 0
004d62b0 740d                     je         0x4d62bf
004d62b2 b90f000000               mov        ecx, 0xf
004d62b7 8b75e8                   mov        esi, dword ptr [ebp - 0x18]
004d62ba 8b7d08                   mov        edi, dword ptr [ebp + 8]
004d62bd f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
004d62bf 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004d62c2 83c101                   add        ecx, 1
004d62c5 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004d62c8 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004d62cb 5f                       pop        edi
004d62cc 5e                       pop        esi
004d62cd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d62d0 33cd                     xor        ecx, ebp
004d62d2 e817c70600               call       0x5429ee
004d62d7 8be5                     mov        esp, ebp
004d62d9 5d                       pop        ebp
004d62da c20400                   ret        4
004d62dd cc                       int3       
004d62de cc                       int3       
004d62df cc                       int3       
