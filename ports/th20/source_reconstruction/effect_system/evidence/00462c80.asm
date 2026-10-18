00462c80 53                       push       ebx
00462c81 8bdc                     mov        ebx, esp
00462c83 83ec08                   sub        esp, 8
00462c86 83e4f8                   and        esp, 0xfffffff8
00462c89 83c404                   add        esp, 4
00462c8c 55                       push       ebp
00462c8d 8b6b04                   mov        ebp, dword ptr [ebx + 4]
00462c90 896c2404                 mov        dword ptr [esp + 4], ebp
00462c94 8bec                     mov        ebp, esp
00462c96 81ec00010000             sub        esp, 0x100
00462c9c a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00462ca1 33c5                     xor        eax, ebp
00462ca3 8945fc                   mov        dword ptr [ebp - 4], eax
00462ca6 894df4                   mov        dword ptr [ebp - 0xc], ecx
00462ca9 8b4308                   mov        eax, dword ptr [ebx + 8]
00462cac 51                       push       ecx
00462cad f30f104018               movss      xmm0, dword ptr [eax + 0x18]
00462cb2 f30f110424               movss      dword ptr [esp], xmm0
00462cb7 8d4df8                   lea        ecx, [ebp - 8]
00462cba e85165fcff               call       0x429210
00462cbf 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00462cc2 8b5308                   mov        edx, dword ptr [ebx + 8]
00462cc5 8b4224                   mov        eax, dword ptr [edx + 0x24]
00462cc8 898144010000             mov        dword ptr [ecx + 0x144], eax
00462cce c745f000000000           mov        dword ptr [ebp - 0x10], 0
00462cd5 c745e800000000           mov        dword ptr [ebp - 0x18], 0
00462cdc eb09                     jmp        0x462ce7
00462cde 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00462ce1 83c101                   add        ecx, 1
00462ce4 894de8                   mov        dword ptr [ebp - 0x18], ecx
00462ce7 837de804                 cmp        dword ptr [ebp - 0x18], 4
00462ceb 0f8d5a010000             jge        0x462e4b
00462cf1 51                       push       ecx
00462cf2 0f57c0                   xorps      xmm0, xmm0
00462cf5 f30f110424               movss      dword ptr [esp], xmm0
00462cfa 51                       push       ecx
00462cfb 0f57c0                   xorps      xmm0, xmm0
00462cfe f30f110424               movss      dword ptr [esp], xmm0
00462d03 8d8d24ffffff             lea        ecx, [ebp - 0xdc]
00462d09 e84265fcff               call       0x429250
00462d0e 8b10                     mov        edx, dword ptr [eax]
00462d10 8b4004                   mov        eax, dword ptr [eax + 4]
00462d13 899540ffffff             mov        dword ptr [ebp - 0xc0], edx
00462d19 898544ffffff             mov        dword ptr [ebp - 0xbc], eax
00462d1f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00462d22 83c108                   add        ecx, 8
00462d25 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
00462d28 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00462d2b 8955d8                   mov        dword ptr [ebp - 0x28], edx
00462d2e 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
00462d31 50                       push       eax
00462d32 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00462d35 e836a9ffff               call       0x45d670
00462d3a 8b8d40ffffff             mov        ecx, dword ptr [ebp - 0xc0]
00462d40 8b9544ffffff             mov        edx, dword ptr [ebp - 0xbc]
00462d46 8908                     mov        dword ptr [eax], ecx
00462d48 895004                   mov        dword ptr [eax + 4], edx
00462d4b 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00462d4e 83c001                   add        eax, 1
00462d51 8945f0                   mov        dword ptr [ebp - 0x10], eax
00462d54 8b4b08                   mov        ecx, dword ptr [ebx + 8]
00462d57 f30f104124               movss      xmm0, dword ptr [ecx + 0x24]
00462d5c f30f1145c8               movss      dword ptr [ebp - 0x38], xmm0
00462d61 8d4df8                   lea        ecx, [ebp - 8]
00462d64 e87765fcff               call       0x4292e0
00462d69 d95dc4                   fstp       dword ptr [ebp - 0x3c]
00462d6c 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00462d6f 83c208                   add        edx, 8
00462d72 8955cc                   mov        dword ptr [ebp - 0x34], edx
00462d75 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00462d78 8945d0                   mov        dword ptr [ebp - 0x30], eax
00462d7b 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00462d7e 51                       push       ecx
00462d7f 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00462d82 e8e9a8ffff               call       0x45d670
00462d87 8945c0                   mov        dword ptr [ebp - 0x40], eax
00462d8a 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00462d8d 83c201                   add        edx, 1
00462d90 8955f0                   mov        dword ptr [ebp - 0x10], edx
00462d93 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00462d96 89852cffffff             mov        dword ptr [ebp - 0xd4], eax
00462d9c 51                       push       ecx
00462d9d f30f1045c8               movss      xmm0, dword ptr [ebp - 0x38]
00462da2 f30f110424               movss      dword ptr [esp], xmm0
00462da7 51                       push       ecx
00462da8 f30f1045c4               movss      xmm0, dword ptr [ebp - 0x3c]
00462dad f30f110424               movss      dword ptr [esp], xmm0
00462db2 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00462db5 51                       push       ecx
00462db6 e87565fdff               call       0x439330
00462dbb 83c40c                   add        esp, 0xc
00462dbe f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00462dc6 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00462dce 51                       push       ecx
00462dcf f30f110424               movss      dword ptr [esp], xmm0
00462dd4 8d4df8                   lea        ecx, [ebp - 8]
00462dd7 e8e402ffff               call       0x4530c0
00462ddc 8b5308                   mov        edx, dword ptr [ebx + 8]
00462ddf f30f104224               movss      xmm0, dword ptr [edx + 0x24]
00462de4 f30f1145b4               movss      dword ptr [ebp - 0x4c], xmm0
00462de9 8d4df8                   lea        ecx, [ebp - 8]
00462dec e8ef64fcff               call       0x4292e0
00462df1 d95db0                   fstp       dword ptr [ebp - 0x50]
00462df4 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00462df7 83c008                   add        eax, 8
00462dfa 8945b8                   mov        dword ptr [ebp - 0x48], eax
00462dfd 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00462e00 894dbc                   mov        dword ptr [ebp - 0x44], ecx
00462e03 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
00462e06 52                       push       edx
00462e07 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00462e0a e861a8ffff               call       0x45d670
00462e0f 8945ac                   mov        dword ptr [ebp - 0x54], eax
00462e12 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00462e15 83c001                   add        eax, 1
00462e18 8945f0                   mov        dword ptr [ebp - 0x10], eax
00462e1b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00462e1e 898d3cffffff             mov        dword ptr [ebp - 0xc4], ecx
00462e24 51                       push       ecx
00462e25 f30f1045b4               movss      xmm0, dword ptr [ebp - 0x4c]
00462e2a f30f110424               movss      dword ptr [esp], xmm0
00462e2f 51                       push       ecx
00462e30 f30f1045b0               movss      xmm0, dword ptr [ebp - 0x50]
00462e35 f30f110424               movss      dword ptr [esp], xmm0
00462e3a 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
00462e3d 52                       push       edx
00462e3e e8ed64fdff               call       0x439330
00462e43 83c40c                   add        esp, 0xc
00462e46 e993feffff               jmp        0x462cde
00462e4b 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00462e4e 05c8000000               add        eax, 0xc8
00462e53 8945a8                   mov        dword ptr [ebp - 0x58], eax
00462e56 68202020ff               push       0xff202020
00462e5b 6a00                     push       0
00462e5d 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00462e60 e81b17fbff               call       0x414580
00462e65 8bc8                     mov        ecx, eax
00462e67 e8e4a7ffff               call       0x45d650
00462e6c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00462e6f 81c1c8000000             add        ecx, 0xc8
00462e75 894da4                   mov        dword ptr [ebp - 0x5c], ecx
00462e78 6820202080               push       0x80202020
00462e7d 6a01                     push       1
00462e7f 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
00462e82 e8f916fbff               call       0x414580
00462e87 8bc8                     mov        ecx, eax
00462e89 e8c2a7ffff               call       0x45d650
00462e8e 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00462e91 81c2c8000000             add        edx, 0xc8
00462e97 8955a0                   mov        dword ptr [ebp - 0x60], edx
00462e9a 6820202080               push       0x80202020
00462e9f 6a02                     push       2
00462ea1 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00462ea4 e8d716fbff               call       0x414580
00462ea9 8bc8                     mov        ecx, eax
00462eab e8a0a7ffff               call       0x45d650
00462eb0 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00462eb3 05c8000000               add        eax, 0xc8
00462eb8 89459c                   mov        dword ptr [ebp - 0x64], eax
00462ebb 68808080ff               push       0xff808080
00462ec0 6a03                     push       3
00462ec2 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
00462ec5 e8b616fbff               call       0x414580
00462eca 8bc8                     mov        ecx, eax
00462ecc e87fa7ffff               call       0x45d650
00462ed1 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00462ed4 81c1c8000000             add        ecx, 0xc8
00462eda 894d98                   mov        dword ptr [ebp - 0x68], ecx
00462edd 6880808080               push       0x80808080
00462ee2 6a04                     push       4
00462ee4 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00462ee7 e89416fbff               call       0x414580
00462eec 8bc8                     mov        ecx, eax
00462eee e85da7ffff               call       0x45d650
00462ef3 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00462ef6 81c2c8000000             add        edx, 0xc8
00462efc 895594                   mov        dword ptr [ebp - 0x6c], edx
00462eff 6880808080               push       0x80808080
00462f04 6a05                     push       5
00462f06 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00462f09 e87216fbff               call       0x414580
00462f0e 8bc8                     mov        ecx, eax
00462f10 e83ba7ffff               call       0x45d650
00462f15 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00462f18 05c8000000               add        eax, 0xc8
00462f1d 894590                   mov        dword ptr [ebp - 0x70], eax
00462f20 68f0f0f0ff               push       0xfff0f0f0
00462f25 6a06                     push       6
00462f27 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00462f2a e85116fbff               call       0x414580
00462f2f 8bc8                     mov        ecx, eax
00462f31 e81aa7ffff               call       0x45d650
00462f36 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00462f39 81c1c8000000             add        ecx, 0xc8
00462f3f 894d8c                   mov        dword ptr [ebp - 0x74], ecx
00462f42 68f0f0f080               push       0x80f0f0f0
00462f47 6a07                     push       7
00462f49 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00462f4c e82f16fbff               call       0x414580
00462f51 8bc8                     mov        ecx, eax
00462f53 e8f8a6ffff               call       0x45d650
00462f58 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00462f5b 81c2c8000000             add        edx, 0xc8
00462f61 895588                   mov        dword ptr [ebp - 0x78], edx
00462f64 68f0f0f080               push       0x80f0f0f0
00462f69 6a08                     push       8
00462f6b 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00462f6e e80d16fbff               call       0x414580
00462f73 8bc8                     mov        ecx, eax
00462f75 e8d6a6ffff               call       0x45d650
00462f7a 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00462f7d 05c8000000               add        eax, 0xc8
00462f82 894584                   mov        dword ptr [ebp - 0x7c], eax
00462f85 68808080ff               push       0xff808080
00462f8a 6a09                     push       9
00462f8c 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
00462f8f e8ec15fbff               call       0x414580
00462f94 8bc8                     mov        ecx, eax
00462f96 e8b5a6ffff               call       0x45d650
00462f9b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00462f9e 81c1c8000000             add        ecx, 0xc8
00462fa4 894d80                   mov        dword ptr [ebp - 0x80], ecx
00462fa7 6880808080               push       0x80808080
00462fac 6a0a                     push       0xa
00462fae 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00462fb1 e8ca15fbff               call       0x414580
00462fb6 8bc8                     mov        ecx, eax
00462fb8 e893a6ffff               call       0x45d650
00462fbd 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00462fc0 81c2c8000000             add        edx, 0xc8
00462fc6 89957cffffff             mov        dword ptr [ebp - 0x84], edx
00462fcc 6880808080               push       0x80808080
00462fd1 6a0b                     push       0xb
00462fd3 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
00462fd9 e8a215fbff               call       0x414580
00462fde 8bc8                     mov        ecx, eax
00462fe0 e86ba6ffff               call       0x45d650
00462fe5 8b4308                   mov        eax, dword ptr [ebx + 8]
00462fe8 8b4820                   mov        ecx, dword ptr [eax + 0x20]
00462feb 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00462fee 898a48010000             mov        dword ptr [edx + 0x148], ecx
00462ff4 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00462ff7 8b4804                   mov        ecx, dword ptr [eax + 4]
00462ffa 898d74ffffff             mov        dword ptr [ebp - 0x8c], ecx
00463000 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00463003 81c148010000             add        ecx, 0x148
00463009 e8f292faff               call       0x40c300
0046300e 898578ffffff             mov        dword ptr [ebp - 0x88], eax
00463014 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
0046301a 52                       push       edx
0046301b 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
00463021 e8eac9feff               call       0x44fa10
00463026 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00463029 8b4804                   mov        ecx, dword ptr [eax + 4]
0046302c 898d70ffffff             mov        dword ptr [ebp - 0x90], ecx
00463032 8b5308                   mov        edx, dword ptr [ebx + 8]
00463035 52                       push       edx
00463036 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
0046303c e81fadffff               call       0x45dd60
00463041 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00463044 8b4804                   mov        ecx, dword ptr [eax + 4]
00463047 898d6cffffff             mov        dword ptr [ebp - 0x94], ecx
0046304d 6a1e                     push       0x1e
0046304f 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00463055 e8f6d2feff               call       0x450350
0046305a 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0046305d 8b4204                   mov        eax, dword ptr [edx + 4]
00463060 898568ffffff             mov        dword ptr [ebp - 0x98], eax
00463066 6a01                     push       1
00463068 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
0046306e e8dd140000               call       0x464550
00463073 90                       nop        
00463074 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
0046307b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0046307e 81c128010000             add        ecx, 0x128
00463084 894de0                   mov        dword ptr [ebp - 0x20], ecx
00463087 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0046308a e8118dfaff               call       0x40bda0
0046308f 8945ec                   mov        dword ptr [ebp - 0x14], eax
00463092 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00463095 e8d6e8fbff               call       0x421970
0046309a 898564ffffff             mov        dword ptr [ebp - 0x9c], eax
004630a0 eb09                     jmp        0x4630ab
004630a2 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004630a5 83c204                   add        edx, 4
004630a8 8955ec                   mov        dword ptr [ebp - 0x14], edx
004630ab 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004630ae 3b8564ffffff             cmp        eax, dword ptr [ebp - 0x9c]
004630b4 0f84a6000000             je         0x463160
004630ba 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004630bd 894ddc                   mov        dword ptr [ebp - 0x24], ecx
004630c0 e89b880b00               call       0x51b960
004630c5 8bc8                     mov        ecx, eax
004630c7 e834e6faff               call       0x411700
004630cc 89855cffffff             mov        dword ptr [ebp - 0xa4], eax
004630d2 51                       push       ecx
004630d3 0f57c0                   xorps      xmm0, xmm0
004630d6 f30f110424               movss      dword ptr [esp], xmm0
004630db 51                       push       ecx
004630dc 0f57c0                   xorps      xmm0, xmm0
004630df f30f110424               movss      dword ptr [esp], xmm0
004630e4 51                       push       ecx
004630e5 0f57c0                   xorps      xmm0, xmm0
004630e8 f30f110424               movss      dword ptr [esp], xmm0
004630ed 8d8d18ffffff             lea        ecx, [ebp - 0xe8]
004630f3 e8d8fcfbff               call       0x422dd0
004630f8 898560ffffff             mov        dword ptr [ebp - 0xa0], eax
004630fe 6a00                     push       0
00463100 8b9560ffffff             mov        edx, dword ptr [ebp - 0xa0]
00463106 52                       push       edx
00463107 6a01                     push       1
00463109 682cf05600               push       0x56f02c
0046310e 8d8538ffffff             lea        eax, [ebp - 0xc8]
00463114 50                       push       eax
00463115 8b8d5cffffff             mov        ecx, dword ptr [ebp - 0xa4]
0046311b e8e0140000               call       0x464600
00463120 8b08                     mov        ecx, dword ptr [eax]
00463122 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
00463125 890a                     mov        dword ptr [edx], ecx
00463127 6a00                     push       0
00463129 e8520f0000               call       0x464080
0046312e 83c404                   add        esp, 4
00463131 898558ffffff             mov        dword ptr [ebp - 0xa8], eax
00463137 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0046313a 50                       push       eax
0046313b 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
00463141 e88a100000               call       0x4641d0
00463146 83c007                   add        eax, 7
00463149 50                       push       eax
0046314a 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0046314d e87ebffeff               call       0x44f0d0
00463152 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00463155 83c101                   add        ecx, 1
00463158 894de4                   mov        dword ptr [ebp - 0x1c], ecx
0046315b e942ffffff               jmp        0x4630a2
00463160 e8fb870b00               call       0x51b960
00463165 8bc8                     mov        ecx, eax
00463167 e894e5faff               call       0x411700
0046316c 898550ffffff             mov        dword ptr [ebp - 0xb0], eax
00463172 51                       push       ecx
00463173 0f57c0                   xorps      xmm0, xmm0
00463176 f30f110424               movss      dword ptr [esp], xmm0
0046317b 51                       push       ecx
0046317c 0f57c0                   xorps      xmm0, xmm0
0046317f f30f110424               movss      dword ptr [esp], xmm0
00463184 51                       push       ecx
00463185 0f57c0                   xorps      xmm0, xmm0
00463188 f30f110424               movss      dword ptr [esp], xmm0
0046318d 8d8d0cffffff             lea        ecx, [ebp - 0xf4]
00463193 e838fcfbff               call       0x422dd0
00463198 898554ffffff             mov        dword ptr [ebp - 0xac], eax
0046319e 6a00                     push       0
004631a0 8b9554ffffff             mov        edx, dword ptr [ebp - 0xac]
004631a6 52                       push       edx
004631a7 6a0f                     push       0xf
004631a9 682cf05600               push       0x56f02c
004631ae 8d8534ffffff             lea        eax, [ebp - 0xcc]
004631b4 50                       push       eax
004631b5 8b8d50ffffff             mov        ecx, dword ptr [ebp - 0xb0]
004631bb e840140000               call       0x464600
004631c0 8b08                     mov        ecx, dword ptr [eax]
004631c2 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004631c5 898a38010000             mov        dword ptr [edx + 0x138], ecx
004631cb e890870b00               call       0x51b960
004631d0 8bc8                     mov        ecx, eax
004631d2 e829e5faff               call       0x411700
004631d7 898548ffffff             mov        dword ptr [ebp - 0xb8], eax
004631dd 51                       push       ecx
004631de 0f57c0                   xorps      xmm0, xmm0
004631e1 f30f110424               movss      dword ptr [esp], xmm0
004631e6 51                       push       ecx
004631e7 0f57c0                   xorps      xmm0, xmm0
004631ea f30f110424               movss      dword ptr [esp], xmm0
004631ef 51                       push       ecx
004631f0 0f57c0                   xorps      xmm0, xmm0
004631f3 f30f110424               movss      dword ptr [esp], xmm0
004631f8 8d8d00ffffff             lea        ecx, [ebp - 0x100]
004631fe e8cdfbfbff               call       0x422dd0
00463203 89854cffffff             mov        dword ptr [ebp - 0xb4], eax
00463209 6a00                     push       0
0046320b 8b854cffffff             mov        eax, dword ptr [ebp - 0xb4]
00463211 50                       push       eax
00463212 6a00                     push       0
00463214 682cf05600               push       0x56f02c
00463219 8d8d30ffffff             lea        ecx, [ebp - 0xd0]
0046321f 51                       push       ecx
00463220 8b8d48ffffff             mov        ecx, dword ptr [ebp - 0xb8]
00463226 e8d5130000               call       0x464600
0046322b 8b10                     mov        edx, dword ptr [eax]
0046322d 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00463230 89903c010000             mov        dword ptr [eax + 0x13c], edx
00463236 6a00                     push       0
00463238 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0046323b 81c14c010000             add        ecx, 0x14c
00463241 e8da02fcff               call       0x423520
00463246 6aff                     push       -1
00463248 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0046324b 81c15c010000             add        ecx, 0x15c
00463251 e8ca02fcff               call       0x423520
00463256 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00463259 c7816c01000000000000     mov        dword ptr [ecx + 0x16c], 0
00463263 33c0                     xor        eax, eax
00463265 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00463268 33cd                     xor        ecx, ebp
0046326a e87ff70d00               call       0x5429ee
0046326f 8be5                     mov        esp, ebp
00463271 5d                       pop        ebp
00463272 8be3                     mov        esp, ebx
00463274 5b                       pop        ebx
00463275 c20800                   ret        8
00463278 cc                       int3       
00463279 cc                       int3       
0046327a cc                       int3       
0046327b cc                       int3       
0046327c cc                       int3       
0046327d cc                       int3       
0046327e cc                       int3       
0046327f cc                       int3       
