00473490 55                       push       ebp
00473491 8bec                     mov        ebp, esp
00473493 83ec14                   sub        esp, 0x14
00473496 894dfc                   mov        dword ptr [ebp - 4], ecx
00473499 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047349c 8b4808                   mov        ecx, dword ptr [eax + 8]
0047349f e8dcf8f9ff               call       0x412d80
004734a4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004734a7 8b490c                   mov        ecx, dword ptr [ecx + 0xc]
004734aa e8d1f8f9ff               call       0x412d80
004734af 8b55fc                   mov        edx, dword ptr [ebp - 4]
004734b2 8b8a68330000             mov        ecx, dword ptr [edx + 0x3368]
004734b8 e8c3f8f9ff               call       0x412d80
004734bd c745f000000000           mov        dword ptr [ebp - 0x10], 0
004734c4 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004734cb eb09                     jmp        0x4734d6
004734cd 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004734d0 83c001                   add        eax, 1
004734d3 8945f4                   mov        dword ptr [ebp - 0xc], eax
004734d6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004734d9 8b9124330000             mov        edx, dword ptr [ecx + 0x3324]
004734df 0fbf02                   movsx      eax, word ptr [edx]
004734e2 3945f4                   cmp        dword ptr [ebp - 0xc], eax
004734e5 0f8d91000000             jge        0x47357c
004734eb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004734ee 8b9128330000             mov        edx, dword ptr [ecx + 0x3328]
004734f4 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004734f7 8b0c82                   mov        ecx, dword ptr [edx + eax*4]
004734fa c6410301                 mov        byte ptr [ecx + 3], 1
004734fe 8b55fc                   mov        edx, dword ptr [ebp - 4]
00473501 8b8228330000             mov        eax, dword ptr [edx + 0x3328]
00473507 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0047350a 8b1488                   mov        edx, dword ptr [eax + ecx*4]
0047350d 83c230                   add        edx, 0x30
00473510 8955f8                   mov        dword ptr [ebp - 8], edx
00473513 8b45f8                   mov        eax, dword ptr [ebp - 8]
00473516 0fbf08                   movsx      ecx, word ptr [eax]
00473519 85c9                     test       ecx, ecx
0047351b 7c5a                     jl         0x473577
0047351d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00473520 8b8234330000             mov        eax, dword ptr [edx + 0x3334]
00473526 8945ec                   mov        dword ptr [ebp - 0x14], eax
00473529 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0047352c 0fbf5104                 movsx      edx, word ptr [ecx + 4]
00473530 52                       push       edx
00473531 6945f0e4050000           imul       eax, dword ptr [ebp - 0x10], 0x5e4
00473538 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047353b 038120330000             add        eax, dword ptr [ecx + 0x3320]
00473541 50                       push       eax
00473542 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00473545 e8364efcff               call       0x438380
0047354a 8b55f8                   mov        edx, dword ptr [ebp - 8]
0047354d 668b45f0                 mov        ax, word ptr [ebp - 0x10]
00473551 66894206                 mov        word ptr [edx + 6], ax
00473555 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00473558 83c101                   add        ecx, 1
0047355b 894df0                   mov        dword ptr [ebp - 0x10], ecx
0047355e 8b55f8                   mov        edx, dword ptr [ebp - 8]
00473561 0fbf4202                 movsx      eax, word ptr [edx + 2]
00473565 50                       push       eax
00473566 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00473569 51                       push       ecx
0047356a e8f1dbffff               call       0x471160
0047356f 83c408                   add        esp, 8
00473572 8945f8                   mov        dword ptr [ebp - 8], eax
00473575 eb9c                     jmp        0x473513
00473577 e951ffffff               jmp        0x4734cd
0047357c 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047357f c7422000000000           mov        dword ptr [edx + 0x20], 0
00473586 8be5                     mov        esp, ebp
00473588 5d                       pop        ebp
00473589 c3                       ret        
0047358a cc                       int3       
0047358b cc                       int3       
0047358c cc                       int3       
0047358d cc                       int3       
0047358e cc                       int3       
0047358f cc                       int3       
