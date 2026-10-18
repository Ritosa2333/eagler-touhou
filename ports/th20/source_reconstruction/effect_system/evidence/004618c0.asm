004618c0 55                       push       ebp
004618c1 8bec                     mov        ebp, esp
004618c3 83ec5c                   sub        esp, 0x5c
004618c6 894df8                   mov        dword ptr [ebp - 8], ecx
004618c9 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
004618d0 7416                     je         0x4618e8
004618d2 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004618d8 e8732a0000               call       0x464350
004618dd 85c0                     test       eax, eax
004618df 7507                     jne        0x4618e8
004618e1 33c0                     xor        eax, eax
004618e3 e987020000               jmp        0x461b6f
004618e8 8b45f8                   mov        eax, dword ptr [ebp - 8]
004618eb 054c010000               add        eax, 0x14c
004618f0 8945d8                   mov        dword ptr [ebp - 0x28], eax
004618f3 6a00                     push       0
004618f5 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004618f8 e8431cfcff               call       0x423540
004618fd 90                       nop        
004618fe 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00461901 81c15c010000             add        ecx, 0x15c
00461907 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
0046190a 6a00                     push       0
0046190c 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0046190f e85cf7ffff               call       0x461070
00461914 0fb6d0                   movzx      edx, al
00461917 85d2                     test       edx, edx
00461919 743d                     je         0x461958
0046191b 8b45f8                   mov        eax, dword ptr [ebp - 8]
0046191e 055c010000               add        eax, 0x15c
00461923 8945d0                   mov        dword ptr [ebp - 0x30], eax
00461926 6a00                     push       0
00461928 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0046192b e8f07afcff               call       0x429420
00461930 90                       nop        
00461931 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00461934 81c15c010000             add        ecx, 0x15c
0046193a 894dcc                   mov        dword ptr [ebp - 0x34], ecx
0046193d 6a00                     push       0
0046193f 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00461942 e8491cfcff               call       0x423590
00461947 0fb6d0                   movzx      edx, al
0046194a 85d2                     test       edx, edx
0046194c 740a                     je         0x461958
0046194e b801000000               mov        eax, 1
00461953 e917020000               jmp        0x461b6f
00461958 6a00                     push       0
0046195a e8d1280000               call       0x464230
0046195f 83c404                   add        esp, 4
00461962 8bc8                     mov        ecx, eax
00461964 e807290000               call       0x464270
00461969 8845ff                   mov        byte ptr [ebp - 1], al
0046196c 8b45f8                   mov        eax, dword ptr [ebp - 8]
0046196f 0594010000               add        eax, 0x194
00461974 8945c8                   mov        dword ptr [ebp - 0x38], eax
00461977 6a00                     push       0
00461979 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0046197c e8efbcffff               call       0x45d670
00461981 8a4dff                   mov        cl, byte ptr [ebp - 1]
00461984 8808                     mov        byte ptr [eax], cl
00461986 6a00                     push       0
00461988 e8a3280000               call       0x464230
0046198d 83c404                   add        esp, 4
00461990 8bc8                     mov        ecx, eax
00461992 e8e92a0000               call       0x464480
00461997 8845fe                   mov        byte ptr [ebp - 2], al
0046199a 8b55f8                   mov        edx, dword ptr [ebp - 8]
0046199d 81c294010000             add        edx, 0x194
004619a3 8955c4                   mov        dword ptr [ebp - 0x3c], edx
004619a6 6a01                     push       1
004619a8 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004619ab e8c0bcffff               call       0x45d670
004619b0 8a4dfe                   mov        cl, byte ptr [ebp - 2]
004619b3 8808                     mov        byte ptr [eax], cl
004619b5 6a00                     push       0
004619b7 e874280000               call       0x464230
004619bc 83c404                   add        esp, 4
004619bf 8bc8                     mov        ecx, eax
004619c1 e81a290000               call       0x4642e0
004619c6 8845fd                   mov        byte ptr [ebp - 3], al
004619c9 8b55f8                   mov        edx, dword ptr [ebp - 8]
004619cc 81c294010000             add        edx, 0x194
004619d2 8955c0                   mov        dword ptr [ebp - 0x40], edx
004619d5 6a02                     push       2
004619d7 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004619da e891bcffff               call       0x45d670
004619df 8a4dfd                   mov        cl, byte ptr [ebp - 3]
004619e2 8808                     mov        byte ptr [eax], cl
004619e4 6a00                     push       0
004619e6 e845280000               call       0x464230
004619eb 83c404                   add        esp, 4
004619ee 8bc8                     mov        ecx, eax
004619f0 e8bb290000               call       0x4643b0
004619f5 8845fc                   mov        byte ptr [ebp - 4], al
004619f8 8b55f8                   mov        edx, dword ptr [ebp - 8]
004619fb 81c294010000             add        edx, 0x194
00461a01 8955bc                   mov        dword ptr [ebp - 0x44], edx
00461a04 6a03                     push       3
00461a06 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00461a09 e862bcffff               call       0x45d670
00461a0e 8a4dfc                   mov        cl, byte ptr [ebp - 4]
00461a11 8808                     mov        byte ptr [eax], cl
00461a13 c745e800000000           mov        dword ptr [ebp - 0x18], 0
00461a1a 8b55f8                   mov        edx, dword ptr [ebp - 8]
00461a1d 81c294010000             add        edx, 0x194
00461a23 8955e0                   mov        dword ptr [ebp - 0x20], edx
00461a26 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00461a29 e872a3faff               call       0x40bda0
00461a2e 8945ec                   mov        dword ptr [ebp - 0x14], eax
00461a31 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00461a34 e8a7240000               call       0x463ee0
00461a39 8945b8                   mov        dword ptr [ebp - 0x48], eax
00461a3c eb09                     jmp        0x461a47
00461a3e 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00461a41 83c008                   add        eax, 8
00461a44 8945ec                   mov        dword ptr [ebp - 0x14], eax
00461a47 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00461a4a 3b4db8                   cmp        ecx, dword ptr [ebp - 0x48]
00461a4d 0f849f000000             je         0x461af2
00461a53 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00461a56 8955f4                   mov        dword ptr [ebp - 0xc], edx
00461a59 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00461a5c 0fb608                   movzx      ecx, byte ptr [eax]
00461a5f 85c9                     test       ecx, ecx
00461a61 7549                     jne        0x461aac
00461a63 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00461a66 837a0400                 cmp        dword ptr [edx + 4], 0
00461a6a 7c3e                     jl         0x461aaa
00461a6c 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00461a6f 83780400                 cmp        dword ptr [eax + 4], 0
00461a73 7526                     jne        0x461a9b
00461a75 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00461a78 81c128010000             add        ecx, 0x128
00461a7e 894db4                   mov        dword ptr [ebp - 0x4c], ecx
00461a81 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00461a84 52                       push       edx
00461a85 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00461a88 e8f32afbff               call       0x414580
00461a8d 8945b0                   mov        dword ptr [ebp - 0x50], eax
00461a90 6a1c                     push       0x1c
00461a92 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00461a95 e836d6feff               call       0x44f0d0
00461a9a 90                       nop        
00461a9b 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00461a9e 8b4804                   mov        ecx, dword ptr [eax + 4]
00461aa1 83e901                   sub        ecx, 1
00461aa4 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00461aa7 894a04                   mov        dword ptr [edx + 4], ecx
00461aaa eb38                     jmp        0x461ae4
00461aac 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00461aaf 83780400                 cmp        dword ptr [eax + 4], 0
00461ab3 7d2f                     jge        0x461ae4
00461ab5 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00461ab8 81c128010000             add        ecx, 0x128
00461abe 894dac                   mov        dword ptr [ebp - 0x54], ecx
00461ac1 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00461ac4 52                       push       edx
00461ac5 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00461ac8 e8b32afbff               call       0x414580
00461acd 8945a8                   mov        dword ptr [ebp - 0x58], eax
00461ad0 6a1b                     push       0x1b
00461ad2 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00461ad5 e8f6d5feff               call       0x44f0d0
00461ada 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00461add c740040a000000           mov        dword ptr [eax + 4], 0xa
00461ae4 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00461ae7 83c101                   add        ecx, 1
00461aea 894de8                   mov        dword ptr [ebp - 0x18], ecx
00461aed e94cffffff               jmp        0x461a3e
00461af2 8b55f8                   mov        edx, dword ptr [ebp - 8]
00461af5 81c274010000             add        edx, 0x174
00461afb 8955dc                   mov        dword ptr [ebp - 0x24], edx
00461afe 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00461b01 e89aa2faff               call       0x40bda0
00461b06 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00461b09 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00461b0c e8cf230000               call       0x463ee0
00461b11 8945a4                   mov        dword ptr [ebp - 0x5c], eax
00461b14 eb09                     jmp        0x461b1f
00461b16 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
00461b19 83c008                   add        eax, 8
00461b1c 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00461b1f 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00461b22 3b4da4                   cmp        ecx, dword ptr [ebp - 0x5c]
00461b25 7446                     je         0x461b6d
00461b27 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
00461b2a 8955f0                   mov        dword ptr [ebp - 0x10], edx
00461b2d 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00461b30 0fb608                   movzx      ecx, byte ptr [eax]
00461b33 85c9                     test       ecx, ecx
00461b35 7434                     je         0x461b6b
00461b37 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00461b3a f30f104204               movss      xmm0, dword ptr [edx + 4]
00461b3f f30f580584f05600         addss      xmm0, dword ptr [0x56f084]
00461b47 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00461b4a f30f114004               movss      dword ptr [eax + 4], xmm0
00461b4f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00461b52 f30f104104               movss      xmm0, dword ptr [ecx + 4]
00461b57 0f2f0588f05600           comiss     xmm0, dword ptr [0x56f088]
00461b5e 720b                     jb         0x461b6b
00461b60 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00461b63 0f57c0                   xorps      xmm0, xmm0
00461b66 f30f114204               movss      dword ptr [edx + 4], xmm0
00461b6b eba9                     jmp        0x461b16
00461b6d 33c0                     xor        eax, eax
00461b6f 8be5                     mov        esp, ebp
00461b71 5d                       pop        ebp
00461b72 c3                       ret        
00461b73 cc                       int3       
00461b74 cc                       int3       
00461b75 cc                       int3       
00461b76 cc                       int3       
00461b77 cc                       int3       
00461b78 cc                       int3       
00461b79 cc                       int3       
00461b7a cc                       int3       
00461b7b cc                       int3       
00461b7c cc                       int3       
00461b7d cc                       int3       
00461b7e cc                       int3       
00461b7f cc                       int3       
