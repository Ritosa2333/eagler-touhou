004cc950 55                       push       ebp
004cc951 8bec                     mov        ebp, esp
004cc953 81ec74020000             sub        esp, 0x274
004cc959 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004cc95e 33c5                     xor        eax, ebp
004cc960 8945fc                   mov        dword ptr [ebp - 4], eax
004cc963 898dd4fdffff             mov        dword ptr [ebp - 0x22c], ecx
004cc969 837d1c00                 cmp        dword ptr [ebp + 0x1c], 0
004cc96d 7416                     je         0x4cc985
004cc96f 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cc975 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004cc97c 7407                     je         0x4cc985
004cc97e 33c0                     xor        eax, eax
004cc980 e977040000               jmp        0x4ccdfc
004cc985 c785ccfdffff00000000     mov        dword ptr [ebp - 0x234], 0
004cc98f c785d0fdffff00000000     mov        dword ptr [ebp - 0x230], 0
004cc999 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cc99f e86c64f5ff               call       0x422e10
004cc9a4 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cc9aa e86164f5ff               call       0x422e10
004cc9af 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cc9b5 83c158                   add        ecx, 0x58
004cc9b8 8b11                     mov        edx, dword ptr [ecx]
004cc9ba 8995d8fdffff             mov        dword ptr [ebp - 0x228], edx
004cc9c0 8b4104                   mov        eax, dword ptr [ecx + 4]
004cc9c3 8985dcfdffff             mov        dword ptr [ebp - 0x224], eax
004cc9c9 8b4908                   mov        ecx, dword ptr [ecx + 8]
004cc9cc 898de0fdffff             mov        dword ptr [ebp - 0x220], ecx
004cc9d2 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cc9da f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cc9e2 f30f1185c8fdffff         movss      dword ptr [ebp - 0x238], xmm0
004cc9ea 6800020000               push       0x200
004cc9ef 6a00                     push       0
004cc9f1 8d95fcfdffff             lea        edx, [ebp - 0x204]
004cc9f7 52                       push       edx
004cc9f8 e8637d0700               call       0x544760
004cc9fd 83c40c                   add        esp, 0xc
004cca00 0f57c0                   xorps      xmm0, xmm0
004cca03 f30f1185ecfdffff         movss      dword ptr [ebp - 0x214], xmm0
004cca0b f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cca13 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cca1b 51                       push       ecx
004cca1c f30f110424               movss      dword ptr [esp], xmm0
004cca21 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cca27 51                       push       ecx
004cca28 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004cca2d f30f110424               movss      dword ptr [esp], xmm0
004cca32 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cca38 51                       push       ecx
004cca39 e8f2c8f6ff               call       0x439330
004cca3e 83c40c                   add        esp, 0xc
004cca41 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cca47 83c258                   add        edx, 0x58
004cca4a 8995c0fdffff             mov        dword ptr [ebp - 0x240], edx
004cca50 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004cca56 50                       push       eax
004cca57 8d8d8cfdffff             lea        ecx, [ebp - 0x274]
004cca5d 51                       push       ecx
004cca5e 8b8dc0fdffff             mov        ecx, dword ptr [ebp - 0x240]
004cca64 e807cbf5ff               call       0x429570
004cca69 8b10                     mov        edx, dword ptr [eax]
004cca6b 8995f0fdffff             mov        dword ptr [ebp - 0x210], edx
004cca71 8b4804                   mov        ecx, dword ptr [eax + 4]
004cca74 898df4fdffff             mov        dword ptr [ebp - 0x20c], ecx
004cca7a 8b5008                   mov        edx, dword ptr [eax + 8]
004cca7d 8995f8fdffff             mov        dword ptr [ebp - 0x208], edx
004cca83 0f57c0                   xorps      xmm0, xmm0
004cca86 f30f1185f8fdffff         movss      dword ptr [ebp - 0x208], xmm0
004cca8e 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004cca94 50                       push       eax
004cca95 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cca9b e840ccf5ff               call       0x4296e0
004ccaa0 90                       nop        
004ccaa1 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004ccaa9 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004ccab1 f30f5885c8fdffff         addss      xmm0, dword ptr [ebp - 0x238]
004ccab9 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ccabf f30f104974               movss      xmm1, dword ptr [ecx + 0x74]
004ccac4 0f2fc8                   comiss     xmm1, xmm0
004ccac7 0f86f4020000             jbe        0x4ccdc1
004ccacd 51                       push       ecx
004ccace f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004ccad3 f30f110424               movss      dword ptr [esp], xmm0
004ccad8 51                       push       ecx
004ccad9 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004ccade f30f110424               movss      dword ptr [esp], xmm0
004ccae3 51                       push       ecx
004ccae4 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004ccae9 f30f110424               movss      dword ptr [esp], xmm0
004ccaee 8b5508                   mov        edx, dword ptr [ebp + 8]
004ccaf1 51                       push       ecx
004ccaf2 f30f104204               movss      xmm0, dword ptr [edx + 4]
004ccaf7 f30f110424               movss      dword ptr [esp], xmm0
004ccafc 8b4508                   mov        eax, dword ptr [ebp + 8]
004ccaff 51                       push       ecx
004ccb00 f30f1000                 movss      xmm0, dword ptr [eax]
004ccb04 f30f110424               movss      dword ptr [esp], xmm0
004ccb09 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ccb0f 51                       push       ecx
004ccb10 f30f104170               movss      xmm0, dword ptr [ecx + 0x70]
004ccb15 f30f110424               movss      dword ptr [esp], xmm0
004ccb1a 51                       push       ecx
004ccb1b f30f1085e8fdffff         movss      xmm0, dword ptr [ebp - 0x218]
004ccb23 f30f110424               movss      dword ptr [esp], xmm0
004ccb28 51                       push       ecx
004ccb29 f30f1085e4fdffff         movss      xmm0, dword ptr [ebp - 0x21c]
004ccb31 f30f110424               movss      dword ptr [esp], xmm0
004ccb36 51                       push       ecx
004ccb37 f30f1085f4fdffff         movss      xmm0, dword ptr [ebp - 0x20c]
004ccb3f f30f110424               movss      dword ptr [esp], xmm0
004ccb44 51                       push       ecx
004ccb45 f30f1085f0fdffff         movss      xmm0, dword ptr [ebp - 0x210]
004ccb4d f30f110424               movss      dword ptr [esp], xmm0
004ccb52 e859adf8ff               call       0x4578b0
004ccb57 83c428                   add        esp, 0x28
004ccb5a 0fb6d0                   movzx      edx, al
004ccb5d 85d2                     test       edx, edx
004ccb5f 0f841e020000             je         0x4ccd83
004ccb65 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004ccb6b c68405fcfdffff01         mov        byte ptr [ebp + eax - 0x204], 1
004ccb73 8b8dccfdffff             mov        ecx, dword ptr [ebp - 0x234]
004ccb79 83c101                   add        ecx, 1
004ccb7c 898dccfdffff             mov        dword ptr [ebp - 0x234], ecx
004ccb82 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004ccb88 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004ccb8e e86df7f3ff               call       0x40c300
004ccb93 05886d2800               add        eax, 0x286d88
004ccb98 8985c4fdffff             mov        dword ptr [ebp - 0x23c], eax
004ccb9e 8b85c4fdffff             mov        eax, dword ptr [ebp - 0x23c]
004ccba4 8b08                     mov        ecx, dword ptr [eax]
004ccba6 83c101                   add        ecx, 1
004ccba9 8b95c4fdffff             mov        edx, dword ptr [ebp - 0x23c]
004ccbaf 890a                     mov        dword ptr [edx], ecx
004ccbb1 51                       push       ecx
004ccbb2 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
004ccbba f30f110424               movss      dword ptr [esp], xmm0
004ccbbf 51                       push       ecx
004ccbc0 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
004ccbc8 f30f110424               movss      dword ptr [esp], xmm0
004ccbcd 8d85f0fdffff             lea        eax, [ebp - 0x210]
004ccbd3 50                       push       eax
004ccbd4 e8b787fbff               call       0x485390
004ccbd9 83c40c                   add        esp, 0xc
004ccbdc 85c0                     test       eax, eax
004ccbde 0f859f010000             jne        0x4ccd83
004ccbe4 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ccbea 83b9e406000011           cmp        dword ptr [ecx + 0x6e4], 0x11
004ccbf1 7e1e                     jle        0x4ccc11
004ccbf3 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004ccbf9 83bae406000022           cmp        dword ptr [edx + 0x6e4], 0x22
004ccc00 740f                     je         0x4ccc11
004ccc02 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004ccc08 83b8e406000026           cmp        dword ptr [eax + 0x6e4], 0x26
004ccc0f 756f                     jne        0x4ccc80
004ccc11 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ccc17 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004ccc1d e8def6f3ff               call       0x40c300
004ccc22 8bc8                     mov        ecx, eax
004ccc24 e8878dfbff               call       0x4859b0
004ccc29 8985b8fdffff             mov        dword ptr [ebp - 0x248], eax
004ccc2f 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004ccc35 8b8250070000             mov        eax, dword ptr [edx + 0x750]
004ccc3b 8d8c00d4000000           lea        ecx, [eax + eax + 0xd4]
004ccc42 898dbcfdffff             mov        dword ptr [ebp - 0x244], ecx
004ccc48 6a00                     push       0
004ccc4a 6aff                     push       -1
004ccc4c 51                       push       ecx
004ccc4d 0f57c0                   xorps      xmm0, xmm0
004ccc50 f30f110424               movss      dword ptr [esp], xmm0
004ccc55 8d95f0fdffff             lea        edx, [ebp - 0x210]
004ccc5b 52                       push       edx
004ccc5c 8b85bcfdffff             mov        eax, dword ptr [ebp - 0x244]
004ccc62 50                       push       eax
004ccc63 6814fd5600               push       0x56fd14
004ccc68 8d8da0fdffff             lea        ecx, [ebp - 0x260]
004ccc6e 51                       push       ecx
004ccc6f 8b8db8fdffff             mov        ecx, dword ptr [ebp - 0x248]
004ccc75 e866c4faff               call       0x4790e0
004ccc7a 90                       nop        
004ccc7b e903010000               jmp        0x4ccd83
004ccc80 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004ccc86 83bae40600001f           cmp        dword ptr [edx + 0x6e4], 0x1f
004ccc8d 7e0f                     jle        0x4ccc9e
004ccc8f 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004ccc95 83b8e40600001b           cmp        dword ptr [eax + 0x6e4], 0x1b
004ccc9c 756c                     jne        0x4ccd0a
004ccc9e 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ccca4 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004cccaa e851f6f3ff               call       0x40c300
004cccaf 8bc8                     mov        ecx, eax
004cccb1 e8fa8cfbff               call       0x4859b0
004cccb6 8985b0fdffff             mov        dword ptr [ebp - 0x250], eax
004cccbc 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cccc2 8b8250070000             mov        eax, dword ptr [edx + 0x750]
004cccc8 8d8c0004010000           lea        ecx, [eax + eax + 0x104]
004ccccf 898db4fdffff             mov        dword ptr [ebp - 0x24c], ecx
004cccd5 6a00                     push       0
004cccd7 6aff                     push       -1
004cccd9 51                       push       ecx
004cccda 0f57c0                   xorps      xmm0, xmm0
004cccdd f30f110424               movss      dword ptr [esp], xmm0
004ccce2 8d95f0fdffff             lea        edx, [ebp - 0x210]
004ccce8 52                       push       edx
004ccce9 8b85b4fdffff             mov        eax, dword ptr [ebp - 0x24c]
004cccef 50                       push       eax
004cccf0 6814fd5600               push       0x56fd14
004cccf5 8d8d9cfdffff             lea        ecx, [ebp - 0x264]
004cccfb 51                       push       ecx
004cccfc 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004ccd02 e8d9c3faff               call       0x4790e0
004ccd07 90                       nop        
004ccd08 eb79                     jmp        0x4ccd83
004ccd0a 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004ccd10 83bae406000021           cmp        dword ptr [edx + 0x6e4], 0x21
004ccd17 7f6a                     jg         0x4ccd83
004ccd19 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004ccd1f 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004ccd25 e8d6f5f3ff               call       0x40c300
004ccd2a 8bc8                     mov        ecx, eax
004ccd2c e87f8cfbff               call       0x4859b0
004ccd31 8985a8fdffff             mov        dword ptr [ebp - 0x258], eax
004ccd37 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ccd3d 8b9150070000             mov        edx, dword ptr [ecx + 0x750]
004ccd43 8d84121c010000           lea        eax, [edx + edx + 0x11c]
004ccd4a 8985acfdffff             mov        dword ptr [ebp - 0x254], eax
004ccd50 6a00                     push       0
004ccd52 6aff                     push       -1
004ccd54 51                       push       ecx
004ccd55 0f57c0                   xorps      xmm0, xmm0
004ccd58 f30f110424               movss      dword ptr [esp], xmm0
004ccd5d 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004ccd63 51                       push       ecx
004ccd64 8b95acfdffff             mov        edx, dword ptr [ebp - 0x254]
004ccd6a 52                       push       edx
004ccd6b 6814fd5600               push       0x56fd14
004ccd70 8d8598fdffff             lea        eax, [ebp - 0x268]
004ccd76 50                       push       eax
004ccd77 8b8da8fdffff             mov        ecx, dword ptr [ebp - 0x258]
004ccd7d e85ec3faff               call       0x4790e0
004ccd82 90                       nop        
004ccd83 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004ccd89 51                       push       ecx
004ccd8a 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004ccd90 e84bc9f5ff               call       0x4296e0
004ccd95 f30f1085c8fdffff         movss      xmm0, dword ptr [ebp - 0x238]
004ccd9d f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
004ccda5 f30f1185c8fdffff         movss      dword ptr [ebp - 0x238], xmm0
004ccdad 8b95d0fdffff             mov        edx, dword ptr [ebp - 0x230]
004ccdb3 83c201                   add        edx, 1
004ccdb6 8995d0fdffff             mov        dword ptr [ebp - 0x230], edx
004ccdbc e9e0fcffff               jmp        0x4ccaa1
004ccdc1 83bdccfdffff00           cmp        dword ptr [ebp - 0x234], 0
004ccdc8 742c                     je         0x4ccdf6
004ccdca 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004ccdd0 8b08                     mov        ecx, dword ptr [eax]
004ccdd2 8b5138                   mov        edx, dword ptr [ecx + 0x38]
004ccdd5 8995a4fdffff             mov        dword ptr [ebp - 0x25c], edx
004ccddb 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004ccde1 50                       push       eax
004ccde2 8d8dfcfdffff             lea        ecx, [ebp - 0x204]
004ccde8 51                       push       ecx
004ccde9 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ccdef ff95a4fdffff             call       dword ptr [ebp - 0x25c]
004ccdf5 90                       nop        
004ccdf6 8b85ccfdffff             mov        eax, dword ptr [ebp - 0x234]
004ccdfc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ccdff 33cd                     xor        ecx, ebp
004cce01 e8e85b0700               call       0x5429ee
004cce06 8be5                     mov        esp, ebp
004cce08 5d                       pop        ebp
004cce09 c21800                   ret        0x18
004cce0c cc                       int3       
004cce0d cc                       int3       
004cce0e cc                       int3       
004cce0f cc                       int3       
