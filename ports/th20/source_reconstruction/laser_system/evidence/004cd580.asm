004cd580 55                       push       ebp
004cd581 8bec                     mov        ebp, esp
004cd583 81ec74020000             sub        esp, 0x274
004cd589 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004cd58e 33c5                     xor        eax, ebp
004cd590 8945fc                   mov        dword ptr [ebp - 4], eax
004cd593 898dd4fdffff             mov        dword ptr [ebp - 0x22c], ecx
004cd599 837d1c00                 cmp        dword ptr [ebp + 0x1c], 0
004cd59d 7416                     je         0x4cd5b5
004cd59f 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cd5a5 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004cd5ac 7407                     je         0x4cd5b5
004cd5ae 33c0                     xor        eax, eax
004cd5b0 e970040000               jmp        0x4cda25
004cd5b5 c785ccfdffff00000000     mov        dword ptr [ebp - 0x234], 0
004cd5bf c785d0fdffff00000000     mov        dword ptr [ebp - 0x230], 0
004cd5c9 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cd5cf e83c58f5ff               call       0x422e10
004cd5d4 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cd5da e83158f5ff               call       0x422e10
004cd5df 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cd5e5 83c158                   add        ecx, 0x58
004cd5e8 8b11                     mov        edx, dword ptr [ecx]
004cd5ea 8995d8fdffff             mov        dword ptr [ebp - 0x228], edx
004cd5f0 8b4104                   mov        eax, dword ptr [ecx + 4]
004cd5f3 8985dcfdffff             mov        dword ptr [ebp - 0x224], eax
004cd5f9 8b4908                   mov        ecx, dword ptr [ecx + 8]
004cd5fc 898de0fdffff             mov        dword ptr [ebp - 0x220], ecx
004cd602 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cd60a f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cd612 f30f1185c8fdffff         movss      dword ptr [ebp - 0x238], xmm0
004cd61a 6800020000               push       0x200
004cd61f 6a00                     push       0
004cd621 8d95fcfdffff             lea        edx, [ebp - 0x204]
004cd627 52                       push       edx
004cd628 e833710700               call       0x544760
004cd62d 83c40c                   add        esp, 0xc
004cd630 0f57c0                   xorps      xmm0, xmm0
004cd633 f30f1185ecfdffff         movss      dword ptr [ebp - 0x214], xmm0
004cd63b f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cd643 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cd64b 51                       push       ecx
004cd64c f30f110424               movss      dword ptr [esp], xmm0
004cd651 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cd657 51                       push       ecx
004cd658 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004cd65d f30f110424               movss      dword ptr [esp], xmm0
004cd662 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cd668 51                       push       ecx
004cd669 e8c2bcf6ff               call       0x439330
004cd66e 83c40c                   add        esp, 0xc
004cd671 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cd677 83c258                   add        edx, 0x58
004cd67a 8995c0fdffff             mov        dword ptr [ebp - 0x240], edx
004cd680 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004cd686 50                       push       eax
004cd687 8d8d8cfdffff             lea        ecx, [ebp - 0x274]
004cd68d 51                       push       ecx
004cd68e 8b8dc0fdffff             mov        ecx, dword ptr [ebp - 0x240]
004cd694 e8d7bef5ff               call       0x429570
004cd699 8b10                     mov        edx, dword ptr [eax]
004cd69b 8995f0fdffff             mov        dword ptr [ebp - 0x210], edx
004cd6a1 8b4804                   mov        ecx, dword ptr [eax + 4]
004cd6a4 898df4fdffff             mov        dword ptr [ebp - 0x20c], ecx
004cd6aa 8b5008                   mov        edx, dword ptr [eax + 8]
004cd6ad 8995f8fdffff             mov        dword ptr [ebp - 0x208], edx
004cd6b3 0f57c0                   xorps      xmm0, xmm0
004cd6b6 f30f1185f8fdffff         movss      dword ptr [ebp - 0x208], xmm0
004cd6be 8d85e4fdffff             lea        eax, [ebp - 0x21c]
004cd6c4 50                       push       eax
004cd6c5 8d8de4fdffff             lea        ecx, [ebp - 0x21c]
004cd6cb e810c0f5ff               call       0x4296e0
004cd6d0 90                       nop        
004cd6d1 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004cd6d9 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004cd6e1 f30f5885c8fdffff         addss      xmm0, dword ptr [ebp - 0x238]
004cd6e9 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cd6ef f30f104974               movss      xmm1, dword ptr [ecx + 0x74]
004cd6f4 0f2fc8                   comiss     xmm1, xmm0
004cd6f7 0f86ed020000             jbe        0x4cd9ea
004cd6fd 8b5514                   mov        edx, dword ptr [ebp + 0x14]
004cd700 52                       push       edx
004cd701 51                       push       ecx
004cd702 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004cd707 f30f110424               movss      dword ptr [esp], xmm0
004cd70c 51                       push       ecx
004cd70d f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004cd712 f30f110424               movss      dword ptr [esp], xmm0
004cd717 8b4508                   mov        eax, dword ptr [ebp + 8]
004cd71a 51                       push       ecx
004cd71b f30f104004               movss      xmm0, dword ptr [eax + 4]
004cd720 f30f110424               movss      dword ptr [esp], xmm0
004cd725 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004cd728 51                       push       ecx
004cd729 f30f1001                 movss      xmm0, dword ptr [ecx]
004cd72d f30f110424               movss      dword ptr [esp], xmm0
004cd732 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cd738 51                       push       ecx
004cd739 f30f104270               movss      xmm0, dword ptr [edx + 0x70]
004cd73e f30f110424               movss      dword ptr [esp], xmm0
004cd743 51                       push       ecx
004cd744 f30f1085e8fdffff         movss      xmm0, dword ptr [ebp - 0x218]
004cd74c f30f110424               movss      dword ptr [esp], xmm0
004cd751 51                       push       ecx
004cd752 f30f1085e4fdffff         movss      xmm0, dword ptr [ebp - 0x21c]
004cd75a f30f110424               movss      dword ptr [esp], xmm0
004cd75f 51                       push       ecx
004cd760 f30f1085f4fdffff         movss      xmm0, dword ptr [ebp - 0x20c]
004cd768 f30f110424               movss      dword ptr [esp], xmm0
004cd76d 51                       push       ecx
004cd76e f30f1085f0fdffff         movss      xmm0, dword ptr [ebp - 0x210]
004cd776 f30f110424               movss      dword ptr [esp], xmm0
004cd77b e890a4f8ff               call       0x457c10
004cd780 83c428                   add        esp, 0x28
004cd783 0fb6c0                   movzx      eax, al
004cd786 85c0                     test       eax, eax
004cd788 0f841e020000             je         0x4cd9ac
004cd78e 8b8dd0fdffff             mov        ecx, dword ptr [ebp - 0x230]
004cd794 c6840dfcfdffff01         mov        byte ptr [ebp + ecx - 0x204], 1
004cd79c 8b95ccfdffff             mov        edx, dword ptr [ebp - 0x234]
004cd7a2 83c201                   add        edx, 1
004cd7a5 8995ccfdffff             mov        dword ptr [ebp - 0x234], edx
004cd7ab 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cd7b1 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004cd7b7 e844ebf3ff               call       0x40c300
004cd7bc 05886d2800               add        eax, 0x286d88
004cd7c1 8985c4fdffff             mov        dword ptr [ebp - 0x23c], eax
004cd7c7 8b8dc4fdffff             mov        ecx, dword ptr [ebp - 0x23c]
004cd7cd 8b11                     mov        edx, dword ptr [ecx]
004cd7cf 83c201                   add        edx, 1
004cd7d2 8b85c4fdffff             mov        eax, dword ptr [ebp - 0x23c]
004cd7d8 8910                     mov        dword ptr [eax], edx
004cd7da 51                       push       ecx
004cd7db f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
004cd7e3 f30f110424               movss      dword ptr [esp], xmm0
004cd7e8 51                       push       ecx
004cd7e9 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
004cd7f1 f30f110424               movss      dword ptr [esp], xmm0
004cd7f6 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cd7fc 51                       push       ecx
004cd7fd e88e7bfbff               call       0x485390
004cd802 83c40c                   add        esp, 0xc
004cd805 85c0                     test       eax, eax
004cd807 0f859f010000             jne        0x4cd9ac
004cd80d 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cd813 83bae406000011           cmp        dword ptr [edx + 0x6e4], 0x11
004cd81a 7e1e                     jle        0x4cd83a
004cd81c 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cd822 83b8e406000022           cmp        dword ptr [eax + 0x6e4], 0x22
004cd829 740f                     je         0x4cd83a
004cd82b 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cd831 83b9e406000026           cmp        dword ptr [ecx + 0x6e4], 0x26
004cd838 756f                     jne        0x4cd8a9
004cd83a 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cd840 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004cd846 e8b5eaf3ff               call       0x40c300
004cd84b 8bc8                     mov        ecx, eax
004cd84d e85e81fbff               call       0x4859b0
004cd852 8985b8fdffff             mov        dword ptr [ebp - 0x248], eax
004cd858 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cd85e 8b8850070000             mov        ecx, dword ptr [eax + 0x750]
004cd864 8d9409d4000000           lea        edx, [ecx + ecx + 0xd4]
004cd86b 8995bcfdffff             mov        dword ptr [ebp - 0x244], edx
004cd871 6a00                     push       0
004cd873 6aff                     push       -1
004cd875 51                       push       ecx
004cd876 0f57c0                   xorps      xmm0, xmm0
004cd879 f30f110424               movss      dword ptr [esp], xmm0
004cd87e 8d85f0fdffff             lea        eax, [ebp - 0x210]
004cd884 50                       push       eax
004cd885 8b8dbcfdffff             mov        ecx, dword ptr [ebp - 0x244]
004cd88b 51                       push       ecx
004cd88c 6814fd5600               push       0x56fd14
004cd891 8d95a0fdffff             lea        edx, [ebp - 0x260]
004cd897 52                       push       edx
004cd898 8b8db8fdffff             mov        ecx, dword ptr [ebp - 0x248]
004cd89e e83db8faff               call       0x4790e0
004cd8a3 90                       nop        
004cd8a4 e903010000               jmp        0x4cd9ac
004cd8a9 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cd8af 83b8e40600001f           cmp        dword ptr [eax + 0x6e4], 0x1f
004cd8b6 7e0f                     jle        0x4cd8c7
004cd8b8 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cd8be 83b9e40600001b           cmp        dword ptr [ecx + 0x6e4], 0x1b
004cd8c5 756c                     jne        0x4cd933
004cd8c7 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cd8cd 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004cd8d3 e828eaf3ff               call       0x40c300
004cd8d8 8bc8                     mov        ecx, eax
004cd8da e8d180fbff               call       0x4859b0
004cd8df 8985b0fdffff             mov        dword ptr [ebp - 0x250], eax
004cd8e5 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cd8eb 8b8850070000             mov        ecx, dword ptr [eax + 0x750]
004cd8f1 8d940904010000           lea        edx, [ecx + ecx + 0x104]
004cd8f8 8995b4fdffff             mov        dword ptr [ebp - 0x24c], edx
004cd8fe 6a00                     push       0
004cd900 6aff                     push       -1
004cd902 51                       push       ecx
004cd903 0f57c0                   xorps      xmm0, xmm0
004cd906 f30f110424               movss      dword ptr [esp], xmm0
004cd90b 8d85f0fdffff             lea        eax, [ebp - 0x210]
004cd911 50                       push       eax
004cd912 8b8db4fdffff             mov        ecx, dword ptr [ebp - 0x24c]
004cd918 51                       push       ecx
004cd919 6814fd5600               push       0x56fd14
004cd91e 8d959cfdffff             lea        edx, [ebp - 0x264]
004cd924 52                       push       edx
004cd925 8b8db0fdffff             mov        ecx, dword ptr [ebp - 0x250]
004cd92b e8b0b7faff               call       0x4790e0
004cd930 90                       nop        
004cd931 eb79                     jmp        0x4cd9ac
004cd933 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cd939 83b8e406000021           cmp        dword ptr [eax + 0x6e4], 0x21
004cd940 7f6a                     jg         0x4cd9ac
004cd942 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cd948 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004cd94e e8ade9f3ff               call       0x40c300
004cd953 8bc8                     mov        ecx, eax
004cd955 e85680fbff               call       0x4859b0
004cd95a 8985a8fdffff             mov        dword ptr [ebp - 0x258], eax
004cd960 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cd966 8b8250070000             mov        eax, dword ptr [edx + 0x750]
004cd96c 8d8c001c010000           lea        ecx, [eax + eax + 0x11c]
004cd973 898dacfdffff             mov        dword ptr [ebp - 0x254], ecx
004cd979 6a00                     push       0
004cd97b 6aff                     push       -1
004cd97d 51                       push       ecx
004cd97e 0f57c0                   xorps      xmm0, xmm0
004cd981 f30f110424               movss      dword ptr [esp], xmm0
004cd986 8d95f0fdffff             lea        edx, [ebp - 0x210]
004cd98c 52                       push       edx
004cd98d 8b85acfdffff             mov        eax, dword ptr [ebp - 0x254]
004cd993 50                       push       eax
004cd994 6814fd5600               push       0x56fd14
004cd999 8d8d98fdffff             lea        ecx, [ebp - 0x268]
004cd99f 51                       push       ecx
004cd9a0 8b8da8fdffff             mov        ecx, dword ptr [ebp - 0x258]
004cd9a6 e835b7faff               call       0x4790e0
004cd9ab 90                       nop        
004cd9ac 8d95e4fdffff             lea        edx, [ebp - 0x21c]
004cd9b2 52                       push       edx
004cd9b3 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cd9b9 e822bdf5ff               call       0x4296e0
004cd9be f30f1085c8fdffff         movss      xmm0, dword ptr [ebp - 0x238]
004cd9c6 f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
004cd9ce f30f1185c8fdffff         movss      dword ptr [ebp - 0x238], xmm0
004cd9d6 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004cd9dc 83c001                   add        eax, 1
004cd9df 8985d0fdffff             mov        dword ptr [ebp - 0x230], eax
004cd9e5 e9e7fcffff               jmp        0x4cd6d1
004cd9ea 83bdccfdffff00           cmp        dword ptr [ebp - 0x234], 0
004cd9f1 742c                     je         0x4cda1f
004cd9f3 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cd9f9 8b11                     mov        edx, dword ptr [ecx]
004cd9fb 8b4238                   mov        eax, dword ptr [edx + 0x38]
004cd9fe 8985a4fdffff             mov        dword ptr [ebp - 0x25c], eax
004cda04 8b8dd0fdffff             mov        ecx, dword ptr [ebp - 0x230]
004cda0a 51                       push       ecx
004cda0b 8d95fcfdffff             lea        edx, [ebp - 0x204]
004cda11 52                       push       edx
004cda12 8b8dd4fdffff             mov        ecx, dword ptr [ebp - 0x22c]
004cda18 ff95a4fdffff             call       dword ptr [ebp - 0x25c]
004cda1e 90                       nop        
004cda1f 8b85ccfdffff             mov        eax, dword ptr [ebp - 0x234]
004cda25 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cda28 33cd                     xor        ecx, ebp
004cda2a e8bf4f0700               call       0x5429ee
004cda2f 8be5                     mov        esp, ebp
004cda31 5d                       pop        ebp
004cda32 c21800                   ret        0x18
004cda35 cc                       int3       
004cda36 cc                       int3       
004cda37 cc                       int3       
004cda38 cc                       int3       
004cda39 cc                       int3       
004cda3a cc                       int3       
004cda3b cc                       int3       
004cda3c cc                       int3       
004cda3d cc                       int3       
004cda3e cc                       int3       
004cda3f cc                       int3       
