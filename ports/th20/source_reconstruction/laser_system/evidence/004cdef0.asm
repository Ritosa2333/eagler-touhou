004cdef0 55                       push       ebp
004cdef1 8bec                     mov        ebp, esp
004cdef3 6aff                     push       -1
004cdef5 680d9b5600               push       0x569b0d
004cdefa 64a100000000             mov        eax, dword ptr fs:[0]
004cdf00 50                       push       eax
004cdf01 81ec84000000             sub        esp, 0x84
004cdf07 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004cdf0c 33c5                     xor        eax, ebp
004cdf0e 8945f0                   mov        dword ptr [ebp - 0x10], eax
004cdf11 56                       push       esi
004cdf12 57                       push       edi
004cdf13 50                       push       eax
004cdf14 8d45f4                   lea        eax, [ebp - 0xc]
004cdf17 64a300000000             mov        dword ptr fs:[0], eax
004cdf1d 894d8c                   mov        dword ptr [ebp - 0x74], ecx
004cdf20 c7459000000000           mov        dword ptr [ebp - 0x70], 0
004cdf27 8b458c                   mov        eax, dword ptr [ebp - 0x74]
004cdf2a 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
004cdf2d 3b8818070000             cmp        ecx, dword ptr [eax + 0x718]
004cdf33 7d18                     jge        0x4cdf4d
004cdf35 8b5508                   mov        edx, dword ptr [ebp + 8]
004cdf38 035590                   add        edx, dword ptr [ebp - 0x70]
004cdf3b 0fb602                   movzx      eax, byte ptr [edx]
004cdf3e 85c0                     test       eax, eax
004cdf40 740b                     je         0x4cdf4d
004cdf42 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
004cdf45 83c101                   add        ecx, 1
004cdf48 894d90                   mov        dword ptr [ebp - 0x70], ecx
004cdf4b ebda                     jmp        0x4cdf27
004cdf4d 837d9000                 cmp        dword ptr [ebp - 0x70], 0
004cdf51 0f84fa000000             je         0x4ce051
004cdf57 c7458800000000           mov        dword ptr [ebp - 0x78], 0
004cdf5e 8b5590                   mov        edx, dword ptr [ebp - 0x70]
004cdf61 895584                   mov        dword ptr [ebp - 0x7c], edx
004cdf64 eb12                     jmp        0x4cdf78
004cdf66 8b4584                   mov        eax, dword ptr [ebp - 0x7c]
004cdf69 83c001                   add        eax, 1
004cdf6c 894584                   mov        dword ptr [ebp - 0x7c], eax
004cdf6f 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004cdf72 83c101                   add        ecx, 1
004cdf75 894d88                   mov        dword ptr [ebp - 0x78], ecx
004cdf78 8b558c                   mov        edx, dword ptr [ebp - 0x74]
004cdf7b 8b4584                   mov        eax, dword ptr [ebp - 0x7c]
004cdf7e 3b8218070000             cmp        eax, dword ptr [edx + 0x718]
004cdf84 7d12                     jge        0x4cdf98
004cdf86 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004cdf89 034d88                   add        ecx, dword ptr [ebp - 0x78]
004cdf8c 8b5508                   mov        edx, dword ptr [ebp + 8]
004cdf8f 035584                   add        edx, dword ptr [ebp - 0x7c]
004cdf92 8a02                     mov        al, byte ptr [edx]
004cdf94 8801                     mov        byte ptr [ecx], al
004cdf96 ebce                     jmp        0x4cdf66
004cdf98 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004cdf9b 83b92413000000           cmp        dword ptr [ecx + 0x1324], 0
004cdfa2 745a                     je         0x4cdffe
004cdfa4 c7458800000000           mov        dword ptr [ebp - 0x78], 0
004cdfab 8b5590                   mov        edx, dword ptr [ebp - 0x70]
004cdfae 895580                   mov        dword ptr [ebp - 0x80], edx
004cdfb1 eb12                     jmp        0x4cdfc5
004cdfb3 8b4580                   mov        eax, dword ptr [ebp - 0x80]
004cdfb6 83c001                   add        eax, 1
004cdfb9 894580                   mov        dword ptr [ebp - 0x80], eax
004cdfbc 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004cdfbf 83c101                   add        ecx, 1
004cdfc2 894d88                   mov        dword ptr [ebp - 0x78], ecx
004cdfc5 8b558c                   mov        edx, dword ptr [ebp - 0x74]
004cdfc8 8b4580                   mov        eax, dword ptr [ebp - 0x80]
004cdfcb 3b8218070000             cmp        eax, dword ptr [edx + 0x718]
004cdfd1 7d2b                     jge        0x4cdffe
004cdfd3 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004cdfd6 c1e105                   shl        ecx, 5
004cdfd9 8b558c                   mov        edx, dword ptr [ebp - 0x74]
004cdfdc 8bb228130000             mov        esi, dword ptr [edx + 0x1328]
004cdfe2 03f1                     add        esi, ecx
004cdfe4 8b4588                   mov        eax, dword ptr [ebp - 0x78]
004cdfe7 c1e005                   shl        eax, 5
004cdfea 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004cdfed 8bb928130000             mov        edi, dword ptr [ecx + 0x1328]
004cdff3 03f8                     add        edi, eax
004cdff5 b908000000               mov        ecx, 8
004cdffa f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
004cdffc ebb5                     jmp        0x4cdfb3
004cdffe 8b5590                   mov        edx, dword ptr [ebp - 0x70]
004ce001 52                       push       edx
004ce002 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004ce005 83c148                   add        ecx, 0x48
004ce008 e863b1ffff               call       0x4c9170
004ce00d 8b458c                   mov        eax, dword ptr [ebp - 0x74]
004ce010 8b8818070000             mov        ecx, dword ptr [eax + 0x718]
004ce016 2b4d90                   sub        ecx, dword ptr [ebp - 0x70]
004ce019 8b558c                   mov        edx, dword ptr [ebp - 0x74]
004ce01c 898a18070000             mov        dword ptr [edx + 0x718], ecx
004ce022 8b458c                   mov        eax, dword ptr [ebp - 0x74]
004ce025 83b81807000004           cmp        dword ptr [eax + 0x718], 4
004ce02c 7d1c                     jge        0x4ce04a
004ce02e 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004ce031 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
004ce034 83e2f9                   and        edx, 0xfffffff9
004ce037 83ca02                   or         edx, 2
004ce03a 8b458c                   mov        eax, dword ptr [ebp - 0x74]
004ce03d 89501c                   mov        dword ptr [eax + 0x1c], edx
004ce040 b801000000               mov        eax, 1
004ce045 e9ab010000               jmp        0x4ce1f5
004ce04a c7459000000000           mov        dword ptr [ebp - 0x70], 0
004ce051 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004ce054 8b5590                   mov        edx, dword ptr [ebp - 0x70]
004ce057 3b9118070000             cmp        edx, dword ptr [ecx + 0x718]
004ce05d 7d18                     jge        0x4ce077
004ce05f 8b4508                   mov        eax, dword ptr [ebp + 8]
004ce062 034590                   add        eax, dword ptr [ebp - 0x70]
004ce065 0fb608                   movzx      ecx, byte ptr [eax]
004ce068 85c9                     test       ecx, ecx
004ce06a 750b                     jne        0x4ce077
004ce06c 8b5590                   mov        edx, dword ptr [ebp - 0x70]
004ce06f 83c201                   add        edx, 1
004ce072 895590                   mov        dword ptr [ebp - 0x70], edx
004ce075 ebda                     jmp        0x4ce051
004ce077 8b4590                   mov        eax, dword ptr [ebp - 0x70]
004ce07a 89857cffffff             mov        dword ptr [ebp - 0x84], eax
004ce080 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004ce083 8b5590                   mov        edx, dword ptr [ebp - 0x70]
004ce086 3b9118070000             cmp        edx, dword ptr [ecx + 0x718]
004ce08c 0f8d28010000             jge        0x4ce1ba
004ce092 8b458c                   mov        eax, dword ptr [ebp - 0x74]
004ce095 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
004ce098 3b8818070000             cmp        ecx, dword ptr [eax + 0x718]
004ce09e 7d18                     jge        0x4ce0b8
004ce0a0 8b5508                   mov        edx, dword ptr [ebp + 8]
004ce0a3 035590                   add        edx, dword ptr [ebp - 0x70]
004ce0a6 0fb602                   movzx      eax, byte ptr [edx]
004ce0a9 85c0                     test       eax, eax
004ce0ab 740b                     je         0x4ce0b8
004ce0ad 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
004ce0b0 83c101                   add        ecx, 1
004ce0b3 894d90                   mov        dword ptr [ebp - 0x70], ecx
004ce0b6 ebda                     jmp        0x4ce092
004ce0b8 8b5590                   mov        edx, dword ptr [ebp - 0x70]
004ce0bb 3b550c                   cmp        edx, dword ptr [ebp + 0xc]
004ce0be 7c05                     jl         0x4ce0c5
004ce0c0 e9f5000000               jmp        0x4ce1ba
004ce0c5 c7458800000000           mov        dword ptr [ebp - 0x78], 0
004ce0cc 8b4590                   mov        eax, dword ptr [ebp - 0x70]
004ce0cf 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
004ce0d5 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004ce0d8 8b5590                   mov        edx, dword ptr [ebp - 0x70]
004ce0db 3b9118070000             cmp        edx, dword ptr [ecx + 0x718]
004ce0e1 7d21                     jge        0x4ce104
004ce0e3 8b4508                   mov        eax, dword ptr [ebp + 8]
004ce0e6 034590                   add        eax, dword ptr [ebp - 0x70]
004ce0e9 0fb608                   movzx      ecx, byte ptr [eax]
004ce0ec 85c9                     test       ecx, ecx
004ce0ee 7514                     jne        0x4ce104
004ce0f0 8b5590                   mov        edx, dword ptr [ebp - 0x70]
004ce0f3 83c201                   add        edx, 1
004ce0f6 895590                   mov        dword ptr [ebp - 0x70], edx
004ce0f9 8b4588                   mov        eax, dword ptr [ebp - 0x78]
004ce0fc 83c001                   add        eax, 1
004ce0ff 894588                   mov        dword ptr [ebp - 0x78], eax
004ce102 ebd1                     jmp        0x4ce0d5
004ce104 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004ce107 83b92413000000           cmp        dword ptr [ecx + 0x1324], 0
004ce10e 0f85a1000000             jne        0x4ce1b5
004ce114 837d8804                 cmp        dword ptr [ebp - 0x78], 4
004ce118 0f8c97000000             jl         0x4ce1b5
004ce11e 8b558c                   mov        edx, dword ptr [ebp - 0x74]
004ce121 81c2f8060000             add        edx, 0x6f8
004ce127 52                       push       edx
004ce128 8d4d94                   lea        ecx, [ebp - 0x6c]
004ce12b e880a3ffff               call       0x4c84b0
004ce130 c745fc00000000           mov        dword ptr [ebp - 4], 0
004ce137 8b458c                   mov        eax, dword ptr [ebp - 0x74]
004ce13a 0530130000               add        eax, 0x1330
004ce13f 8945dc                   mov        dword ptr [ebp - 0x24], eax
004ce142 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004ce145 894db4                   mov        dword ptr [ebp - 0x4c], ecx
004ce148 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004ce14b 83c148                   add        ecx, 0x48
004ce14e e87d5af5ff               call       0x423bd0
004ce153 d99d78ffffff             fstp       dword ptr [ebp - 0x88]
004ce159 f30f108578ffffff         movss      xmm0, dword ptr [ebp - 0x88]
004ce161 f30f2a8d74ffffff         cvtsi2ss   xmm1, dword ptr [ebp - 0x8c]
004ce169 f30f5cc1                 subss      xmm0, xmm1
004ce16d f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
004ce172 c745d0ffffffff           mov        dword ptr [ebp - 0x30], 0xffffffff
004ce179 c745e4e7030000           mov        dword ptr [ebp - 0x1c], 0x3e7
004ce180 8b558c                   mov        edx, dword ptr [ebp - 0x74]
004ce183 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004ce189 e802e9f4ff               call       0x41ca90
004ce18e 898570ffffff             mov        dword ptr [ebp - 0x90], eax
004ce194 8d4594                   lea        eax, [ebp - 0x6c]
004ce197 50                       push       eax
004ce198 6a02                     push       2
004ce19a 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
004ce1a0 e84b660000               call       0x4d47f0
004ce1a5 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004ce1ac 8d4d94                   lea        ecx, [ebp - 0x6c]
004ce1af e8fcd6fbff               call       0x48b8b0
004ce1b4 90                       nop        
004ce1b5 e9c6feffff               jmp        0x4ce080
004ce1ba 83bd7cffffff04           cmp        dword ptr [ebp - 0x84], 4
004ce1c1 7c1e                     jl         0x4ce1e1
004ce1c3 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004ce1c6 8b957cffffff             mov        edx, dword ptr [ebp - 0x84]
004ce1cc 899118070000             mov        dword ptr [ecx + 0x718], edx
004ce1d2 8b458c                   mov        eax, dword ptr [ebp - 0x74]
004ce1d5 c780201300000f270000     mov        dword ptr [eax + 0x1320], 0x270f
004ce1df eb12                     jmp        0x4ce1f3
004ce1e1 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004ce1e4 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
004ce1e7 83e2f9                   and        edx, 0xfffffff9
004ce1ea 83ca02                   or         edx, 2
004ce1ed 8b458c                   mov        eax, dword ptr [ebp - 0x74]
004ce1f0 89501c                   mov        dword ptr [eax + 0x1c], edx
004ce1f3 33c0                     xor        eax, eax
004ce1f5 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004ce1f8 64890d00000000           mov        dword ptr fs:[0], ecx
004ce1ff 59                       pop        ecx
004ce200 5f                       pop        edi
004ce201 5e                       pop        esi
004ce202 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004ce205 33cd                     xor        ecx, ebp
004ce207 e8e2470700               call       0x5429ee
004ce20c 8be5                     mov        esp, ebp
004ce20e 5d                       pop        ebp
004ce20f c20800                   ret        8
004ce212 cc                       int3       
004ce213 cc                       int3       
004ce214 cc                       int3       
004ce215 cc                       int3       
004ce216 cc                       int3       
004ce217 cc                       int3       
004ce218 cc                       int3       
004ce219 cc                       int3       
004ce21a cc                       int3       
004ce21b cc                       int3       
004ce21c cc                       int3       
004ce21d cc                       int3       
004ce21e cc                       int3       
004ce21f cc                       int3       
