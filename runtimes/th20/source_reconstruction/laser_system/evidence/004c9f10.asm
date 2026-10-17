004c9f10 55                       push       ebp
004c9f11 8bec                     mov        ebp, esp
004c9f13 81ec68020000             sub        esp, 0x268
004c9f19 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c9f1e 33c5                     xor        eax, ebp
004c9f20 8945fc                   mov        dword ptr [ebp - 4], eax
004c9f23 898de0fdffff             mov        dword ptr [ebp - 0x220], ecx
004c9f29 837d1800                 cmp        dword ptr [ebp + 0x18], 0
004c9f2d 7416                     je         0x4c9f45
004c9f2f 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004c9f35 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004c9f3c 7407                     je         0x4c9f45
004c9f3e 33c0                     xor        eax, eax
004c9f40 e926040000               jmp        0x4ca36b
004c9f45 c785d8fdffff00000000     mov        dword ptr [ebp - 0x228], 0
004c9f4f c785dcfdffff00000000     mov        dword ptr [ebp - 0x224], 0
004c9f59 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004c9f5f e8ac8ef5ff               call       0x422e10
004c9f64 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004c9f6a e8a18ef5ff               call       0x422e10
004c9f6f f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004c9f77 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004c9f7f f30f1185d4fdffff         movss      dword ptr [ebp - 0x22c], xmm0
004c9f87 6800020000               push       0x200
004c9f8c 6a00                     push       0
004c9f8e 8d8dfcfdffff             lea        ecx, [ebp - 0x204]
004c9f94 51                       push       ecx
004c9f95 e8c6a70700               call       0x544760
004c9f9a 83c40c                   add        esp, 0xc
004c9f9d f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004c9fa5 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004c9fad 51                       push       ecx
004c9fae f30f110424               movss      dword ptr [esp], xmm0
004c9fb3 8b95e0fdffff             mov        edx, dword ptr [ebp - 0x220]
004c9fb9 51                       push       ecx
004c9fba f30f104270               movss      xmm0, dword ptr [edx + 0x70]
004c9fbf f30f110424               movss      dword ptr [esp], xmm0
004c9fc4 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004c9fca 50                       push       eax
004c9fcb e860f3f6ff               call       0x439330
004c9fd0 83c40c                   add        esp, 0xc
004c9fd3 0f57c0                   xorps      xmm0, xmm0
004c9fd6 f30f1185ecfdffff         movss      dword ptr [ebp - 0x214], xmm0
004c9fde 8b8de0fdffff             mov        ecx, dword ptr [ebp - 0x220]
004c9fe4 83c158                   add        ecx, 0x58
004c9fe7 898dccfdffff             mov        dword ptr [ebp - 0x234], ecx
004c9fed 8d95e4fdffff             lea        edx, [ebp - 0x21c]
004c9ff3 52                       push       edx
004c9ff4 8d8598fdffff             lea        eax, [ebp - 0x268]
004c9ffa 50                       push       eax
004c9ffb 8b8dccfdffff             mov        ecx, dword ptr [ebp - 0x234]
004ca001 e86af5f5ff               call       0x429570
004ca006 8b08                     mov        ecx, dword ptr [eax]
004ca008 898df0fdffff             mov        dword ptr [ebp - 0x210], ecx
004ca00e 8b5004                   mov        edx, dword ptr [eax + 4]
004ca011 8995f4fdffff             mov        dword ptr [ebp - 0x20c], edx
004ca017 8b4008                   mov        eax, dword ptr [eax + 8]
004ca01a 8985f8fdffff             mov        dword ptr [ebp - 0x208], eax
004ca020 0f57c0                   xorps      xmm0, xmm0
004ca023 f30f1185f8fdffff         movss      dword ptr [ebp - 0x208], xmm0
004ca02b 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004ca031 51                       push       ecx
004ca032 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004ca038 e8a3f6f5ff               call       0x4296e0
004ca03d 90                       nop        
004ca03e f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004ca046 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004ca04e f30f5885d4fdffff         addss      xmm0, dword ptr [ebp - 0x22c]
004ca056 8b95e0fdffff             mov        edx, dword ptr [ebp - 0x220]
004ca05c f30f104a74               movss      xmm1, dword ptr [edx + 0x74]
004ca061 0f2fc8                   comiss     xmm1, xmm0
004ca064 0f82c6020000             jb         0x4ca330
004ca06a 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004ca070 51                       push       ecx
004ca071 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004ca076 f30f110424               movss      dword ptr [esp], xmm0
004ca07b 51                       push       ecx
004ca07c f30f1085e8fdffff         movss      xmm0, dword ptr [ebp - 0x218]
004ca084 f30f110424               movss      dword ptr [esp], xmm0
004ca089 51                       push       ecx
004ca08a f30f1085e4fdffff         movss      xmm0, dword ptr [ebp - 0x21c]
004ca092 f30f110424               movss      dword ptr [esp], xmm0
004ca097 51                       push       ecx
004ca098 f30f1085f4fdffff         movss      xmm0, dword ptr [ebp - 0x20c]
004ca0a0 f30f110424               movss      dword ptr [esp], xmm0
004ca0a5 51                       push       ecx
004ca0a6 f30f1085f0fdffff         movss      xmm0, dword ptr [ebp - 0x210]
004ca0ae f30f110424               movss      dword ptr [esp], xmm0
004ca0b3 51                       push       ecx
004ca0b4 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004ca0b9 f30f110424               movss      dword ptr [esp], xmm0
004ca0be 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004ca0c1 51                       push       ecx
004ca0c2 f30f104104               movss      xmm0, dword ptr [ecx + 4]
004ca0c7 f30f110424               movss      dword ptr [esp], xmm0
004ca0cc 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004ca0cf 51                       push       ecx
004ca0d0 f30f1002                 movss      xmm0, dword ptr [edx]
004ca0d4 f30f110424               movss      dword ptr [esp], xmm0
004ca0d9 8b4508                   mov        eax, dword ptr [ebp + 8]
004ca0dc 51                       push       ecx
004ca0dd f30f104004               movss      xmm0, dword ptr [eax + 4]
004ca0e2 f30f110424               movss      dword ptr [esp], xmm0
004ca0e7 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004ca0ea 51                       push       ecx
004ca0eb f30f1001                 movss      xmm0, dword ptr [ecx]
004ca0ef f30f110424               movss      dword ptr [esp], xmm0
004ca0f4 e8c7dff8ff               call       0x4580c0
004ca0f9 83c428                   add        esp, 0x28
004ca0fc 0fb6d0                   movzx      edx, al
004ca0ff 85d2                     test       edx, edx
004ca101 0f84eb010000             je         0x4ca2f2
004ca107 8b85dcfdffff             mov        eax, dword ptr [ebp - 0x224]
004ca10d c68405fcfdffff01         mov        byte ptr [ebp + eax - 0x204], 1
004ca115 8b8dd8fdffff             mov        ecx, dword ptr [ebp - 0x228]
004ca11b 83c101                   add        ecx, 1
004ca11e 898dd8fdffff             mov        dword ptr [ebp - 0x228], ecx
004ca124 8b95e0fdffff             mov        edx, dword ptr [ebp - 0x220]
004ca12a 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004ca130 e8cb21f4ff               call       0x40c300
004ca135 05886d2800               add        eax, 0x286d88
004ca13a 8985d0fdffff             mov        dword ptr [ebp - 0x230], eax
004ca140 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004ca146 8b08                     mov        ecx, dword ptr [eax]
004ca148 83c101                   add        ecx, 1
004ca14b 8b95d0fdffff             mov        edx, dword ptr [ebp - 0x230]
004ca151 890a                     mov        dword ptr [edx], ecx
004ca153 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004ca159 83b8e406000011           cmp        dword ptr [eax + 0x6e4], 0x11
004ca160 7e1e                     jle        0x4ca180
004ca162 8b8de0fdffff             mov        ecx, dword ptr [ebp - 0x220]
004ca168 83b9e406000022           cmp        dword ptr [ecx + 0x6e4], 0x22
004ca16f 740f                     je         0x4ca180
004ca171 8b95e0fdffff             mov        edx, dword ptr [ebp - 0x220]
004ca177 83bae406000026           cmp        dword ptr [edx + 0x6e4], 0x26
004ca17e 756f                     jne        0x4ca1ef
004ca180 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004ca186 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004ca18c e86f21f4ff               call       0x40c300
004ca191 8bc8                     mov        ecx, eax
004ca193 e818b8fbff               call       0x4859b0
004ca198 8985c4fdffff             mov        dword ptr [ebp - 0x23c], eax
004ca19e 8b8de0fdffff             mov        ecx, dword ptr [ebp - 0x220]
004ca1a4 8b9150070000             mov        edx, dword ptr [ecx + 0x750]
004ca1aa 8d8412d4000000           lea        eax, [edx + edx + 0xd4]
004ca1b1 8985c8fdffff             mov        dword ptr [ebp - 0x238], eax
004ca1b7 6a00                     push       0
004ca1b9 6aff                     push       -1
004ca1bb 51                       push       ecx
004ca1bc 0f57c0                   xorps      xmm0, xmm0
004ca1bf f30f110424               movss      dword ptr [esp], xmm0
004ca1c4 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004ca1ca 51                       push       ecx
004ca1cb 8b95c8fdffff             mov        edx, dword ptr [ebp - 0x238]
004ca1d1 52                       push       edx
004ca1d2 6814fd5600               push       0x56fd14
004ca1d7 8d85acfdffff             lea        eax, [ebp - 0x254]
004ca1dd 50                       push       eax
004ca1de 8b8dc4fdffff             mov        ecx, dword ptr [ebp - 0x23c]
004ca1e4 e8f7eefaff               call       0x4790e0
004ca1e9 90                       nop        
004ca1ea e903010000               jmp        0x4ca2f2
004ca1ef 8b8de0fdffff             mov        ecx, dword ptr [ebp - 0x220]
004ca1f5 83b9e40600001f           cmp        dword ptr [ecx + 0x6e4], 0x1f
004ca1fc 7e0f                     jle        0x4ca20d
004ca1fe 8b95e0fdffff             mov        edx, dword ptr [ebp - 0x220]
004ca204 83bae40600001b           cmp        dword ptr [edx + 0x6e4], 0x1b
004ca20b 756c                     jne        0x4ca279
004ca20d 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004ca213 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004ca219 e8e220f4ff               call       0x40c300
004ca21e 8bc8                     mov        ecx, eax
004ca220 e88bb7fbff               call       0x4859b0
004ca225 8985bcfdffff             mov        dword ptr [ebp - 0x244], eax
004ca22b 8b8de0fdffff             mov        ecx, dword ptr [ebp - 0x220]
004ca231 8b9150070000             mov        edx, dword ptr [ecx + 0x750]
004ca237 8d841204010000           lea        eax, [edx + edx + 0x104]
004ca23e 8985c0fdffff             mov        dword ptr [ebp - 0x240], eax
004ca244 6a00                     push       0
004ca246 6aff                     push       -1
004ca248 51                       push       ecx
004ca249 0f57c0                   xorps      xmm0, xmm0
004ca24c f30f110424               movss      dword ptr [esp], xmm0
004ca251 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004ca257 51                       push       ecx
004ca258 8b95c0fdffff             mov        edx, dword ptr [ebp - 0x240]
004ca25e 52                       push       edx
004ca25f 6814fd5600               push       0x56fd14
004ca264 8d85a8fdffff             lea        eax, [ebp - 0x258]
004ca26a 50                       push       eax
004ca26b 8b8dbcfdffff             mov        ecx, dword ptr [ebp - 0x244]
004ca271 e86aeefaff               call       0x4790e0
004ca276 90                       nop        
004ca277 eb79                     jmp        0x4ca2f2
004ca279 8b8de0fdffff             mov        ecx, dword ptr [ebp - 0x220]
004ca27f 83b9e406000021           cmp        dword ptr [ecx + 0x6e4], 0x21
004ca286 7f6a                     jg         0x4ca2f2
004ca288 8b95e0fdffff             mov        edx, dword ptr [ebp - 0x220]
004ca28e 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004ca294 e86720f4ff               call       0x40c300
004ca299 8bc8                     mov        ecx, eax
004ca29b e810b7fbff               call       0x4859b0
004ca2a0 8985b4fdffff             mov        dword ptr [ebp - 0x24c], eax
004ca2a6 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004ca2ac 8b8850070000             mov        ecx, dword ptr [eax + 0x750]
004ca2b2 8d94091c010000           lea        edx, [ecx + ecx + 0x11c]
004ca2b9 8995b8fdffff             mov        dword ptr [ebp - 0x248], edx
004ca2bf 6a00                     push       0
004ca2c1 6aff                     push       -1
004ca2c3 51                       push       ecx
004ca2c4 0f57c0                   xorps      xmm0, xmm0
004ca2c7 f30f110424               movss      dword ptr [esp], xmm0
004ca2cc 8d85f0fdffff             lea        eax, [ebp - 0x210]
004ca2d2 50                       push       eax
004ca2d3 8b8db8fdffff             mov        ecx, dword ptr [ebp - 0x248]
004ca2d9 51                       push       ecx
004ca2da 6814fd5600               push       0x56fd14
004ca2df 8d95a4fdffff             lea        edx, [ebp - 0x25c]
004ca2e5 52                       push       edx
004ca2e6 8b8db4fdffff             mov        ecx, dword ptr [ebp - 0x24c]
004ca2ec e8efedfaff               call       0x4790e0
004ca2f1 90                       nop        
004ca2f2 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004ca2f8 50                       push       eax
004ca2f9 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004ca2ff e8dcf3f5ff               call       0x4296e0
004ca304 f30f1085d4fdffff         movss      xmm0, dword ptr [ebp - 0x22c]
004ca30c f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
004ca314 f30f1185d4fdffff         movss      dword ptr [ebp - 0x22c], xmm0
004ca31c 8b8ddcfdffff             mov        ecx, dword ptr [ebp - 0x224]
004ca322 83c101                   add        ecx, 1
004ca325 898ddcfdffff             mov        dword ptr [ebp - 0x224], ecx
004ca32b e90efdffff               jmp        0x4ca03e
004ca330 83bdd8fdffff00           cmp        dword ptr [ebp - 0x228], 0
004ca337 742c                     je         0x4ca365
004ca339 8b95e0fdffff             mov        edx, dword ptr [ebp - 0x220]
004ca33f 8b02                     mov        eax, dword ptr [edx]
004ca341 8b4838                   mov        ecx, dword ptr [eax + 0x38]
004ca344 898db0fdffff             mov        dword ptr [ebp - 0x250], ecx
004ca34a 8b95dcfdffff             mov        edx, dword ptr [ebp - 0x224]
004ca350 52                       push       edx
004ca351 8d85fcfdffff             lea        eax, [ebp - 0x204]
004ca357 50                       push       eax
004ca358 8b8de0fdffff             mov        ecx, dword ptr [ebp - 0x220]
004ca35e ff95b0fdffff             call       dword ptr [ebp - 0x250]
004ca364 90                       nop        
004ca365 8b85d8fdffff             mov        eax, dword ptr [ebp - 0x228]
004ca36b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ca36e 33cd                     xor        ecx, ebp
004ca370 e879860700               call       0x5429ee
004ca375 8be5                     mov        esp, ebp
004ca377 5d                       pop        ebp
004ca378 c21400                   ret        0x14
004ca37b cc                       int3       
004ca37c cc                       int3       
004ca37d cc                       int3       
004ca37e cc                       int3       
004ca37f cc                       int3       
