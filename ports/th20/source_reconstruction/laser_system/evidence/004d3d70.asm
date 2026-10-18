004d3d70 55                       push       ebp
004d3d71 8bec                     mov        ebp, esp
004d3d73 83ec3c                   sub        esp, 0x3c
004d3d76 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004d3d7b 33c5                     xor        eax, ebp
004d3d7d 8945fc                   mov        dword ptr [ebp - 4], eax
004d3d80 894dec                   mov        dword ptr [ebp - 0x14], ecx
004d3d83 8b4508                   mov        eax, dword ptr [ebp + 8]
004d3d86 50                       push       eax
004d3d87 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d3d8a 81c1f8060000             add        ecx, 0x6f8
004d3d90 e8fb50ffff               call       0x4c8e90
004d3d95 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d3d98 c7412003000000           mov        dword ptr [ecx + 0x20], 3
004d3d9f 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d3da2 c7422401000000           mov        dword ptr [edx + 0x24], 1
004d3da9 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d3dac 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d3daf 8b914c070000             mov        edx, dword ptr [ecx + 0x74c]
004d3db5 8990e4060000             mov        dword ptr [eax + 0x6e4], edx
004d3dbb 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d3dbe 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d3dc1 8b9150070000             mov        edx, dword ptr [ecx + 0x750]
004d3dc7 8990e8060000             mov        dword ptr [eax + 0x6e8], edx
004d3dcd 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d3dd0 8b8868070000             mov        ecx, dword ptr [eax + 0x768]
004d3dd6 51                       push       ecx
004d3dd7 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d3dda e8f13f0000               call       0x4d7dd0
004d3ddf 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d3de2 c782d00600008080d0ff     mov        dword ptr [edx + 0x6d0], 0xffd08080
004d3dec 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d3def 8b88e4060000             mov        ecx, dword ptr [eax + 0x6e4]
004d3df5 51                       push       ecx
004d3df6 e8e518fbff               call       0x4856e0
004d3dfb 83c404                   add        esp, 4
004d3dfe 8b904c010000             mov        edx, dword ptr [eax + 0x14c]
004d3e04 8955e8                   mov        dword ptr [ebp - 0x18], edx
004d3e07 837de806                 cmp        dword ptr [ebp - 0x18], 6
004d3e0b 7402                     je         0x4d3e0f
004d3e0d eb3f                     jmp        0x4d3e4e
004d3e0f 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d3e12 8b88e4060000             mov        ecx, dword ptr [eax + 0x6e4]
004d3e18 51                       push       ecx
004d3e19 e8c218fbff               call       0x4856e0
004d3e1e 83c404                   add        esp, 4
004d3e21 83c004                   add        eax, 4
004d3e24 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004d3e27 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d3e2a 6b82e806000014           imul       eax, dword ptr [edx + 0x6e8], 0x14
004d3e31 8945e0                   mov        dword ptr [ebp - 0x20], eax
004d3e34 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004d3e37 034de0                   add        ecx, dword ptr [ebp - 0x20]
004d3e3a ba04000000               mov        edx, 4
004d3e3f c1e202                   shl        edx, 2
004d3e42 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d3e45 8b0c11                   mov        ecx, dword ptr [ecx + edx]
004d3e48 8988d0060000             mov        dword ptr [eax + 0x6d0], ecx
004d3e4e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d3e51 81c170070000             add        ecx, 0x770
004d3e57 e8745bf5ff               call       0x4299d0
004d3e5c 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d3e5f 81c270070000             add        edx, 0x770
004d3e65 8955dc                   mov        dword ptr [ebp - 0x24], edx
004d3e68 6850f54c00               push       0x4cf550
004d3e6d 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004d3e70 e8bb4bf6ff               call       0x438a30
004d3e75 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d3e78 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d3e7b 8988380d0000             mov        dword ptr [eax + 0xd38], ecx
004d3e81 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d3e84 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004d3e8a e8018cf4ff               call       0x41ca90
004d3e8f 8b4048                   mov        eax, dword ptr [eax + 0x48]
004d3e92 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004d3e95 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d3e98 8b91e4060000             mov        edx, dword ptr [ecx + 0x6e4]
004d3e9e 52                       push       edx
004d3e9f e81c18fbff               call       0x4856c0
004d3ea4 83c404                   add        esp, 4
004d3ea7 8945d8                   mov        dword ptr [ebp - 0x28], eax
004d3eaa 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004d3ead 50                       push       eax
004d3eae 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d3eb1 81c170070000             add        ecx, 0x770
004d3eb7 51                       push       ecx
004d3eb8 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004d3ebb e8e044f6ff               call       0x4383a0
004d3ec0 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d3ec3 81c270070000             add        edx, 0x770
004d3ec9 8955d0                   mov        dword ptr [ebp - 0x30], edx
004d3ecc 6a02                     push       2
004d3ece 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004d3ed1 e81a4af6ff               call       0x4388f0
004d3ed6 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d3ed9 81c170070000             add        ecx, 0x770
004d3edf e8ec76f5ff               call       0x42b5d0
004d3ee4 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d3ee7 81c170070000             add        ecx, 0x770
004d3eed e8bea6f8ff               call       0x45e5b0
004d3ef2 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d3ef5 c680080c000001           mov        byte ptr [eax + 0xc08], 1
004d3efc 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d3eff c781180c000000000000     mov        dword ptr [ecx + 0xc18], 0
004d3f09 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d3f0c c7821c0c000002000000     mov        dword ptr [edx + 0xc1c], 2
004d3f16 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d3f19 8a88130c0000             mov        cl, byte ptr [eax + 0xc13]
004d3f1f 80e1fc                   and        cl, 0xfc
004d3f22 80c901                   or         cl, 1
004d3f25 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d3f28 888a130c0000             mov        byte ptr [edx + 0xc13], cl
004d3f2e 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d3f31 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004d3f37 e8548bf4ff               call       0x41ca90
004d3f3c 8b4848                   mov        ecx, dword ptr [eax + 0x48]
004d3f3f 894dc8                   mov        dword ptr [ebp - 0x38], ecx
004d3f42 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d3f45 8b8250070000             mov        eax, dword ptr [edx + 0x750]
004d3f4b 83c03a                   add        eax, 0x3a
004d3f4e 8945cc                   mov        dword ptr [ebp - 0x34], eax
004d3f51 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004d3f54 51                       push       ecx
004d3f55 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d3f58 81c2540d0000             add        edx, 0xd54
004d3f5e 52                       push       edx
004d3f5f 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004d3f62 e81944f6ff               call       0x438380
004d3f67 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d3f6a 05540d0000               add        eax, 0xd54
004d3f6f 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004d3f72 6a02                     push       2
004d3f74 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004d3f77 e87449f6ff               call       0x4388f0
004d3f7c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d3f7f 81c1540d0000             add        ecx, 0xd54
004d3f85 e84676f5ff               call       0x42b5d0
004d3f8a 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d3f8d 81c1540d0000             add        ecx, 0xd54
004d3f93 e818a6f8ff               call       0x45e5b0
004d3f98 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d3f9b c681ec11000001           mov        byte ptr [ecx + 0x11ec], 1
004d3fa2 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d3fa5 8a82f7110000             mov        al, byte ptr [edx + 0x11f7]
004d3fab 24fc                     and        al, 0xfc
004d3fad 0c01                     or         al, 1
004d3faf 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d3fb2 8881f7110000             mov        byte ptr [ecx + 0x11f7], al
004d3fb8 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d3fbb 83ba3807000000           cmp        dword ptr [edx + 0x738], 0
004d3fc2 7c1e                     jl         0x4d3fe2
004d3fc4 51                       push       ecx
004d3fc5 0f57c0                   xorps      xmm0, xmm0
004d3fc8 f30f110424               movss      dword ptr [esp], xmm0
004d3fcd 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d3fd0 8b8838070000             mov        ecx, dword ptr [eax + 0x738]
004d3fd6 51                       push       ecx
004d3fd7 b930a85b00               mov        ecx, 0x5ba830
004d3fdc e8cf2ef5ff               call       0x426eb0
004d3fe1 90                       nop        
004d3fe2 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d3fe5 81c2f8060000             add        edx, 0x6f8
004d3feb 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d3fee 83c058                   add        eax, 0x58
004d3ff1 8b0a                     mov        ecx, dword ptr [edx]
004d3ff3 8908                     mov        dword ptr [eax], ecx
004d3ff5 8b4a04                   mov        ecx, dword ptr [edx + 4]
004d3ff8 894804                   mov        dword ptr [eax + 4], ecx
004d3ffb 8b5208                   mov        edx, dword ptr [edx + 8]
004d3ffe 895008                   mov        dword ptr [eax + 8], edx
004d4001 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d4004 f30f108044070000         movss      xmm0, dword ptr [eax + 0x744]
004d400c 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
004d4013 9f                       lahf       
004d4014 f6c444                   test       ah, 0x44
004d4017 7b60                     jnp        0x4d4079
004d4019 8d4df0                   lea        ecx, [ebp - 0x10]
004d401c e8efedf4ff               call       0x422e10
004d4021 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d4024 51                       push       ecx
004d4025 f30f108144070000         movss      xmm0, dword ptr [ecx + 0x744]
004d402d f30f110424               movss      dword ptr [esp], xmm0
004d4032 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d4035 51                       push       ecx
004d4036 f30f108210070000         movss      xmm0, dword ptr [edx + 0x710]
004d403e f30f110424               movss      dword ptr [esp], xmm0
004d4043 8d45f0                   lea        eax, [ebp - 0x10]
004d4046 50                       push       eax
004d4047 e8e452f6ff               call       0x439330
004d404c 83c40c                   add        esp, 0xc
004d404f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d4052 f30f104158               movss      xmm0, dword ptr [ecx + 0x58]
004d4057 f30f5845f0               addss      xmm0, dword ptr [ebp - 0x10]
004d405c 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d405f f30f114258               movss      dword ptr [edx + 0x58], xmm0
004d4064 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d4067 f30f10405c               movss      xmm0, dword ptr [eax + 0x5c]
004d406c f30f5845f4               addss      xmm0, dword ptr [ebp - 0xc]
004d4071 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d4074 f30f11415c               movss      dword ptr [ecx + 0x5c], xmm0
004d4079 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d407c 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d407f 8b881c070000             mov        ecx, dword ptr [eax + 0x71c]
004d4085 894a74                   mov        dword ptr [edx + 0x74], ecx
004d4088 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d408b f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
004d4093 f30f114278               movss      dword ptr [edx + 0x78], xmm0
004d4098 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d409b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d409e 8b9124070000             mov        edx, dword ptr [ecx + 0x724]
004d40a4 89507c                   mov        dword ptr [eax + 0x7c], edx
004d40a7 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d40aa 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d40ad 8b9110070000             mov        edx, dword ptr [ecx + 0x710]
004d40b3 895070                   mov        dword ptr [eax + 0x70], edx
004d40b6 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d40b9 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d40bc 8b9148070000             mov        edx, dword ptr [ecx + 0x748]
004d40c2 899090060000             mov        dword ptr [eax + 0x690], edx
004d40c8 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d40cb 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d40ce 8b9140070000             mov        edx, dword ptr [ecx + 0x740]
004d40d4 89908c000000             mov        dword ptr [eax + 0x8c], edx
004d40da 6a00                     push       0
004d40dc 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d40df 83c138                   add        ecx, 0x38
004d40e2 e839f4f4ff               call       0x423520
004d40e7 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d40ea c7806c07000000000000     mov        dword ptr [eax + 0x76c], 0
004d40f4 68e0155700               push       0x5715e0
004d40f9 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004d40ff e8cc3effff               call       0x4c7fd0
004d4104 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d4107 8981d8060000             mov        dword ptr [ecx + 0x6d8], eax
004d410d 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d4110 c782dc06000001000000     mov        dword ptr [edx + 0x6dc], 1
004d411a 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d411d 83b8e406000026           cmp        dword ptr [eax + 0x6e4], 0x26
004d4124 7512                     jne        0x4d4138
004d4126 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d4129 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
004d412c 81ca80000000             or         edx, 0x80
004d4132 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d4135 89501c                   mov        dword ptr [eax + 0x1c], edx
004d4138 33c0                     xor        eax, eax
004d413a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d413d 33cd                     xor        ecx, ebp
004d413f e8aae80600               call       0x5429ee
004d4144 8be5                     mov        esp, ebp
004d4146 5d                       pop        ebp
004d4147 c20400                   ret        4
004d414a cc                       int3       
004d414b cc                       int3       
004d414c cc                       int3       
004d414d cc                       int3       
004d414e cc                       int3       
004d414f cc                       int3       
