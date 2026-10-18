004cf080 55                       push       ebp
004cf081 8bec                     mov        ebp, esp
004cf083 81ec74020000             sub        esp, 0x274
004cf089 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004cf08e 33c5                     xor        eax, ebp
004cf090 8945fc                   mov        dword ptr [ebp - 4], eax
004cf093 898dd4fdffff             mov        dword ptr [ebp - 0x22c], ecx
004cf099 837d2000                 cmp        dword ptr [ebp + 0x20], 0
004cf09d 7416                     je         0x4cf0b5
004cf09f 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cf0a5 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004cf0ac 7407                     je         0x4cf0b5
004cf0ae 33c0                     xor        eax, eax
004cf0b0 e984040000               jmp        0x4cf539
004cf0b5 c785d0fdffff00000000     mov        dword ptr [ebp - 0x230], 0
004cf0bf c785ccfdffff00000000     mov        dword ptr [ebp - 0x234], 0
004cf0c9 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cf0cf e83c3df5ff               call       0x422e10
004cf0d4 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cf0da e8313df5ff               call       0x422e10
004cf0df 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cf0e5 83c158                   add        ecx, 0x58
004cf0e8 8b11                     mov        edx, dword ptr [ecx]
004cf0ea 8995d8fdffff             mov        dword ptr [ebp - 0x228], edx
004cf0f0 8b4104                   mov        eax, dword ptr [ecx + 4]
004cf0f3 8985dcfdffff             mov        dword ptr [ebp - 0x224], eax
004cf0f9 8b4908                   mov        ecx, dword ptr [ecx + 8]
004cf0fc 898de0fdffff             mov        dword ptr [ebp - 0x220], ecx
004cf102 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cf10a f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cf112 f30f1185c8fdffff         movss      dword ptr [ebp - 0x238], xmm0
004cf11a 6800020000               push       0x200
004cf11f 6a00                     push       0
004cf121 8d95fcfdffff             lea        edx, [ebp - 0x204]
004cf127 52                       push       edx
004cf128 e833560700               call       0x544760
004cf12d 83c40c                   add        esp, 0xc
004cf130 0f57c0                   xorps      xmm0, xmm0
004cf133 f30f1185ecfdffff         movss      dword ptr [ebp - 0x214], xmm0
004cf13b f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cf143 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cf14b 51                       push       ecx
004cf14c f30f110424               movss      dword ptr [esp], xmm0
004cf151 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cf157 51                       push       ecx
004cf158 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004cf15d f30f110424               movss      dword ptr [esp], xmm0
004cf162 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cf168 51                       push       ecx
004cf169 e8c2a1f6ff               call       0x439330
004cf16e 83c40c                   add        esp, 0xc
004cf171 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cf177 83c258                   add        edx, 0x58
004cf17a 8995c0fdffff             mov        dword ptr [ebp - 0x240], edx
004cf180 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004cf186 50                       push       eax
004cf187 8d8d8cfdffff             lea        ecx, [ebp - 0x274]
004cf18d 51                       push       ecx
004cf18e 8b8dc0fdffff             mov        ecx, dword ptr [ebp - 0x240]
004cf194 e8d7a3f5ff               call       0x429570
004cf199 8b10                     mov        edx, dword ptr [eax]
004cf19b 8995f0fdffff             mov        dword ptr [ebp - 0x210], edx
004cf1a1 8b4804                   mov        ecx, dword ptr [eax + 4]
004cf1a4 898df4fdffff             mov        dword ptr [ebp - 0x20c], ecx
004cf1aa 8b5008                   mov        edx, dword ptr [eax + 8]
004cf1ad 8995f8fdffff             mov        dword ptr [ebp - 0x208], edx
004cf1b3 0f57c0                   xorps      xmm0, xmm0
004cf1b6 f30f1185f8fdffff         movss      dword ptr [ebp - 0x208], xmm0
004cf1be 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004cf1c4 50                       push       eax
004cf1c5 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cf1cb e810a5f5ff               call       0x4296e0
004cf1d0 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004cf1d5 f30f59450c               mulss      xmm0, dword ptr [ebp + 0xc]
004cf1da f30f11450c               movss      dword ptr [ebp + 0xc], xmm0
004cf1df f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cf1e7 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cf1ef f30f5885c8fdffff         addss      xmm0, dword ptr [ebp - 0x238]
004cf1f7 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cf1fd f30f104974               movss      xmm1, dword ptr [ecx + 0x74]
004cf202 0f2fc8                   comiss     xmm1, xmm0
004cf205 0f82c5020000             jb         0x4cf4d0
004cf20b 8b5518                   mov        edx, dword ptr [ebp + 0x18]
004cf20e 52                       push       edx
004cf20f 51                       push       ecx
004cf210 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004cf215 f30f110424               movss      dword ptr [esp], xmm0
004cf21a 51                       push       ecx
004cf21b f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004cf220 f30f110424               movss      dword ptr [esp], xmm0
004cf225 51                       push       ecx
004cf226 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004cf22b f30f110424               movss      dword ptr [esp], xmm0
004cf230 8b4508                   mov        eax, dword ptr [ebp + 8]
004cf233 51                       push       ecx
004cf234 f30f104004               movss      xmm0, dword ptr [eax + 4]
004cf239 f30f110424               movss      dword ptr [esp], xmm0
004cf23e 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004cf241 51                       push       ecx
004cf242 f30f1001                 movss      xmm0, dword ptr [ecx]
004cf246 f30f110424               movss      dword ptr [esp], xmm0
004cf24b 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cf251 51                       push       ecx
004cf252 f30f104270               movss      xmm0, dword ptr [edx + 0x70]
004cf257 f30f110424               movss      dword ptr [esp], xmm0
004cf25c 51                       push       ecx
004cf25d f30f1085e8fdffff         movss      xmm0, dword ptr [ebp - 0x218]
004cf265 f30f110424               movss      dword ptr [esp], xmm0
004cf26a 51                       push       ecx
004cf26b f30f1085e4fdffff         movss      xmm0, dword ptr [ebp - 0x21c]
004cf273 f30f110424               movss      dword ptr [esp], xmm0
004cf278 51                       push       ecx
004cf279 f30f1085f4fdffff         movss      xmm0, dword ptr [ebp - 0x20c]
004cf281 f30f110424               movss      dword ptr [esp], xmm0
004cf286 51                       push       ecx
004cf287 f30f1085f0fdffff         movss      xmm0, dword ptr [ebp - 0x210]
004cf28f f30f110424               movss      dword ptr [esp], xmm0
004cf294 e8d793f8ff               call       0x458670
004cf299 83c42c                   add        esp, 0x2c
004cf29c 0fb6c0                   movzx      eax, al
004cf29f 85c0                     test       eax, eax
004cf2a1 0f84eb010000             je         0x4cf492
004cf2a7 8b8dccfdffff             mov        ecx, dword ptr [ebp - 0x234]
004cf2ad c6840dfcfdffff01         mov        byte ptr [ebp + ecx - 0x204], 1
004cf2b5 8b95d0fdffff             mov        edx, dword ptr [ebp - 0x230]
004cf2bb 83c201                   add        edx, 1
004cf2be 8995d0fdffff             mov        dword ptr [ebp - 0x230], edx
004cf2c4 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cf2ca 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004cf2d0 e82bd0f3ff               call       0x40c300
004cf2d5 05886d2800               add        eax, 0x286d88
004cf2da 8985c4fdffff             mov        dword ptr [ebp - 0x23c], eax
004cf2e0 8b8dc4fdffff             mov        ecx, dword ptr [ebp - 0x23c]
004cf2e6 8b11                     mov        edx, dword ptr [ecx]
004cf2e8 83c201                   add        edx, 1
004cf2eb 8b85c4fdffff             mov        eax, dword ptr [ebp - 0x23c]
004cf2f1 8910                     mov        dword ptr [eax], edx
004cf2f3 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cf2f9 83b9e406000011           cmp        dword ptr [ecx + 0x6e4], 0x11
004cf300 7e1e                     jle        0x4cf320
004cf302 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cf308 83bae406000022           cmp        dword ptr [edx + 0x6e4], 0x22
004cf30f 740f                     je         0x4cf320
004cf311 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cf317 83b8e406000026           cmp        dword ptr [eax + 0x6e4], 0x26
004cf31e 756f                     jne        0x4cf38f
004cf320 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cf326 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004cf32c e8cfcff3ff               call       0x40c300
004cf331 8bc8                     mov        ecx, eax
004cf333 e87866fbff               call       0x4859b0
004cf338 8985b8fdffff             mov        dword ptr [ebp - 0x248], eax
004cf33e 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cf344 8b8220070000             mov        eax, dword ptr [edx + 0x720]
004cf34a 8d8c00d4000000           lea        ecx, [eax + eax + 0xd4]
004cf351 898dbcfdffff             mov        dword ptr [ebp - 0x244], ecx
004cf357 6a00                     push       0
004cf359 6aff                     push       -1
004cf35b 51                       push       ecx
004cf35c 0f57c0                   xorps      xmm0, xmm0
004cf35f f30f110424               movss      dword ptr [esp], xmm0
004cf364 8d95f0fdffff             lea        edx, [ebp - 0x210]
004cf36a 52                       push       edx
004cf36b 8b85bcfdffff             mov        eax, dword ptr [ebp - 0x244]
004cf371 50                       push       eax
004cf372 6814fd5600               push       0x56fd14
004cf377 8d8da0fdffff             lea        ecx, [ebp - 0x260]
004cf37d 51                       push       ecx
004cf37e 8b8db8fdffff             mov        ecx, dword ptr [ebp - 0x248]
004cf384 e8579dfaff               call       0x4790e0
004cf389 90                       nop        
004cf38a e903010000               jmp        0x4cf492
004cf38f 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cf395 83bae40600001f           cmp        dword ptr [edx + 0x6e4], 0x1f
004cf39c 7e0f                     jle        0x4cf3ad
004cf39e 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cf3a4 83b8e40600001b           cmp        dword ptr [eax + 0x6e4], 0x1b
004cf3ab 756c                     jne        0x4cf419
004cf3ad 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cf3b3 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004cf3b9 e842cff3ff               call       0x40c300
004cf3be 8bc8                     mov        ecx, eax
004cf3c0 e8eb65fbff               call       0x4859b0
004cf3c5 8985b0fdffff             mov        dword ptr [ebp - 0x250], eax
004cf3cb 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cf3d1 8b8220070000             mov        eax, dword ptr [edx + 0x720]
004cf3d7 8d8c0004010000           lea        ecx, [eax + eax + 0x104]
004cf3de 898db4fdffff             mov        dword ptr [ebp - 0x24c], ecx
004cf3e4 6a00                     push       0
004cf3e6 6aff                     push       -1
004cf3e8 51                       push       ecx
004cf3e9 0f57c0                   xorps      xmm0, xmm0
004cf3ec f30f110424               movss      dword ptr [esp], xmm0
004cf3f1 8d95f0fdffff             lea        edx, [ebp - 0x210]
004cf3f7 52                       push       edx
004cf3f8 8b85b4fdffff             mov        eax, dword ptr [ebp - 0x24c]
004cf3fe 50                       push       eax
004cf3ff 6814fd5600               push       0x56fd14
004cf404 8d8d9cfdffff             lea        ecx, [ebp - 0x264]
004cf40a 51                       push       ecx
004cf40b 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004cf411 e8ca9cfaff               call       0x4790e0
004cf416 90                       nop        
004cf417 eb79                     jmp        0x4cf492
004cf419 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cf41f 83bae406000021           cmp        dword ptr [edx + 0x6e4], 0x21
004cf426 7f6a                     jg         0x4cf492
004cf428 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cf42e 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004cf434 e8c7cef3ff               call       0x40c300
004cf439 8bc8                     mov        ecx, eax
004cf43b e87065fbff               call       0x4859b0
004cf440 8985a8fdffff             mov        dword ptr [ebp - 0x258], eax
004cf446 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cf44c 8b9120070000             mov        edx, dword ptr [ecx + 0x720]
004cf452 8d84121c010000           lea        eax, [edx + edx + 0x11c]
004cf459 8985acfdffff             mov        dword ptr [ebp - 0x254], eax
004cf45f 6a00                     push       0
004cf461 6aff                     push       -1
004cf463 51                       push       ecx
004cf464 0f57c0                   xorps      xmm0, xmm0
004cf467 f30f110424               movss      dword ptr [esp], xmm0
004cf46c 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cf472 51                       push       ecx
004cf473 8b95acfdffff             mov        edx, dword ptr [ebp - 0x254]
004cf479 52                       push       edx
004cf47a 6814fd5600               push       0x56fd14
004cf47f 8d8598fdffff             lea        eax, [ebp - 0x268]
004cf485 50                       push       eax
004cf486 8b8da8fdffff             mov        ecx, dword ptr [ebp - 0x258]
004cf48c e84f9cfaff               call       0x4790e0
004cf491 90                       nop        
004cf492 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cf498 51                       push       ecx
004cf499 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cf49f e83ca2f5ff               call       0x4296e0
004cf4a4 f30f1085c8fdffff         movss      xmm0, dword ptr [ebp - 0x238]
004cf4ac f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
004cf4b4 f30f1185c8fdffff         movss      dword ptr [ebp - 0x238], xmm0
004cf4bc 8b95ccfdffff             mov        edx, dword ptr [ebp - 0x234]
004cf4c2 83c201                   add        edx, 1
004cf4c5 8995ccfdffff             mov        dword ptr [ebp - 0x234], edx
004cf4cb e90ffdffff               jmp        0x4cf1df
004cf4d0 83bdd0fdffff00           cmp        dword ptr [ebp - 0x230], 0
004cf4d7 745a                     je         0x4cf533
004cf4d9 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004cf4df 3b85ccfdffff             cmp        eax, dword ptr [ebp - 0x234]
004cf4e5 7c20                     jl         0x4cf507
004cf4e7 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cf4ed 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
004cf4f0 83e2f9                   and        edx, 0xfffffff9
004cf4f3 83ca02                   or         edx, 2
004cf4f6 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cf4fc 89501c                   mov        dword ptr [eax + 0x1c], edx
004cf4ff 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004cf505 eb32                     jmp        0x4cf539
004cf507 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cf50d 8b11                     mov        edx, dword ptr [ecx]
004cf50f 8b4238                   mov        eax, dword ptr [edx + 0x38]
004cf512 8985a4fdffff             mov        dword ptr [ebp - 0x25c], eax
004cf518 8b8dccfdffff             mov        ecx, dword ptr [ebp - 0x234]
004cf51e 51                       push       ecx
004cf51f 8d95fcfdffff             lea        edx, [ebp - 0x204]
004cf525 52                       push       edx
004cf526 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cf52c ff95a4fdffff             call       dword ptr [ebp - 0x25c]
004cf532 90                       nop        
004cf533 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004cf539 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cf53c 33cd                     xor        ecx, ebp
004cf53e e8ab340700               call       0x5429ee
004cf543 8be5                     mov        esp, ebp
004cf545 5d                       pop        ebp
004cf546 c21c00                   ret        0x1c
004cf549 cc                       int3       
004cf54a cc                       int3       
004cf54b cc                       int3       
004cf54c cc                       int3       
004cf54d cc                       int3       
004cf54e cc                       int3       
004cf54f cc                       int3       
