004cb970 55                       push       ebp
004cb971 8bec                     mov        ebp, esp
004cb973 81ecb8020000             sub        esp, 0x2b8
004cb979 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004cb97e 33c5                     xor        eax, ebp
004cb980 8945fc                   mov        dword ptr [ebp - 4], eax
004cb983 898db0fdffff             mov        dword ptr [ebp - 0x250], ecx
004cb989 837d1800                 cmp        dword ptr [ebp + 0x18], 0
004cb98d 7416                     je         0x4cb9a5
004cb98f 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cb995 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004cb99c 7407                     je         0x4cb9a5
004cb99e 33c0                     xor        eax, eax
004cb9a0 e9dd050000               jmp        0x4cbf82
004cb9a5 c785a0fdffff00000000     mov        dword ptr [ebp - 0x260], 0
004cb9af c785a4fdffff00000000     mov        dword ptr [ebp - 0x25c], 0
004cb9b9 8d8dd8fdffff             lea        ecx, [ebp - 0x228]
004cb9bf e84c74f5ff               call       0x422e10
004cb9c4 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cb9ca e84174f5ff               call       0x422e10
004cb9cf 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cb9d5 e83674f5ff               call       0x422e10
004cb9da 8d8dccfdffff             lea        ecx, [ebp - 0x234]
004cb9e0 e82b74f5ff               call       0x422e10
004cb9e5 8d8dc0fdffff             lea        ecx, [ebp - 0x240]
004cb9eb e82074f5ff               call       0x422e10
004cb9f0 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004cb9f6 83c158                   add        ecx, 0x58
004cb9f9 8b11                     mov        edx, dword ptr [ecx]
004cb9fb 8995b4fdffff             mov        dword ptr [ebp - 0x24c], edx
004cba01 8b4104                   mov        eax, dword ptr [ecx + 4]
004cba04 8985b8fdffff             mov        dword ptr [ebp - 0x248], eax
004cba0a 8b4908                   mov        ecx, dword ptr [ecx + 8]
004cba0d 898dbcfdffff             mov        dword ptr [ebp - 0x244], ecx
004cba13 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cba1b f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cba23 f30f1185a8fdffff         movss      dword ptr [ebp - 0x258], xmm0
004cba2b 6800020000               push       0x200
004cba30 6a00                     push       0
004cba32 8d95fcfdffff             lea        edx, [ebp - 0x204]
004cba38 52                       push       edx
004cba39 e8228d0700               call       0x544760
004cba3e 83c40c                   add        esp, 0xc
004cba41 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cba47 83c058                   add        eax, 0x58
004cba4a 898598fdffff             mov        dword ptr [ebp - 0x268], eax
004cba50 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004cba53 51                       push       ecx
004cba54 8d9560fdffff             lea        edx, [ebp - 0x2a0]
004cba5a 52                       push       edx
004cba5b 8b8d98fdffff             mov        ecx, dword ptr [ebp - 0x268]
004cba61 e8dad9f5ff               call       0x429440
004cba66 8b08                     mov        ecx, dword ptr [eax]
004cba68 898df0fdffff             mov        dword ptr [ebp - 0x210], ecx
004cba6e 8b5004                   mov        edx, dword ptr [eax + 4]
004cba71 8995f4fdffff             mov        dword ptr [ebp - 0x20c], edx
004cba77 8b4008                   mov        eax, dword ptr [eax + 8]
004cba7a 8985f8fdffff             mov        dword ptr [ebp - 0x208], eax
004cba80 0f57c0                   xorps      xmm0, xmm0
004cba83 f30f1185f8fdffff         movss      dword ptr [ebp - 0x208], xmm0
004cba8b 51                       push       ecx
004cba8c f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004cba91 f30f110424               movss      dword ptr [esp], xmm0
004cba96 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cba9c 51                       push       ecx
004cba9d 8d95f0fdffff             lea        edx, [ebp - 0x210]
004cbaa3 52                       push       edx
004cbaa4 e8f7d4f8ff               call       0x458fa0
004cbaa9 83c40c                   add        esp, 0xc
004cbaac f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cbab4 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cbabc 51                       push       ecx
004cbabd f30f110424               movss      dword ptr [esp], xmm0
004cbac2 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cbac8 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004cbacd f30f584510               addss      xmm0, dword ptr [ebp + 0x10]
004cbad2 51                       push       ecx
004cbad3 f30f110424               movss      dword ptr [esp], xmm0
004cbad8 e863caf6ff               call       0x438540
004cbadd d91c24                   fstp       dword ptr [esp]
004cbae0 8d8dc0fdffff             lea        ecx, [ebp - 0x240]
004cbae6 51                       push       ecx
004cbae7 e844d8f6ff               call       0x439330
004cbaec 83c40c                   add        esp, 0xc
004cbaef 0f57c0                   xorps      xmm0, xmm0
004cbaf2 f30f1185c8fdffff         movss      dword ptr [ebp - 0x238], xmm0
004cbafa 51                       push       ecx
004cbafb f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
004cbb03 f30f110424               movss      dword ptr [esp], xmm0
004cbb08 8d9554fdffff             lea        edx, [ebp - 0x2ac]
004cbb0e 52                       push       edx
004cbb0f 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004cbb12 e8d974f8ff               call       0x452ff0
004cbb17 8b08                     mov        ecx, dword ptr [eax]
004cbb19 898de4fdffff             mov        dword ptr [ebp - 0x21c], ecx
004cbb1f 8b5004                   mov        edx, dword ptr [eax + 4]
004cbb22 8995e8fdffff             mov        dword ptr [ebp - 0x218], edx
004cbb28 8b4008                   mov        eax, dword ptr [eax + 8]
004cbb2b 8985ecfdffff             mov        dword ptr [ebp - 0x214], eax
004cbb31 8d8dc0fdffff             lea        ecx, [ebp - 0x240]
004cbb37 51                       push       ecx
004cbb38 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cbb3e e89ddbf5ff               call       0x4296e0
004cbb43 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cbb4b f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cbb53 51                       push       ecx
004cbb54 f30f110424               movss      dword ptr [esp], xmm0
004cbb59 8b95b0fdffff             mov        edx, dword ptr [ebp - 0x250]
004cbb5f 51                       push       ecx
004cbb60 f30f104270               movss      xmm0, dword ptr [edx + 0x70]
004cbb65 f30f110424               movss      dword ptr [esp], xmm0
004cbb6a 8d85ccfdffff             lea        eax, [ebp - 0x234]
004cbb70 50                       push       eax
004cbb71 e8bad7f6ff               call       0x439330
004cbb76 83c40c                   add        esp, 0xc
004cbb79 0f57c0                   xorps      xmm0, xmm0
004cbb7c f30f1185d4fdffff         movss      dword ptr [ebp - 0x22c], xmm0
004cbb84 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004cbb8a 83c158                   add        ecx, 0x58
004cbb8d 898d94fdffff             mov        dword ptr [ebp - 0x26c], ecx
004cbb93 8d95ccfdffff             lea        edx, [ebp - 0x234]
004cbb99 52                       push       edx
004cbb9a 8d8548fdffff             lea        eax, [ebp - 0x2b8]
004cbba0 50                       push       eax
004cbba1 8b8d94fdffff             mov        ecx, dword ptr [ebp - 0x26c]
004cbba7 e8c4d9f5ff               call       0x429570
004cbbac 8b08                     mov        ecx, dword ptr [eax]
004cbbae 898dd8fdffff             mov        dword ptr [ebp - 0x228], ecx
004cbbb4 8b5004                   mov        edx, dword ptr [eax + 4]
004cbbb7 8995dcfdffff             mov        dword ptr [ebp - 0x224], edx
004cbbbd 8b4008                   mov        eax, dword ptr [eax + 8]
004cbbc0 8985e0fdffff             mov        dword ptr [ebp - 0x220], eax
004cbbc6 0f57c0                   xorps      xmm0, xmm0
004cbbc9 f30f1185e0fdffff         movss      dword ptr [ebp - 0x220], xmm0
004cbbd1 8d8dccfdffff             lea        ecx, [ebp - 0x234]
004cbbd7 51                       push       ecx
004cbbd8 8d8dccfdffff             lea        ecx, [ebp - 0x234]
004cbbde e8fddaf5ff               call       0x4296e0
004cbbe3 90                       nop        
004cbbe4 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cbbec f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cbbf4 f30f5885a8fdffff         addss      xmm0, dword ptr [ebp - 0x258]
004cbbfc 8b95b0fdffff             mov        edx, dword ptr [ebp - 0x250]
004cbc02 f30f104a74               movss      xmm1, dword ptr [edx + 0x74]
004cbc07 0f2fc8                   comiss     xmm1, xmm0
004cbc0a 0f826c030000             jb         0x4cbf7c
004cbc10 8b451c                   mov        eax, dword ptr [ebp + 0x1c]
004cbc13 833800                   cmp        dword ptr [eax], 0
004cbc16 0f85a0010000             jne        0x4cbdbc
004cbc1c 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004cbc22 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004cbc28 e8036bf4ff               call       0x412730
004cbc2d 898590fdffff             mov        dword ptr [ebp - 0x270], eax
004cbc33 6a00                     push       0
004cbc35 8b8d90fdffff             mov        ecx, dword ptr [ebp - 0x270]
004cbc3b e8209afbff               call       0x485660
004cbc40 89858cfdffff             mov        dword ptr [ebp - 0x274], eax
004cbc46 6a00                     push       0
004cbc48 8b8d8cfdffff             mov        ecx, dword ptr [ebp - 0x274]
004cbc4e e82daf0000               call       0x4d6b80
004cbc53 8bc8                     mov        ecx, eax
004cbc55 e876c0f6ff               call       0x437cd0
004cbc5a f30f104048               movss      xmm0, dword ptr [eax + 0x48]
004cbc5f f30f590588175700         mulss      xmm0, dword ptr [0x571788]
004cbc67 f30f118578fdffff         movss      dword ptr [ebp - 0x288], xmm0
004cbc6f 8b95b0fdffff             mov        edx, dword ptr [ebp - 0x250]
004cbc75 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004cbc7b e8b06af4ff               call       0x412730
004cbc80 898588fdffff             mov        dword ptr [ebp - 0x278], eax
004cbc86 6a00                     push       0
004cbc88 8b8d88fdffff             mov        ecx, dword ptr [ebp - 0x278]
004cbc8e e8cd99fbff               call       0x485660
004cbc93 898584fdffff             mov        dword ptr [ebp - 0x27c], eax
004cbc99 6a00                     push       0
004cbc9b 8b8d84fdffff             mov        ecx, dword ptr [ebp - 0x27c]
004cbca1 e8daae0000               call       0x4d6b80
004cbca6 8bc8                     mov        ecx, eax
004cbca8 e823c0f6ff               call       0x437cd0
004cbcad f30f10404c               movss      xmm0, dword ptr [eax + 0x4c]
004cbcb2 f30f590588175700         mulss      xmm0, dword ptr [0x571788]
004cbcba f30f118574fdffff         movss      dword ptr [ebp - 0x28c], xmm0
004cbcc2 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cbcc8 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004cbcce e85d6af4ff               call       0x412730
004cbcd3 898580fdffff             mov        dword ptr [ebp - 0x280], eax
004cbcd9 6a00                     push       0
004cbcdb 8b8d80fdffff             mov        ecx, dword ptr [ebp - 0x280]
004cbce1 e87a99fbff               call       0x485660
004cbce6 8bc8                     mov        ecx, eax
004cbce8 e8d3e5faff               call       0x47a2c0
004cbced f30f104004               movss      xmm0, dword ptr [eax + 4]
004cbcf2 f30f118570fdffff         movss      dword ptr [ebp - 0x290], xmm0
004cbcfa 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004cbd00 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004cbd06 e8256af4ff               call       0x412730
004cbd0b 89857cfdffff             mov        dword ptr [ebp - 0x284], eax
004cbd11 6a00                     push       0
004cbd13 8b8d7cfdffff             mov        ecx, dword ptr [ebp - 0x284]
004cbd19 e84299fbff               call       0x485660
004cbd1e 8bc8                     mov        ecx, eax
004cbd20 e89be5faff               call       0x47a2c0
004cbd25 f30f1000                 movss      xmm0, dword ptr [eax]
004cbd29 f30f11856cfdffff         movss      dword ptr [ebp - 0x294], xmm0
004cbd31 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cbd39 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cbd41 51                       push       ecx
004cbd42 f30f110424               movss      dword ptr [esp], xmm0
004cbd47 51                       push       ecx
004cbd48 f30f1085dcfdffff         movss      xmm0, dword ptr [ebp - 0x224]
004cbd50 f30f110424               movss      dword ptr [esp], xmm0
004cbd55 51                       push       ecx
004cbd56 f30f1085d8fdffff         movss      xmm0, dword ptr [ebp - 0x228]
004cbd5e f30f110424               movss      dword ptr [esp], xmm0
004cbd63 51                       push       ecx
004cbd64 0f57c0                   xorps      xmm0, xmm0
004cbd67 f30f110424               movss      dword ptr [esp], xmm0
004cbd6c 51                       push       ecx
004cbd6d f30f108578fdffff         movss      xmm0, dword ptr [ebp - 0x288]
004cbd75 f30f110424               movss      dword ptr [esp], xmm0
004cbd7a 51                       push       ecx
004cbd7b f30f108574fdffff         movss      xmm0, dword ptr [ebp - 0x28c]
004cbd83 f30f110424               movss      dword ptr [esp], xmm0
004cbd88 51                       push       ecx
004cbd89 f30f108570fdffff         movss      xmm0, dword ptr [ebp - 0x290]
004cbd91 f30f110424               movss      dword ptr [esp], xmm0
004cbd96 51                       push       ecx
004cbd97 f30f10856cfdffff         movss      xmm0, dword ptr [ebp - 0x294]
004cbd9f f30f110424               movss      dword ptr [esp], xmm0
004cbda4 e867b8f8ff               call       0x457610
004cbda9 83c420                   add        esp, 0x20
004cbdac 0fb6d0                   movzx      edx, al
004cbdaf 85d2                     test       edx, edx
004cbdb1 7409                     je         0x4cbdbc
004cbdb3 8b451c                   mov        eax, dword ptr [ebp + 0x1c]
004cbdb6 c70001000000             mov        dword ptr [eax], 1
004cbdbc f30f1085e4fdffff         movss      xmm0, dword ptr [ebp - 0x21c]
004cbdc4 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
004cbdcb 0f2f85f0fdffff           comiss     xmm0, dword ptr [ebp - 0x210]
004cbdd2 773a                     ja         0x4cbe0e
004cbdd4 f30f1085f0fdffff         movss      xmm0, dword ptr [ebp - 0x210]
004cbddc 0f2f85e4fdffff           comiss     xmm0, dword ptr [ebp - 0x21c]
004cbde3 7729                     ja         0x4cbe0e
004cbde5 f30f1085e8fdffff         movss      xmm0, dword ptr [ebp - 0x218]
004cbded 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
004cbdf4 0f2f85f4fdffff           comiss     xmm0, dword ptr [ebp - 0x20c]
004cbdfb 7711                     ja         0x4cbe0e
004cbdfd f30f1085f4fdffff         movss      xmm0, dword ptr [ebp - 0x20c]
004cbe05 0f2f85e8fdffff           comiss     xmm0, dword ptr [ebp - 0x218]
004cbe0c 760a                     jbe        0x4cbe18
004cbe0e e919010000               jmp        0x4cbf2c
004cbe13 e914010000               jmp        0x4cbf2c
004cbe18 8b8da4fdffff             mov        ecx, dword ptr [ebp - 0x25c]
004cbe1e c6840dfcfdffff01         mov        byte ptr [ebp + ecx - 0x204], 1
004cbe26 8b95a0fdffff             mov        edx, dword ptr [ebp - 0x260]
004cbe2c 83c201                   add        edx, 1
004cbe2f 8995a0fdffff             mov        dword ptr [ebp - 0x260], edx
004cbe35 c785acfdffff12000000     mov        dword ptr [ebp - 0x254], 0x12
004cbe3f 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cbe45 f30f104078               movss      xmm0, dword ptr [eax + 0x78]
004cbe4a 0f2f057cfe5600           comiss     xmm0, dword ptr [0x56fe7c]
004cbe51 7240                     jb         0x4cbe93
004cbe53 f30f2a85acfdffff         cvtsi2ss   xmm0, dword ptr [ebp - 0x254]
004cbe5b f30f100d7cfe5600         movss      xmm1, dword ptr [0x56fe7c]
004cbe63 f30f5c0d90cd5600         subss      xmm1, dword ptr [0x56cd90]
004cbe6b f30f5e0d30fa5600         divss      xmm1, dword ptr [0x56fa30]
004cbe73 f30f590dd8c85600         mulss      xmm1, dword ptr [0x56c8d8]
004cbe7b f30f58c1                 addss      xmm0, xmm1
004cbe7f f30f5805ccc85600         addss      xmm0, dword ptr [0x56c8cc]
004cbe87 f30f2cc8                 cvttss2si  ecx, xmm0
004cbe8b 898dacfdffff             mov        dword ptr [ebp - 0x254], ecx
004cbe91 eb69                     jmp        0x4cbefc
004cbe93 8b95b0fdffff             mov        edx, dword ptr [ebp - 0x250]
004cbe99 f30f104278               movss      xmm0, dword ptr [edx + 0x78]
004cbe9e 0f2f0590cd5600           comiss     xmm0, dword ptr [0x56cd90]
004cbea5 7255                     jb         0x4cbefc
004cbea7 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cbead f30f10057cfe5600         movss      xmm0, dword ptr [0x56fe7c]
004cbeb5 0f2f4078                 comiss     xmm0, dword ptr [eax + 0x78]
004cbeb9 7641                     jbe        0x4cbefc
004cbebb f30f2a85acfdffff         cvtsi2ss   xmm0, dword ptr [ebp - 0x254]
004cbec3 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004cbec9 f30f104978               movss      xmm1, dword ptr [ecx + 0x78]
004cbece f30f5c0d90cd5600         subss      xmm1, dword ptr [0x56cd90]
004cbed6 f30f5e0d30fa5600         divss      xmm1, dword ptr [0x56fa30]
004cbede f30f590dd8c85600         mulss      xmm1, dword ptr [0x56c8d8]
004cbee6 f30f58c1                 addss      xmm0, xmm1
004cbeea f30f5805ccc85600         addss      xmm0, dword ptr [0x56c8cc]
004cbef2 f30f2cd0                 cvttss2si  edx, xmm0
004cbef6 8995acfdffff             mov        dword ptr [ebp - 0x254], edx
004cbefc 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cbf02 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004cbf08 e8830bf5ff               call       0x41ca90
004cbf0d 83c04c                   add        eax, 0x4c
004cbf10 89859cfdffff             mov        dword ptr [ebp - 0x264], eax
004cbf16 8b8d9cfdffff             mov        ecx, dword ptr [ebp - 0x264]
004cbf1c 8b11                     mov        edx, dword ptr [ecx]
004cbf1e 0395acfdffff             add        edx, dword ptr [ebp - 0x254]
004cbf24 8b859cfdffff             mov        eax, dword ptr [ebp - 0x264]
004cbf2a 8910                     mov        dword ptr [eax], edx
004cbf2c 8d8dccfdffff             lea        ecx, [ebp - 0x234]
004cbf32 51                       push       ecx
004cbf33 8d8dd8fdffff             lea        ecx, [ebp - 0x228]
004cbf39 e8a2d7f5ff               call       0x4296e0
004cbf3e 8d95c0fdffff             lea        edx, [ebp - 0x240]
004cbf44 52                       push       edx
004cbf45 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cbf4b e890d7f5ff               call       0x4296e0
004cbf50 f30f1085a8fdffff         movss      xmm0, dword ptr [ebp - 0x258]
004cbf58 f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
004cbf60 f30f1185a8fdffff         movss      dword ptr [ebp - 0x258], xmm0
004cbf68 8b85a4fdffff             mov        eax, dword ptr [ebp - 0x25c]
004cbf6e 83c001                   add        eax, 1
004cbf71 8985a4fdffff             mov        dword ptr [ebp - 0x25c], eax
004cbf77 e968fcffff               jmp        0x4cbbe4
004cbf7c 8b85a0fdffff             mov        eax, dword ptr [ebp - 0x260]
004cbf82 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cbf85 33cd                     xor        ecx, ebp
004cbf87 e8626a0700               call       0x5429ee
004cbf8c 8be5                     mov        esp, ebp
004cbf8e 5d                       pop        ebp
004cbf8f c21800                   ret        0x18
004cbf92 cc                       int3       
004cbf93 cc                       int3       
004cbf94 cc                       int3       
004cbf95 cc                       int3       
004cbf96 cc                       int3       
004cbf97 cc                       int3       
004cbf98 cc                       int3       
004cbf99 cc                       int3       
004cbf9a cc                       int3       
004cbf9b cc                       int3       
004cbf9c cc                       int3       
004cbf9d cc                       int3       
004cbf9e cc                       int3       
004cbf9f cc                       int3       
