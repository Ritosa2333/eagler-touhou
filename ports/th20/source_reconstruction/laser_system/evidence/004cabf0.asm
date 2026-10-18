004cabf0 55                       push       ebp
004cabf1 8bec                     mov        ebp, esp
004cabf3 81ec78020000             sub        esp, 0x278
004cabf9 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004cabfe 33c5                     xor        eax, ebp
004cac00 8945fc                   mov        dword ptr [ebp - 4], eax
004cac03 898dd4fdffff             mov        dword ptr [ebp - 0x22c], ecx
004cac09 837d1400                 cmp        dword ptr [ebp + 0x14], 0
004cac0d 7416                     je         0x4cac25
004cac0f 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cac15 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004cac1c 7407                     je         0x4cac25
004cac1e 33c0                     xor        eax, eax
004cac20 e91e040000               jmp        0x4cb043
004cac25 c785ccfdffff00000000     mov        dword ptr [ebp - 0x234], 0
004cac2f c785d0fdffff00000000     mov        dword ptr [ebp - 0x230], 0
004cac39 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cac3f e8cc81f5ff               call       0x422e10
004cac44 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cac4a e8c181f5ff               call       0x422e10
004cac4f 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cac55 83c158                   add        ecx, 0x58
004cac58 8b11                     mov        edx, dword ptr [ecx]
004cac5a 8995d8fdffff             mov        dword ptr [ebp - 0x228], edx
004cac60 8b4104                   mov        eax, dword ptr [ecx + 4]
004cac63 8985dcfdffff             mov        dword ptr [ebp - 0x224], eax
004cac69 8b4908                   mov        ecx, dword ptr [ecx + 8]
004cac6c 898de0fdffff             mov        dword ptr [ebp - 0x220], ecx
004cac72 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cac7a f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cac82 f30f1185c8fdffff         movss      dword ptr [ebp - 0x238], xmm0
004cac8a 6800020000               push       0x200
004cac8f 6a00                     push       0
004cac91 8d95fcfdffff             lea        edx, [ebp - 0x204]
004cac97 52                       push       edx
004cac98 e8c39a0700               call       0x544760
004cac9d 83c40c                   add        esp, 0xc
004caca0 0f57c0                   xorps      xmm0, xmm0
004caca3 f30f1185ecfdffff         movss      dword ptr [ebp - 0x214], xmm0
004cacab f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cacb3 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cacbb 51                       push       ecx
004cacbc f30f110424               movss      dword ptr [esp], xmm0
004cacc1 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cacc7 51                       push       ecx
004cacc8 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004caccd f30f110424               movss      dword ptr [esp], xmm0
004cacd2 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cacd8 51                       push       ecx
004cacd9 e852e6f6ff               call       0x439330
004cacde 83c40c                   add        esp, 0xc
004cace1 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cace7 83c258                   add        edx, 0x58
004cacea 8995c0fdffff             mov        dword ptr [ebp - 0x240], edx
004cacf0 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004cacf6 50                       push       eax
004cacf7 8d8d88fdffff             lea        ecx, [ebp - 0x278]
004cacfd 51                       push       ecx
004cacfe 8b8dc0fdffff             mov        ecx, dword ptr [ebp - 0x240]
004cad04 e867e8f5ff               call       0x429570
004cad09 8b10                     mov        edx, dword ptr [eax]
004cad0b 8995f0fdffff             mov        dword ptr [ebp - 0x210], edx
004cad11 8b4804                   mov        ecx, dword ptr [eax + 4]
004cad14 898df4fdffff             mov        dword ptr [ebp - 0x20c], ecx
004cad1a 8b5008                   mov        edx, dword ptr [eax + 8]
004cad1d 8995f8fdffff             mov        dword ptr [ebp - 0x208], edx
004cad23 0f57c0                   xorps      xmm0, xmm0
004cad26 f30f1185f8fdffff         movss      dword ptr [ebp - 0x208], xmm0
004cad2e 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004cad34 50                       push       eax
004cad35 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cad3b e8a0e9f5ff               call       0x4296e0
004cad40 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004cad45 f30f59450c               mulss      xmm0, dword ptr [ebp + 0xc]
004cad4a f30f11450c               movss      dword ptr [ebp + 0xc], xmm0
004cad4f f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cad57 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cad5f f30f5885c8fdffff         addss      xmm0, dword ptr [ebp - 0x238]
004cad67 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cad6d f30f104974               movss      xmm1, dword ptr [ecx + 0x74]
004cad72 0f2fc8                   comiss     xmm1, xmm0
004cad75 0f868d020000             jbe        0x4cb008
004cad7b 8d95f0fdffff             lea        edx, [ebp - 0x210]
004cad81 52                       push       edx
004cad82 8b4508                   mov        eax, dword ptr [ebp + 8]
004cad85 50                       push       eax
004cad86 e845b3f8ff               call       0x4560d0
004cad8b 83c408                   add        esp, 8
004cad8e d99dbcfdffff             fstp       dword ptr [ebp - 0x244]
004cad94 f30f1085bcfdffff         movss      xmm0, dword ptr [ebp - 0x244]
004cad9c 0f2f450c                 comiss     xmm0, dword ptr [ebp + 0xc]
004cada0 760a                     jbe        0x4cadac
004cada2 e923020000               jmp        0x4cafca
004cada7 e91e020000               jmp        0x4cafca
004cadac 8b8dd0fdffff             mov        ecx, dword ptr [ebp - 0x230]
004cadb2 c6840dfcfdffff01         mov        byte ptr [ebp + ecx - 0x204], 1
004cadba 8b95ccfdffff             mov        edx, dword ptr [ebp - 0x234]
004cadc0 83c201                   add        edx, 1
004cadc3 8995ccfdffff             mov        dword ptr [ebp - 0x234], edx
004cadc9 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cadcf 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004cadd5 e82615f4ff               call       0x40c300
004cadda 05886d2800               add        eax, 0x286d88
004caddf 8985c4fdffff             mov        dword ptr [ebp - 0x23c], eax
004cade5 8b8dc4fdffff             mov        ecx, dword ptr [ebp - 0x23c]
004cadeb 8b11                     mov        edx, dword ptr [ecx]
004caded 83c201                   add        edx, 1
004cadf0 8b85c4fdffff             mov        eax, dword ptr [ebp - 0x23c]
004cadf6 8910                     mov        dword ptr [eax], edx
004cadf8 51                       push       ecx
004cadf9 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
004cae01 f30f110424               movss      dword ptr [esp], xmm0
004cae06 51                       push       ecx
004cae07 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
004cae0f f30f110424               movss      dword ptr [esp], xmm0
004cae14 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cae1a 51                       push       ecx
004cae1b e870a5fbff               call       0x485390
004cae20 83c40c                   add        esp, 0xc
004cae23 85c0                     test       eax, eax
004cae25 0f859f010000             jne        0x4cafca
004cae2b 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cae31 83bae406000011           cmp        dword ptr [edx + 0x6e4], 0x11
004cae38 7e1e                     jle        0x4cae58
004cae3a 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cae40 83b8e406000022           cmp        dword ptr [eax + 0x6e4], 0x22
004cae47 740f                     je         0x4cae58
004cae49 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cae4f 83b9e406000026           cmp        dword ptr [ecx + 0x6e4], 0x26
004cae56 756f                     jne        0x4caec7
004cae58 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cae5e 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004cae64 e89714f4ff               call       0x40c300
004cae69 8bc8                     mov        ecx, eax
004cae6b e840abfbff               call       0x4859b0
004cae70 8985b4fdffff             mov        dword ptr [ebp - 0x24c], eax
004cae76 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cae7c 8b8850070000             mov        ecx, dword ptr [eax + 0x750]
004cae82 8d9409d4000000           lea        edx, [ecx + ecx + 0xd4]
004cae89 8995b8fdffff             mov        dword ptr [ebp - 0x248], edx
004cae8f 6a00                     push       0
004cae91 6aff                     push       -1
004cae93 51                       push       ecx
004cae94 0f57c0                   xorps      xmm0, xmm0
004cae97 f30f110424               movss      dword ptr [esp], xmm0
004cae9c 8d85f0fdffff             lea        eax, [ebp - 0x210]
004caea2 50                       push       eax
004caea3 8b8db8fdffff             mov        ecx, dword ptr [ebp - 0x248]
004caea9 51                       push       ecx
004caeaa 6814fd5600               push       0x56fd14
004caeaf 8d959cfdffff             lea        edx, [ebp - 0x264]
004caeb5 52                       push       edx
004caeb6 8b8db4fdffff             mov        ecx, dword ptr [ebp - 0x24c]
004caebc e81fe2faff               call       0x4790e0
004caec1 90                       nop        
004caec2 e903010000               jmp        0x4cafca
004caec7 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004caecd 83b8e40600001f           cmp        dword ptr [eax + 0x6e4], 0x1f
004caed4 7e0f                     jle        0x4caee5
004caed6 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004caedc 83b9e40600001b           cmp        dword ptr [ecx + 0x6e4], 0x1b
004caee3 756c                     jne        0x4caf51
004caee5 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004caeeb 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004caef1 e80a14f4ff               call       0x40c300
004caef6 8bc8                     mov        ecx, eax
004caef8 e8b3aafbff               call       0x4859b0
004caefd 8985acfdffff             mov        dword ptr [ebp - 0x254], eax
004caf03 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004caf09 8b8850070000             mov        ecx, dword ptr [eax + 0x750]
004caf0f 8d940904010000           lea        edx, [ecx + ecx + 0x104]
004caf16 8995b0fdffff             mov        dword ptr [ebp - 0x250], edx
004caf1c 6a00                     push       0
004caf1e 6aff                     push       -1
004caf20 51                       push       ecx
004caf21 0f57c0                   xorps      xmm0, xmm0
004caf24 f30f110424               movss      dword ptr [esp], xmm0
004caf29 8d85f0fdffff             lea        eax, [ebp - 0x210]
004caf2f 50                       push       eax
004caf30 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004caf36 51                       push       ecx
004caf37 6814fd5600               push       0x56fd14
004caf3c 8d9598fdffff             lea        edx, [ebp - 0x268]
004caf42 52                       push       edx
004caf43 8b8dacfdffff             mov        ecx, dword ptr [ebp - 0x254]
004caf49 e892e1faff               call       0x4790e0
004caf4e 90                       nop        
004caf4f eb79                     jmp        0x4cafca
004caf51 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004caf57 83b8e406000021           cmp        dword ptr [eax + 0x6e4], 0x21
004caf5e 7f6a                     jg         0x4cafca
004caf60 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004caf66 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004caf6c e88f13f4ff               call       0x40c300
004caf71 8bc8                     mov        ecx, eax
004caf73 e838aafbff               call       0x4859b0
004caf78 8985a4fdffff             mov        dword ptr [ebp - 0x25c], eax
004caf7e 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004caf84 8b8250070000             mov        eax, dword ptr [edx + 0x750]
004caf8a 8d8c001c010000           lea        ecx, [eax + eax + 0x11c]
004caf91 898da8fdffff             mov        dword ptr [ebp - 0x258], ecx
004caf97 6a00                     push       0
004caf99 6aff                     push       -1
004caf9b 51                       push       ecx
004caf9c 0f57c0                   xorps      xmm0, xmm0
004caf9f f30f110424               movss      dword ptr [esp], xmm0
004cafa4 8d95f0fdffff             lea        edx, [ebp - 0x210]
004cafaa 52                       push       edx
004cafab 8b85a8fdffff             mov        eax, dword ptr [ebp - 0x258]
004cafb1 50                       push       eax
004cafb2 6814fd5600               push       0x56fd14
004cafb7 8d8d94fdffff             lea        ecx, [ebp - 0x26c]
004cafbd 51                       push       ecx
004cafbe 8b8da4fdffff             mov        ecx, dword ptr [ebp - 0x25c]
004cafc4 e817e1faff               call       0x4790e0
004cafc9 90                       nop        
004cafca 8d95e4fdffff             lea        edx, [ebp - 0x21c]
004cafd0 52                       push       edx
004cafd1 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cafd7 e804e7f5ff               call       0x4296e0
004cafdc f30f1085c8fdffff         movss      xmm0, dword ptr [ebp - 0x238]
004cafe4 f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
004cafec f30f1185c8fdffff         movss      dword ptr [ebp - 0x238], xmm0
004caff4 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004caffa 83c001                   add        eax, 1
004caffd 8985d0fdffff             mov        dword ptr [ebp - 0x230], eax
004cb003 e947fdffff               jmp        0x4cad4f
004cb008 83bdccfdffff00           cmp        dword ptr [ebp - 0x234], 0
004cb00f 742c                     je         0x4cb03d
004cb011 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cb017 8b11                     mov        edx, dword ptr [ecx]
004cb019 8b4238                   mov        eax, dword ptr [edx + 0x38]
004cb01c 8985a0fdffff             mov        dword ptr [ebp - 0x260], eax
004cb022 8b8dd0fdffff             mov        ecx, dword ptr [ebp - 0x230]
004cb028 51                       push       ecx
004cb029 8d95fcfdffff             lea        edx, [ebp - 0x204]
004cb02f 52                       push       edx
004cb030 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cb036 ff95a0fdffff             call       dword ptr [ebp - 0x260]
004cb03c 90                       nop        
004cb03d 8b85ccfdffff             mov        eax, dword ptr [ebp - 0x234]
004cb043 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cb046 33cd                     xor        ecx, ebp
004cb048 e8a1790700               call       0x5429ee
004cb04d 8be5                     mov        esp, ebp
004cb04f 5d                       pop        ebp
004cb050 c21000                   ret        0x10
004cb053 cc                       int3       
004cb054 cc                       int3       
004cb055 cc                       int3       
004cb056 cc                       int3       
004cb057 cc                       int3       
004cb058 cc                       int3       
004cb059 cc                       int3       
004cb05a cc                       int3       
004cb05b cc                       int3       
004cb05c cc                       int3       
004cb05d cc                       int3       
004cb05e cc                       int3       
004cb05f cc                       int3       
