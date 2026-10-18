004cbfa0 55                       push       ebp
004cbfa1 8bec                     mov        ebp, esp
004cbfa3 81ecbc020000             sub        esp, 0x2bc
004cbfa9 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004cbfae 33c5                     xor        eax, ebp
004cbfb0 8945fc                   mov        dword ptr [ebp - 4], eax
004cbfb3 898db0fdffff             mov        dword ptr [ebp - 0x250], ecx
004cbfb9 837d1800                 cmp        dword ptr [ebp + 0x18], 0
004cbfbd 7416                     je         0x4cbfd5
004cbfbf 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cbfc5 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004cbfcc 7407                     je         0x4cbfd5
004cbfce 33c0                     xor        eax, eax
004cbfd0 e9a7060000               jmp        0x4cc67c
004cbfd5 c785a0fdffff00000000     mov        dword ptr [ebp - 0x260], 0
004cbfdf c785a4fdffff00000000     mov        dword ptr [ebp - 0x25c], 0
004cbfe9 8d8dd8fdffff             lea        ecx, [ebp - 0x228]
004cbfef e81c6ef5ff               call       0x422e10
004cbff4 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cbffa e8116ef5ff               call       0x422e10
004cbfff 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cc005 e8066ef5ff               call       0x422e10
004cc00a 8d8dccfdffff             lea        ecx, [ebp - 0x234]
004cc010 e8fb6df5ff               call       0x422e10
004cc015 8d8dc0fdffff             lea        ecx, [ebp - 0x240]
004cc01b e8f06df5ff               call       0x422e10
004cc020 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004cc026 83c158                   add        ecx, 0x58
004cc029 8b11                     mov        edx, dword ptr [ecx]
004cc02b 8995b4fdffff             mov        dword ptr [ebp - 0x24c], edx
004cc031 8b4104                   mov        eax, dword ptr [ecx + 4]
004cc034 8985b8fdffff             mov        dword ptr [ebp - 0x248], eax
004cc03a 8b4908                   mov        ecx, dword ptr [ecx + 8]
004cc03d 898dbcfdffff             mov        dword ptr [ebp - 0x244], ecx
004cc043 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cc04b f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cc053 f30f1185a8fdffff         movss      dword ptr [ebp - 0x258], xmm0
004cc05b 6800020000               push       0x200
004cc060 6a00                     push       0
004cc062 8d95fcfdffff             lea        edx, [ebp - 0x204]
004cc068 52                       push       edx
004cc069 e8f2860700               call       0x544760
004cc06e 83c40c                   add        esp, 0xc
004cc071 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cc077 83c058                   add        eax, 0x58
004cc07a 898598fdffff             mov        dword ptr [ebp - 0x268], eax
004cc080 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004cc083 51                       push       ecx
004cc084 8d955cfdffff             lea        edx, [ebp - 0x2a4]
004cc08a 52                       push       edx
004cc08b 8b8d98fdffff             mov        ecx, dword ptr [ebp - 0x268]
004cc091 e8aad3f5ff               call       0x429440
004cc096 8b08                     mov        ecx, dword ptr [eax]
004cc098 898df0fdffff             mov        dword ptr [ebp - 0x210], ecx
004cc09e 8b5004                   mov        edx, dword ptr [eax + 4]
004cc0a1 8995f4fdffff             mov        dword ptr [ebp - 0x20c], edx
004cc0a7 8b4008                   mov        eax, dword ptr [eax + 8]
004cc0aa 8985f8fdffff             mov        dword ptr [ebp - 0x208], eax
004cc0b0 0f57c0                   xorps      xmm0, xmm0
004cc0b3 f30f1185f8fdffff         movss      dword ptr [ebp - 0x208], xmm0
004cc0bb 51                       push       ecx
004cc0bc f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004cc0c1 f30f110424               movss      dword ptr [esp], xmm0
004cc0c6 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cc0cc 51                       push       ecx
004cc0cd 8d95f0fdffff             lea        edx, [ebp - 0x210]
004cc0d3 52                       push       edx
004cc0d4 e8c7cef8ff               call       0x458fa0
004cc0d9 83c40c                   add        esp, 0xc
004cc0dc f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cc0e4 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cc0ec 51                       push       ecx
004cc0ed f30f110424               movss      dword ptr [esp], xmm0
004cc0f2 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cc0f8 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004cc0fd f30f584510               addss      xmm0, dword ptr [ebp + 0x10]
004cc102 51                       push       ecx
004cc103 f30f110424               movss      dword ptr [esp], xmm0
004cc108 e833c4f6ff               call       0x438540
004cc10d d91c24                   fstp       dword ptr [esp]
004cc110 8d8dc0fdffff             lea        ecx, [ebp - 0x240]
004cc116 51                       push       ecx
004cc117 e814d2f6ff               call       0x439330
004cc11c 83c40c                   add        esp, 0xc
004cc11f 0f57c0                   xorps      xmm0, xmm0
004cc122 f30f1185c8fdffff         movss      dword ptr [ebp - 0x238], xmm0
004cc12a 51                       push       ecx
004cc12b f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
004cc133 f30f110424               movss      dword ptr [esp], xmm0
004cc138 8d9550fdffff             lea        edx, [ebp - 0x2b0]
004cc13e 52                       push       edx
004cc13f 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004cc142 e8a96ef8ff               call       0x452ff0
004cc147 8b08                     mov        ecx, dword ptr [eax]
004cc149 898de4fdffff             mov        dword ptr [ebp - 0x21c], ecx
004cc14f 8b5004                   mov        edx, dword ptr [eax + 4]
004cc152 8995e8fdffff             mov        dword ptr [ebp - 0x218], edx
004cc158 8b4008                   mov        eax, dword ptr [eax + 8]
004cc15b 8985ecfdffff             mov        dword ptr [ebp - 0x214], eax
004cc161 8d8dc0fdffff             lea        ecx, [ebp - 0x240]
004cc167 51                       push       ecx
004cc168 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cc16e e86dd5f5ff               call       0x4296e0
004cc173 8d95c0fdffff             lea        edx, [ebp - 0x240]
004cc179 52                       push       edx
004cc17a 8d8dc0fdffff             lea        ecx, [ebp - 0x240]
004cc180 e85bd5f5ff               call       0x4296e0
004cc185 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cc18d f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cc195 51                       push       ecx
004cc196 f30f110424               movss      dword ptr [esp], xmm0
004cc19b 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cc1a1 51                       push       ecx
004cc1a2 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004cc1a7 f30f110424               movss      dword ptr [esp], xmm0
004cc1ac 8d8dccfdffff             lea        ecx, [ebp - 0x234]
004cc1b2 51                       push       ecx
004cc1b3 e878d1f6ff               call       0x439330
004cc1b8 83c40c                   add        esp, 0xc
004cc1bb 0f57c0                   xorps      xmm0, xmm0
004cc1be f30f1185d4fdffff         movss      dword ptr [ebp - 0x22c], xmm0
004cc1c6 8b95b0fdffff             mov        edx, dword ptr [ebp - 0x250]
004cc1cc 83c258                   add        edx, 0x58
004cc1cf 899594fdffff             mov        dword ptr [ebp - 0x26c], edx
004cc1d5 8d85ccfdffff             lea        eax, [ebp - 0x234]
004cc1db 50                       push       eax
004cc1dc 8d8d44fdffff             lea        ecx, [ebp - 0x2bc]
004cc1e2 51                       push       ecx
004cc1e3 8b8d94fdffff             mov        ecx, dword ptr [ebp - 0x26c]
004cc1e9 e882d3f5ff               call       0x429570
004cc1ee 8b10                     mov        edx, dword ptr [eax]
004cc1f0 8995d8fdffff             mov        dword ptr [ebp - 0x228], edx
004cc1f6 8b4804                   mov        ecx, dword ptr [eax + 4]
004cc1f9 898ddcfdffff             mov        dword ptr [ebp - 0x224], ecx
004cc1ff 8b5008                   mov        edx, dword ptr [eax + 8]
004cc202 8995e0fdffff             mov        dword ptr [ebp - 0x220], edx
004cc208 0f57c0                   xorps      xmm0, xmm0
004cc20b f30f1185e0fdffff         movss      dword ptr [ebp - 0x220], xmm0
004cc213 8d85ccfdffff             lea        eax, [ebp - 0x234]
004cc219 50                       push       eax
004cc21a 8d8dccfdffff             lea        ecx, [ebp - 0x234]
004cc220 e8bbd4f5ff               call       0x4296e0
004cc225 90                       nop        
004cc226 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cc22e f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cc236 f30f5885a8fdffff         addss      xmm0, dword ptr [ebp - 0x258]
004cc23e 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004cc244 f30f104974               movss      xmm1, dword ptr [ecx + 0x74]
004cc249 0f2fc8                   comiss     xmm1, xmm0
004cc24c 0f8224040000             jb         0x4cc676
004cc252 8b551c                   mov        edx, dword ptr [ebp + 0x1c]
004cc255 833a00                   cmp        dword ptr [edx], 0
004cc258 0f85cc010000             jne        0x4cc42a
004cc25e 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cc264 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004cc26a e8c164f4ff               call       0x412730
004cc26f 898590fdffff             mov        dword ptr [ebp - 0x270], eax
004cc275 6a00                     push       0
004cc277 8b8d90fdffff             mov        ecx, dword ptr [ebp - 0x270]
004cc27d e8de93fbff               call       0x485660
004cc282 85c0                     test       eax, eax
004cc284 0f84a0010000             je         0x4cc42a
004cc28a 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004cc290 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004cc296 e89564f4ff               call       0x412730
004cc29b 89858cfdffff             mov        dword ptr [ebp - 0x274], eax
004cc2a1 6a00                     push       0
004cc2a3 8b8d8cfdffff             mov        ecx, dword ptr [ebp - 0x274]
004cc2a9 e8b293fbff               call       0x485660
004cc2ae 898588fdffff             mov        dword ptr [ebp - 0x278], eax
004cc2b4 6a00                     push       0
004cc2b6 8b8d88fdffff             mov        ecx, dword ptr [ebp - 0x278]
004cc2bc e8bfa80000               call       0x4d6b80
004cc2c1 8bc8                     mov        ecx, eax
004cc2c3 e808baf6ff               call       0x437cd0
004cc2c8 f30f104048               movss      xmm0, dword ptr [eax + 0x48]
004cc2cd f30f590588175700         mulss      xmm0, dword ptr [0x571788]
004cc2d5 f30f118574fdffff         movss      dword ptr [ebp - 0x28c], xmm0
004cc2dd 8b95b0fdffff             mov        edx, dword ptr [ebp - 0x250]
004cc2e3 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004cc2e9 e84264f4ff               call       0x412730
004cc2ee 898584fdffff             mov        dword ptr [ebp - 0x27c], eax
004cc2f4 6a00                     push       0
004cc2f6 8b8d84fdffff             mov        ecx, dword ptr [ebp - 0x27c]
004cc2fc e85f93fbff               call       0x485660
004cc301 898580fdffff             mov        dword ptr [ebp - 0x280], eax
004cc307 6a00                     push       0
004cc309 8b8d80fdffff             mov        ecx, dword ptr [ebp - 0x280]
004cc30f e86ca80000               call       0x4d6b80
004cc314 8bc8                     mov        ecx, eax
004cc316 e8b5b9f6ff               call       0x437cd0
004cc31b f30f10404c               movss      xmm0, dword ptr [eax + 0x4c]
004cc320 f30f590588175700         mulss      xmm0, dword ptr [0x571788]
004cc328 f30f118570fdffff         movss      dword ptr [ebp - 0x290], xmm0
004cc330 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cc336 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004cc33c e8ef63f4ff               call       0x412730
004cc341 89857cfdffff             mov        dword ptr [ebp - 0x284], eax
004cc347 6a00                     push       0
004cc349 8b8d7cfdffff             mov        ecx, dword ptr [ebp - 0x284]
004cc34f e80c93fbff               call       0x485660
004cc354 8bc8                     mov        ecx, eax
004cc356 e865dffaff               call       0x47a2c0
004cc35b f30f104004               movss      xmm0, dword ptr [eax + 4]
004cc360 f30f11856cfdffff         movss      dword ptr [ebp - 0x294], xmm0
004cc368 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004cc36e 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004cc374 e8b763f4ff               call       0x412730
004cc379 898578fdffff             mov        dword ptr [ebp - 0x288], eax
004cc37f 6a00                     push       0
004cc381 8b8d78fdffff             mov        ecx, dword ptr [ebp - 0x288]
004cc387 e8d492fbff               call       0x485660
004cc38c 8bc8                     mov        ecx, eax
004cc38e e82ddffaff               call       0x47a2c0
004cc393 f30f1000                 movss      xmm0, dword ptr [eax]
004cc397 f30f118568fdffff         movss      dword ptr [ebp - 0x298], xmm0
004cc39f f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cc3a7 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cc3af 51                       push       ecx
004cc3b0 f30f110424               movss      dword ptr [esp], xmm0
004cc3b5 51                       push       ecx
004cc3b6 f30f1085dcfdffff         movss      xmm0, dword ptr [ebp - 0x224]
004cc3be f30f110424               movss      dword ptr [esp], xmm0
004cc3c3 51                       push       ecx
004cc3c4 f30f1085d8fdffff         movss      xmm0, dword ptr [ebp - 0x228]
004cc3cc f30f110424               movss      dword ptr [esp], xmm0
004cc3d1 51                       push       ecx
004cc3d2 0f57c0                   xorps      xmm0, xmm0
004cc3d5 f30f110424               movss      dword ptr [esp], xmm0
004cc3da 51                       push       ecx
004cc3db f30f108574fdffff         movss      xmm0, dword ptr [ebp - 0x28c]
004cc3e3 f30f110424               movss      dword ptr [esp], xmm0
004cc3e8 51                       push       ecx
004cc3e9 f30f108570fdffff         movss      xmm0, dword ptr [ebp - 0x290]
004cc3f1 f30f110424               movss      dword ptr [esp], xmm0
004cc3f6 51                       push       ecx
004cc3f7 f30f10856cfdffff         movss      xmm0, dword ptr [ebp - 0x294]
004cc3ff f30f110424               movss      dword ptr [esp], xmm0
004cc404 51                       push       ecx
004cc405 f30f108568fdffff         movss      xmm0, dword ptr [ebp - 0x298]
004cc40d f30f110424               movss      dword ptr [esp], xmm0
004cc412 e8f9b1f8ff               call       0x457610
004cc417 83c420                   add        esp, 0x20
004cc41a 0fb6d0                   movzx      edx, al
004cc41d 85d2                     test       edx, edx
004cc41f 7409                     je         0x4cc42a
004cc421 8b451c                   mov        eax, dword ptr [ebp + 0x1c]
004cc424 c70001000000             mov        dword ptr [eax], 1
004cc42a f30f1085e4fdffff         movss      xmm0, dword ptr [ebp - 0x21c]
004cc432 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
004cc439 0f2f85f0fdffff           comiss     xmm0, dword ptr [ebp - 0x210]
004cc440 773a                     ja         0x4cc47c
004cc442 f30f1085f0fdffff         movss      xmm0, dword ptr [ebp - 0x210]
004cc44a 0f2f85e4fdffff           comiss     xmm0, dword ptr [ebp - 0x21c]
004cc451 7729                     ja         0x4cc47c
004cc453 f30f1085e8fdffff         movss      xmm0, dword ptr [ebp - 0x218]
004cc45b 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
004cc462 0f2f85f4fdffff           comiss     xmm0, dword ptr [ebp - 0x20c]
004cc469 7711                     ja         0x4cc47c
004cc46b f30f1085f4fdffff         movss      xmm0, dword ptr [ebp - 0x20c]
004cc473 0f2f85e8fdffff           comiss     xmm0, dword ptr [ebp - 0x218]
004cc47a 760a                     jbe        0x4cc486
004cc47c e9a5010000               jmp        0x4cc626
004cc481 e9a0010000               jmp        0x4cc626
004cc486 8b8da4fdffff             mov        ecx, dword ptr [ebp - 0x25c]
004cc48c c6840dfcfdffff01         mov        byte ptr [ebp + ecx - 0x204], 1
004cc494 8b95a0fdffff             mov        edx, dword ptr [ebp - 0x260]
004cc49a 83c201                   add        edx, 1
004cc49d 8995a0fdffff             mov        dword ptr [ebp - 0x260], edx
004cc4a3 c785acfdffff0f000000     mov        dword ptr [ebp - 0x254], 0xf
004cc4ad 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cc4b3 f30f10407c               movss      xmm0, dword ptr [eax + 0x7c]
004cc4b8 0f2f05a8f75600           comiss     xmm0, dword ptr [0x56f7a8]
004cc4bf 720f                     jb         0x4cc4d0
004cc4c1 6b8dacfdffff03           imul       ecx, dword ptr [ebp - 0x254], 3
004cc4c8 898dacfdffff             mov        dword ptr [ebp - 0x254], ecx
004cc4ce eb69                     jmp        0x4cc539
004cc4d0 8b95b0fdffff             mov        edx, dword ptr [ebp - 0x250]
004cc4d6 f30f10427c               movss      xmm0, dword ptr [edx + 0x7c]
004cc4db 0f2f05e0c85600           comiss     xmm0, dword ptr [0x56c8e0]
004cc4e2 7255                     jb         0x4cc539
004cc4e4 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cc4ea f30f1005a8f75600         movss      xmm0, dword ptr [0x56f7a8]
004cc4f2 0f2f407c                 comiss     xmm0, dword ptr [eax + 0x7c]
004cc4f6 7641                     jbe        0x4cc539
004cc4f8 f30f2a85acfdffff         cvtsi2ss   xmm0, dword ptr [ebp - 0x254]
004cc500 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004cc506 f30f10497c               movss      xmm1, dword ptr [ecx + 0x7c]
004cc50b f30f5c0de0c85600         subss      xmm1, dword ptr [0x56c8e0]
004cc513 f30f5e0dbcd75600         divss      xmm1, dword ptr [0x56d7bc]
004cc51b f30f590dd0c85600         mulss      xmm1, dword ptr [0x56c8d0]
004cc523 f30f580dccc85600         addss      xmm1, dword ptr [0x56c8cc]
004cc52b f30f59c1                 mulss      xmm0, xmm1
004cc52f f30f2cd0                 cvttss2si  edx, xmm0
004cc533 8995acfdffff             mov        dword ptr [ebp - 0x254], edx
004cc539 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cc53f f30f104078               movss      xmm0, dword ptr [eax + 0x78]
004cc544 0f2f057cfe5600           comiss     xmm0, dword ptr [0x56fe7c]
004cc54b 7240                     jb         0x4cc58d
004cc54d f30f2a85acfdffff         cvtsi2ss   xmm0, dword ptr [ebp - 0x254]
004cc555 f30f100d7cfe5600         movss      xmm1, dword ptr [0x56fe7c]
004cc55d f30f5c0d90cd5600         subss      xmm1, dword ptr [0x56cd90]
004cc565 f30f5e0d30fa5600         divss      xmm1, dword ptr [0x56fa30]
004cc56d f30f590dd8c85600         mulss      xmm1, dword ptr [0x56c8d8]
004cc575 f30f58c1                 addss      xmm0, xmm1
004cc579 f30f5805ccc85600         addss      xmm0, dword ptr [0x56c8cc]
004cc581 f30f2cc8                 cvttss2si  ecx, xmm0
004cc585 898dacfdffff             mov        dword ptr [ebp - 0x254], ecx
004cc58b eb69                     jmp        0x4cc5f6
004cc58d 8b95b0fdffff             mov        edx, dword ptr [ebp - 0x250]
004cc593 f30f104278               movss      xmm0, dword ptr [edx + 0x78]
004cc598 0f2f0590cd5600           comiss     xmm0, dword ptr [0x56cd90]
004cc59f 7255                     jb         0x4cc5f6
004cc5a1 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cc5a7 f30f10057cfe5600         movss      xmm0, dword ptr [0x56fe7c]
004cc5af 0f2f4078                 comiss     xmm0, dword ptr [eax + 0x78]
004cc5b3 7641                     jbe        0x4cc5f6
004cc5b5 f30f2a85acfdffff         cvtsi2ss   xmm0, dword ptr [ebp - 0x254]
004cc5bd 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004cc5c3 f30f104978               movss      xmm1, dword ptr [ecx + 0x78]
004cc5c8 f30f5c0d90cd5600         subss      xmm1, dword ptr [0x56cd90]
004cc5d0 f30f5e0d30fa5600         divss      xmm1, dword ptr [0x56fa30]
004cc5d8 f30f590dd8c85600         mulss      xmm1, dword ptr [0x56c8d8]
004cc5e0 f30f58c1                 addss      xmm0, xmm1
004cc5e4 f30f5805ccc85600         addss      xmm0, dword ptr [0x56c8cc]
004cc5ec f30f2cd0                 cvttss2si  edx, xmm0
004cc5f0 8995acfdffff             mov        dword ptr [ebp - 0x254], edx
004cc5f6 8b85b0fdffff             mov        eax, dword ptr [ebp - 0x250]
004cc5fc 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004cc602 e88904f5ff               call       0x41ca90
004cc607 83c04c                   add        eax, 0x4c
004cc60a 89859cfdffff             mov        dword ptr [ebp - 0x264], eax
004cc610 8b8d9cfdffff             mov        ecx, dword ptr [ebp - 0x264]
004cc616 8b11                     mov        edx, dword ptr [ecx]
004cc618 0395acfdffff             add        edx, dword ptr [ebp - 0x254]
004cc61e 8b859cfdffff             mov        eax, dword ptr [ebp - 0x264]
004cc624 8910                     mov        dword ptr [eax], edx
004cc626 8d8dccfdffff             lea        ecx, [ebp - 0x234]
004cc62c 51                       push       ecx
004cc62d 8d8dd8fdffff             lea        ecx, [ebp - 0x228]
004cc633 e8a8d0f5ff               call       0x4296e0
004cc638 8d95c0fdffff             lea        edx, [ebp - 0x240]
004cc63e 52                       push       edx
004cc63f 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cc645 e896d0f5ff               call       0x4296e0
004cc64a f30f1085a8fdffff         movss      xmm0, dword ptr [ebp - 0x258]
004cc652 f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
004cc65a f30f1185a8fdffff         movss      dword ptr [ebp - 0x258], xmm0
004cc662 8b85a4fdffff             mov        eax, dword ptr [ebp - 0x25c]
004cc668 83c001                   add        eax, 1
004cc66b 8985a4fdffff             mov        dword ptr [ebp - 0x25c], eax
004cc671 e9b0fbffff               jmp        0x4cc226
004cc676 8b85a0fdffff             mov        eax, dword ptr [ebp - 0x260]
004cc67c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cc67f 33cd                     xor        ecx, ebp
004cc681 e868630700               call       0x5429ee
004cc686 8be5                     mov        esp, ebp
004cc688 5d                       pop        ebp
004cc689 c21800                   ret        0x18
004cc68c cc                       int3       
004cc68d cc                       int3       
004cc68e cc                       int3       
004cc68f cc                       int3       
