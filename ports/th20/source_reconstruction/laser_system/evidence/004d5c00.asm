004d5c00 55                       push       ebp
004d5c01 8bec                     mov        ebp, esp
004d5c03 83ec3c                   sub        esp, 0x3c
004d5c06 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004d5c0b 33c5                     xor        eax, ebp
004d5c0d 8945fc                   mov        dword ptr [ebp - 4], eax
004d5c10 894ddc                   mov        dword ptr [ebp - 0x24], ecx
004d5c13 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004d5c16 8b88d8060000             mov        ecx, dword ptr [eax + 0x6d8]
004d5c1c 894de0                   mov        dword ptr [ebp - 0x20], ecx
004d5c1f c745d400000000           mov        dword ptr [ebp - 0x2c], 0
004d5c26 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
004d5c29 8b421c                   mov        eax, dword ptr [edx + 0x1c]
004d5c2c 83c840                   or         eax, 0x40
004d5c2f 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004d5c32 89411c                   mov        dword ptr [ecx + 0x1c], eax
004d5c35 0f57c0                   xorps      xmm0, xmm0
004d5c38 f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
004d5c3d 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
004d5c40 8b8228130000             mov        eax, dword ptr [edx + 0x1328]
004d5c46 8945d8                   mov        dword ptr [ebp - 0x28], eax
004d5c49 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004d5c4c f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004d5c54 f30f118184000000         movss      dword ptr [ecx + 0x84], xmm0
004d5c5c c745d000000000           mov        dword ptr [ebp - 0x30], 0
004d5c63 eb09                     jmp        0x4d5c6e
004d5c65 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
004d5c68 83c201                   add        edx, 1
004d5c6b 8955d0                   mov        dword ptr [ebp - 0x30], edx
004d5c6e 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004d5c71 8b8818070000             mov        ecx, dword ptr [eax + 0x718]
004d5c77 83e901                   sub        ecx, 1
004d5c7a 394dd0                   cmp        dword ptr [ebp - 0x30], ecx
004d5c7d 0f8d7d010000             jge        0x4d5e00
004d5c83 8d4df0                   lea        ecx, [ebp - 0x10]
004d5c86 e885d1f4ff               call       0x422e10
004d5c8b 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004d5c8e f30f10421c               movss      xmm0, dword ptr [edx + 0x1c]
004d5c93 f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
004d5c9b 51                       push       ecx
004d5c9c f30f110424               movss      dword ptr [esp], xmm0
004d5ca1 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004d5ca4 51                       push       ecx
004d5ca5 f30f104018               movss      xmm0, dword ptr [eax + 0x18]
004d5caa f30f110424               movss      dword ptr [esp], xmm0
004d5caf 8d4df0                   lea        ecx, [ebp - 0x10]
004d5cb2 51                       push       ecx
004d5cb3 e87836f6ff               call       0x439330
004d5cb8 83c40c                   add        esp, 0xc
004d5cbb 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004d5cbe 52                       push       edx
004d5cbf 8d4df0                   lea        ecx, [ebp - 0x10]
004d5cc2 e8193af5ff               call       0x4296e0
004d5cc7 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004d5cca f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
004d5ccf f30f58401c               addss      xmm0, dword ptr [eax + 0x1c]
004d5cd4 f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
004d5cd9 f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
004d5cde 0f2f0590cd5600           comiss     xmm0, dword ptr [0x56cd90]
004d5ce5 0f8207010000             jb         0x4d5df2
004d5ceb 8d4de4                   lea        ecx, [ebp - 0x1c]
004d5cee e81dd1f4ff               call       0x422e10
004d5cf3 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004d5cf6 f30f104178               movss      xmm0, dword ptr [ecx + 0x78]
004d5cfb f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
004d5d03 f30f1145c4               movss      dword ptr [ebp - 0x3c], xmm0
004d5d08 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004d5d0b f30f10421c               movss      xmm0, dword ptr [edx + 0x1c]
004d5d10 f30f1145c8               movss      dword ptr [ebp - 0x38], xmm0
004d5d15 f30f1045c8               movss      xmm0, dword ptr [ebp - 0x38]
004d5d1a f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004d5d22 51                       push       ecx
004d5d23 f30f110424               movss      dword ptr [esp], xmm0
004d5d28 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004d5d2b 51                       push       ecx
004d5d2c f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004d5d31 f30f110424               movss      dword ptr [esp], xmm0
004d5d36 8d4de4                   lea        ecx, [ebp - 0x1c]
004d5d39 51                       push       ecx
004d5d3a e8f135f6ff               call       0x439330
004d5d3f 83c40c                   add        esp, 0xc
004d5d42 8d55e4                   lea        edx, [ebp - 0x1c]
004d5d45 52                       push       edx
004d5d46 8d4df0                   lea        ecx, [ebp - 0x10]
004d5d49 e89239f5ff               call       0x4296e0
004d5d4e 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004d5d51 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004d5d54 8908                     mov        dword ptr [eax], ecx
004d5d56 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004d5d59 895004                   mov        dword ptr [eax + 4], edx
004d5d5c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004d5d5f 894808                   mov        dword ptr [eax + 8], ecx
004d5d62 f30f1045c8               movss      xmm0, dword ptr [ebp - 0x38]
004d5d67 f30f590598f75600         mulss      xmm0, dword ptr [0x56f798]
004d5d6f 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004d5d72 f30f114218               movss      dword ptr [edx + 0x18], xmm0
004d5d77 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004d5d7a f30f1045c4               movss      xmm0, dword ptr [ebp - 0x3c]
004d5d7f f30f59801c130000         mulss      xmm0, dword ptr [eax + 0x131c]
004d5d87 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004d5d8a f30f11411c               movss      dword ptr [ecx + 0x1c], xmm0
004d5d8f 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004d5d92 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004d5d95 8b4818                   mov        ecx, dword ptr [eax + 0x18]
004d5d98 894a24                   mov        dword ptr [edx + 0x24], ecx
004d5d9b 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004d5d9e 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004d5da1 8b4818                   mov        ecx, dword ptr [eax + 0x18]
004d5da4 894a28                   mov        dword ptr [edx + 0x28], ecx
004d5da7 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004d5daa 8b4238                   mov        eax, dword ptr [edx + 0x38]
004d5dad 83e0fe                   and        eax, 0xfffffffe
004d5db0 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004d5db3 894138                   mov        dword ptr [ecx + 0x38], eax
004d5db6 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004d5db9 8b4238                   mov        eax, dword ptr [edx + 0x38]
004d5dbc 83e0fd                   and        eax, 0xfffffffd
004d5dbf 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004d5dc2 894138                   mov        dword ptr [ecx + 0x38], eax
004d5dc5 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004d5dc8 8b4238                   mov        eax, dword ptr [edx + 0x38]
004d5dcb 83c804                   or         eax, 4
004d5dce 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004d5dd1 894138                   mov        dword ptr [ecx + 0x38], eax
004d5dd4 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004d5dd7 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004d5dda 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004d5ddd 894a2c                   mov        dword ptr [edx + 0x2c], ecx
004d5de0 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
004d5de3 83c201                   add        edx, 1
004d5de6 8955d4                   mov        dword ptr [ebp - 0x2c], edx
004d5de9 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004d5dec 83c03c                   add        eax, 0x3c
004d5def 8945e0                   mov        dword ptr [ebp - 0x20], eax
004d5df2 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004d5df5 83c120                   add        ecx, 0x20
004d5df8 894dd8                   mov        dword ptr [ebp - 0x28], ecx
004d5dfb e965feffff               jmp        0x4d5c65
004d5e00 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
004d5e03 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
004d5e06 8982e0060000             mov        dword ptr [edx + 0x6e0], eax
004d5e0c 837dd400                 cmp        dword ptr [ebp - 0x2c], 0
004d5e10 7421                     je         0x4d5e33
004d5e12 837d0800                 cmp        dword ptr [ebp + 8], 0
004d5e16 741b                     je         0x4d5e33
004d5e18 6b4dd43c                 imul       ecx, dword ptr [ebp - 0x2c], 0x3c
004d5e1c 51                       push       ecx
004d5e1d 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
004d5e20 8b82d8060000             mov        eax, dword ptr [edx + 0x6d8]
004d5e26 50                       push       eax
004d5e27 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004d5e2a 51                       push       ecx
004d5e2b e8f0df0600               call       0x543e20
004d5e30 83c40c                   add        esp, 0xc
004d5e33 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
004d5e36 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d5e39 33cd                     xor        ecx, ebp
004d5e3b e8aecb0600               call       0x5429ee
004d5e40 8be5                     mov        esp, ebp
004d5e42 5d                       pop        ebp
004d5e43 c20400                   ret        4
004d5e46 cc                       int3       
004d5e47 cc                       int3       
004d5e48 cc                       int3       
004d5e49 cc                       int3       
004d5e4a cc                       int3       
004d5e4b cc                       int3       
004d5e4c cc                       int3       
004d5e4d cc                       int3       
004d5e4e cc                       int3       
004d5e4f cc                       int3       
