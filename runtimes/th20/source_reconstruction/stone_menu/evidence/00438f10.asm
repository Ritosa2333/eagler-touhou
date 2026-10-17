00438f10 55                       push       ebp
00438f11 8bec                     mov        ebp, esp
00438f13 83ec18                   sub        esp, 0x18
00438f16 56                       push       esi
00438f17 57                       push       edi
00438f18 894dfc                   mov        dword ptr [ebp - 4], ecx
00438f1b 8b45fc                   mov        eax, dword ptr [ebp - 4]
00438f1e 8b4820                   mov        ecx, dword ptr [eax + 0x20]
00438f21 51                       push       ecx
00438f22 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00438f25 e876edffff               call       0x437ca0
00438f2a 8945f8                   mov        dword ptr [ebp - 8], eax
00438f2d 8b55f8                   mov        edx, dword ptr [ebp - 8]
00438f30 f30f104508               movss      xmm0, dword ptr [ebp + 8]
00438f35 f30f5e4220               divss      xmm0, dword ptr [edx + 0x20]
00438f3a 8b45f8                   mov        eax, dword ptr [ebp - 8]
00438f3d f30f104838               movss      xmm1, dword ptr [eax + 0x38]
00438f42 f30f58c8                 addss      xmm1, xmm0
00438f46 f30f114df4               movss      dword ptr [ebp - 0xc], xmm1
00438f4b b908000000               mov        ecx, 8
00438f50 d1e1                     shl        ecx, 1
00438f52 8b55fc                   mov        edx, dword ptr [ebp - 4]
00438f55 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00438f5a f30f11840a78030000       movss      dword ptr [edx + ecx + 0x378], xmm0
00438f63 b808000000               mov        eax, 8
00438f68 6bc800                   imul       ecx, eax, 0
00438f6b 8b55fc                   mov        edx, dword ptr [ebp - 4]
00438f6e f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00438f73 f30f11840a78030000       movss      dword ptr [edx + ecx + 0x378], xmm0
00438f7c f30f104508               movss      xmm0, dword ptr [ebp + 8]
00438f81 f30f584510               addss      xmm0, dword ptr [ebp + 0x10]
00438f86 8b45f8                   mov        eax, dword ptr [ebp - 8]
00438f89 f30f5e4020               divss      xmm0, dword ptr [eax + 0x20]
00438f8e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00438f91 f30f104938               movss      xmm1, dword ptr [ecx + 0x38]
00438f96 f30f58c8                 addss      xmm1, xmm0
00438f9a f30f114df0               movss      dword ptr [ebp - 0x10], xmm1
00438f9f ba08000000               mov        edx, 8
00438fa4 6bc203                   imul       eax, edx, 3
00438fa7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00438faa f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00438faf f30f11840178030000       movss      dword ptr [ecx + eax + 0x378], xmm0
00438fb8 ba08000000               mov        edx, 8
00438fbd c1e200                   shl        edx, 0
00438fc0 8b45fc                   mov        eax, dword ptr [ebp - 4]
00438fc3 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00438fc8 f30f11841078030000       movss      dword ptr [eax + edx + 0x378], xmm0
00438fd1 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00438fd4 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00438fd9 f30f5e411c               divss      xmm0, dword ptr [ecx + 0x1c]
00438fde 8b55f8                   mov        edx, dword ptr [ebp - 8]
00438fe1 f30f104a3c               movss      xmm1, dword ptr [edx + 0x3c]
00438fe6 f30f58c8                 addss      xmm1, xmm0
00438fea f30f114dec               movss      dword ptr [ebp - 0x14], xmm1
00438fef b808000000               mov        eax, 8
00438ff4 c1e000                   shl        eax, 0
00438ff7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00438ffa f30f1045ec               movss      xmm0, dword ptr [ebp - 0x14]
00438fff f30f1184017c030000       movss      dword ptr [ecx + eax + 0x37c], xmm0
00439008 ba08000000               mov        edx, 8
0043900d 6bc200                   imul       eax, edx, 0
00439010 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00439013 f30f1045ec               movss      xmm0, dword ptr [ebp - 0x14]
00439018 f30f1184017c030000       movss      dword ptr [ecx + eax + 0x37c], xmm0
00439021 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00439026 f30f584514               addss      xmm0, dword ptr [ebp + 0x14]
0043902b 8b55f8                   mov        edx, dword ptr [ebp - 8]
0043902e f30f5e421c               divss      xmm0, dword ptr [edx + 0x1c]
00439033 8b45f8                   mov        eax, dword ptr [ebp - 8]
00439036 f30f10483c               movss      xmm1, dword ptr [eax + 0x3c]
0043903b f30f58c8                 addss      xmm1, xmm0
0043903f f30f114de8               movss      dword ptr [ebp - 0x18], xmm1
00439044 b908000000               mov        ecx, 8
00439049 6bd103                   imul       edx, ecx, 3
0043904c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0043904f f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00439054 f30f1184107c030000       movss      dword ptr [eax + edx + 0x37c], xmm0
0043905d b908000000               mov        ecx, 8
00439062 d1e1                     shl        ecx, 1
00439064 8b55fc                   mov        edx, dword ptr [ebp - 4]
00439067 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
0043906c f30f11840a7c030000       movss      dword ptr [edx + ecx + 0x37c], xmm0
00439075 b808000000               mov        eax, 8
0043907a c1e000                   shl        eax, 0
0043907d b908000000               mov        ecx, 8
00439082 6bd100                   imul       edx, ecx, 0
00439085 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00439088 8b75fc                   mov        esi, dword ptr [ebp - 4]
0043908b f30f10840178030000       movss      xmm0, dword ptr [ecx + eax + 0x378]
00439094 f30f5c841678030000       subss      xmm0, dword ptr [esi + edx + 0x378]
0043909d 8b55fc                   mov        edx, dword ptr [ebp - 4]
004390a0 f30f118298030000         movss      dword ptr [edx + 0x398], xmm0
004390a8 b808000000               mov        eax, 8
004390ad d1e0                     shl        eax, 1
004390af b908000000               mov        ecx, 8
004390b4 6bd100                   imul       edx, ecx, 0
004390b7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004390ba 8b75fc                   mov        esi, dword ptr [ebp - 4]
004390bd f30f1084017c030000       movss      xmm0, dword ptr [ecx + eax + 0x37c]
004390c6 f30f5c84167c030000       subss      xmm0, dword ptr [esi + edx + 0x37c]
004390cf 8b55fc                   mov        edx, dword ptr [ebp - 4]
004390d2 f30f11829c030000         movss      dword ptr [edx + 0x39c], xmm0
004390da 8b45fc                   mov        eax, dword ptr [ebp - 4]
004390dd f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004390e2 f30f114070               movss      dword ptr [eax + 0x70], xmm0
004390e7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004390ea f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004390ef f30f114174               movss      dword ptr [ecx + 0x74], xmm0
004390f4 8b7dfc                   mov        edi, dword ptr [ebp - 4]
004390f7 81c7b8030000             add        edi, 0x3b8
004390fd b910000000               mov        ecx, 0x10
00439102 be68e05600               mov        esi, 0x56e068
00439107 f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
00439109 8b7dfc                   mov        edi, dword ptr [ebp - 4]
0043910c 81c7f8030000             add        edi, 0x3f8
00439112 b910000000               mov        ecx, 0x10
00439117 be68e05600               mov        esi, 0x56e068
0043911c f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
0043911e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00439121 f30f104270               movss      xmm0, dword ptr [edx + 0x70]
00439126 f30f5e05a0cd5600         divss      xmm0, dword ptr [0x56cda0]
0043912e b810000000               mov        eax, 0x10
00439133 6bc800                   imul       ecx, eax, 0
00439136 8b55fc                   mov        edx, dword ptr [ebp - 4]
00439139 8d840ab8030000           lea        eax, [edx + ecx + 0x3b8]
00439140 b904000000               mov        ecx, 4
00439145 6bd100                   imul       edx, ecx, 0
00439148 f30f110410               movss      dword ptr [eax + edx], xmm0
0043914d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00439150 f30f104074               movss      xmm0, dword ptr [eax + 0x74]
00439155 f30f5e05a0cd5600         divss      xmm0, dword ptr [0x56cda0]
0043915d b910000000               mov        ecx, 0x10
00439162 c1e100                   shl        ecx, 0
00439165 8b55fc                   mov        edx, dword ptr [ebp - 4]
00439168 8d840ab8030000           lea        eax, [edx + ecx + 0x3b8]
0043916f b904000000               mov        ecx, 4
00439174 c1e100                   shl        ecx, 0
00439177 f30f110408               movss      dword ptr [eax + ecx], xmm0
0043917c 8b55fc                   mov        edx, dword ptr [ebp - 4]
0043917f 8b45f8                   mov        eax, dword ptr [ebp - 8]
00439182 f30f104270               movss      xmm0, dword ptr [edx + 0x70]
00439187 f30f5e4020               divss      xmm0, dword ptr [eax + 0x20]
0043918c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0043918f f30f594150               mulss      xmm0, dword ptr [ecx + 0x50]
00439194 ba10000000               mov        edx, 0x10
00439199 6bc200                   imul       eax, edx, 0
0043919c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0043919f 8d9401f8030000           lea        edx, [ecx + eax + 0x3f8]
004391a6 b804000000               mov        eax, 4
004391ab 6bc800                   imul       ecx, eax, 0
004391ae f30f11040a               movss      dword ptr [edx + ecx], xmm0
004391b3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004391b6 8b45f8                   mov        eax, dword ptr [ebp - 8]
004391b9 f30f104274               movss      xmm0, dword ptr [edx + 0x74]
004391be f30f5e401c               divss      xmm0, dword ptr [eax + 0x1c]
004391c3 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004391c6 f30f594154               mulss      xmm0, dword ptr [ecx + 0x54]
004391cb ba10000000               mov        edx, 0x10
004391d0 c1e200                   shl        edx, 0
004391d3 8b45fc                   mov        eax, dword ptr [ebp - 4]
004391d6 8d8c10f8030000           lea        ecx, [eax + edx + 0x3f8]
004391dd ba04000000               mov        edx, 4
004391e2 c1e200                   shl        edx, 0
004391e5 f30f110411               movss      dword ptr [ecx + edx], xmm0
004391ea 8b75fc                   mov        esi, dword ptr [ebp - 4]
004391ed 81c6b8030000             add        esi, 0x3b8
004391f3 8b7dfc                   mov        edi, dword ptr [ebp - 4]
004391f6 81c77c050000             add        edi, 0x57c
004391fc b910000000               mov        ecx, 0x10
00439201 f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
00439203 8b45f8                   mov        eax, dword ptr [ebp - 8]
00439206 f30f10400c               movss      xmm0, dword ptr [eax + 0xc]
0043920b f30f584508               addss      xmm0, dword ptr [ebp + 8]
00439210 f30f2cc8                 cvttss2si  ecx, xmm0
00439214 8b55fc                   mov        edx, dword ptr [ebp - 4]
00439217 898aa8030000             mov        dword ptr [edx + 0x3a8], ecx
0043921d 8b45f8                   mov        eax, dword ptr [ebp - 8]
00439220 f30f104010               movss      xmm0, dword ptr [eax + 0x10]
00439225 f30f58450c               addss      xmm0, dword ptr [ebp + 0xc]
0043922a f30f2cc8                 cvttss2si  ecx, xmm0
0043922e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00439231 898aac030000             mov        dword ptr [edx + 0x3ac], ecx
00439237 8b45f8                   mov        eax, dword ptr [ebp - 8]
0043923a f30f10400c               movss      xmm0, dword ptr [eax + 0xc]
0043923f f30f584508               addss      xmm0, dword ptr [ebp + 8]
00439244 f30f584510               addss      xmm0, dword ptr [ebp + 0x10]
00439249 f30f2cc8                 cvttss2si  ecx, xmm0
0043924d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00439250 898ab0030000             mov        dword ptr [edx + 0x3b0], ecx
00439256 8b45f8                   mov        eax, dword ptr [ebp - 8]
00439259 f30f104010               movss      xmm0, dword ptr [eax + 0x10]
0043925e f30f58450c               addss      xmm0, dword ptr [ebp + 0xc]
00439263 f30f584514               addss      xmm0, dword ptr [ebp + 0x14]
00439268 f30f2cc8                 cvttss2si  ecx, xmm0
0043926c 8b55fc                   mov        edx, dword ptr [ebp - 4]
0043926f 898ab4030000             mov        dword ptr [edx + 0x3b4], ecx
00439275 5f                       pop        edi
00439276 5e                       pop        esi
00439277 8be5                     mov        esp, ebp
00439279 5d                       pop        ebp
0043927a c21000                   ret        0x10
0043927d cc                       int3       
0043927e cc                       int3       
0043927f cc                       int3       
