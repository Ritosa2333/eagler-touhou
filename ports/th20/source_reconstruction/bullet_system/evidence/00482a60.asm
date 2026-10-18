00482a60 55                       push       ebp
00482a61 8bec                     mov        ebp, esp
00482a63 83ec50                   sub        esp, 0x50
00482a66 56                       push       esi
00482a67 894dfc                   mov        dword ptr [ebp - 4], ecx
00482a6a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00482a6d 8b8820050000             mov        ecx, dword ptr [eax + 0x520]
00482a73 e88898f8ff               call       0x40c300
00482a78 8bc8                     mov        ecx, eax
00482a7a e8112f0000               call       0x485990
00482a7f 0f57c0                   xorps      xmm0, xmm0
00482a82 0f2f00                   comiss     xmm0, dword ptr [eax]
00482a85 727f                     jb         0x482b06
00482a87 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482a8a 81c1a0000000             add        ecx, 0xa0
00482a90 894df0                   mov        dword ptr [ebp - 0x10], ecx
00482a93 6a04                     push       4
00482a95 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00482a98 e8e39bffff               call       0x47c680
00482a9d f30f10401c               movss      xmm0, dword ptr [eax + 0x1c]
00482aa2 f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
00482aa7 8b55fc                   mov        edx, dword ptr [ebp - 4]
00482aaa 81c2a0000000             add        edx, 0xa0
00482ab0 8955ec                   mov        dword ptr [ebp - 0x14], edx
00482ab3 6a04                     push       4
00482ab5 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00482ab8 e8c39bffff               call       0x47c680
00482abd f30f104018               movss      xmm0, dword ptr [eax + 0x18]
00482ac2 f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00482ac7 51                       push       ecx
00482ac8 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00482acd f30f110424               movss      dword ptr [esp], xmm0
00482ad2 51                       push       ecx
00482ad3 f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00482ad8 f30f110424               movss      dword ptr [esp], xmm0
00482add 51                       push       ecx
00482ade 0f57c0                   xorps      xmm0, xmm0
00482ae1 f30f110424               movss      dword ptr [esp], xmm0
00482ae6 51                       push       ecx
00482ae7 0f57c0                   xorps      xmm0, xmm0
00482aea f30f110424               movss      dword ptr [esp], xmm0
00482aef 8b45fc                   mov        eax, dword ptr [ebp - 4]
00482af2 83c064                   add        eax, 0x64
00482af5 50                       push       eax
00482af6 e825290000               call       0x485420
00482afb 83c414                   add        esp, 0x14
00482afe 85c0                     test       eax, eax
00482b00 0f85a2000000             jne        0x482ba8
00482b06 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482b09 8b8920050000             mov        ecx, dword ptr [ecx + 0x520]
00482b0f e8ec97f8ff               call       0x40c300
00482b14 8bc8                     mov        ecx, eax
00482b16 e8752e0000               call       0x485990
00482b1b f30f1000                 movss      xmm0, dword ptr [eax]
00482b1f 0f2f05e8e05600           comiss     xmm0, dword ptr [0x56e0e8]
00482b26 0f866d020000             jbe        0x482d99
00482b2c 8b55fc                   mov        edx, dword ptr [ebp - 4]
00482b2f 8b8a20050000             mov        ecx, dword ptr [edx + 0x520]
00482b35 e8c697f8ff               call       0x40c300
00482b3a 8bc8                     mov        ecx, eax
00482b3c e84f2e0000               call       0x485990
00482b41 f30f104004               movss      xmm0, dword ptr [eax + 4]
00482b46 f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
00482b4b 8b45fc                   mov        eax, dword ptr [ebp - 4]
00482b4e 8b8820050000             mov        ecx, dword ptr [eax + 0x520]
00482b54 e8a797f8ff               call       0x40c300
00482b59 8bc8                     mov        ecx, eax
00482b5b e8302e0000               call       0x485990
00482b60 f30f1000                 movss      xmm0, dword ptr [eax]
00482b64 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00482b69 51                       push       ecx
00482b6a f30f1045e0               movss      xmm0, dword ptr [ebp - 0x20]
00482b6f f30f110424               movss      dword ptr [esp], xmm0
00482b74 51                       push       ecx
00482b75 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00482b7a f30f110424               movss      dword ptr [esp], xmm0
00482b7f 51                       push       ecx
00482b80 0f57c0                   xorps      xmm0, xmm0
00482b83 f30f110424               movss      dword ptr [esp], xmm0
00482b88 51                       push       ecx
00482b89 0f57c0                   xorps      xmm0, xmm0
00482b8c f30f110424               movss      dword ptr [esp], xmm0
00482b91 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482b94 83c164                   add        ecx, 0x64
00482b97 51                       push       ecx
00482b98 e883280000               call       0x485420
00482b9d 83c414                   add        esp, 0x14
00482ba0 85c0                     test       eax, eax
00482ba2 0f84f1010000             je         0x482d99
00482ba8 c745f800000000           mov        dword ptr [ebp - 8], 0
00482baf 8b55fc                   mov        edx, dword ptr [ebp - 4]
00482bb2 81c2a0000000             add        edx, 0xa0
00482bb8 8955d8                   mov        dword ptr [ebp - 0x28], edx
00482bbb 6a04                     push       4
00482bbd 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00482bc0 e8bb9affff               call       0x47c680
00482bc5 8b403c                   mov        eax, dword ptr [eax + 0x3c]
00482bc8 83e001                   and        eax, 1
00482bcb 7413                     je         0x482be0
00482bcd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482bd0 e87b050000               call       0x483150
00482bd5 85c0                     test       eax, eax
00482bd7 7407                     je         0x482be0
00482bd9 c745f801000000           mov        dword ptr [ebp - 8], 1
00482be0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482be3 81c1a0000000             add        ecx, 0xa0
00482be9 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
00482bec 6a04                     push       4
00482bee 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00482bf1 e88a9affff               call       0x47c680
00482bf6 8b503c                   mov        edx, dword ptr [eax + 0x3c]
00482bf9 83e202                   and        edx, 2
00482bfc 7413                     je         0x482c11
00482bfe 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482c01 e89a010000               call       0x482da0
00482c06 85c0                     test       eax, eax
00482c08 7407                     je         0x482c11
00482c0a c745f801000000           mov        dword ptr [ebp - 8], 1
00482c11 8b45fc                   mov        eax, dword ptr [ebp - 4]
00482c14 05a0000000               add        eax, 0xa0
00482c19 8945d0                   mov        dword ptr [ebp - 0x30], eax
00482c1c 6a04                     push       4
00482c1e 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00482c21 e85a9affff               call       0x47c680
00482c26 8b483c                   mov        ecx, dword ptr [eax + 0x3c]
00482c29 83e108                   and        ecx, 8
00482c2c 7413                     je         0x482c41
00482c2e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482c31 e8da030000               call       0x483010
00482c36 85c0                     test       eax, eax
00482c38 7407                     je         0x482c41
00482c3a c745f801000000           mov        dword ptr [ebp - 8], 1
00482c41 8b55fc                   mov        edx, dword ptr [ebp - 4]
00482c44 81c2a0000000             add        edx, 0xa0
00482c4a 8955cc                   mov        dword ptr [ebp - 0x34], edx
00482c4d 6a04                     push       4
00482c4f 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00482c52 e8299affff               call       0x47c680
00482c57 8b403c                   mov        eax, dword ptr [eax + 0x3c]
00482c5a 83e004                   and        eax, 4
00482c5d 7413                     je         0x482c72
00482c5f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482c62 e859020000               call       0x482ec0
00482c67 85c0                     test       eax, eax
00482c69 7407                     je         0x482c72
00482c6b c745f801000000           mov        dword ptr [ebp - 8], 1
00482c72 837df800                 cmp        dword ptr [ebp - 8], 0
00482c76 0f84cc000000             je         0x482d48
00482c7c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482c7f 81c1a0000000             add        ecx, 0xa0
00482c85 894dc8                   mov        dword ptr [ebp - 0x38], ecx
00482c88 6a04                     push       4
00482c8a 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00482c8d e8ee99ffff               call       0x47c680
00482c92 f30f104010               movss      xmm0, dword ptr [eax + 0x10]
00482c97 0f2f0574fd5600           comiss     xmm0, dword ptr [0x56fd74]
00482c9e 761f                     jbe        0x482cbf
00482ca0 8b55fc                   mov        edx, dword ptr [ebp - 4]
00482ca3 81c2a0000000             add        edx, 0xa0
00482ca9 8955c4                   mov        dword ptr [ebp - 0x3c], edx
00482cac 6a04                     push       4
00482cae 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00482cb1 e8ca99ffff               call       0x47c680
00482cb6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482cb9 8b5010                   mov        edx, dword ptr [eax + 0x10]
00482cbc 895120                   mov        dword ptr [ecx + 0x20], edx
00482cbf 8b45fc                   mov        eax, dword ptr [ebp - 4]
00482cc2 f30f104020               movss      xmm0, dword ptr [eax + 0x20]
00482cc7 f30f1145c0               movss      dword ptr [ebp - 0x40], xmm0
00482ccc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482ccf 81c184000000             add        ecx, 0x84
00482cd5 e80666faff               call       0x4292e0
00482cda d95dbc                   fstp       dword ptr [ebp - 0x44]
00482cdd 51                       push       ecx
00482cde f30f1045c0               movss      xmm0, dword ptr [ebp - 0x40]
00482ce3 f30f110424               movss      dword ptr [esp], xmm0
00482ce8 51                       push       ecx
00482ce9 f30f1045bc               movss      xmm0, dword ptr [ebp - 0x44]
00482cee f30f110424               movss      dword ptr [esp], xmm0
00482cf3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482cf6 83c170                   add        ecx, 0x70
00482cf9 51                       push       ecx
00482cfa e83166fbff               call       0x439330
00482cff 83c40c                   add        esp, 0xc
00482d02 8b55fc                   mov        edx, dword ptr [ebp - 4]
00482d05 81c2a0000000             add        edx, 0xa0
00482d0b 8955b8                   mov        dword ptr [ebp - 0x48], edx
00482d0e 6a04                     push       4
00482d10 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00482d13 e86899ffff               call       0x47c680
00482d18 83c030                   add        eax, 0x30
00482d1b 8945f4                   mov        dword ptr [ebp - 0xc], eax
00482d1e 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00482d21 8b08                     mov        ecx, dword ptr [eax]
00482d23 83c101                   add        ecx, 1
00482d26 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00482d29 890a                     mov        dword ptr [edx], ecx
00482d2b 8b45fc                   mov        eax, dword ptr [ebp - 4]
00482d2e 83784000                 cmp        dword ptr [eax + 0x40], 0
00482d32 7c14                     jl         0x482d48
00482d34 6a00                     push       0
00482d36 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482d39 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00482d3c 52                       push       edx
00482d3d b930a85b00               mov        ecx, 0x5ba830
00482d42 e82940faff               call       0x426d70
00482d47 90                       nop        
00482d48 8b45fc                   mov        eax, dword ptr [ebp - 4]
00482d4b 05a0000000               add        eax, 0xa0
00482d50 8945b0                   mov        dword ptr [ebp - 0x50], eax
00482d53 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482d56 81c1a0000000             add        ecx, 0xa0
00482d5c 894db4                   mov        dword ptr [ebp - 0x4c], ecx
00482d5f 6a04                     push       4
00482d61 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00482d64 e81799ffff               call       0x47c680
00482d69 8bf0                     mov        esi, eax
00482d6b 6a04                     push       4
00482d6d 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00482d70 e80b99ffff               call       0x47c680
00482d75 8b5030                   mov        edx, dword ptr [eax + 0x30]
00482d78 3b5634                   cmp        edx, dword ptr [esi + 0x34]
00482d7b 7c1c                     jl         0x482d99
00482d7d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00482d80 8b8890000000             mov        ecx, dword ptr [eax + 0x90]
00482d86 83e1bf                   and        ecx, 0xffffffbf
00482d89 8b55fc                   mov        edx, dword ptr [ebp - 4]
00482d8c 898a90000000             mov        dword ptr [edx + 0x90], ecx
00482d92 b801000000               mov        eax, 1
00482d97 eb02                     jmp        0x482d9b
00482d99 33c0                     xor        eax, eax
00482d9b 5e                       pop        esi
00482d9c 8be5                     mov        esp, ebp
00482d9e 5d                       pop        ebp
00482d9f c3                       ret        
