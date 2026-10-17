0050ff70 55                       push       ebp
0050ff71 8bec                     mov        ebp, esp
0050ff73 83ec30                   sub        esp, 0x30
0050ff76 894dec                   mov        dword ptr [ebp - 0x14], ecx
0050ff79 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0050ff7c 8b88900f0000             mov        ecx, dword ptr [eax + 0xf90]
0050ff82 51                       push       ecx
0050ff83 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0050ff89 e81277f6ff               call       0x4776a0
0050ff8e 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0050ff91 81c200060000             add        edx, 0x600
0050ff97 8955fc                   mov        dword ptr [ebp - 4], edx
0050ff9a c745f400000000           mov        dword ptr [ebp - 0xc], 0
0050ffa1 eb12                     jmp        0x50ffb5
0050ffa3 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0050ffa6 83c001                   add        eax, 1
0050ffa9 8945f4                   mov        dword ptr [ebp - 0xc], eax
0050ffac 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050ffaf 83c144                   add        ecx, 0x44
0050ffb2 894dfc                   mov        dword ptr [ebp - 4], ecx
0050ffb5 837df40d                 cmp        dword ptr [ebp - 0xc], 0xd
0050ffb9 0f8d9d000000             jge        0x51005c
0050ffbf 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050ffc2 0fb64238                 movzx      eax, byte ptr [edx + 0x38]
0050ffc6 85c0                     test       eax, eax
0050ffc8 7502                     jne        0x50ffcc
0050ffca ebd7                     jmp        0x50ffa3
0050ffcc b9e4ef5a00               mov        ecx, 0x5aefe4
0050ffd1 e80a93f1ff               call       0x4292e0
0050ffd6 d95de8                   fstp       dword ptr [ebp - 0x18]
0050ffd9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050ffdc f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
0050ffe1 f30f594118               mulss      xmm0, dword ptr [ecx + 0x18]
0050ffe6 f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
0050ffeb 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050ffee f30f104210               movss      xmm0, dword ptr [edx + 0x10]
0050fff3 f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
0050fff8 f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
0050fffd f30f5c45e0               subss      xmm0, dword ptr [ebp - 0x20]
00510002 8b45fc                   mov        eax, dword ptr [ebp - 4]
00510005 f30f114010               movss      dword ptr [eax + 0x10], xmm0
0051000a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051000d f30f104118               movss      xmm0, dword ptr [ecx + 0x18]
00510012 f30f59058c175700         mulss      xmm0, dword ptr [0x57178c]
0051001a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051001d f30f114218               movss      dword ptr [edx + 0x18], xmm0
00510022 8b45fc                   mov        eax, dword ptr [ebp - 4]
00510025 83c020                   add        eax, 0x20
00510028 8945dc                   mov        dword ptr [ebp - 0x24], eax
0051002b 6a00                     push       0
0051002d 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00510030 e80b35f1ff               call       0x423540
00510035 90                       nop        
00510036 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00510039 83c120                   add        ecx, 0x20
0051003c 894dd8                   mov        dword ptr [ebp - 0x28], ecx
0051003f 6a3c                     push       0x3c
00510041 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00510044 e82710f5ff               call       0x461070
00510049 0fb6d0                   movzx      edx, al
0051004c 85d2                     test       edx, edx
0051004e 7407                     je         0x510057
00510050 8b45fc                   mov        eax, dword ptr [ebp - 4]
00510053 c6403800                 mov        byte ptr [eax + 0x38], 0
00510057 e947ffffff               jmp        0x50ffa3
0051005c c745f000000000           mov        dword ptr [ebp - 0x10], 0
00510063 eb12                     jmp        0x510077
00510065 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00510068 83c101                   add        ecx, 1
0051006b 894df0                   mov        dword ptr [ebp - 0x10], ecx
0051006e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00510071 83c244                   add        edx, 0x44
00510074 8955fc                   mov        dword ptr [ebp - 4], edx
00510077 837df005                 cmp        dword ptr [ebp - 0x10], 5
0051007b 0f8d83000000             jge        0x510104
00510081 8b45fc                   mov        eax, dword ptr [ebp - 4]
00510084 0fb64838                 movzx      ecx, byte ptr [eax + 0x38]
00510088 85c9                     test       ecx, ecx
0051008a 7502                     jne        0x51008e
0051008c ebd7                     jmp        0x510065
0051008e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00510091 83c220                   add        edx, 0x20
00510094 8955d4                   mov        dword ptr [ebp - 0x2c], edx
00510097 6a00                     push       0
00510099 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0051009c e89f34f1ff               call       0x423540
005100a1 90                       nop        
005100a2 8b45fc                   mov        eax, dword ptr [ebp - 4]
005100a5 83c020                   add        eax, 0x20
005100a8 8945d0                   mov        dword ptr [ebp - 0x30], eax
005100ab 6a3c                     push       0x3c
005100ad 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
005100b0 e8bb0ff5ff               call       0x461070
005100b5 0fb6c8                   movzx      ecx, al
005100b8 85c9                     test       ecx, ecx
005100ba 7443                     je         0x5100ff
005100bc 8b55fc                   mov        edx, dword ptr [ebp - 4]
005100bf 8b421c                   mov        eax, dword ptr [edx + 0x1c]
005100c2 25000000ff               and        eax, 0xff000000
005100c7 c1e818                   shr        eax, 0x18
005100ca 8945f8                   mov        dword ptr [ebp - 8], eax
005100cd 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005100d0 83e904                   sub        ecx, 4
005100d3 894df8                   mov        dword ptr [ebp - 8], ecx
005100d6 837df800                 cmp        dword ptr [ebp - 8], 0
005100da 7f09                     jg         0x5100e5
005100dc 8b55fc                   mov        edx, dword ptr [ebp - 4]
005100df c6423800                 mov        byte ptr [edx + 0x38], 0
005100e3 eb1a                     jmp        0x5100ff
005100e5 8b45f8                   mov        eax, dword ptr [ebp - 8]
005100e8 c1e018                   shl        eax, 0x18
005100eb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005100ee 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
005100f1 81e2ffffff00             and        edx, 0xffffff
005100f7 0bc2                     or         eax, edx
005100f9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005100fc 89411c                   mov        dword ptr [ecx + 0x1c], eax
005100ff e961ffffff               jmp        0x510065
00510104 b801000000               mov        eax, 1
00510109 8be5                     mov        esp, ebp
0051010b 5d                       pop        ebp
0051010c c3                       ret        
0051010d cc                       int3       
0051010e cc                       int3       
0051010f cc                       int3       
