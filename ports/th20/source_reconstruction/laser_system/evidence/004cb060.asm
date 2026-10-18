004cb060 55                       push       ebp
004cb061 8bec                     mov        ebp, esp
004cb063 81ec7c020000             sub        esp, 0x27c
004cb069 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004cb06e 33c5                     xor        eax, ebp
004cb070 8945fc                   mov        dword ptr [ebp - 4], eax
004cb073 898dd4fdffff             mov        dword ptr [ebp - 0x22c], ecx
004cb079 837d1400                 cmp        dword ptr [ebp + 0x14], 0
004cb07d 7416                     je         0x4cb095
004cb07f 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cb085 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004cb08c 7407                     je         0x4cb095
004cb08e 33c0                     xor        eax, eax
004cb090 e94d040000               jmp        0x4cb4e2
004cb095 c785d0fdffff00000000     mov        dword ptr [ebp - 0x230], 0
004cb09f c785ccfdffff00000000     mov        dword ptr [ebp - 0x234], 0
004cb0a9 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cb0af e85c7df5ff               call       0x422e10
004cb0b4 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cb0ba e8517df5ff               call       0x422e10
004cb0bf 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cb0c5 83c158                   add        ecx, 0x58
004cb0c8 8b11                     mov        edx, dword ptr [ecx]
004cb0ca 8995d8fdffff             mov        dword ptr [ebp - 0x228], edx
004cb0d0 8b4104                   mov        eax, dword ptr [ecx + 4]
004cb0d3 8985dcfdffff             mov        dword ptr [ebp - 0x224], eax
004cb0d9 8b4908                   mov        ecx, dword ptr [ecx + 8]
004cb0dc 898de0fdffff             mov        dword ptr [ebp - 0x220], ecx
004cb0e2 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cb0ea f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cb0f2 f30f1185c4fdffff         movss      dword ptr [ebp - 0x23c], xmm0
004cb0fa 6800020000               push       0x200
004cb0ff 6a00                     push       0
004cb101 8d95fcfdffff             lea        edx, [ebp - 0x204]
004cb107 52                       push       edx
004cb108 e853960700               call       0x544760
004cb10d 83c40c                   add        esp, 0xc
004cb110 0f57c0                   xorps      xmm0, xmm0
004cb113 f30f1185ecfdffff         movss      dword ptr [ebp - 0x214], xmm0
004cb11b f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cb123 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cb12b 51                       push       ecx
004cb12c f30f110424               movss      dword ptr [esp], xmm0
004cb131 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cb137 51                       push       ecx
004cb138 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004cb13d f30f110424               movss      dword ptr [esp], xmm0
004cb142 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cb148 51                       push       ecx
004cb149 e8e2e1f6ff               call       0x439330
004cb14e 83c40c                   add        esp, 0xc
004cb151 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cb157 83c258                   add        edx, 0x58
004cb15a 8995bcfdffff             mov        dword ptr [ebp - 0x244], edx
004cb160 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004cb166 50                       push       eax
004cb167 8d8d84fdffff             lea        ecx, [ebp - 0x27c]
004cb16d 51                       push       ecx
004cb16e 8b8dbcfdffff             mov        ecx, dword ptr [ebp - 0x244]
004cb174 e8f7e3f5ff               call       0x429570
004cb179 8b10                     mov        edx, dword ptr [eax]
004cb17b 8995f0fdffff             mov        dword ptr [ebp - 0x210], edx
004cb181 8b4804                   mov        ecx, dword ptr [eax + 4]
004cb184 898df4fdffff             mov        dword ptr [ebp - 0x20c], ecx
004cb18a 8b5008                   mov        edx, dword ptr [eax + 8]
004cb18d 8995f8fdffff             mov        dword ptr [ebp - 0x208], edx
004cb193 0f57c0                   xorps      xmm0, xmm0
004cb196 f30f1185f8fdffff         movss      dword ptr [ebp - 0x208], xmm0
004cb19e 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004cb1a4 50                       push       eax
004cb1a5 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cb1ab e830e5f5ff               call       0x4296e0
004cb1b0 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004cb1b5 f30f59450c               mulss      xmm0, dword ptr [ebp + 0xc]
004cb1ba f30f11450c               movss      dword ptr [ebp + 0xc], xmm0
004cb1bf c785c8fdffff00000000     mov        dword ptr [ebp - 0x238], 0
004cb1c9 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cb1d1 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cb1d9 f30f5885c4fdffff         addss      xmm0, dword ptr [ebp - 0x23c]
004cb1e1 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cb1e7 f30f104974               movss      xmm1, dword ptr [ecx + 0x74]
004cb1ec 0f2fc8                   comiss     xmm1, xmm0
004cb1ef 0f8284020000             jb         0x4cb479
004cb1f5 8d95f0fdffff             lea        edx, [ebp - 0x210]
004cb1fb 52                       push       edx
004cb1fc 8b4508                   mov        eax, dword ptr [ebp + 8]
004cb1ff 50                       push       eax
004cb200 e8cbaef8ff               call       0x4560d0
004cb205 83c408                   add        esp, 8
004cb208 d99db8fdffff             fstp       dword ptr [ebp - 0x248]
004cb20e f30f1085b8fdffff         movss      xmm0, dword ptr [ebp - 0x248]
004cb216 0f2f450c                 comiss     xmm0, dword ptr [ebp + 0xc]
004cb21a 760a                     jbe        0x4cb226
004cb21c e91a020000               jmp        0x4cb43b
004cb221 e915020000               jmp        0x4cb43b
004cb226 8b8dccfdffff             mov        ecx, dword ptr [ebp - 0x234]
004cb22c c6840dfcfdffff01         mov        byte ptr [ebp + ecx - 0x204], 1
004cb234 8b95d0fdffff             mov        edx, dword ptr [ebp - 0x230]
004cb23a 83c201                   add        edx, 1
004cb23d 8995d0fdffff             mov        dword ptr [ebp - 0x230], edx
004cb243 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cb249 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004cb24f e8ac10f4ff               call       0x40c300
004cb254 05886d2800               add        eax, 0x286d88
004cb259 8985c0fdffff             mov        dword ptr [ebp - 0x240], eax
004cb25f 8b8dc0fdffff             mov        ecx, dword ptr [ebp - 0x240]
004cb265 8b11                     mov        edx, dword ptr [ecx]
004cb267 83c201                   add        edx, 1
004cb26a 8b85c0fdffff             mov        eax, dword ptr [ebp - 0x240]
004cb270 8910                     mov        dword ptr [eax], edx
004cb272 8b8dc8fdffff             mov        ecx, dword ptr [ebp - 0x238]
004cb278 81e103000080             and        ecx, 0x80000003
004cb27e 7905                     jns        0x4cb285
004cb280 49                       dec        ecx
004cb281 83c9fc                   or         ecx, 0xfffffffc
004cb284 41                       inc        ecx
004cb285 85c9                     test       ecx, ecx
004cb287 0f859f010000             jne        0x4cb42c
004cb28d 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cb293 83bae406000011           cmp        dword ptr [edx + 0x6e4], 0x11
004cb29a 7e1e                     jle        0x4cb2ba
004cb29c 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cb2a2 83b8e406000022           cmp        dword ptr [eax + 0x6e4], 0x22
004cb2a9 740f                     je         0x4cb2ba
004cb2ab 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cb2b1 83b9e406000026           cmp        dword ptr [ecx + 0x6e4], 0x26
004cb2b8 756f                     jne        0x4cb329
004cb2ba 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cb2c0 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004cb2c6 e83510f4ff               call       0x40c300
004cb2cb 8bc8                     mov        ecx, eax
004cb2cd e8dea6fbff               call       0x4859b0
004cb2d2 8985b0fdffff             mov        dword ptr [ebp - 0x250], eax
004cb2d8 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cb2de 8b8820070000             mov        ecx, dword ptr [eax + 0x720]
004cb2e4 8d9409d4000000           lea        edx, [ecx + ecx + 0xd4]
004cb2eb 8995b4fdffff             mov        dword ptr [ebp - 0x24c], edx
004cb2f1 6a00                     push       0
004cb2f3 6aff                     push       -1
004cb2f5 51                       push       ecx
004cb2f6 0f57c0                   xorps      xmm0, xmm0
004cb2f9 f30f110424               movss      dword ptr [esp], xmm0
004cb2fe 8d85f0fdffff             lea        eax, [ebp - 0x210]
004cb304 50                       push       eax
004cb305 8b8db4fdffff             mov        ecx, dword ptr [ebp - 0x24c]
004cb30b 51                       push       ecx
004cb30c 6814fd5600               push       0x56fd14
004cb311 8d9598fdffff             lea        edx, [ebp - 0x268]
004cb317 52                       push       edx
004cb318 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004cb31e e8bdddfaff               call       0x4790e0
004cb323 90                       nop        
004cb324 e903010000               jmp        0x4cb42c
004cb329 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cb32f 83b8e40600001f           cmp        dword ptr [eax + 0x6e4], 0x1f
004cb336 7e0f                     jle        0x4cb347
004cb338 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cb33e 83b9e40600001b           cmp        dword ptr [ecx + 0x6e4], 0x1b
004cb345 756c                     jne        0x4cb3b3
004cb347 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cb34d 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004cb353 e8a80ff4ff               call       0x40c300
004cb358 8bc8                     mov        ecx, eax
004cb35a e851a6fbff               call       0x4859b0
004cb35f 8985a8fdffff             mov        dword ptr [ebp - 0x258], eax
004cb365 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cb36b 8b8820070000             mov        ecx, dword ptr [eax + 0x720]
004cb371 8d940904010000           lea        edx, [ecx + ecx + 0x104]
004cb378 8995acfdffff             mov        dword ptr [ebp - 0x254], edx
004cb37e 6a00                     push       0
004cb380 6aff                     push       -1
004cb382 51                       push       ecx
004cb383 0f57c0                   xorps      xmm0, xmm0
004cb386 f30f110424               movss      dword ptr [esp], xmm0
004cb38b 8d85f0fdffff             lea        eax, [ebp - 0x210]
004cb391 50                       push       eax
004cb392 8b8dacfdffff             mov        ecx, dword ptr [ebp - 0x254]
004cb398 51                       push       ecx
004cb399 6814fd5600               push       0x56fd14
004cb39e 8d9594fdffff             lea        edx, [ebp - 0x26c]
004cb3a4 52                       push       edx
004cb3a5 8b8da8fdffff             mov        ecx, dword ptr [ebp - 0x258]
004cb3ab e830ddfaff               call       0x4790e0
004cb3b0 90                       nop        
004cb3b1 eb79                     jmp        0x4cb42c
004cb3b3 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cb3b9 83b8e406000021           cmp        dword ptr [eax + 0x6e4], 0x21
004cb3c0 7f6a                     jg         0x4cb42c
004cb3c2 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cb3c8 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004cb3ce e82d0ff4ff               call       0x40c300
004cb3d3 8bc8                     mov        ecx, eax
004cb3d5 e8d6a5fbff               call       0x4859b0
004cb3da 8985a0fdffff             mov        dword ptr [ebp - 0x260], eax
004cb3e0 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cb3e6 8b8220070000             mov        eax, dword ptr [edx + 0x720]
004cb3ec 8d8c001c010000           lea        ecx, [eax + eax + 0x11c]
004cb3f3 898da4fdffff             mov        dword ptr [ebp - 0x25c], ecx
004cb3f9 6a00                     push       0
004cb3fb 6aff                     push       -1
004cb3fd 51                       push       ecx
004cb3fe 0f57c0                   xorps      xmm0, xmm0
004cb401 f30f110424               movss      dword ptr [esp], xmm0
004cb406 8d95f0fdffff             lea        edx, [ebp - 0x210]
004cb40c 52                       push       edx
004cb40d 8b85a4fdffff             mov        eax, dword ptr [ebp - 0x25c]
004cb413 50                       push       eax
004cb414 6814fd5600               push       0x56fd14
004cb419 8d8d90fdffff             lea        ecx, [ebp - 0x270]
004cb41f 51                       push       ecx
004cb420 8b8da0fdffff             mov        ecx, dword ptr [ebp - 0x260]
004cb426 e8b5dcfaff               call       0x4790e0
004cb42b 90                       nop        
004cb42c 8b95c8fdffff             mov        edx, dword ptr [ebp - 0x238]
004cb432 83c201                   add        edx, 1
004cb435 8995c8fdffff             mov        dword ptr [ebp - 0x238], edx
004cb43b 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004cb441 50                       push       eax
004cb442 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cb448 e893e2f5ff               call       0x4296e0
004cb44d f30f1085c4fdffff         movss      xmm0, dword ptr [ebp - 0x23c]
004cb455 f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
004cb45d f30f1185c4fdffff         movss      dword ptr [ebp - 0x23c], xmm0
004cb465 8b8dccfdffff             mov        ecx, dword ptr [ebp - 0x234]
004cb46b 83c101                   add        ecx, 1
004cb46e 898dccfdffff             mov        dword ptr [ebp - 0x234], ecx
004cb474 e950fdffff               jmp        0x4cb1c9
004cb479 83bdd0fdffff00           cmp        dword ptr [ebp - 0x230], 0
004cb480 745a                     je         0x4cb4dc
004cb482 8b95d0fdffff             mov        edx, dword ptr [ebp - 0x230]
004cb488 3b95ccfdffff             cmp        edx, dword ptr [ebp - 0x234]
004cb48e 7c20                     jl         0x4cb4b0
004cb490 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cb496 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004cb499 83e1f9                   and        ecx, 0xfffffff9
004cb49c 83c902                   or         ecx, 2
004cb49f 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cb4a5 894a1c                   mov        dword ptr [edx + 0x1c], ecx
004cb4a8 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004cb4ae eb32                     jmp        0x4cb4e2
004cb4b0 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cb4b6 8b08                     mov        ecx, dword ptr [eax]
004cb4b8 8b5138                   mov        edx, dword ptr [ecx + 0x38]
004cb4bb 89959cfdffff             mov        dword ptr [ebp - 0x264], edx
004cb4c1 8b85ccfdffff             mov        eax, dword ptr [ebp - 0x234]
004cb4c7 50                       push       eax
004cb4c8 8d8dfcfdffff             lea        ecx, [ebp - 0x204]
004cb4ce 51                       push       ecx
004cb4cf 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cb4d5 ff959cfdffff             call       dword ptr [ebp - 0x264]
004cb4db 90                       nop        
004cb4dc 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004cb4e2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cb4e5 33cd                     xor        ecx, ebp
004cb4e7 e802750700               call       0x5429ee
004cb4ec 8be5                     mov        esp, ebp
004cb4ee 5d                       pop        ebp
004cb4ef c21000                   ret        0x10
004cb4f2 cc                       int3       
004cb4f3 cc                       int3       
004cb4f4 cc                       int3       
004cb4f5 cc                       int3       
004cb4f6 cc                       int3       
004cb4f7 cc                       int3       
004cb4f8 cc                       int3       
004cb4f9 cc                       int3       
004cb4fa cc                       int3       
004cb4fb cc                       int3       
004cb4fc cc                       int3       
004cb4fd cc                       int3       
004cb4fe cc                       int3       
004cb4ff cc                       int3       
