004ca380 55                       push       ebp
004ca381 8bec                     mov        ebp, esp
004ca383 81ec74020000             sub        esp, 0x274
004ca389 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004ca38e 33c5                     xor        eax, ebp
004ca390 8945fc                   mov        dword ptr [ebp - 4], eax
004ca393 898dd4fdffff             mov        dword ptr [ebp - 0x22c], ecx
004ca399 837d1800                 cmp        dword ptr [ebp + 0x18], 0
004ca39d 7416                     je         0x4ca3b5
004ca39f 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004ca3a5 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004ca3ac 7407                     je         0x4ca3b5
004ca3ae 33c0                     xor        eax, eax
004ca3b0 e968040000               jmp        0x4ca81d
004ca3b5 c785d0fdffff00000000     mov        dword ptr [ebp - 0x230], 0
004ca3bf c785ccfdffff00000000     mov        dword ptr [ebp - 0x234], 0
004ca3c9 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004ca3cf e83c8af5ff               call       0x422e10
004ca3d4 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004ca3da e8318af5ff               call       0x422e10
004ca3df 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ca3e5 83c158                   add        ecx, 0x58
004ca3e8 8b11                     mov        edx, dword ptr [ecx]
004ca3ea 8995d8fdffff             mov        dword ptr [ebp - 0x228], edx
004ca3f0 8b4104                   mov        eax, dword ptr [ecx + 4]
004ca3f3 8985dcfdffff             mov        dword ptr [ebp - 0x224], eax
004ca3f9 8b4908                   mov        ecx, dword ptr [ecx + 8]
004ca3fc 898de0fdffff             mov        dword ptr [ebp - 0x220], ecx
004ca402 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004ca40a f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004ca412 f30f1185c8fdffff         movss      dword ptr [ebp - 0x238], xmm0
004ca41a 6800020000               push       0x200
004ca41f 6a00                     push       0
004ca421 8d95fcfdffff             lea        edx, [ebp - 0x204]
004ca427 52                       push       edx
004ca428 e833a30700               call       0x544760
004ca42d 83c40c                   add        esp, 0xc
004ca430 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004ca438 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004ca440 51                       push       ecx
004ca441 f30f110424               movss      dword ptr [esp], xmm0
004ca446 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004ca44c 51                       push       ecx
004ca44d f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004ca452 f30f110424               movss      dword ptr [esp], xmm0
004ca457 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004ca45d 51                       push       ecx
004ca45e e8cdeef6ff               call       0x439330
004ca463 83c40c                   add        esp, 0xc
004ca466 0f57c0                   xorps      xmm0, xmm0
004ca469 f30f1185ecfdffff         movss      dword ptr [ebp - 0x214], xmm0
004ca471 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004ca477 83c258                   add        edx, 0x58
004ca47a 8995c0fdffff             mov        dword ptr [ebp - 0x240], edx
004ca480 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004ca486 50                       push       eax
004ca487 8d8d8cfdffff             lea        ecx, [ebp - 0x274]
004ca48d 51                       push       ecx
004ca48e 8b8dc0fdffff             mov        ecx, dword ptr [ebp - 0x240]
004ca494 e8d7f0f5ff               call       0x429570
004ca499 8b10                     mov        edx, dword ptr [eax]
004ca49b 8995f0fdffff             mov        dword ptr [ebp - 0x210], edx
004ca4a1 8b4804                   mov        ecx, dword ptr [eax + 4]
004ca4a4 898df4fdffff             mov        dword ptr [ebp - 0x20c], ecx
004ca4aa 8b5008                   mov        edx, dword ptr [eax + 8]
004ca4ad 8995f8fdffff             mov        dword ptr [ebp - 0x208], edx
004ca4b3 0f57c0                   xorps      xmm0, xmm0
004ca4b6 f30f1185f8fdffff         movss      dword ptr [ebp - 0x208], xmm0
004ca4be 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004ca4c4 50                       push       eax
004ca4c5 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004ca4cb e810f2f5ff               call       0x4296e0
004ca4d0 90                       nop        
004ca4d1 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004ca4d9 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004ca4e1 f30f5885c8fdffff         addss      xmm0, dword ptr [ebp - 0x238]
004ca4e9 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ca4ef f30f104974               movss      xmm1, dword ptr [ecx + 0x74]
004ca4f4 0f2fc8                   comiss     xmm1, xmm0
004ca4f7 0f82b7020000             jb         0x4ca7b4
004ca4fd 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004ca503 51                       push       ecx
004ca504 f30f104270               movss      xmm0, dword ptr [edx + 0x70]
004ca509 f30f110424               movss      dword ptr [esp], xmm0
004ca50e 51                       push       ecx
004ca50f f30f1085e8fdffff         movss      xmm0, dword ptr [ebp - 0x218]
004ca517 f30f110424               movss      dword ptr [esp], xmm0
004ca51c 51                       push       ecx
004ca51d f30f1085e4fdffff         movss      xmm0, dword ptr [ebp - 0x21c]
004ca525 f30f110424               movss      dword ptr [esp], xmm0
004ca52a 51                       push       ecx
004ca52b f30f1085f4fdffff         movss      xmm0, dword ptr [ebp - 0x20c]
004ca533 f30f110424               movss      dword ptr [esp], xmm0
004ca538 51                       push       ecx
004ca539 f30f1085f0fdffff         movss      xmm0, dword ptr [ebp - 0x210]
004ca541 f30f110424               movss      dword ptr [esp], xmm0
004ca546 51                       push       ecx
004ca547 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004ca54c f30f110424               movss      dword ptr [esp], xmm0
004ca551 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004ca554 51                       push       ecx
004ca555 f30f104004               movss      xmm0, dword ptr [eax + 4]
004ca55a f30f110424               movss      dword ptr [esp], xmm0
004ca55f 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004ca562 51                       push       ecx
004ca563 f30f1001                 movss      xmm0, dword ptr [ecx]
004ca567 f30f110424               movss      dword ptr [esp], xmm0
004ca56c 8b5508                   mov        edx, dword ptr [ebp + 8]
004ca56f 51                       push       ecx
004ca570 f30f104204               movss      xmm0, dword ptr [edx + 4]
004ca575 f30f110424               movss      dword ptr [esp], xmm0
004ca57a 8b4508                   mov        eax, dword ptr [ebp + 8]
004ca57d 51                       push       ecx
004ca57e f30f1000                 movss      xmm0, dword ptr [eax]
004ca582 f30f110424               movss      dword ptr [esp], xmm0
004ca587 e834dbf8ff               call       0x4580c0
004ca58c 83c428                   add        esp, 0x28
004ca58f 0fb6c8                   movzx      ecx, al
004ca592 85c9                     test       ecx, ecx
004ca594 0f84dc010000             je         0x4ca776
004ca59a 8b95ccfdffff             mov        edx, dword ptr [ebp - 0x234]
004ca5a0 c68415fcfdffff01         mov        byte ptr [ebp + edx - 0x204], 1
004ca5a8 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004ca5ae 83c001                   add        eax, 1
004ca5b1 8985d0fdffff             mov        dword ptr [ebp - 0x230], eax
004ca5b7 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ca5bd 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004ca5c3 e8381df4ff               call       0x40c300
004ca5c8 05886d2800               add        eax, 0x286d88
004ca5cd 8985c4fdffff             mov        dword ptr [ebp - 0x23c], eax
004ca5d3 8b95c4fdffff             mov        edx, dword ptr [ebp - 0x23c]
004ca5d9 8b02                     mov        eax, dword ptr [edx]
004ca5db 83c001                   add        eax, 1
004ca5de 8b8dc4fdffff             mov        ecx, dword ptr [ebp - 0x23c]
004ca5e4 8901                     mov        dword ptr [ecx], eax
004ca5e6 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004ca5ec 83bae406000011           cmp        dword ptr [edx + 0x6e4], 0x11
004ca5f3 7e1e                     jle        0x4ca613
004ca5f5 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004ca5fb 83b8e406000022           cmp        dword ptr [eax + 0x6e4], 0x22
004ca602 740f                     je         0x4ca613
004ca604 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ca60a 83b9e406000026           cmp        dword ptr [ecx + 0x6e4], 0x26
004ca611 756f                     jne        0x4ca682
004ca613 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004ca619 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004ca61f e8dc1cf4ff               call       0x40c300
004ca624 8bc8                     mov        ecx, eax
004ca626 e885b3fbff               call       0x4859b0
004ca62b 8985b8fdffff             mov        dword ptr [ebp - 0x248], eax
004ca631 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004ca637 8b8820070000             mov        ecx, dword ptr [eax + 0x720]
004ca63d 8d9409d4000000           lea        edx, [ecx + ecx + 0xd4]
004ca644 8995bcfdffff             mov        dword ptr [ebp - 0x244], edx
004ca64a 6a00                     push       0
004ca64c 6aff                     push       -1
004ca64e 51                       push       ecx
004ca64f 0f57c0                   xorps      xmm0, xmm0
004ca652 f30f110424               movss      dword ptr [esp], xmm0
004ca657 8d85f0fdffff             lea        eax, [ebp - 0x210]
004ca65d 50                       push       eax
004ca65e 8b8dbcfdffff             mov        ecx, dword ptr [ebp - 0x244]
004ca664 51                       push       ecx
004ca665 6814fd5600               push       0x56fd14
004ca66a 8d95a0fdffff             lea        edx, [ebp - 0x260]
004ca670 52                       push       edx
004ca671 8b8db8fdffff             mov        ecx, dword ptr [ebp - 0x248]
004ca677 e864eafaff               call       0x4790e0
004ca67c 90                       nop        
004ca67d e9f4000000               jmp        0x4ca776
004ca682 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004ca688 83b8e40600001e           cmp        dword ptr [eax + 0x6e4], 0x1e
004ca68f 7f6c                     jg         0x4ca6fd
004ca691 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ca697 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004ca69d e85e1cf4ff               call       0x40c300
004ca6a2 8bc8                     mov        ecx, eax
004ca6a4 e807b3fbff               call       0x4859b0
004ca6a9 8985b0fdffff             mov        dword ptr [ebp - 0x250], eax
004ca6af 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004ca6b5 8b8220070000             mov        eax, dword ptr [edx + 0x720]
004ca6bb 8d8c0004010000           lea        ecx, [eax + eax + 0x104]
004ca6c2 898db4fdffff             mov        dword ptr [ebp - 0x24c], ecx
004ca6c8 6a00                     push       0
004ca6ca 6aff                     push       -1
004ca6cc 51                       push       ecx
004ca6cd 0f57c0                   xorps      xmm0, xmm0
004ca6d0 f30f110424               movss      dword ptr [esp], xmm0
004ca6d5 8d95f0fdffff             lea        edx, [ebp - 0x210]
004ca6db 52                       push       edx
004ca6dc 8b85b4fdffff             mov        eax, dword ptr [ebp - 0x24c]
004ca6e2 50                       push       eax
004ca6e3 6814fd5600               push       0x56fd14
004ca6e8 8d8d9cfdffff             lea        ecx, [ebp - 0x264]
004ca6ee 51                       push       ecx
004ca6ef 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004ca6f5 e8e6e9faff               call       0x4790e0
004ca6fa 90                       nop        
004ca6fb eb79                     jmp        0x4ca776
004ca6fd 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004ca703 83bae406000021           cmp        dword ptr [edx + 0x6e4], 0x21
004ca70a 7f6a                     jg         0x4ca776
004ca70c 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004ca712 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004ca718 e8e31bf4ff               call       0x40c300
004ca71d 8bc8                     mov        ecx, eax
004ca71f e88cb2fbff               call       0x4859b0
004ca724 8985a8fdffff             mov        dword ptr [ebp - 0x258], eax
004ca72a 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ca730 8b9120070000             mov        edx, dword ptr [ecx + 0x720]
004ca736 8d84121c010000           lea        eax, [edx + edx + 0x11c]
004ca73d 8985acfdffff             mov        dword ptr [ebp - 0x254], eax
004ca743 6a00                     push       0
004ca745 6aff                     push       -1
004ca747 51                       push       ecx
004ca748 0f57c0                   xorps      xmm0, xmm0
004ca74b f30f110424               movss      dword ptr [esp], xmm0
004ca750 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004ca756 51                       push       ecx
004ca757 8b95acfdffff             mov        edx, dword ptr [ebp - 0x254]
004ca75d 52                       push       edx
004ca75e 6814fd5600               push       0x56fd14
004ca763 8d8598fdffff             lea        eax, [ebp - 0x268]
004ca769 50                       push       eax
004ca76a 8b8da8fdffff             mov        ecx, dword ptr [ebp - 0x258]
004ca770 e86be9faff               call       0x4790e0
004ca775 90                       nop        
004ca776 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004ca77c 51                       push       ecx
004ca77d 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004ca783 e858eff5ff               call       0x4296e0
004ca788 f30f1085c8fdffff         movss      xmm0, dword ptr [ebp - 0x238]
004ca790 f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
004ca798 f30f1185c8fdffff         movss      dword ptr [ebp - 0x238], xmm0
004ca7a0 8b95ccfdffff             mov        edx, dword ptr [ebp - 0x234]
004ca7a6 83c201                   add        edx, 1
004ca7a9 8995ccfdffff             mov        dword ptr [ebp - 0x234], edx
004ca7af e91dfdffff               jmp        0x4ca4d1
004ca7b4 83bdd0fdffff00           cmp        dword ptr [ebp - 0x230], 0
004ca7bb 745a                     je         0x4ca817
004ca7bd 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004ca7c3 3b85ccfdffff             cmp        eax, dword ptr [ebp - 0x234]
004ca7c9 7c20                     jl         0x4ca7eb
004ca7cb 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ca7d1 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
004ca7d4 83e2f9                   and        edx, 0xfffffff9
004ca7d7 83ca02                   or         edx, 2
004ca7da 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004ca7e0 89501c                   mov        dword ptr [eax + 0x1c], edx
004ca7e3 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004ca7e9 eb32                     jmp        0x4ca81d
004ca7eb 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ca7f1 8b11                     mov        edx, dword ptr [ecx]
004ca7f3 8b4238                   mov        eax, dword ptr [edx + 0x38]
004ca7f6 8985a4fdffff             mov        dword ptr [ebp - 0x25c], eax
004ca7fc 8b8dccfdffff             mov        ecx, dword ptr [ebp - 0x234]
004ca802 51                       push       ecx
004ca803 8d95fcfdffff             lea        edx, [ebp - 0x204]
004ca809 52                       push       edx
004ca80a 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004ca810 ff95a4fdffff             call       dword ptr [ebp - 0x25c]
004ca816 90                       nop        
004ca817 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004ca81d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ca820 33cd                     xor        ecx, ebp
004ca822 e8c7810700               call       0x5429ee
004ca827 8be5                     mov        esp, ebp
004ca829 5d                       pop        ebp
004ca82a c21400                   ret        0x14
004ca82d cc                       int3       
004ca82e cc                       int3       
004ca82f cc                       int3       
