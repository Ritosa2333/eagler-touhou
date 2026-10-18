004609e0 55                       push       ebp
004609e1 8bec                     mov        ebp, esp
004609e3 83ec38                   sub        esp, 0x38
004609e6 894df8                   mov        dword ptr [ebp - 8], ecx
004609e9 8b45f8                   mov        eax, dword ptr [ebp - 8]
004609ec 05d0030000               add        eax, 0x3d0
004609f1 8945ec                   mov        dword ptr [ebp - 0x14], eax
004609f4 6a50                     push       0x50
004609f6 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004609f9 e8622bfcff               call       0x423560
004609fe 0fb6c8                   movzx      ecx, al
00460a01 85c9                     test       ecx, ecx
00460a03 0f8425010000             je         0x460b2e
00460a09 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00460a0c 81c1d0030000             add        ecx, 0x3d0
00460a12 e819c6ffff               call       0x45d030
00460a17 85c0                     test       eax, eax
00460a19 0f840d010000             je         0x460b2c
00460a1f 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00460a22 81c1d0030000             add        ecx, 0x3d0
00460a28 e863f5faff               call       0x40ff90
00460a2d 8945e8                   mov        dword ptr [ebp - 0x18], eax
00460a30 837de846                 cmp        dword ptr [ebp - 0x18], 0x46
00460a34 0f8cac000000             jl         0x460ae6
00460a3a c745f400000000           mov        dword ptr [ebp - 0xc], 0
00460a41 eb09                     jmp        0x460a4c
00460a43 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00460a46 83c201                   add        edx, 1
00460a49 8955f4                   mov        dword ptr [ebp - 0xc], edx
00460a4c 837df450                 cmp        dword ptr [ebp - 0xc], 0x50
00460a50 0f8d90000000             jge        0x460ae6
00460a56 8b45f8                   mov        eax, dword ptr [ebp - 8]
00460a59 0588020000               add        eax, 0x288
00460a5e 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00460a61 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00460a64 51                       push       ecx
00460a65 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00460a68 e8133bfbff               call       0x414580
00460a6d 0fb65003                 movzx      edx, byte ptr [eax + 3]
00460a71 f30f2ac2                 cvtsi2ss   xmm0, edx
00460a75 b9c4a45b00               mov        ecx, 0x5ba4c4
00460a7a f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00460a7f e8ac8dfcff               call       0x429830
00460a84 d95de0                   fstp       dword ptr [ebp - 0x20]
00460a87 f30f1045e0               movss      xmm0, dword ptr [ebp - 0x20]
00460a8c f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
00460a94 f30f5805d0c85600         addss      xmm0, dword ptr [0x56c8d0]
00460a9c f30f104ddc               movss      xmm1, dword ptr [ebp - 0x24]
00460aa1 f30f5ec8                 divss      xmm1, xmm0
00460aa5 f30f2cc1                 cvttss2si  eax, xmm1
00460aa9 0fb6c8                   movzx      ecx, al
00460aac 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
00460aaf 8b55f8                   mov        edx, dword ptr [ebp - 8]
00460ab2 81c288020000             add        edx, 0x288
00460ab8 8955d8                   mov        dword ptr [ebp - 0x28], edx
00460abb 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00460abe 50                       push       eax
00460abf 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00460ac2 e8b93afbff               call       0x414580
00460ac7 83c003                   add        eax, 3
00460aca 8945f0                   mov        dword ptr [ebp - 0x10], eax
00460acd 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00460ad0 8a11                     mov        dl, byte ptr [ecx]
00460ad2 8855ff                   mov        byte ptr [ebp - 1], dl
00460ad5 0fb645ff                 movzx      eax, byte ptr [ebp - 1]
00460ad9 2b45d4                   sub        eax, dword ptr [ebp - 0x2c]
00460adc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00460adf 8801                     mov        byte ptr [ecx], al
00460ae1 e95dffffff               jmp        0x460a43
00460ae6 6a00                     push       0
00460ae8 e843fdffff               call       0x460830
00460aed 83c404                   add        esp, 4
00460af0 85c0                     test       eax, eax
00460af2 7438                     je         0x460b2c
00460af4 8b55f8                   mov        edx, dword ptr [ebp - 8]
00460af7 0fb682f0030000           movzx      eax, byte ptr [edx + 0x3f0]
00460afe 85c0                     test       eax, eax
00460b00 742a                     je         0x460b2c
00460b02 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00460b05 8b5104                   mov        edx, dword ptr [ecx + 4]
00460b08 8955cc                   mov        dword ptr [ebp - 0x34], edx
00460b0b 6a00                     push       0
00460b0d e81efdffff               call       0x460830
00460b12 83c404                   add        esp, 4
00460b15 8bc8                     mov        ecx, eax
00460b17 e834fdffff               call       0x460850
00460b1c 8945d0                   mov        dword ptr [ebp - 0x30], eax
00460b1f 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00460b22 50                       push       eax
00460b23 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00460b26 e835d2ffff               call       0x45dd60
00460b2b 90                       nop        
00460b2c eb07                     jmp        0x460b35
00460b2e b801000000               mov        eax, 1
00460b33 eb18                     jmp        0x460b4d
00460b35 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00460b38 81c1d0030000             add        ecx, 0x3d0
00460b3e 894dc8                   mov        dword ptr [ebp - 0x38], ecx
00460b41 6a00                     push       0
00460b43 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00460b46 e8f529fcff               call       0x423540
00460b4b 33c0                     xor        eax, eax
00460b4d 8be5                     mov        esp, ebp
00460b4f 5d                       pop        ebp
00460b50 c3                       ret        
00460b51 cc                       int3       
00460b52 cc                       int3       
00460b53 cc                       int3       
00460b54 cc                       int3       
00460b55 cc                       int3       
00460b56 cc                       int3       
00460b57 cc                       int3       
00460b58 cc                       int3       
00460b59 cc                       int3       
00460b5a cc                       int3       
00460b5b cc                       int3       
00460b5c cc                       int3       
00460b5d cc                       int3       
00460b5e cc                       int3       
00460b5f cc                       int3       
