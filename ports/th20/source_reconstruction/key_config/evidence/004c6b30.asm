004c6b30 55                       push       ebp
004c6b31 8bec                     mov        ebp, esp
004c6b33 83ec78                   sub        esp, 0x78
004c6b36 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c6b3b 33c5                     xor        eax, ebp
004c6b3d 8945fc                   mov        dword ptr [ebp - 4], eax
004c6b40 56                       push       esi
004c6b41 894de0                   mov        dword ptr [ebp - 0x20], ecx
004c6b44 6a07                     push       7
004c6b46 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6b4c e8af1ffcff               call       0x488b00
004c6b51 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c6b54 05e0000000               add        eax, 0xe0
004c6b59 8b08                     mov        ecx, dword ptr [eax]
004c6b5b 894df0                   mov        dword ptr [ebp - 0x10], ecx
004c6b5e 8b5004                   mov        edx, dword ptr [eax + 4]
004c6b61 8955f4                   mov        dword ptr [ebp - 0xc], edx
004c6b64 8b4008                   mov        eax, dword ptr [eax + 8]
004c6b67 8945f8                   mov        dword ptr [ebp - 8], eax
004c6b6a 8d4de4                   lea        ecx, [ebp - 0x1c]
004c6b6d e89ec2f5ff               call       0x422e10
004c6b72 6a01                     push       1
004c6b74 6a00                     push       0
004c6b76 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6b7c e89f1affff               call       0x4b8620
004c6b81 6aff                     push       -1
004c6b83 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6b89 e8429ffaff               call       0x470ad0
004c6b8e 68000000ff               push       0xff000000
004c6b93 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6b99 e8129ffaff               call       0x470ab0
004c6b9e f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004c6ba3 f30f580558105700         addss      xmm0, dword ptr [0x571058]
004c6bab f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004c6bb0 6804105700               push       0x571004
004c6bb5 8d4df0                   lea        ecx, [ebp - 0x10]
004c6bb8 51                       push       ecx
004c6bb9 8b1598065c00             mov        edx, dword ptr [0x5c0698]
004c6bbf 52                       push       edx
004c6bc0 e8cb5dfaff               call       0x46c990
004c6bc5 83c40c                   add        esp, 0xc
004c6bc8 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
004c6bcd f30f580548fd5600         addss      xmm0, dword ptr [0x56fd48]
004c6bd5 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004c6bda 68808080ff               push       0xff808080
004c6bdf 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6be5 e8e69efaff               call       0x470ad0
004c6bea 68000000ff               push       0xff000000
004c6bef 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6bf5 e8b69efaff               call       0x470ab0
004c6bfa 90                       nop        
004c6bfb 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c6bfe 8b880c010000             mov        ecx, dword ptr [eax + 0x10c]
004c6c04 51                       push       ecx
004c6c05 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c6c0b e810f2ffff               call       0x4c5e20
004c6c10 8bc8                     mov        ecx, eax
004c6c12 e839f2ffff               call       0x4c5e50
004c6c17 85c0                     test       eax, eax
004c6c19 7423                     je         0x4c6c3e
004c6c1b 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c6c1e 83c260                   add        edx, 0x60
004c6c21 8955d4                   mov        dword ptr [ebp - 0x2c], edx
004c6c24 6810105700               push       0x571010
004c6c29 8d45f0                   lea        eax, [ebp - 0x10]
004c6c2c 50                       push       eax
004c6c2d 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6c33 51                       push       ecx
004c6c34 e8575dfaff               call       0x46c990
004c6c39 83c40c                   add        esp, 0xc
004c6c3c eb68                     jmp        0x4c6ca6
004c6c3e 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c6c41 8b820c010000             mov        eax, dword ptr [edx + 0x10c]
004c6c47 50                       push       eax
004c6c48 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c6c4e e8cdf1ffff               call       0x4c5e20
004c6c53 8bc8                     mov        ecx, eax
004c6c55 e856f2ffff               call       0x4c5eb0
004c6c5a 85c0                     test       eax, eax
004c6c5c 7425                     je         0x4c6c83
004c6c5e 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c6c61 81c180000000             add        ecx, 0x80
004c6c67 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
004c6c6a 681c105700               push       0x57101c
004c6c6f 8d55f0                   lea        edx, [ebp - 0x10]
004c6c72 52                       push       edx
004c6c73 a198065c00               mov        eax, dword ptr [0x5c0698]
004c6c78 50                       push       eax
004c6c79 e8125dfaff               call       0x46c990
004c6c7e 83c40c                   add        esp, 0xc
004c6c81 eb23                     jmp        0x4c6ca6
004c6c83 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c6c86 81c1a0000000             add        ecx, 0xa0
004c6c8c 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
004c6c8f 6824105700               push       0x571024
004c6c94 8d55f0                   lea        edx, [ebp - 0x10]
004c6c97 52                       push       edx
004c6c98 a198065c00               mov        eax, dword ptr [0x5c0698]
004c6c9d 50                       push       eax
004c6c9e e8ed5cfaff               call       0x46c990
004c6ca3 83c40c                   add        esp, 0xc
004c6ca6 6aff                     push       -1
004c6ca8 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6cae e81d9efaff               call       0x470ad0
004c6cb3 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004c6cb8 f30f5c0554105700         subss      xmm0, dword ptr [0x571054]
004c6cc0 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004c6cc5 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
004c6cca f30f580548fd5600         addss      xmm0, dword ptr [0x56fd48]
004c6cd2 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004c6cd7 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
004c6cdc f30f580550105700         addss      xmm0, dword ptr [0x571050]
004c6ce4 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004c6ce9 6a01                     push       1
004c6ceb 6a01                     push       1
004c6ced 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6cf3 e82819ffff               call       0x4b8620
004c6cf8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c6cfb 8b910c010000             mov        edx, dword ptr [ecx + 0x10c]
004c6d01 52                       push       edx
004c6d02 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c6d08 e813f1ffff               call       0x4c5e20
004c6d0d 8bc8                     mov        ecx, eax
004c6d0f e86cf1ffff               call       0x4c5e80
004c6d14 85c0                     test       eax, eax
004c6d16 7409                     je         0x4c6d21
004c6d18 c745d009000000           mov        dword ptr [ebp - 0x30], 9
004c6d1f eb07                     jmp        0x4c6d28
004c6d21 c745d006000000           mov        dword ptr [ebp - 0x30], 6
004c6d28 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004c6d2b 8945d8                   mov        dword ptr [ebp - 0x28], eax
004c6d2e c745dc00000000           mov        dword ptr [ebp - 0x24], 0
004c6d35 eb09                     jmp        0x4c6d40
004c6d37 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004c6d3a 83c101                   add        ecx, 1
004c6d3d 894ddc                   mov        dword ptr [ebp - 0x24], ecx
004c6d40 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
004c6d43 3b55d8                   cmp        edx, dword ptr [ebp - 0x28]
004c6d46 0f8dbd010000             jge        0x4c6f09
004c6d4c 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c6d4f 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004c6d52 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c6d55 894de8                   mov        dword ptr [ebp - 0x18], ecx
004c6d58 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c6d5b 8955ec                   mov        dword ptr [ebp - 0x14], edx
004c6d5e 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c6d61 81c1ec000000             add        ecx, 0xec
004c6d67 e82492f4ff               call       0x40ff90
004c6d6c 50                       push       eax
004c6d6d 6a00                     push       0
004c6d6f 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c6d72 83c014                   add        eax, 0x14
004c6d75 50                       push       eax
004c6d76 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004c6d79 51                       push       ecx
004c6d7a 8d55f0                   lea        edx, [ebp - 0x10]
004c6d7d 52                       push       edx
004c6d7e e80d48faff               call       0x46b590
004c6d83 83c414                   add        esp, 0x14
004c6d86 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c6d89 8b880c010000             mov        ecx, dword ptr [eax + 0x10c]
004c6d8f 51                       push       ecx
004c6d90 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c6d96 e885f0ffff               call       0x4c5e20
004c6d9b 8bc8                     mov        ecx, eax
004c6d9d e8def0ffff               call       0x4c5e80
004c6da2 85c0                     test       eax, eax
004c6da4 740f                     je         0x4c6db5
004c6da6 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
004c6da9 8b049528fe5a00           mov        eax, dword ptr [edx*4 + 0x5afe28]
004c6db0 8945cc                   mov        dword ptr [ebp - 0x34], eax
004c6db3 eb0d                     jmp        0x4c6dc2
004c6db5 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004c6db8 8b148d10fe5a00           mov        edx, dword ptr [ecx*4 + 0x5afe10]
004c6dbf 8955cc                   mov        dword ptr [ebp - 0x34], edx
004c6dc2 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
004c6dc5 50                       push       eax
004c6dc6 8d4df0                   lea        ecx, [ebp - 0x10]
004c6dc9 51                       push       ecx
004c6dca 8b1598065c00             mov        edx, dword ptr [0x5c0698]
004c6dd0 52                       push       edx
004c6dd1 e8ba5bfaff               call       0x46c990
004c6dd6 83c40c                   add        esp, 0xc
004c6dd9 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c6ddc 8b880c010000             mov        ecx, dword ptr [eax + 0x10c]
004c6de2 51                       push       ecx
004c6de3 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c6de9 e832f0ffff               call       0x4c5e20
004c6dee 8bc8                     mov        ecx, eax
004c6df0 e88bf0ffff               call       0x4c5e80
004c6df5 85c0                     test       eax, eax
004c6df7 740b                     je         0x4c6e04
004c6df9 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004c6dfc 83ea01                   sub        edx, 1
004c6dff 8955c8                   mov        dword ptr [ebp - 0x38], edx
004c6e02 eb09                     jmp        0x4c6e0d
004c6e04 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004c6e07 83e802                   sub        eax, 2
004c6e0a 8945c8                   mov        dword ptr [ebp - 0x38], eax
004c6e0d 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004c6e10 3b4dc8                   cmp        ecx, dword ptr [ebp - 0x38]
004c6e13 0f8dc7000000             jge        0x4c6ee0
004c6e19 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004c6e1e f30f58055c105700         addss      xmm0, dword ptr [0x57105c]
004c6e26 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004c6e2b 6a01                     push       1
004c6e2d 6a00                     push       0
004c6e2f 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6e35 e8e617ffff               call       0x4b8620
004c6e3a 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
004c6e3d 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
004c6e40 0fbf0c50                 movsx      ecx, word ptr [eax + edx*2]
004c6e44 894dbc                   mov        dword ptr [ebp - 0x44], ecx
004c6e47 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c6e4a 8b820c010000             mov        eax, dword ptr [edx + 0x10c]
004c6e50 50                       push       eax
004c6e51 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c6e57 e8c4efffff               call       0x4c5e20
004c6e5c 8bc8                     mov        ecx, eax
004c6e5e e81df0ffff               call       0x4c5e80
004c6e63 85c0                     test       eax, eax
004c6e65 7409                     je         0x4c6e70
004c6e67 c745c000000000           mov        dword ptr [ebp - 0x40], 0
004c6e6e eb36                     jmp        0x4c6ea6
004c6e70 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c6e73 8b910c010000             mov        edx, dword ptr [ecx + 0x10c]
004c6e79 52                       push       edx
004c6e7a 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c6e80 e89befffff               call       0x4c5e20
004c6e85 8bc8                     mov        ecx, eax
004c6e87 e8c4efffff               call       0x4c5e50
004c6e8c 85c0                     test       eax, eax
004c6e8e 7409                     je         0x4c6e99
004c6e90 c745c401000000           mov        dword ptr [ebp - 0x3c], 1
004c6e97 eb07                     jmp        0x4c6ea0
004c6e99 c745c402000000           mov        dword ptr [ebp - 0x3c], 2
004c6ea0 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
004c6ea3 8945c0                   mov        dword ptr [ebp - 0x40], eax
004c6ea6 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004c6ea9 51                       push       ecx
004c6eaa 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
004c6ead 52                       push       edx
004c6eae e8dda9f5ff               call       0x421890
004c6eb3 83c408                   add        esp, 8
004c6eb6 8945b8                   mov        dword ptr [ebp - 0x48], eax
004c6eb9 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
004c6ebc 50                       push       eax
004c6ebd 8d4df0                   lea        ecx, [ebp - 0x10]
004c6ec0 51                       push       ecx
004c6ec1 8b1598065c00             mov        edx, dword ptr [0x5c0698]
004c6ec7 52                       push       edx
004c6ec8 e8c35afaff               call       0x46c990
004c6ecd 83c40c                   add        esp, 0xc
004c6ed0 6a01                     push       1
004c6ed2 6a01                     push       1
004c6ed4 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6eda e84117ffff               call       0x4b8620
004c6edf 90                       nop        
004c6ee0 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004c6ee3 8945f0                   mov        dword ptr [ebp - 0x10], eax
004c6ee6 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004c6ee9 894df4                   mov        dword ptr [ebp - 0xc], ecx
004c6eec 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004c6eef 8955f8                   mov        dword ptr [ebp - 8], edx
004c6ef2 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
004c6ef7 f30f580548fd5600         addss      xmm0, dword ptr [0x56fd48]
004c6eff f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004c6f04 e92efeffff               jmp        0x4c6d37
004c6f09 6a00                     push       0
004c6f0b 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6f11 e8ea1bfcff               call       0x488b00
004c6f16 6aff                     push       -1
004c6f18 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6f1e e8ad9bfaff               call       0x470ad0
004c6f23 6a03                     push       3
004c6f25 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6f2b e850f2ffff               call       0x4c6180
004c6f30 6880c0c0ff               push       0xffc0c080
004c6f35 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6f3b e8909bfaff               call       0x470ad0
004c6f40 6aff                     push       -1
004c6f42 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6f48 e813f2ffff               call       0x4c6160
004c6f4d 68000000ff               push       0xff000000
004c6f52 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6f58 e8539bfaff               call       0x470ab0
004c6f5d 6a00                     push       0
004c6f5f 6a00                     push       0
004c6f61 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c6f67 e8b416ffff               call       0x4b8620
004c6f6c 90                       nop        
004c6f6d 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c6f70 8b880c010000             mov        ecx, dword ptr [eax + 0x10c]
004c6f76 51                       push       ecx
004c6f77 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c6f7d e89eeeffff               call       0x4c5e20
004c6f82 8bc8                     mov        ecx, eax
004c6f84 e8c7eeffff               call       0x4c5e50
004c6f89 85c0                     test       eax, eax
004c6f8b 7520                     jne        0x4c6fad
004c6f8d 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c6f90 8b820c010000             mov        eax, dword ptr [edx + 0x10c]
004c6f96 50                       push       eax
004c6f97 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c6f9d e87eeeffff               call       0x4c5e20
004c6fa2 8bc8                     mov        ecx, eax
004c6fa4 e807efffff               call       0x4c5eb0
004c6fa9 85c0                     test       eax, eax
004c6fab 747d                     je         0x4c702a
004c6fad 51                       push       ecx
004c6fae 0f57c0                   xorps      xmm0, xmm0
004c6fb1 f30f110424               movss      dword ptr [esp], xmm0
004c6fb6 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c6fb9 f30f1081e4000000         movss      xmm0, dword ptr [ecx + 0xe4]
004c6fc1 f30f58050cf15600         addss      xmm0, dword ptr [0x56f10c]
004c6fc9 51                       push       ecx
004c6fca f30f110424               movss      dword ptr [esp], xmm0
004c6fcf 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c6fd2 f30f1082e0000000         movss      xmm0, dword ptr [edx + 0xe0]
004c6fda f30f580558105700         addss      xmm0, dword ptr [0x571058]
004c6fe2 51                       push       ecx
004c6fe3 f30f110424               movss      dword ptr [esp], xmm0
004c6fe8 8d4da0                   lea        ecx, [ebp - 0x60]
004c6feb e8e0bdf5ff               call       0x422dd0
004c6ff0 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004c6ff3 83ec08                   sub        esp, 8
004c6ff6 8bf4                     mov        esi, esp
004c6ff8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c6ffb 83c114                   add        ecx, 0x14
004c6ffe e8fd52f4ff               call       0x40c300
004c7003 8b04854cfe5a00           mov        eax, dword ptr [eax*4 + 0x5afe4c]
004c700a 50                       push       eax
004c700b 8bce                     mov        ecx, esi
004c700d e8be6df4ff               call       0x40ddd0
004c7012 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004c7015 51                       push       ecx
004c7016 8b1598065c00             mov        edx, dword ptr [0x5c0698]
004c701c 52                       push       edx
004c701d e8ee51faff               call       0x46c210
004c7022 83c410                   add        esp, 0x10
004c7025 e9df000000               jmp        0x4c7109
004c702a 51                       push       ecx
004c702b 0f57c0                   xorps      xmm0, xmm0
004c702e f30f110424               movss      dword ptr [esp], xmm0
004c7033 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c7036 f30f1080e4000000         movss      xmm0, dword ptr [eax + 0xe4]
004c703e f30f580588fe5600         addss      xmm0, dword ptr [0x56fe88]
004c7046 51                       push       ecx
004c7047 f30f110424               movss      dword ptr [esp], xmm0
004c704c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c704f f30f1081e0000000         movss      xmm0, dword ptr [ecx + 0xe0]
004c7057 f30f580558105700         addss      xmm0, dword ptr [0x571058]
004c705f 51                       push       ecx
004c7060 f30f110424               movss      dword ptr [esp], xmm0
004c7065 8d4d94                   lea        ecx, [ebp - 0x6c]
004c7068 e863bdf5ff               call       0x422dd0
004c706d 8945b0                   mov        dword ptr [ebp - 0x50], eax
004c7070 83ec08                   sub        esp, 8
004c7073 8bcc                     mov        ecx, esp
004c7075 6830105700               push       0x571030
004c707a e8516df4ff               call       0x40ddd0
004c707f 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
004c7082 52                       push       edx
004c7083 a198065c00               mov        eax, dword ptr [0x5c0698]
004c7088 50                       push       eax
004c7089 e88251faff               call       0x46c210
004c708e 83c410                   add        esp, 0x10
004c7091 51                       push       ecx
004c7092 0f57c0                   xorps      xmm0, xmm0
004c7095 f30f110424               movss      dword ptr [esp], xmm0
004c709a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c709d f30f1081e4000000         movss      xmm0, dword ptr [ecx + 0xe4]
004c70a5 f30f580560105700         addss      xmm0, dword ptr [0x571060]
004c70ad 51                       push       ecx
004c70ae f30f110424               movss      dword ptr [esp], xmm0
004c70b3 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c70b6 f30f1082e0000000         movss      xmm0, dword ptr [edx + 0xe0]
004c70be f30f580558105700         addss      xmm0, dword ptr [0x571058]
004c70c6 51                       push       ecx
004c70c7 f30f110424               movss      dword ptr [esp], xmm0
004c70cc 8d4d88                   lea        ecx, [ebp - 0x78]
004c70cf e8fcbcf5ff               call       0x422dd0
004c70d4 8945ac                   mov        dword ptr [ebp - 0x54], eax
004c70d7 83ec08                   sub        esp, 8
004c70da 8bf4                     mov        esi, esp
004c70dc 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c70df 83c114                   add        ecx, 0x14
004c70e2 e81952f4ff               call       0x40c300
004c70e7 8b048564fe5a00           mov        eax, dword ptr [eax*4 + 0x5afe64]
004c70ee 50                       push       eax
004c70ef 8bce                     mov        ecx, esi
004c70f1 e8da6cf4ff               call       0x40ddd0
004c70f6 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
004c70f9 51                       push       ecx
004c70fa 8b1598065c00             mov        edx, dword ptr [0x5c0698]
004c7100 52                       push       edx
004c7101 e80a51faff               call       0x46c210
004c7106 83c410                   add        esp, 0x10
004c7109 6aff                     push       -1
004c710b 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7111 e8ba99faff               call       0x470ad0
004c7116 51                       push       ecx
004c7117 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004c711f f30f110424               movss      dword ptr [esp], xmm0
004c7124 51                       push       ecx
004c7125 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004c712d f30f110424               movss      dword ptr [esp], xmm0
004c7132 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7138 e8531cffff               call       0x4b8d90
004c713d 6a01                     push       1
004c713f 6a01                     push       1
004c7141 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7147 e8d414ffff               call       0x4b8620
004c714c b801000000               mov        eax, 1
004c7151 5e                       pop        esi
004c7152 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7155 33cd                     xor        ecx, ebp
004c7157 e892b80700               call       0x5429ee
004c715c 8be5                     mov        esp, ebp
004c715e 5d                       pop        ebp
004c715f c3                       ret        
