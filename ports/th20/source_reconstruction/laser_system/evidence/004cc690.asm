004cc690 55                       push       ebp
004cc691 8bec                     mov        ebp, esp
004cc693 81ec40020000             sub        esp, 0x240
004cc699 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004cc69e 33c5                     xor        eax, ebp
004cc6a0 8945fc                   mov        dword ptr [ebp - 4], eax
004cc6a3 898de0fdffff             mov        dword ptr [ebp - 0x220], ecx
004cc6a9 837d1c00                 cmp        dword ptr [ebp + 0x1c], 0
004cc6ad 7416                     je         0x4cc6c5
004cc6af 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004cc6b5 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004cc6bc 7407                     je         0x4cc6c5
004cc6be 33c0                     xor        eax, eax
004cc6c0 e96d020000               jmp        0x4cc932
004cc6c5 c785d8fdffff00000000     mov        dword ptr [ebp - 0x228], 0
004cc6cf c785dcfdffff00000000     mov        dword ptr [ebp - 0x224], 0
004cc6d9 8d8df0fdffff             lea        ecx, [ebp - 0x210]
004cc6df e82c67f5ff               call       0x422e10
004cc6e4 8b8de0fdffff             mov        ecx, dword ptr [ebp - 0x220]
004cc6ea 83c158                   add        ecx, 0x58
004cc6ed 8b11                     mov        edx, dword ptr [ecx]
004cc6ef 8995e4fdffff             mov        dword ptr [ebp - 0x21c], edx
004cc6f5 8b4104                   mov        eax, dword ptr [ecx + 4]
004cc6f8 8985e8fdffff             mov        dword ptr [ebp - 0x218], eax
004cc6fe 8b4908                   mov        ecx, dword ptr [ecx + 8]
004cc701 898decfdffff             mov        dword ptr [ebp - 0x214], ecx
004cc707 6800020000               push       0x200
004cc70c 6a00                     push       0
004cc70e 8d95fcfdffff             lea        edx, [ebp - 0x204]
004cc714 52                       push       edx
004cc715 e846800700               call       0x544760
004cc71a 83c40c                   add        esp, 0xc
004cc71d 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004cc723 8b8828130000             mov        ecx, dword ptr [eax + 0x1328]
004cc729 898dd4fdffff             mov        dword ptr [ebp - 0x22c], ecx
004cc72f c785dcfdffff00000000     mov        dword ptr [ebp - 0x224], 0
004cc739 eb0f                     jmp        0x4cc74a
004cc73b 8b95dcfdffff             mov        edx, dword ptr [ebp - 0x224]
004cc741 83c201                   add        edx, 1
004cc744 8995dcfdffff             mov        dword ptr [ebp - 0x224], edx
004cc74a 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004cc750 8b8ddcfdffff             mov        ecx, dword ptr [ebp - 0x224]
004cc756 3b8818070000             cmp        ecx, dword ptr [eax + 0x718]
004cc75c 0f8d67010000             jge        0x4cc8c9
004cc762 8b95d4fdffff             mov        edx, dword ptr [ebp - 0x22c]
004cc768 8b02                     mov        eax, dword ptr [edx]
004cc76a 8985f0fdffff             mov        dword ptr [ebp - 0x210], eax
004cc770 8b4a04                   mov        ecx, dword ptr [edx + 4]
004cc773 898df4fdffff             mov        dword ptr [ebp - 0x20c], ecx
004cc779 8b5208                   mov        edx, dword ptr [edx + 8]
004cc77c 8995f8fdffff             mov        dword ptr [ebp - 0x208], edx
004cc782 51                       push       ecx
004cc783 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004cc788 f30f110424               movss      dword ptr [esp], xmm0
004cc78d 51                       push       ecx
004cc78e f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004cc793 f30f110424               movss      dword ptr [esp], xmm0
004cc798 51                       push       ecx
004cc799 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004cc79e f30f110424               movss      dword ptr [esp], xmm0
004cc7a3 8b4508                   mov        eax, dword ptr [ebp + 8]
004cc7a6 51                       push       ecx
004cc7a7 f30f104004               movss      xmm0, dword ptr [eax + 4]
004cc7ac f30f110424               movss      dword ptr [esp], xmm0
004cc7b1 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004cc7b4 51                       push       ecx
004cc7b5 f30f1001                 movss      xmm0, dword ptr [ecx]
004cc7b9 f30f110424               movss      dword ptr [esp], xmm0
004cc7be 51                       push       ecx
004cc7bf f30f1085f4fdffff         movss      xmm0, dword ptr [ebp - 0x20c]
004cc7c7 f30f110424               movss      dword ptr [esp], xmm0
004cc7cc 51                       push       ecx
004cc7cd f30f1085f0fdffff         movss      xmm0, dword ptr [ebp - 0x210]
004cc7d5 f30f110424               movss      dword ptr [esp], xmm0
004cc7da e861a8f8ff               call       0x457040
004cc7df 83c41c                   add        esp, 0x1c
004cc7e2 0fb6d0                   movzx      edx, al
004cc7e5 85d2                     test       edx, edx
004cc7e7 0f84c8000000             je         0x4cc8b5
004cc7ed 8b85dcfdffff             mov        eax, dword ptr [ebp - 0x224]
004cc7f3 c68405fcfdffff01         mov        byte ptr [ebp + eax - 0x204], 1
004cc7fb 8b8dd8fdffff             mov        ecx, dword ptr [ebp - 0x228]
004cc801 83c101                   add        ecx, 1
004cc804 898dd8fdffff             mov        dword ptr [ebp - 0x228], ecx
004cc80a 8b95e0fdffff             mov        edx, dword ptr [ebp - 0x220]
004cc810 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004cc816 e8e5faf3ff               call       0x40c300
004cc81b 05886d2800               add        eax, 0x286d88
004cc820 8985d0fdffff             mov        dword ptr [ebp - 0x230], eax
004cc826 8b85d0fdffff             mov        eax, dword ptr [ebp - 0x230]
004cc82c 8b08                     mov        ecx, dword ptr [eax]
004cc82e 83c101                   add        ecx, 1
004cc831 8b95d0fdffff             mov        edx, dword ptr [ebp - 0x230]
004cc837 890a                     mov        dword ptr [edx], ecx
004cc839 8b85dcfdffff             mov        eax, dword ptr [ebp - 0x224]
004cc83f 99                       cdq        
004cc840 b914000000               mov        ecx, 0x14
004cc845 f7f9                     idiv       ecx
004cc847 85d2                     test       edx, edx
004cc849 756a                     jne        0x4cc8b5
004cc84b 8b95e0fdffff             mov        edx, dword ptr [ebp - 0x220]
004cc851 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004cc857 e8a4faf3ff               call       0x40c300
004cc85c 8bc8                     mov        ecx, eax
004cc85e e84d91fbff               call       0x4859b0
004cc863 8985c8fdffff             mov        dword ptr [ebp - 0x238], eax
004cc869 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004cc86f 8b8814070000             mov        ecx, dword ptr [eax + 0x714]
004cc875 8d9409d4000000           lea        edx, [ecx + ecx + 0xd4]
004cc87c 8995ccfdffff             mov        dword ptr [ebp - 0x234], edx
004cc882 6a00                     push       0
004cc884 6aff                     push       -1
004cc886 51                       push       ecx
004cc887 0f57c0                   xorps      xmm0, xmm0
004cc88a f30f110424               movss      dword ptr [esp], xmm0
004cc88f 8d85f0fdffff             lea        eax, [ebp - 0x210]
004cc895 50                       push       eax
004cc896 8b8dccfdffff             mov        ecx, dword ptr [ebp - 0x234]
004cc89c 51                       push       ecx
004cc89d 6814fd5600               push       0x56fd14
004cc8a2 8d95c0fdffff             lea        edx, [ebp - 0x240]
004cc8a8 52                       push       edx
004cc8a9 8b8dc8fdffff             mov        ecx, dword ptr [ebp - 0x238]
004cc8af e82cc8faff               call       0x4790e0
004cc8b4 90                       nop        
004cc8b5 8b85d4fdffff             mov        eax, dword ptr [ebp - 0x22c]
004cc8bb 83c020                   add        eax, 0x20
004cc8be 8985d4fdffff             mov        dword ptr [ebp - 0x22c], eax
004cc8c4 e972feffff               jmp        0x4cc73b
004cc8c9 8b8de0fdffff             mov        ecx, dword ptr [ebp - 0x220]
004cc8cf 8b95d8fdffff             mov        edx, dword ptr [ebp - 0x228]
004cc8d5 3b9118070000             cmp        edx, dword ptr [ecx + 0x718]
004cc8db 7c1a                     jl         0x4cc8f7
004cc8dd 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004cc8e3 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004cc8e6 83e1f9                   and        ecx, 0xfffffff9
004cc8e9 83c902                   or         ecx, 2
004cc8ec 8b95e0fdffff             mov        edx, dword ptr [ebp - 0x220]
004cc8f2 894a1c                   mov        dword ptr [edx + 0x1c], ecx
004cc8f5 eb35                     jmp        0x4cc92c
004cc8f7 83bdd8fdffff00           cmp        dword ptr [ebp - 0x228], 0
004cc8fe 7e2c                     jle        0x4cc92c
004cc900 8b85e0fdffff             mov        eax, dword ptr [ebp - 0x220]
004cc906 8b08                     mov        ecx, dword ptr [eax]
004cc908 8b5138                   mov        edx, dword ptr [ecx + 0x38]
004cc90b 8995c4fdffff             mov        dword ptr [ebp - 0x23c], edx
004cc911 8b85dcfdffff             mov        eax, dword ptr [ebp - 0x224]
004cc917 50                       push       eax
004cc918 8d8dfcfdffff             lea        ecx, [ebp - 0x204]
004cc91e 51                       push       ecx
004cc91f 8b8de0fdffff             mov        ecx, dword ptr [ebp - 0x220]
004cc925 ff95c4fdffff             call       dword ptr [ebp - 0x23c]
004cc92b 90                       nop        
004cc92c 8b85d8fdffff             mov        eax, dword ptr [ebp - 0x228]
004cc932 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cc935 33cd                     xor        ecx, ebp
004cc937 e8b2600700               call       0x5429ee
004cc93c 8be5                     mov        esp, ebp
004cc93e 5d                       pop        ebp
004cc93f c21800                   ret        0x18
004cc942 cc                       int3       
004cc943 cc                       int3       
004cc944 cc                       int3       
004cc945 cc                       int3       
004cc946 cc                       int3       
004cc947 cc                       int3       
004cc948 cc                       int3       
004cc949 cc                       int3       
004cc94a cc                       int3       
004cc94b cc                       int3       
004cc94c cc                       int3       
004cc94d cc                       int3       
004cc94e cc                       int3       
004cc94f cc                       int3       
