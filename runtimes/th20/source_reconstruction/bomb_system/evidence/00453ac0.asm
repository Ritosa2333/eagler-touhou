00453ac0 55                       push       ebp
00453ac1 8bec                     mov        ebp, esp
00453ac3 81ec9c000000             sub        esp, 0x9c
00453ac9 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00453ace 33c5                     xor        eax, ebp
00453ad0 8945fc                   mov        dword ptr [ebp - 4], eax
00453ad3 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
00453ad6 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00453ad9 8b4844                   mov        ecx, dword ptr [eax + 0x44]
00453adc c1e905                   shr        ecx, 5
00453adf 83e101                   and        ecx, 1
00453ae2 7405                     je         0x453ae9
00453ae4 e9f2020000               jmp        0x453ddb
00453ae9 8d4df0                   lea        ecx, [ebp - 0x10]
00453aec e81ff3fcff               call       0x422e10
00453af1 90                       nop        
00453af2 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00453af5 8b4244                   mov        eax, dword ptr [edx + 0x44]
00453af8 83e00f                   and        eax, 0xf
00453afb 8945d0                   mov        dword ptr [ebp - 0x30], eax
00453afe 837dd004                 cmp        dword ptr [ebp - 0x30], 4
00453b02 0f87ca020000             ja         0x453dd2
00453b08 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00453b0b ff248dec3d4500           jmp        dword ptr [ecx*4 + 0x453dec]
00453b12 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00453b15 e8a6270000               call       0x4562c0
00453b1a 8b10                     mov        edx, dword ptr [eax]
00453b1c 8955f0                   mov        dword ptr [ebp - 0x10], edx
00453b1f 8b4804                   mov        ecx, dword ptr [eax + 4]
00453b22 894df4                   mov        dword ptr [ebp - 0xc], ecx
00453b25 8b5008                   mov        edx, dword ptr [eax + 8]
00453b28 8955f8                   mov        dword ptr [ebp - 8], edx
00453b2b 8d45f0                   lea        eax, [ebp - 0x10]
00453b2e 50                       push       eax
00453b2f 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00453b32 e8a95bfdff               call       0x4296e0
00453b37 90                       nop        
00453b38 e995020000               jmp        0x453dd2
00453b3d 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00453b40 f30f104120               movss      xmm0, dword ptr [ecx + 0x20]
00453b45 f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
00453b4a 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00453b4d 83c11c                   add        ecx, 0x1c
00453b50 e88b57fdff               call       0x4292e0
00453b55 d95dc8                   fstp       dword ptr [ebp - 0x38]
00453b58 51                       push       ecx
00453b59 f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
00453b5e f30f110424               movss      dword ptr [esp], xmm0
00453b63 51                       push       ecx
00453b64 f30f1045c8               movss      xmm0, dword ptr [ebp - 0x38]
00453b69 f30f110424               movss      dword ptr [esp], xmm0
00453b6e 8d55f0                   lea        edx, [ebp - 0x10]
00453b71 52                       push       edx
00453b72 e8b957feff               call       0x439330
00453b77 83c40c                   add        esp, 0xc
00453b7a 0f57c0                   xorps      xmm0, xmm0
00453b7d f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00453b82 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00453b85 e836270000               call       0x4562c0
00453b8a 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00453b8d 8d45f0                   lea        eax, [ebp - 0x10]
00453b90 50                       push       eax
00453b91 8d4d88                   lea        ecx, [ebp - 0x78]
00453b94 51                       push       ecx
00453b95 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00453b98 e8d359fdff               call       0x429570
00453b9d 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00453ba0 8b08                     mov        ecx, dword ptr [eax]
00453ba2 890a                     mov        dword ptr [edx], ecx
00453ba4 8b4804                   mov        ecx, dword ptr [eax + 4]
00453ba7 894a04                   mov        dword ptr [edx + 4], ecx
00453baa 8b4008                   mov        eax, dword ptr [eax + 8]
00453bad 894208                   mov        dword ptr [edx + 8], eax
00453bb0 e91d020000               jmp        0x453dd2
00453bb5 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00453bb8 f30f104120               movss      xmm0, dword ptr [ecx + 0x20]
00453bbd f30f1145bc               movss      dword ptr [ebp - 0x44], xmm0
00453bc2 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00453bc5 83c21c                   add        edx, 0x1c
00453bc8 8955c0                   mov        dword ptr [ebp - 0x40], edx
00453bcb 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00453bce 83c028                   add        eax, 0x28
00453bd1 50                       push       eax
00453bd2 8d4d98                   lea        ecx, [ebp - 0x68]
00453bd5 51                       push       ecx
00453bd6 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00453bd9 e80259fdff               call       0x4294e0
00453bde 8bc8                     mov        ecx, eax
00453be0 e8fb56fdff               call       0x4292e0
00453be5 51                       push       ecx
00453be6 d91c24                   fstp       dword ptr [esp]
00453be9 e85249feff               call       0x438540
00453bee 83c404                   add        esp, 4
00453bf1 d95db8                   fstp       dword ptr [ebp - 0x48]
00453bf4 51                       push       ecx
00453bf5 f30f1045bc               movss      xmm0, dword ptr [ebp - 0x44]
00453bfa f30f110424               movss      dword ptr [esp], xmm0
00453bff 51                       push       ecx
00453c00 f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
00453c05 f30f110424               movss      dword ptr [esp], xmm0
00453c0a 8d55f0                   lea        edx, [ebp - 0x10]
00453c0d 52                       push       edx
00453c0e e81d57feff               call       0x439330
00453c13 83c40c                   add        esp, 0xc
00453c16 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00453c19 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00453c1e f30f59402c               mulss      xmm0, dword ptr [eax + 0x2c]
00453c23 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00453c28 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00453c2b 83c128                   add        ecx, 0x28
00453c2e e8ad56fdff               call       0x4292e0
00453c33 51                       push       ecx
00453c34 d91c24                   fstp       dword ptr [esp]
00453c37 8d4df0                   lea        ecx, [ebp - 0x10]
00453c3a 51                       push       ecx
00453c3b 8d55f0                   lea        edx, [ebp - 0x10]
00453c3e 52                       push       edx
00453c3f e85c530000               call       0x458fa0
00453c44 83c40c                   add        esp, 0xc
00453c47 0f57c0                   xorps      xmm0, xmm0
00453c4a f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00453c4f 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00453c52 e869260000               call       0x4562c0
00453c57 8945b4                   mov        dword ptr [ebp - 0x4c], eax
00453c5a 8d45f0                   lea        eax, [ebp - 0x10]
00453c5d 50                       push       eax
00453c5e 8d8d7cffffff             lea        ecx, [ebp - 0x84]
00453c64 51                       push       ecx
00453c65 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00453c68 e80359fdff               call       0x429570
00453c6d 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00453c70 8b08                     mov        ecx, dword ptr [eax]
00453c72 890a                     mov        dword ptr [edx], ecx
00453c74 8b4804                   mov        ecx, dword ptr [eax + 4]
00453c77 894a04                   mov        dword ptr [edx + 4], ecx
00453c7a 8b4008                   mov        eax, dword ptr [eax + 8]
00453c7d 894208                   mov        dword ptr [edx + 8], eax
00453c80 e94d010000               jmp        0x453dd2
00453c85 8d4de4                   lea        ecx, [ebp - 0x1c]
00453c88 e883f1fcff               call       0x422e10
00453c8d 8d4dd8                   lea        ecx, [ebp - 0x28]
00453c90 e87bf1fcff               call       0x422e10
00453c95 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00453c98 8b11                     mov        edx, dword ptr [ecx]
00453c9a 8955d8                   mov        dword ptr [ebp - 0x28], edx
00453c9d 8b4104                   mov        eax, dword ptr [ecx + 4]
00453ca0 8945dc                   mov        dword ptr [ebp - 0x24], eax
00453ca3 8b4908                   mov        ecx, dword ptr [ecx + 8]
00453ca6 894de0                   mov        dword ptr [ebp - 0x20], ecx
00453ca9 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00453cac e80f260000               call       0x4562c0
00453cb1 50                       push       eax
00453cb2 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00453cb5 83c10c                   add        ecx, 0xc
00453cb8 e8235afdff               call       0x4296e0
00453cbd 90                       nop        
00453cbe 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00453cc1 83c228                   add        edx, 0x28
00453cc4 8955b0                   mov        dword ptr [ebp - 0x50], edx
00453cc7 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00453ccf f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00453cd7 51                       push       ecx
00453cd8 f30f110424               movss      dword ptr [esp], xmm0
00453cdd 8d4594                   lea        eax, [ebp - 0x6c]
00453ce0 50                       push       eax
00453ce1 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00453ce4 e8d7f2ffff               call       0x452fc0
00453ce9 8bc8                     mov        ecx, eax
00453ceb e8f055fdff               call       0x4292e0
00453cf0 51                       push       ecx
00453cf1 d91c24                   fstp       dword ptr [esp]
00453cf4 e84748feff               call       0x438540
00453cf9 83c404                   add        esp, 4
00453cfc d95da0                   fstp       dword ptr [ebp - 0x60]
00453cff 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00453d02 83c130                   add        ecx, 0x30
00453d05 e8d655fdff               call       0x4292e0
00453d0a 51                       push       ecx
00453d0b d91c24                   fstp       dword ptr [esp]
00453d0e e80d5bfeff               call       0x439820
00453d13 83c404                   add        esp, 4
00453d16 d95dac                   fstp       dword ptr [ebp - 0x54]
00453d19 f30f1045ac               movss      xmm0, dword ptr [ebp - 0x54]
00453d1e 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00453d21 f30f594120               mulss      xmm0, dword ptr [ecx + 0x20]
00453d26 b9e4ef5a00               mov        ecx, 0x5aefe4
00453d2b f30f1145a8               movss      dword ptr [ebp - 0x58], xmm0
00453d30 e8ab55fdff               call       0x4292e0
00453d35 d95da4                   fstp       dword ptr [ebp - 0x5c]
00453d38 f30f1045a8               movss      xmm0, dword ptr [ebp - 0x58]
00453d3d f30f5945a4               mulss      xmm0, dword ptr [ebp - 0x5c]
00453d42 51                       push       ecx
00453d43 f30f110424               movss      dword ptr [esp], xmm0
00453d48 51                       push       ecx
00453d49 f30f1045a0               movss      xmm0, dword ptr [ebp - 0x60]
00453d4e f30f110424               movss      dword ptr [esp], xmm0
00453d53 8d55e4                   lea        edx, [ebp - 0x1c]
00453d56 52                       push       edx
00453d57 e8d455feff               call       0x439330
00453d5c 83c40c                   add        esp, 0xc
00453d5f 0f57c0                   xorps      xmm0, xmm0
00453d62 f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
00453d67 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00453d6a 83c00c                   add        eax, 0xc
00453d6d 50                       push       eax
00453d6e 8d8d70ffffff             lea        ecx, [ebp - 0x90]
00453d74 51                       push       ecx
00453d75 8d4de4                   lea        ecx, [ebp - 0x1c]
00453d78 e8f357fdff               call       0x429570
00453d7d 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00453d80 8b08                     mov        ecx, dword ptr [eax]
00453d82 890a                     mov        dword ptr [edx], ecx
00453d84 8b4804                   mov        ecx, dword ptr [eax + 4]
00453d87 894a04                   mov        dword ptr [edx + 4], ecx
00453d8a 8b4008                   mov        eax, dword ptr [eax + 8]
00453d8d 894208                   mov        dword ptr [edx + 8], eax
00453d90 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00453d93 894d9c                   mov        dword ptr [ebp - 0x64], ecx
00453d96 8d55d8                   lea        edx, [ebp - 0x28]
00453d99 52                       push       edx
00453d9a 8d8564ffffff             lea        eax, [ebp - 0x9c]
00453da0 50                       push       eax
00453da1 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
00453da4 e89756fdff               call       0x429440
00453da9 8b08                     mov        ecx, dword ptr [eax]
00453dab 894de4                   mov        dword ptr [ebp - 0x1c], ecx
00453dae 8b5004                   mov        edx, dword ptr [eax + 4]
00453db1 8955e8                   mov        dword ptr [ebp - 0x18], edx
00453db4 8b4008                   mov        eax, dword ptr [eax + 8]
00453db7 8945ec                   mov        dword ptr [ebp - 0x14], eax
00453dba 8d4de4                   lea        ecx, [ebp - 0x1c]
00453dbd 51                       push       ecx
00453dbe e84d240000               call       0x456210
00453dc3 d91c24                   fstp       dword ptr [esp]
00453dc6 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00453dc9 83c11c                   add        ecx, 0x1c
00453dcc e84ff1ffff               call       0x452f20
00453dd1 90                       nop        
00453dd2 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00453dd5 e8f6050000               call       0x4543d0
00453dda 90                       nop        
00453ddb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00453dde 33cd                     xor        ecx, ebp
00453de0 e809ec0e00               call       0x5429ee
00453de5 8be5                     mov        esp, ebp
00453de7 5d                       pop        ebp
00453de8 c3                       ret        
00453de9 0f1f00                   nop        dword ptr [eax]
00453dec 123b                     adc        bh, byte ptr [ebx]
00453dee 45                       inc        ebp
00453def 00d2                     add        dl, dl
00453df1 3d45003d3b               cmp        eax, 0x3b3d0045
00453df6 45                       inc        ebp
00453df7 00b53b450085             add        byte ptr [ebp - 0x7affbac5], dh
00453dfd 3c45                     cmp        al, 0x45
