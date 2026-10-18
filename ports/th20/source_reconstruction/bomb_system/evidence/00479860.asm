00479860 55                       push       ebp
00479861 8bec                     mov        ebp, esp
00479863 81ec88000000             sub        esp, 0x88
00479869 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0047986e 33c5                     xor        eax, ebp
00479870 8945fc                   mov        dword ptr [ebp - 4], eax
00479873 894dec                   mov        dword ptr [ebp - 0x14], ecx
00479876 6a00                     push       0
00479878 e8b36ffeff               call       0x460830
0047987d 83c404                   add        esp, 4
00479880 8945c8                   mov        dword ptr [ebp - 0x38], eax
00479883 6a28                     push       0x28
00479885 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00479888 e8d3f7ffff               call       0x479060
0047988d 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00479890 05b8000000               add        eax, 0xb8
00479895 8945e8                   mov        dword ptr [ebp - 0x18], eax
00479898 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047989b 83c174                   add        ecx, 0x74
0047989e 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
004798a1 6a00                     push       0
004798a3 e8886ffeff               call       0x460830
004798a8 83c404                   add        esp, 4
004798ab 8bc8                     mov        ecx, eax
004798ad e89e6ffeff               call       0x460850
004798b2 50                       push       eax
004798b3 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004798b6 e825adfeff               call       0x4645e0
004798bb 90                       nop        
004798bc 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004798bf 83c208                   add        edx, 8
004798c2 8955c0                   mov        dword ptr [ebp - 0x40], edx
004798c5 6a78                     push       0x78
004798c7 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004798ca e8f19cfaff               call       0x4235c0
004798cf 0fb6c0                   movzx      eax, al
004798d2 85c0                     test       eax, eax
004798d4 746f                     je         0x479945
004798d6 c745d400000000           mov        dword ptr [ebp - 0x2c], 0
004798dd 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004798e0 894dcc                   mov        dword ptr [ebp - 0x34], ecx
004798e3 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
004798e6 8955e4                   mov        dword ptr [ebp - 0x1c], edx
004798e9 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
004798ec 0540140000               add        eax, 0x1440
004798f1 8945bc                   mov        dword ptr [ebp - 0x44], eax
004798f4 eb0c                     jmp        0x479902
004798f6 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004798f9 81c1d8000000             add        ecx, 0xd8
004798ff 894de4                   mov        dword ptr [ebp - 0x1c], ecx
00479902 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
00479905 3b55bc                   cmp        edx, dword ptr [ebp - 0x44]
00479908 7422                     je         0x47992c
0047990a 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0047990d 8945b8                   mov        dword ptr [ebp - 0x48], eax
00479910 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00479913 e8280a0000               call       0x47a340
00479918 0fb6c8                   movzx      ecx, al
0047991b 85c9                     test       ecx, ecx
0047991d 740b                     je         0x47992a
0047991f 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00479922 83c201                   add        edx, 1
00479925 8955d4                   mov        dword ptr [ebp - 0x2c], edx
00479928 eb02                     jmp        0x47992c
0047992a ebca                     jmp        0x4798f6
0047992c 837dd400                 cmp        dword ptr [ebp - 0x2c], 0
00479930 7513                     jne        0x479945
00479932 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00479935 83c174                   add        ecx, 0x74
00479938 e803f7ffff               call       0x479040
0047993d 83c8ff                   or         eax, 0xffffffff
00479940 e9bd040000               jmp        0x479e02
00479945 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00479948 83c008                   add        eax, 8
0047994b 8945b4                   mov        dword ptr [ebp - 0x4c], eax
0047994e 68f0000000               push       0xf0
00479953 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00479956 e885ecffff               call       0x4785e0
0047995b 0fb6c8                   movzx      ecx, al
0047995e 85c9                     test       ecx, ecx
00479960 7419                     je         0x47997b
00479962 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00479965 e826060000               call       0x479f90
0047996a 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047996d 83c174                   add        ecx, 0x74
00479970 e8cbf6ffff               call       0x479040
00479975 90                       nop        
00479976 e96c040000               jmp        0x479de7
0047997b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047997e 83c108                   add        ecx, 8
00479981 e8aa36feff               call       0x45d030
00479986 85c0                     test       eax, eax
00479988 0f848f020000             je         0x479c1d
0047998e 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00479991 83c208                   add        edx, 8
00479994 8955b0                   mov        dword ptr [ebp - 0x50], edx
00479997 6a00                     push       0
00479999 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0047999c e83fecffff               call       0x4785e0
004799a1 0fb6c0                   movzx      eax, al
004799a4 85c0                     test       eax, eax
004799a6 0f8414010000             je         0x479ac0
004799ac 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004799af 894df0                   mov        dword ptr [ebp - 0x10], ecx
004799b2 51                       push       ecx
004799b3 0f57c0                   xorps      xmm0, xmm0
004799b6 f30f110424               movss      dword ptr [esp], xmm0
004799bb 8d4df8                   lea        ecx, [ebp - 8]
004799be e84df8faff               call       0x429210
004799c3 90                       nop        
004799c4 c745e000000000           mov        dword ptr [ebp - 0x20], 0
004799cb eb14                     jmp        0x4799e1
004799cd 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004799d0 83c201                   add        edx, 1
004799d3 8955e0                   mov        dword ptr [ebp - 0x20], edx
004799d6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004799d9 05d8000000               add        eax, 0xd8
004799de 8945f0                   mov        dword ptr [ebp - 0x10], eax
004799e1 837de008                 cmp        dword ptr [ebp - 0x20], 8
004799e5 0f8dd0000000             jge        0x479abb
004799eb 6a0f                     push       0xf
004799ed 6a00                     push       0
004799ef e83c6efeff               call       0x460830
004799f4 83c404                   add        esp, 4
004799f7 8bc8                     mov        ecx, eax
004799f9 e8526efeff               call       0x460850
004799fe 50                       push       eax
004799ff 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00479a02 51                       push       ecx
00479a03 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479a06 e885060000               call       0x47a090
00479a0b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479a0e 83c104                   add        ecx, 4
00479a11 e8aa0a0000               call       0x47a4c0
00479a16 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00479a19 83c204                   add        edx, 4
00479a1c 8955ac                   mov        dword ptr [ebp - 0x54], edx
00479a1f 51                       push       ecx
00479a20 0f57c0                   xorps      xmm0, xmm0
00479a23 f30f110424               movss      dword ptr [esp], xmm0
00479a28 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00479a2b e8100b0000               call       0x47a540
00479a30 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00479a33 83c004                   add        eax, 4
00479a36 8945a8                   mov        dword ptr [ebp - 0x58], eax
00479a39 8d4df8                   lea        ecx, [ebp - 8]
00479a3c e89ff8faff               call       0x4292e0
00479a41 51                       push       ecx
00479a42 d91c24                   fstp       dword ptr [esp]
00479a45 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00479a48 e8130b0000               call       0x47a560
00479a4d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479a50 83c104                   add        ecx, 4
00479a53 894da4                   mov        dword ptr [ebp - 0x5c], ecx
00479a56 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00479a5e f30f5e0528fa5600         divss      xmm0, dword ptr [0x56fa28]
00479a66 51                       push       ecx
00479a67 f30f110424               movss      dword ptr [esp], xmm0
00479a6c 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
00479a6f e8ac0a0000               call       0x47a520
00479a74 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00479a7c f30f5e0580f25600         divss      xmm0, dword ptr [0x56f280]
00479a84 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00479a87 f30f1182d4000000         movss      dword ptr [edx + 0xd4], xmm0
00479a8f f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00479a97 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
00479a9f f30f5e05bcd75600         divss      xmm0, dword ptr [0x56d7bc]
00479aa7 51                       push       ecx
00479aa8 f30f110424               movss      dword ptr [esp], xmm0
00479aad 8d4df8                   lea        ecx, [ebp - 8]
00479ab0 e80b96fdff               call       0x4530c0
00479ab5 90                       nop        
00479ab6 e912ffffff               jmp        0x4799cd
00479abb e95d010000               jmp        0x479c1d
00479ac0 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00479ac3 83c008                   add        eax, 8
00479ac6 8945a0                   mov        dword ptr [ebp - 0x60], eax
00479ac9 6a28                     push       0x28
00479acb 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00479ace e80debffff               call       0x4785e0
00479ad3 0fb6c8                   movzx      ecx, al
00479ad6 85c9                     test       ecx, ecx
00479ad8 0f843f010000             je         0x479c1d
00479ade 51                       push       ecx
00479adf 0f57c0                   xorps      xmm0, xmm0
00479ae2 f30f110424               movss      dword ptr [esp], xmm0
00479ae7 8d4df4                   lea        ecx, [ebp - 0xc]
00479aea e821f7faff               call       0x429210
00479aef bad8000000               mov        edx, 0xd8
00479af4 c1e203                   shl        edx, 3
00479af7 0355e8                   add        edx, dword ptr [ebp - 0x18]
00479afa 8955f0                   mov        dword ptr [ebp - 0x10], edx
00479afd 6a00                     push       0
00479aff e82c6dfeff               call       0x460830
00479b04 83c404                   add        esp, 4
00479b07 8bc8                     mov        ecx, eax
00479b09 e8426dfeff               call       0x460850
00479b0e 51                       push       ecx
00479b0f f30f1000                 movss      xmm0, dword ptr [eax]
00479b13 f30f110424               movss      dword ptr [esp], xmm0
00479b18 6a2c                     push       0x2c
00479b1a b930a85b00               mov        ecx, 0x5ba830
00479b1f e88cd3faff               call       0x426eb0
00479b24 90                       nop        
00479b25 c745dc00000000           mov        dword ptr [ebp - 0x24], 0
00479b2c eb15                     jmp        0x479b43
00479b2e 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
00479b31 83c001                   add        eax, 1
00479b34 8945dc                   mov        dword ptr [ebp - 0x24], eax
00479b37 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479b3a 81c1d8000000             add        ecx, 0xd8
00479b40 894df0                   mov        dword ptr [ebp - 0x10], ecx
00479b43 837ddc08                 cmp        dword ptr [ebp - 0x24], 8
00479b47 0f8dd0000000             jge        0x479c1d
00479b4d 6a07                     push       7
00479b4f 6a00                     push       0
00479b51 e8da6cfeff               call       0x460830
00479b56 83c404                   add        esp, 4
00479b59 8bc8                     mov        ecx, eax
00479b5b e8f06cfeff               call       0x460850
00479b60 50                       push       eax
00479b61 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
00479b64 52                       push       edx
00479b65 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479b68 e823050000               call       0x47a090
00479b6d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479b70 83c104                   add        ecx, 4
00479b73 e848090000               call       0x47a4c0
00479b78 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00479b7b 83c004                   add        eax, 4
00479b7e 89459c                   mov        dword ptr [ebp - 0x64], eax
00479b81 51                       push       ecx
00479b82 0f57c0                   xorps      xmm0, xmm0
00479b85 f30f110424               movss      dword ptr [esp], xmm0
00479b8a 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
00479b8d e8ae090000               call       0x47a540
00479b92 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479b95 83c104                   add        ecx, 4
00479b98 894d98                   mov        dword ptr [ebp - 0x68], ecx
00479b9b 8d4df4                   lea        ecx, [ebp - 0xc]
00479b9e e83df7faff               call       0x4292e0
00479ba3 51                       push       ecx
00479ba4 d91c24                   fstp       dword ptr [esp]
00479ba7 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
00479baa e8b1090000               call       0x47a560
00479baf 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00479bb2 83c204                   add        edx, 4
00479bb5 895594                   mov        dword ptr [ebp - 0x6c], edx
00479bb8 f30f1005f8e05600         movss      xmm0, dword ptr [0x56e0f8]
00479bc0 f30f5e0528fa5600         divss      xmm0, dword ptr [0x56fa28]
00479bc8 51                       push       ecx
00479bc9 f30f110424               movss      dword ptr [esp], xmm0
00479bce 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00479bd1 e84a090000               call       0x47a520
00479bd6 f30f1005f8e05600         movss      xmm0, dword ptr [0x56e0f8]
00479bde f30f5e0580f25600         divss      xmm0, dword ptr [0x56f280]
00479be6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00479be9 f30f1180d4000000         movss      dword ptr [eax + 0xd4], xmm0
00479bf1 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00479bf9 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
00479c01 f30f5e05bcd75600         divss      xmm0, dword ptr [0x56d7bc]
00479c09 51                       push       ecx
00479c0a f30f110424               movss      dword ptr [esp], xmm0
00479c0f 8d4df4                   lea        ecx, [ebp - 0xc]
00479c12 e8a994fdff               call       0x4530c0
00479c17 90                       nop        
00479c18 e911ffffff               jmp        0x479b2e
00479c1d 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00479c20 894df0                   mov        dword ptr [ebp - 0x10], ecx
00479c23 c745d000000000           mov        dword ptr [ebp - 0x30], 0
00479c2a eb14                     jmp        0x479c40
00479c2c 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00479c2f 83c201                   add        edx, 1
00479c32 8955d0                   mov        dword ptr [ebp - 0x30], edx
00479c35 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00479c38 05d8000000               add        eax, 0xd8
00479c3d 8945f0                   mov        dword ptr [ebp - 0x10], eax
00479c40 837dd018                 cmp        dword ptr [ebp - 0x30], 0x18
00479c44 0f8dab000000             jge        0x479cf5
00479c4a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479c4d 83b9a000000000           cmp        dword ptr [ecx + 0xa0], 0
00479c54 7502                     jne        0x479c58
00479c56 ebd4                     jmp        0x479c2c
00479c58 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479c5b e800f7ffff               call       0x479360
00479c60 90                       nop        
00479c61 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479c64 81c1cc000000             add        ecx, 0xcc
00479c6a e8a1710400               call       0x4c0e10
00479c6f 85c0                     test       eax, eax
00479c71 747d                     je         0x479cf0
00479c73 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479c76 81c1cc000000             add        ecx, 0xcc
00479c7c e88f710400               call       0x4c0e10
00479c81 8bc8                     mov        ecx, eax
00479c83 e86820feff               call       0x45bcf0
00479c88 3d2c010000               cmp        eax, 0x12c
00479c8d 7c40                     jl         0x479ccf
00479c8f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479c92 e879010000               call       0x479e10
00479c97 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479c9a 83c104                   add        ecx, 4
00479c9d e8fe20f9ff               call       0x40bda0
00479ca2 51                       push       ecx
00479ca3 f30f1000                 movss      xmm0, dword ptr [eax]
00479ca7 f30f110424               movss      dword ptr [esp], xmm0
00479cac 6a1b                     push       0x1b
00479cae b930a85b00               mov        ecx, 0x5ba830
00479cb3 e8f8d1faff               call       0x426eb0
00479cb8 6a00                     push       0
00479cba 6a06                     push       6
00479cbc 6a06                     push       6
00479cbe 6a04                     push       4
00479cc0 6a01                     push       1
00479cc2 e8c9b3faff               call       0x425090
00479cc7 83c414                   add        esp, 0x14
00479cca e95dffffff               jmp        0x479c2c
00479ccf 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00479cd2 81c2cc000000             add        edx, 0xcc
00479cd8 895590                   mov        dword ptr [ebp - 0x70], edx
00479cdb 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479cde 83c104                   add        ecx, 4
00479ce1 e8ba20f9ff               call       0x40bda0
00479ce6 50                       push       eax
00479ce7 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00479cea e801830400               call       0x4c1ff0
00479cef 90                       nop        
00479cf0 e937ffffff               jmp        0x479c2c
00479cf5 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00479cf8 05b8000000               add        eax, 0xb8
00479cfd 8945e8                   mov        dword ptr [ebp - 0x18], eax
00479d00 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00479d03 894df0                   mov        dword ptr [ebp - 0x10], ecx
00479d06 c745d800000000           mov        dword ptr [ebp - 0x28], 0
00479d0d eb14                     jmp        0x479d23
00479d0f 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00479d12 83c201                   add        edx, 1
00479d15 8955d8                   mov        dword ptr [ebp - 0x28], edx
00479d18 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00479d1b 05d8000000               add        eax, 0xd8
00479d20 8945f0                   mov        dword ptr [ebp - 0x10], eax
00479d23 837dd818                 cmp        dword ptr [ebp - 0x28], 0x18
00479d27 0f8dba000000             jge        0x479de7
00479d2d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479d30 83b9a000000000           cmp        dword ptr [ecx + 0xa0], 0
00479d37 7502                     jne        0x479d3b
00479d39 ebd4                     jmp        0x479d0f
00479d3b 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00479d3e 83c208                   add        edx, 8
00479d41 89558c                   mov        dword ptr [ebp - 0x74], edx
00479d44 6a08                     push       8
00479d46 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00479d49 e8f282ffff               call       0x472040
00479d4e 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00479d51 81e107000080             and        ecx, 0x80000007
00479d57 7905                     jns        0x479d5e
00479d59 49                       dec        ecx
00479d5a 83c9f8                   or         ecx, 0xfffffff8
00479d5d 41                       inc        ecx
00479d5e 3bc1                     cmp        eax, ecx
00479d60 7402                     je         0x479d64
00479d62 ebab                     jmp        0x479d0f
00479d64 6a00                     push       0
00479d66 e815f1ffff               call       0x478e80
00479d6b 83c404                   add        esp, 4
00479d6e 894584                   mov        dword ptr [ebp - 0x7c], eax
00479d71 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479d74 83c104                   add        ecx, 4
00479d77 e82420f9ff               call       0x40bda0
00479d7c 894588                   mov        dword ptr [ebp - 0x78], eax
00479d7f 6a00                     push       0
00479d81 689f860100               push       0x1869f
00479d86 6a00                     push       0
00479d88 51                       push       ecx
00479d89 f30f100528fa5600         movss      xmm0, dword ptr [0x56fa28]
00479d91 f30f110424               movss      dword ptr [esp], xmm0
00479d96 8b5588                   mov        edx, dword ptr [ebp - 0x78]
00479d99 52                       push       edx
00479d9a 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
00479d9d e8be310000               call       0x47cf60
00479da2 6a00                     push       0
00479da4 e837f1ffff               call       0x478ee0
00479da9 83c404                   add        esp, 4
00479dac 89857cffffff             mov        dword ptr [ebp - 0x84], eax
00479db2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00479db5 83c104                   add        ecx, 4
00479db8 e8e31ff9ff               call       0x40bda0
00479dbd 894580                   mov        dword ptr [ebp - 0x80], eax
00479dc0 6a01                     push       1
00479dc2 6a00                     push       0
00479dc4 51                       push       ecx
00479dc5 f30f100528fa5600         movss      xmm0, dword ptr [0x56fa28]
00479dcd f30f110424               movss      dword ptr [esp], xmm0
00479dd2 8b4580                   mov        eax, dword ptr [ebp - 0x80]
00479dd5 50                       push       eax
00479dd6 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
00479ddc e8ef0c0500               call       0x4caad0
00479de1 90                       nop        
00479de2 e928ffffff               jmp        0x479d0f
00479de7 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00479dea 83c108                   add        ecx, 8
00479ded 898d78ffffff             mov        dword ptr [ebp - 0x88], ecx
00479df3 6a00                     push       0
00479df5 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00479dfb e84097faff               call       0x423540
00479e00 33c0                     xor        eax, eax
00479e02 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00479e05 33cd                     xor        ecx, ebp
00479e07 e8e28b0c00               call       0x5429ee
00479e0c 8be5                     mov        esp, ebp
00479e0e 5d                       pop        ebp
00479e0f c3                       ret        
