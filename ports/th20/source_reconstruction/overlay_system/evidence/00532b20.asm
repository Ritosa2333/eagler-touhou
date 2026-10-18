00532b20 55                       push       ebp
00532b21 8bec                     mov        ebp, esp
00532b23 83ec48                   sub        esp, 0x48
00532b26 894df8                   mov        dword ptr [ebp - 8], ecx
00532b29 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
00532b30 7459                     je         0x532b8b
00532b32 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
00532b38 e8c3170000               call       0x534300
00532b3d 0fb6c0                   movzx      eax, al
00532b40 85c0                     test       eax, eax
00532b42 7447                     je         0x532b8b
00532b44 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532b47 83795400                 cmp        dword ptr [ecx + 0x54], 0
00532b4b 743e                     je         0x532b8b
00532b4d 8b55f8                   mov        edx, dword ptr [ebp - 8]
00532b50 83c220                   add        edx, 0x20
00532b53 8955f4                   mov        dword ptr [ebp - 0xc], edx
00532b56 6a01                     push       1
00532b58 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00532b5b e830c4f1ff               call       0x44ef90
00532b60 6a00                     push       0
00532b62 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532b65 83c120                   add        ecx, 0x20
00532b68 e833ecedff               call       0x4117a0
00532b6d 8b45f8                   mov        eax, dword ptr [ebp - 8]
00532b70 c7405400000000           mov        dword ptr [eax + 0x54], 0
00532b77 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532b7a 8b5128                   mov        edx, dword ptr [ecx + 0x28]
00532b7d 8b45f8                   mov        eax, dword ptr [ebp - 8]
00532b80 8b12                     mov        edx, dword ptr [edx]
00532b82 8b4828                   mov        ecx, dword ptr [eax + 0x28]
00532b85 8b4258                   mov        eax, dword ptr [edx + 0x58]
00532b88 ffd0                     call       eax
00532b8a 90                       nop        
00532b8b 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
00532b92 0f8442030000             je         0x532eda
00532b98 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
00532b9e e8ad17f3ff               call       0x464350
00532ba3 85c0                     test       eax, eax
00532ba5 0f842f030000             je         0x532eda
00532bab 833da4065c0000           cmp        dword ptr [0x5c06a4], 0
00532bb2 0f8422030000             je         0x532eda
00532bb8 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
00532bbe e89d55f4ff               call       0x478160
00532bc3 85c0                     test       eax, eax
00532bc5 0f850f030000             jne        0x532eda
00532bcb 6a00                     push       0
00532bcd e8ae14f3ff               call       0x464080
00532bd2 83c404                   add        esp, 4
00532bd5 8bc8                     mov        ecx, eax
00532bd7 e8d49eeeff               call       0x41cab0
00532bdc 83f805                   cmp        eax, 5
00532bdf 741c                     je         0x532bfd
00532be1 6a00                     push       0
00532be3 e89814f3ff               call       0x464080
00532be8 83c404                   add        esp, 4
00532beb 8bc8                     mov        ecx, eax
00532bed e8be9eeeff               call       0x41cab0
00532bf2 83f807                   cmp        eax, 7
00532bf5 7406                     je         0x532bfd
00532bf7 c645fe01                 mov        byte ptr [ebp - 2], 1
00532bfb eb04                     jmp        0x532c01
00532bfd c645fe00                 mov        byte ptr [ebp - 2], 0
00532c01 8a4dfe                   mov        cl, byte ptr [ebp - 2]
00532c04 884dff                   mov        byte ptr [ebp - 1], cl
00532c07 8b55f8                   mov        edx, dword ptr [ebp - 8]
00532c0a 837a5402                 cmp        dword ptr [edx + 0x54], 2
00532c0e 7551                     jne        0x532c61
00532c10 8b45f8                   mov        eax, dword ptr [ebp - 8]
00532c13 8b4828                   mov        ecx, dword ptr [eax + 0x28]
00532c16 8b55f8                   mov        edx, dword ptr [ebp - 8]
00532c19 8b01                     mov        eax, dword ptr [ecx]
00532c1b 8b4a28                   mov        ecx, dword ptr [edx + 0x28]
00532c1e 8b5058                   mov        edx, dword ptr [eax + 0x58]
00532c21 ffd2                     call       edx
00532c23 85c0                     test       eax, eax
00532c25 7530                     jne        0x532c57
00532c27 8b45f8                   mov        eax, dword ptr [ebp - 8]
00532c2a f30f100598cd5600         movss      xmm0, dword ptr [0x56cd98]
00532c32 f30f114074               movss      dword ptr [eax + 0x74], xmm0
00532c37 0fb64dff                 movzx      ecx, byte ptr [ebp - 1]
00532c3b 85c9                     test       ecx, ecx
00532c3d 740e                     je         0x532c4d
00532c3f 6a3c                     push       0x3c
00532c41 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532c44 83c164                   add        ecx, 0x64
00532c47 e8d408efff               call       0x423520
00532c4c 90                       nop        
00532c4d e907010000               jmp        0x532d59
00532c52 e902010000               jmp        0x532d59
00532c57 8b55f8                   mov        edx, dword ptr [ebp - 8]
00532c5a c7425401000000           mov        dword ptr [edx + 0x54], 1
00532c61 8b45f8                   mov        eax, dword ptr [ebp - 8]
00532c64 83785401                 cmp        dword ptr [eax + 0x54], 1
00532c68 0f8535010000             jne        0x532da3
00532c6e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532c71 83c120                   add        ecx, 0x20
00532c74 e8972df5ff               call       0x485a10
00532c79 85c0                     test       eax, eax
00532c7b 7549                     jne        0x532cc6
00532c7d 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532c80 8b5124                   mov        edx, dword ptr [ecx + 0x24]
00532c83 8955ec                   mov        dword ptr [ebp - 0x14], edx
00532c86 6a00                     push       0
00532c88 e8a3dbf2ff               call       0x460830
00532c8d 83c404                   add        esp, 4
00532c90 8bc8                     mov        ecx, eax
00532c92 e8b9dbf2ff               call       0x460850
00532c97 8945f0                   mov        dword ptr [ebp - 0x10], eax
00532c9a 6a00                     push       0
00532c9c 6aff                     push       -1
00532c9e 51                       push       ecx
00532c9f 0f57c0                   xorps      xmm0, xmm0
00532ca2 f30f110424               movss      dword ptr [esp], xmm0
00532ca7 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00532caa 50                       push       eax
00532cab 6a1d                     push       0x1d
00532cad 68c4625700               push       0x5762c4
00532cb2 8d4db8                   lea        ecx, [ebp - 0x48]
00532cb5 51                       push       ecx
00532cb6 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00532cb9 e82264f4ff               call       0x4790e0
00532cbe 8b10                     mov        edx, dword ptr [eax]
00532cc0 8b45f8                   mov        eax, dword ptr [ebp - 8]
00532cc3 895020                   mov        dword ptr [eax + 0x20], edx
00532cc6 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532cc9 83c120                   add        ecx, 0x20
00532ccc 894de8                   mov        dword ptr [ebp - 0x18], ecx
00532ccf 6a00                     push       0
00532cd1 e85adbf2ff               call       0x460830
00532cd6 83c404                   add        esp, 4
00532cd9 8bc8                     mov        ecx, eax
00532cdb e870dbf2ff               call       0x460850
00532ce0 50                       push       eax
00532ce1 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00532ce4 e8f718f3ff               call       0x4645e0
00532ce9 90                       nop        
00532cea 8b55f8                   mov        edx, dword ptr [ebp - 8]
00532ced 8b4228                   mov        eax, dword ptr [edx + 0x28]
00532cf0 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532cf3 8b10                     mov        edx, dword ptr [eax]
00532cf5 8b4928                   mov        ecx, dword ptr [ecx + 0x28]
00532cf8 8b4254                   mov        eax, dword ptr [edx + 0x54]
00532cfb ffd0                     call       eax
00532cfd 85c0                     test       eax, eax
00532cff 0f849e000000             je         0x532da3
00532d05 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532d08 8b5128                   mov        edx, dword ptr [ecx + 0x28]
00532d0b 8b45f8                   mov        eax, dword ptr [ebp - 8]
00532d0e 8b12                     mov        edx, dword ptr [edx]
00532d10 8b4828                   mov        ecx, dword ptr [eax + 0x28]
00532d13 8b4258                   mov        eax, dword ptr [edx + 0x58]
00532d16 ffd0                     call       eax
00532d18 90                       nop        
00532d19 6a00                     push       0
00532d1b e86013f3ff               call       0x464080
00532d20 83c404                   add        esp, 4
00532d23 8bc8                     mov        ecx, eax
00532d25 e8869deeff               call       0x41cab0
00532d2a 83f806                   cmp        eax, 6
00532d2d 7504                     jne        0x532d33
00532d2f c645ff00                 mov        byte ptr [ebp - 1], 0
00532d33 0fb64dff                 movzx      ecx, byte ptr [ebp - 1]
00532d37 85c9                     test       ecx, ecx
00532d39 740e                     je         0x532d49
00532d3b 6a04                     push       4
00532d3d 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532d40 83c164                   add        ecx, 0x64
00532d43 e8d807efff               call       0x423520
00532d48 90                       nop        
00532d49 8b55f8                   mov        edx, dword ptr [ebp - 8]
00532d4c f30f1005b4f75600         movss      xmm0, dword ptr [0x56f7b4]
00532d54 f30f114274               movss      dword ptr [edx + 0x74], xmm0
00532d59 8b45f8                   mov        eax, dword ptr [ebp - 8]
00532d5c 83c020                   add        eax, 0x20
00532d5f 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00532d62 6a01                     push       1
00532d64 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00532d67 e824c2f1ff               call       0x44ef90
00532d6c 6a00                     push       0
00532d6e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532d71 83c120                   add        ecx, 0x20
00532d74 e827eaedff               call       0x4117a0
00532d79 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532d7c c7415400000000           mov        dword ptr [ecx + 0x54], 0
00532d83 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532d86 e8251d0000               call       0x534ab0
00532d8b 6a00                     push       0
00532d8d e8ee12f3ff               call       0x464080
00532d92 83c404                   add        esp, 4
00532d95 8945e0                   mov        dword ptr [ebp - 0x20], eax
00532d98 6a00                     push       0
00532d9a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00532d9d e89ebbf8ff               call       0x4be940
00532da2 90                       nop        
00532da3 8b55f8                   mov        edx, dword ptr [ebp - 8]
00532da6 83c264                   add        edx, 0x64
00532da9 8955dc                   mov        dword ptr [ebp - 0x24], edx
00532dac 6a00                     push       0
00532dae 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00532db1 e8bae2f2ff               call       0x461070
00532db6 0fb6c0                   movzx      eax, al
00532db9 85c0                     test       eax, eax
00532dbb 0f84b7000000             je         0x532e78
00532dc1 0fb64dff                 movzx      ecx, byte ptr [ebp - 1]
00532dc5 85c9                     test       ecx, ecx
00532dc7 0f84ab000000             je         0x532e78
00532dcd 6a00                     push       0
00532dcf e8ac60f4ff               call       0x478e80
00532dd4 83c404                   add        esp, 4
00532dd7 8945d0                   mov        dword ptr [ebp - 0x30], eax
00532dda 8b55f8                   mov        edx, dword ptr [ebp - 8]
00532ddd f30f104274               movss      xmm0, dword ptr [edx + 0x74]
00532de2 f30f1145d8               movss      dword ptr [ebp - 0x28], xmm0
00532de7 6a00                     push       0
00532de9 e842daf2ff               call       0x460830
00532dee 83c404                   add        esp, 4
00532df1 8bc8                     mov        ecx, eax
00532df3 e858daf2ff               call       0x460850
00532df8 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00532dfb 6a00                     push       0
00532dfd 689f860100               push       0x1869f
00532e02 6a00                     push       0
00532e04 51                       push       ecx
00532e05 f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
00532e0a f30f110424               movss      dword ptr [esp], xmm0
00532e0f 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00532e12 50                       push       eax
00532e13 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00532e16 e845a1f4ff               call       0x47cf60
00532e1b 6a00                     push       0
00532e1d e8be60f4ff               call       0x478ee0
00532e22 83c404                   add        esp, 4
00532e25 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00532e28 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532e2b f30f104174               movss      xmm0, dword ptr [ecx + 0x74]
00532e30 f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
00532e35 6a00                     push       0
00532e37 e8f4d9f2ff               call       0x460830
00532e3c 83c404                   add        esp, 4
00532e3f 8bc8                     mov        ecx, eax
00532e41 e80adaf2ff               call       0x460850
00532e46 8945c8                   mov        dword ptr [ebp - 0x38], eax
00532e49 6a01                     push       1
00532e4b 6a00                     push       0
00532e4d 51                       push       ecx
00532e4e f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
00532e53 f30f110424               movss      dword ptr [esp], xmm0
00532e58 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
00532e5b 52                       push       edx
00532e5c 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00532e5f e86c7cf9ff               call       0x4caad0
00532e64 8b45f8                   mov        eax, dword ptr [ebp - 8]
00532e67 83c064                   add        eax, 0x64
00532e6a 8945c0                   mov        dword ptr [ebp - 0x40], eax
00532e6d 6a00                     push       0
00532e6f 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00532e72 e8a965efff               call       0x429420
00532e77 90                       nop        
00532e78 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532e7b 8b5128                   mov        edx, dword ptr [ecx + 0x28]
00532e7e 8b45f8                   mov        eax, dword ptr [ebp - 8]
00532e81 8b12                     mov        edx, dword ptr [edx]
00532e83 8b4828                   mov        ecx, dword ptr [eax + 0x28]
00532e86 8b423c                   mov        eax, dword ptr [edx + 0x3c]
00532e89 ffd0                     call       eax
00532e8b 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532e8e 8b512c                   mov        edx, dword ptr [ecx + 0x2c]
00532e91 8b45f8                   mov        eax, dword ptr [ebp - 8]
00532e94 8b12                     mov        edx, dword ptr [edx]
00532e96 8b482c                   mov        ecx, dword ptr [eax + 0x2c]
00532e99 8b4240                   mov        eax, dword ptr [edx + 0x40]
00532e9c ffd0                     call       eax
00532e9e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532ea1 8b5130                   mov        edx, dword ptr [ecx + 0x30]
00532ea4 8b45f8                   mov        eax, dword ptr [ebp - 8]
00532ea7 8b12                     mov        edx, dword ptr [edx]
00532ea9 8b4830                   mov        ecx, dword ptr [eax + 0x30]
00532eac 8b4244                   mov        eax, dword ptr [edx + 0x44]
00532eaf ffd0                     call       eax
00532eb1 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532eb4 8b5134                   mov        edx, dword ptr [ecx + 0x34]
00532eb7 8b45f8                   mov        eax, dword ptr [ebp - 8]
00532eba 8b12                     mov        edx, dword ptr [edx]
00532ebc 8b4834                   mov        ecx, dword ptr [eax + 0x34]
00532ebf 8b4248                   mov        eax, dword ptr [edx + 0x48]
00532ec2 ffd0                     call       eax
00532ec4 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532ec7 83c110                   add        ecx, 0x10
00532eca 894dbc                   mov        dword ptr [ebp - 0x44], ecx
00532ecd 6a00                     push       0
00532ecf 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00532ed2 e86906efff               call       0x423540
00532ed7 90                       nop        
00532ed8 eb37                     jmp        0x532f11
00532eda 833da4065c0000           cmp        dword ptr [0x5c06a4], 0
00532ee1 742e                     je         0x532f11
00532ee3 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
00532ee9 e87252f4ff               call       0x478160
00532eee 85c0                     test       eax, eax
00532ef0 741f                     je         0x532f11
00532ef2 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532ef5 83c120                   add        ecx, 0x20
00532ef8 e8d3cdf1ff               call       0x44fcd0
00532efd 8b55f8                   mov        edx, dword ptr [ebp - 8]
00532f00 8b4228                   mov        eax, dword ptr [edx + 0x28]
00532f03 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00532f06 8b10                     mov        edx, dword ptr [eax]
00532f08 8b4928                   mov        ecx, dword ptr [ecx + 0x28]
00532f0b 8b425c                   mov        eax, dword ptr [edx + 0x5c]
00532f0e ffd0                     call       eax
00532f10 90                       nop        
00532f11 b801000000               mov        eax, 1
00532f16 8be5                     mov        esp, ebp
00532f18 5d                       pop        ebp
00532f19 c3                       ret        
00532f1a cc                       int3       
00532f1b cc                       int3       
00532f1c cc                       int3       
00532f1d cc                       int3       
00532f1e cc                       int3       
00532f1f cc                       int3       
