004cce10 55                       push       ebp
004cce11 8bec                     mov        ebp, esp
004cce13 81ec74020000             sub        esp, 0x274
004cce19 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004cce1e 33c5                     xor        eax, ebp
004cce20 8945fc                   mov        dword ptr [ebp - 4], eax
004cce23 898dd4fdffff             mov        dword ptr [ebp - 0x22c], ecx
004cce29 837d1c00                 cmp        dword ptr [ebp + 0x1c], 0
004cce2d 7416                     je         0x4cce45
004cce2f 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cce35 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004cce3c 7407                     je         0x4cce45
004cce3e 33c0                     xor        eax, eax
004cce40 e972040000               jmp        0x4cd2b7
004cce45 c785d0fdffff00000000     mov        dword ptr [ebp - 0x230], 0
004cce4f c785ccfdffff00000000     mov        dword ptr [ebp - 0x234], 0
004cce59 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cce5f e8ac5ff5ff               call       0x422e10
004cce64 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cce6a e8a15ff5ff               call       0x422e10
004cce6f 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cce75 83c158                   add        ecx, 0x58
004cce78 8b11                     mov        edx, dword ptr [ecx]
004cce7a 8995d8fdffff             mov        dword ptr [ebp - 0x228], edx
004cce80 8b4104                   mov        eax, dword ptr [ecx + 4]
004cce83 8985dcfdffff             mov        dword ptr [ebp - 0x224], eax
004cce89 8b4908                   mov        ecx, dword ptr [ecx + 8]
004cce8c 898de0fdffff             mov        dword ptr [ebp - 0x220], ecx
004cce92 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cce9a f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004ccea2 f30f1185c8fdffff         movss      dword ptr [ebp - 0x238], xmm0
004cceaa 6800020000               push       0x200
004cceaf 6a00                     push       0
004cceb1 8d95fcfdffff             lea        edx, [ebp - 0x204]
004cceb7 52                       push       edx
004cceb8 e8a3780700               call       0x544760
004ccebd 83c40c                   add        esp, 0xc
004ccec0 0f57c0                   xorps      xmm0, xmm0
004ccec3 f30f1185ecfdffff         movss      dword ptr [ebp - 0x214], xmm0
004ccecb f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cced3 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004ccedb 51                       push       ecx
004ccedc f30f110424               movss      dword ptr [esp], xmm0
004ccee1 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004ccee7 51                       push       ecx
004ccee8 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004cceed f30f110424               movss      dword ptr [esp], xmm0
004ccef2 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004ccef8 51                       push       ecx
004ccef9 e832c4f6ff               call       0x439330
004ccefe 83c40c                   add        esp, 0xc
004ccf01 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004ccf07 83c258                   add        edx, 0x58
004ccf0a 8995c0fdffff             mov        dword ptr [ebp - 0x240], edx
004ccf10 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004ccf16 50                       push       eax
004ccf17 8d8d8cfdffff             lea        ecx, [ebp - 0x274]
004ccf1d 51                       push       ecx
004ccf1e 8b8dc0fdffff             mov        ecx, dword ptr [ebp - 0x240]
004ccf24 e847c6f5ff               call       0x429570
004ccf29 8b10                     mov        edx, dword ptr [eax]
004ccf2b 8995f0fdffff             mov        dword ptr [ebp - 0x210], edx
004ccf31 8b4804                   mov        ecx, dword ptr [eax + 4]
004ccf34 898df4fdffff             mov        dword ptr [ebp - 0x20c], ecx
004ccf3a 8b5008                   mov        edx, dword ptr [eax + 8]
004ccf3d 8995f8fdffff             mov        dword ptr [ebp - 0x208], edx
004ccf43 0f57c0                   xorps      xmm0, xmm0
004ccf46 f30f1185f8fdffff         movss      dword ptr [ebp - 0x208], xmm0
004ccf4e 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004ccf54 50                       push       eax
004ccf55 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004ccf5b e880c7f5ff               call       0x4296e0
004ccf60 90                       nop        
004ccf61 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004ccf69 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004ccf71 f30f5885c8fdffff         addss      xmm0, dword ptr [ebp - 0x238]
004ccf79 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ccf7f f30f104974               movss      xmm1, dword ptr [ecx + 0x74]
004ccf84 0f2fc8                   comiss     xmm1, xmm0
004ccf87 0f82c1020000             jb         0x4cd24e
004ccf8d 51                       push       ecx
004ccf8e f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004ccf93 f30f110424               movss      dword ptr [esp], xmm0
004ccf98 51                       push       ecx
004ccf99 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004ccf9e f30f110424               movss      dword ptr [esp], xmm0
004ccfa3 51                       push       ecx
004ccfa4 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004ccfa9 f30f110424               movss      dword ptr [esp], xmm0
004ccfae 8b5508                   mov        edx, dword ptr [ebp + 8]
004ccfb1 51                       push       ecx
004ccfb2 f30f104204               movss      xmm0, dword ptr [edx + 4]
004ccfb7 f30f110424               movss      dword ptr [esp], xmm0
004ccfbc 8b4508                   mov        eax, dword ptr [ebp + 8]
004ccfbf 51                       push       ecx
004ccfc0 f30f1000                 movss      xmm0, dword ptr [eax]
004ccfc4 f30f110424               movss      dword ptr [esp], xmm0
004ccfc9 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ccfcf 51                       push       ecx
004ccfd0 f30f104170               movss      xmm0, dword ptr [ecx + 0x70]
004ccfd5 f30f110424               movss      dword ptr [esp], xmm0
004ccfda 51                       push       ecx
004ccfdb f30f1085e8fdffff         movss      xmm0, dword ptr [ebp - 0x218]
004ccfe3 f30f110424               movss      dword ptr [esp], xmm0
004ccfe8 51                       push       ecx
004ccfe9 f30f1085e4fdffff         movss      xmm0, dword ptr [ebp - 0x21c]
004ccff1 f30f110424               movss      dword ptr [esp], xmm0
004ccff6 51                       push       ecx
004ccff7 f30f1085f4fdffff         movss      xmm0, dword ptr [ebp - 0x20c]
004ccfff f30f110424               movss      dword ptr [esp], xmm0
004cd004 51                       push       ecx
004cd005 f30f1085f0fdffff         movss      xmm0, dword ptr [ebp - 0x210]
004cd00d f30f110424               movss      dword ptr [esp], xmm0
004cd012 e899a8f8ff               call       0x4578b0
004cd017 83c428                   add        esp, 0x28
004cd01a 0fb6d0                   movzx      edx, al
004cd01d 85d2                     test       edx, edx
004cd01f 0f84eb010000             je         0x4cd210
004cd025 8b85ccfdffff             mov        eax, dword ptr [ebp - 0x234]
004cd02b c68405fcfdffff01         mov        byte ptr [ebp + eax - 0x204], 1
004cd033 8b8dd0fdffff             mov        ecx, dword ptr [ebp - 0x230]
004cd039 83c101                   add        ecx, 1
004cd03c 898dd0fdffff             mov        dword ptr [ebp - 0x230], ecx
004cd042 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cd048 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004cd04e e8adf2f3ff               call       0x40c300
004cd053 05886d2800               add        eax, 0x286d88
004cd058 8985c4fdffff             mov        dword ptr [ebp - 0x23c], eax
004cd05e 8b85c4fdffff             mov        eax, dword ptr [ebp - 0x23c]
004cd064 8b08                     mov        ecx, dword ptr [eax]
004cd066 83c101                   add        ecx, 1
004cd069 8b95c4fdffff             mov        edx, dword ptr [ebp - 0x23c]
004cd06f 890a                     mov        dword ptr [edx], ecx
004cd071 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cd077 83b8e406000011           cmp        dword ptr [eax + 0x6e4], 0x11
004cd07e 7e1e                     jle        0x4cd09e
004cd080 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cd086 83b9e406000022           cmp        dword ptr [ecx + 0x6e4], 0x22
004cd08d 740f                     je         0x4cd09e
004cd08f 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cd095 83bae406000026           cmp        dword ptr [edx + 0x6e4], 0x26
004cd09c 756f                     jne        0x4cd10d
004cd09e 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cd0a4 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004cd0aa e851f2f3ff               call       0x40c300
004cd0af 8bc8                     mov        ecx, eax
004cd0b1 e8fa88fbff               call       0x4859b0
004cd0b6 8985b8fdffff             mov        dword ptr [ebp - 0x248], eax
004cd0bc 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cd0c2 8b9120070000             mov        edx, dword ptr [ecx + 0x720]
004cd0c8 8d8412d4000000           lea        eax, [edx + edx + 0xd4]
004cd0cf 8985bcfdffff             mov        dword ptr [ebp - 0x244], eax
004cd0d5 6a00                     push       0
004cd0d7 6aff                     push       -1
004cd0d9 51                       push       ecx
004cd0da 0f57c0                   xorps      xmm0, xmm0
004cd0dd f30f110424               movss      dword ptr [esp], xmm0
004cd0e2 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cd0e8 51                       push       ecx
004cd0e9 8b95bcfdffff             mov        edx, dword ptr [ebp - 0x244]
004cd0ef 52                       push       edx
004cd0f0 6814fd5600               push       0x56fd14
004cd0f5 8d85a0fdffff             lea        eax, [ebp - 0x260]
004cd0fb 50                       push       eax
004cd0fc 8b8db8fdffff             mov        ecx, dword ptr [ebp - 0x248]
004cd102 e8d9bffaff               call       0x4790e0
004cd107 90                       nop        
004cd108 e903010000               jmp        0x4cd210
004cd10d 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cd113 83b9e40600001f           cmp        dword ptr [ecx + 0x6e4], 0x1f
004cd11a 7e0f                     jle        0x4cd12b
004cd11c 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cd122 83bae40600001b           cmp        dword ptr [edx + 0x6e4], 0x1b
004cd129 756c                     jne        0x4cd197
004cd12b 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cd131 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004cd137 e8c4f1f3ff               call       0x40c300
004cd13c 8bc8                     mov        ecx, eax
004cd13e e86d88fbff               call       0x4859b0
004cd143 8985b0fdffff             mov        dword ptr [ebp - 0x250], eax
004cd149 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cd14f 8b9120070000             mov        edx, dword ptr [ecx + 0x720]
004cd155 8d841204010000           lea        eax, [edx + edx + 0x104]
004cd15c 8985b4fdffff             mov        dword ptr [ebp - 0x24c], eax
004cd162 6a00                     push       0
004cd164 6aff                     push       -1
004cd166 51                       push       ecx
004cd167 0f57c0                   xorps      xmm0, xmm0
004cd16a f30f110424               movss      dword ptr [esp], xmm0
004cd16f 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cd175 51                       push       ecx
004cd176 8b95b4fdffff             mov        edx, dword ptr [ebp - 0x24c]
004cd17c 52                       push       edx
004cd17d 6814fd5600               push       0x56fd14
004cd182 8d859cfdffff             lea        eax, [ebp - 0x264]
004cd188 50                       push       eax
004cd189 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004cd18f e84cbffaff               call       0x4790e0
004cd194 90                       nop        
004cd195 eb79                     jmp        0x4cd210
004cd197 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cd19d 83b9e406000021           cmp        dword ptr [ecx + 0x6e4], 0x21
004cd1a4 7f6a                     jg         0x4cd210
004cd1a6 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cd1ac 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004cd1b2 e849f1f3ff               call       0x40c300
004cd1b7 8bc8                     mov        ecx, eax
004cd1b9 e8f287fbff               call       0x4859b0
004cd1be 8985a8fdffff             mov        dword ptr [ebp - 0x258], eax
004cd1c4 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cd1ca 8b8820070000             mov        ecx, dword ptr [eax + 0x720]
004cd1d0 8d94091c010000           lea        edx, [ecx + ecx + 0x11c]
004cd1d7 8995acfdffff             mov        dword ptr [ebp - 0x254], edx
004cd1dd 6a00                     push       0
004cd1df 6aff                     push       -1
004cd1e1 51                       push       ecx
004cd1e2 0f57c0                   xorps      xmm0, xmm0
004cd1e5 f30f110424               movss      dword ptr [esp], xmm0
004cd1ea 8d85f0fdffff             lea        eax, [ebp - 0x210]
004cd1f0 50                       push       eax
004cd1f1 8b8dacfdffff             mov        ecx, dword ptr [ebp - 0x254]
004cd1f7 51                       push       ecx
004cd1f8 6814fd5600               push       0x56fd14
004cd1fd 8d9598fdffff             lea        edx, [ebp - 0x268]
004cd203 52                       push       edx
004cd204 8b8da8fdffff             mov        ecx, dword ptr [ebp - 0x258]
004cd20a e8d1befaff               call       0x4790e0
004cd20f 90                       nop        
004cd210 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004cd216 50                       push       eax
004cd217 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cd21d e8bec4f5ff               call       0x4296e0
004cd222 f30f1085c8fdffff         movss      xmm0, dword ptr [ebp - 0x238]
004cd22a f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
004cd232 f30f1185c8fdffff         movss      dword ptr [ebp - 0x238], xmm0
004cd23a 8b8dccfdffff             mov        ecx, dword ptr [ebp - 0x234]
004cd240 83c101                   add        ecx, 1
004cd243 898dccfdffff             mov        dword ptr [ebp - 0x234], ecx
004cd249 e913fdffff               jmp        0x4ccf61
004cd24e 83bdd0fdffff00           cmp        dword ptr [ebp - 0x230], 0
004cd255 745a                     je         0x4cd2b1
004cd257 8b95d0fdffff             mov        edx, dword ptr [ebp - 0x230]
004cd25d 3b95ccfdffff             cmp        edx, dword ptr [ebp - 0x234]
004cd263 7c20                     jl         0x4cd285
004cd265 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cd26b 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004cd26e 83e1f9                   and        ecx, 0xfffffff9
004cd271 83c902                   or         ecx, 2
004cd274 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cd27a 894a1c                   mov        dword ptr [edx + 0x1c], ecx
004cd27d 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004cd283 eb32                     jmp        0x4cd2b7
004cd285 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cd28b 8b08                     mov        ecx, dword ptr [eax]
004cd28d 8b5138                   mov        edx, dword ptr [ecx + 0x38]
004cd290 8995a4fdffff             mov        dword ptr [ebp - 0x25c], edx
004cd296 8b85ccfdffff             mov        eax, dword ptr [ebp - 0x234]
004cd29c 50                       push       eax
004cd29d 8d8dfcfdffff             lea        ecx, [ebp - 0x204]
004cd2a3 51                       push       ecx
004cd2a4 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cd2aa ff95a4fdffff             call       dword ptr [ebp - 0x25c]
004cd2b0 90                       nop        
004cd2b1 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004cd2b7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cd2ba 33cd                     xor        ecx, ebp
004cd2bc e82d570700               call       0x5429ee
004cd2c1 8be5                     mov        esp, ebp
004cd2c3 5d                       pop        ebp
004cd2c4 c21800                   ret        0x18
004cd2c7 cc                       int3       
004cd2c8 cc                       int3       
004cd2c9 cc                       int3       
004cd2ca cc                       int3       
004cd2cb cc                       int3       
004cd2cc cc                       int3       
004cd2cd cc                       int3       
004cd2ce cc                       int3       
004cd2cf cc                       int3       
