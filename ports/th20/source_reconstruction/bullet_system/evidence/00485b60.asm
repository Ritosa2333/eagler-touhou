00485b60 55                       push       ebp
00485b61 8bec                     mov        ebp, esp
00485b63 81ec88000000             sub        esp, 0x88
00485b69 894dfc                   mov        dword ptr [ebp - 4], ecx
00485b6c 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485b6f 8b4814                   mov        ecx, dword ptr [eax + 0x14]
00485b72 81c900040000             or         ecx, 0x400
00485b78 8b55fc                   mov        edx, dword ptr [ebp - 4]
00485b7b 894a14                   mov        dword ptr [edx + 0x14], ecx
00485b7e 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485b81 0508050000               add        eax, 0x508
00485b86 8945ec                   mov        dword ptr [ebp - 0x14], eax
00485b89 6a00                     push       0
00485b8b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00485b8e e8add9f9ff               call       0x423540
00485b93 90                       nop        
00485b94 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485b97 8b5114                   mov        edx, dword ptr [ecx + 0x14]
00485b9a c1ea03                   shr        edx, 3
00485b9d 83e201                   and        edx, 1
00485ba0 7410                     je         0x485bb2
00485ba2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485ba5 e8967effff               call       0x47da40
00485baa 83c8ff                   or         eax, 0xffffffff
00485bad e9e0060000               jmp        0x486292
00485bb2 b9e4ef5a00               mov        ecx, 0x5aefe4
00485bb7 e82437faff               call       0x4292e0
00485bbc d95df0                   fstp       dword ptr [ebp - 0x10]
00485bbf 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485bc2 83785002                 cmp        dword ptr [eax + 0x50], 2
00485bc6 7467                     je         0x485c2f
00485bc8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485bcb 83795003                 cmp        dword ptr [ecx + 0x50], 3
00485bcf 745e                     je         0x485c2f
00485bd1 8b55fc                   mov        edx, dword ptr [ebp - 4]
00485bd4 837a5004                 cmp        dword ptr [edx + 0x50], 4
00485bd8 7455                     je         0x485c2f
00485bda 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485bdd 8b4814                   mov        ecx, dword ptr [eax + 0x14]
00485be0 c1e909                   shr        ecx, 9
00485be3 83e101                   and        ecx, 1
00485be6 7547                     jne        0x485c2f
00485be8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485beb 81c1d8040000             add        ecx, 0x4d8
00485bf1 e8dadff9ff               call       0x423bd0
00485bf6 d95de8                   fstp       dword ptr [ebp - 0x18]
00485bf9 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00485bfe 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485c01 81c1e8040000             add        ecx, 0x4e8
00485c07 f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00485c0c e8bfdff9ff               call       0x423bd0
00485c11 d95de0                   fstp       dword ptr [ebp - 0x20]
00485c14 f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00485c19 f30f5945e0               mulss      xmm0, dword ptr [ebp - 0x20]
00485c1e 51                       push       ecx
00485c1f f30f110424               movss      dword ptr [esp], xmm0
00485c24 b9e4ef5a00               mov        ecx, 0x5aefe4
00485c29 e87236faff               call       0x4292a0
00485c2e 90                       nop        
00485c2f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00485c32 8b8290000000             mov        eax, dword ptr [edx + 0x90]
00485c38 c1e816                   shr        eax, 0x16
00485c3b 83e001                   and        eax, 1
00485c3e 7461                     je         0x485ca1
00485c40 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485c43 81c174040000             add        ecx, 0x474
00485c49 e8c244faff               call       0x42a110
00485c4e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485c51 d95948                   fstp       dword ptr [ecx + 0x48]
00485c54 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485c57 81c174040000             add        ecx, 0x474
00485c5d e88e6ef9ff               call       0x41caf0
00485c62 85c0                     test       eax, eax
00485c64 753b                     jne        0x485ca1
00485c66 8b55fc                   mov        edx, dword ptr [ebp - 4]
00485c69 8b8290000000             mov        eax, dword ptr [edx + 0x90]
00485c6f 25ffffbfff               and        eax, 0xffbfffff
00485c74 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485c77 898190000000             mov        dword ptr [ecx + 0x90], eax
00485c7d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00485c80 f30f104248               movss      xmm0, dword ptr [edx + 0x48]
00485c85 0f2e05ccc85600           ucomiss    xmm0, dword ptr [0x56c8cc]
00485c8c 9f                       lahf       
00485c8d f6c444                   test       ah, 0x44
00485c90 7a0f                     jp         0x485ca1
00485c92 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485c95 8b4814                   mov        ecx, dword ptr [eax + 0x14]
00485c98 83e1bf                   and        ecx, 0xffffffbf
00485c9b 8b55fc                   mov        edx, dword ptr [ebp - 4]
00485c9e 894a14                   mov        dword ptr [edx + 0x14], ecx
00485ca1 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485ca4 8b4850                   mov        ecx, dword ptr [eax + 0x50]
00485ca7 894df4                   mov        dword ptr [ebp - 0xc], ecx
00485caa 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00485cad 83ea01                   sub        edx, 1
00485cb0 8955f4                   mov        dword ptr [ebp - 0xc], edx
00485cb3 837df403                 cmp        dword ptr [ebp - 0xc], 3
00485cb7 0f8778040000             ja         0x486135
00485cbd 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00485cc0 ff248598624800           jmp        dword ptr [eax*4 + 0x486298]
00485cc7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485cca 83c170                   add        ecx, 0x70
00485ccd 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00485cd0 51                       push       ecx
00485cd1 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
00485cd9 f30f110424               movss      dword ptr [esp], xmm0
00485cde 8d559c                   lea        edx, [ebp - 0x64]
00485ce1 52                       push       edx
00485ce2 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00485ce5 e806d3fcff               call       0x452ff0
00485cea 50                       push       eax
00485ceb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485cee 83c164                   add        ecx, 0x64
00485cf1 e8ea39faff               call       0x4296e0
00485cf6 90                       nop        
00485cf7 51                       push       ecx
00485cf8 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00485d00 f30f110424               movss      dword ptr [esp], xmm0
00485d05 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485d08 81c1d8040000             add        ecx, 0x4d8
00485d0e e8dd68ffff               call       0x47c5f0
00485d13 51                       push       ecx
00485d14 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00485d1c f30f110424               movss      dword ptr [esp], xmm0
00485d21 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485d24 81c1e8040000             add        ecx, 0x4e8
00485d2a e8c168ffff               call       0x47c5f0
00485d2f 51                       push       ecx
00485d30 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00485d38 f30f110424               movss      dword ptr [esp], xmm0
00485d3d b9e4ef5a00               mov        ecx, 0x5aefe4
00485d42 e85935faff               call       0x4292a0
00485d47 90                       nop        
00485d48 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485d4b 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
00485d4e e86d73fdff               call       0x45d0c0
00485d53 833800                   cmp        dword ptr [eax], 0
00485d56 7505                     jne        0x485d5d
00485d58 e9d8030000               jmp        0x486135
00485d5d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485d60 c7415001000000           mov        dword ptr [ecx + 0x50], 1
00485d67 8b55fc                   mov        edx, dword ptr [ebp - 4]
00485d6a 8b8290000000             mov        eax, dword ptr [edx + 0x90]
00485d70 c1e81a                   shr        eax, 0x1a
00485d73 83e001                   and        eax, 1
00485d76 7517                     jne        0x485d8f
00485d78 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485d7b e8707fffff               call       0x47dcf0
00485d80 90                       nop        
00485d81 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485d84 83795002                 cmp        dword ptr [ecx + 0x50], 2
00485d88 7505                     jne        0x485d8f
00485d8a e968ffffff               jmp        0x485cf7
00485d8f c745f800000000           mov        dword ptr [ebp - 8], 0
00485d96 8b55fc                   mov        edx, dword ptr [ebp - 4]
00485d99 8b8290000000             mov        eax, dword ptr [edx + 0x90]
00485d9f 83e001                   and        eax, 1
00485da2 740e                     je         0x485db2
00485da4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485da7 e834e3ffff               call       0x4840e0
00485dac 0345f8                   add        eax, dword ptr [ebp - 8]
00485daf 8945f8                   mov        dword ptr [ebp - 8], eax
00485db2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485db5 8b9190000000             mov        edx, dword ptr [ecx + 0x90]
00485dbb c1ea02                   shr        edx, 2
00485dbe 83e201                   and        edx, 1
00485dc1 740e                     je         0x485dd1
00485dc3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485dc6 e835e1ffff               call       0x483f00
00485dcb 0345f8                   add        eax, dword ptr [ebp - 8]
00485dce 8945f8                   mov        dword ptr [ebp - 8], eax
00485dd1 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485dd4 8b8890000000             mov        ecx, dword ptr [eax + 0x90]
00485dda c1e915                   shr        ecx, 0x15
00485ddd 83e101                   and        ecx, 1
00485de0 740e                     je         0x485df0
00485de2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485de5 e836dfffff               call       0x483d20
00485dea 0345f8                   add        eax, dword ptr [ebp - 8]
00485ded 8945f8                   mov        dword ptr [ebp - 8], eax
00485df0 8b55fc                   mov        edx, dword ptr [ebp - 4]
00485df3 8b8290000000             mov        eax, dword ptr [edx + 0x90]
00485df9 c1e803                   shr        eax, 3
00485dfc 83e001                   and        eax, 1
00485dff 740e                     je         0x485e0f
00485e01 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485e04 e817dbffff               call       0x483920
00485e09 0345f8                   add        eax, dword ptr [ebp - 8]
00485e0c 8945f8                   mov        dword ptr [ebp - 8], eax
00485e0f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485e12 8b9190000000             mov        edx, dword ptr [ecx + 0x90]
00485e18 c1ea04                   shr        edx, 4
00485e1b 83e201                   and        edx, 1
00485e1e 740e                     je         0x485e2e
00485e20 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485e23 e858d7ffff               call       0x483580
00485e28 0345f8                   add        eax, dword ptr [ebp - 8]
00485e2b 8945f8                   mov        dword ptr [ebp - 8], eax
00485e2e 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485e31 8b8890000000             mov        ecx, dword ptr [eax + 0x90]
00485e37 c1e906                   shr        ecx, 6
00485e3a 83e101                   and        ecx, 1
00485e3d 740e                     je         0x485e4d
00485e3f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485e42 e819ccffff               call       0x482a60
00485e47 0345f8                   add        eax, dword ptr [ebp - 8]
00485e4a 8945f8                   mov        dword ptr [ebp - 8], eax
00485e4d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00485e50 8b8290000000             mov        eax, dword ptr [edx + 0x90]
00485e56 c1e81f                   shr        eax, 0x1f
00485e59 83e001                   and        eax, 1
00485e5c 740e                     je         0x485e6c
00485e5e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485e61 e84aafffff               call       0x480db0
00485e66 0345f8                   add        eax, dword ptr [ebp - 8]
00485e69 8945f8                   mov        dword ptr [ebp - 8], eax
00485e6c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485e6f 8b9190000000             mov        edx, dword ptr [ecx + 0x90]
00485e75 c1ea11                   shr        edx, 0x11
00485e78 83e201                   and        edx, 1
00485e7b 740e                     je         0x485e8b
00485e7d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485e80 e85bc9ffff               call       0x4827e0
00485e85 0345f8                   add        eax, dword ptr [ebp - 8]
00485e88 8945f8                   mov        dword ptr [ebp - 8], eax
00485e8b 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485e8e 8b8890000000             mov        ecx, dword ptr [eax + 0x90]
00485e94 c1e913                   shr        ecx, 0x13
00485e97 83e101                   and        ecx, 1
00485e9a 740e                     je         0x485eaa
00485e9c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485e9f e85c69ffff               call       0x47c800
00485ea4 0345f8                   add        eax, dword ptr [ebp - 8]
00485ea7 8945f8                   mov        dword ptr [ebp - 8], eax
00485eaa 8b55fc                   mov        edx, dword ptr [ebp - 4]
00485ead 8b8290000000             mov        eax, dword ptr [edx + 0x90]
00485eb3 c1e808                   shr        eax, 8
00485eb6 83e001                   and        eax, 1
00485eb9 740e                     je         0x485ec9
00485ebb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485ebe e83db3ffff               call       0x481200
00485ec3 0345f8                   add        eax, dword ptr [ebp - 8]
00485ec6 8945f8                   mov        dword ptr [ebp - 8], eax
00485ec9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485ecc 8b9190000000             mov        edx, dword ptr [ecx + 0x90]
00485ed2 c1ea1e                   shr        edx, 0x1e
00485ed5 83e201                   and        edx, 1
00485ed8 746e                     je         0x485f48
00485eda 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485edd 05a0000000               add        eax, 0xa0
00485ee2 8945d8                   mov        dword ptr [ebp - 0x28], eax
00485ee5 6a05                     push       5
00485ee7 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00485eea e89167ffff               call       0x47c680
00485eef 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00485ef2 6a00                     push       0
00485ef4 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00485ef7 e894d6f9ff               call       0x423590
00485efc 0fb6c8                   movzx      ecx, al
00485eff 85c9                     test       ecx, ecx
00485f01 7422                     je         0x485f25
00485f03 8b55fc                   mov        edx, dword ptr [ebp - 4]
00485f06 8b8290000000             mov        eax, dword ptr [edx + 0x90]
00485f0c 25ffffffbf               and        eax, 0xbfffffff
00485f11 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485f14 898190000000             mov        dword ptr [ecx + 0x90], eax
00485f1a 8b55f8                   mov        edx, dword ptr [ebp - 8]
00485f1d 83c201                   add        edx, 1
00485f20 8955f8                   mov        dword ptr [ebp - 8], edx
00485f23 eb23                     jmp        0x485f48
00485f25 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485f28 05a0000000               add        eax, 0xa0
00485f2d 8945d0                   mov        dword ptr [ebp - 0x30], eax
00485f30 6a05                     push       5
00485f32 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00485f35 e84667ffff               call       0x47c680
00485f3a 8945cc                   mov        dword ptr [ebp - 0x34], eax
00485f3d 6a00                     push       0
00485f3f 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00485f42 e8d934faff               call       0x429420
00485f47 90                       nop        
00485f48 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485f4b 8b9190000000             mov        edx, dword ptr [ecx + 0x90]
00485f51 c1ea1a                   shr        edx, 0x1a
00485f54 83e201                   and        edx, 1
00485f57 0f8496000000             je         0x485ff3
00485f5d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485f60 05a0000000               add        eax, 0xa0
00485f65 8945c8                   mov        dword ptr [ebp - 0x38], eax
00485f68 6a0d                     push       0xd
00485f6a 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00485f6d e80e67ffff               call       0x47c680
00485f72 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00485f75 6a00                     push       0
00485f77 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00485f7a e811d6f9ff               call       0x423590
00485f7f 0fb6c8                   movzx      ecx, al
00485f82 85c9                     test       ecx, ecx
00485f84 7438                     je         0x485fbe
00485f86 8b55fc                   mov        edx, dword ptr [ebp - 4]
00485f89 8b8290000000             mov        eax, dword ptr [edx + 0x90]
00485f8f 25fffffffb               and        eax, 0xfbffffff
00485f94 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485f97 898190000000             mov        dword ptr [ecx + 0x90], eax
00485f9d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00485fa0 8b4214                   mov        eax, dword ptr [edx + 0x14]
00485fa3 25fffdffff               and        eax, 0xfffffdff
00485fa8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485fab 894114                   mov        dword ptr [ecx + 0x14], eax
00485fae 8b55f8                   mov        edx, dword ptr [ebp - 8]
00485fb1 83c201                   add        edx, 1
00485fb4 8955f8                   mov        dword ptr [ebp - 8], edx
00485fb7 e9abfdffff               jmp        0x485d67
00485fbc eb35                     jmp        0x485ff3
00485fbe 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485fc1 8b4814                   mov        ecx, dword ptr [eax + 0x14]
00485fc4 81c900020000             or         ecx, 0x200
00485fca 8b55fc                   mov        edx, dword ptr [ebp - 4]
00485fcd 894a14                   mov        dword ptr [edx + 0x14], ecx
00485fd0 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485fd3 05a0000000               add        eax, 0xa0
00485fd8 8945c0                   mov        dword ptr [ebp - 0x40], eax
00485fdb 6a0d                     push       0xd
00485fdd 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00485fe0 e89b66ffff               call       0x47c680
00485fe5 8945bc                   mov        dword ptr [ebp - 0x44], eax
00485fe8 6a00                     push       0
00485fea 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00485fed e82e34faff               call       0x429420
00485ff2 90                       nop        
00485ff3 837df800                 cmp        dword ptr [ebp - 8], 0
00485ff7 7405                     je         0x485ffe
00485ff9 e969fdffff               jmp        0x485d67
00485ffe 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00486001 8b5114                   mov        edx, dword ptr [ecx + 0x14]
00486004 c1ea09                   shr        edx, 9
00486007 83e201                   and        edx, 1
0048600a 7530                     jne        0x48603c
0048600c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048600f 83c070                   add        eax, 0x70
00486012 8945b8                   mov        dword ptr [ebp - 0x48], eax
00486015 b9e4ef5a00               mov        ecx, 0x5aefe4
0048601a e8c132faff               call       0x4292e0
0048601f 51                       push       ecx
00486020 d91c24                   fstp       dword ptr [esp]
00486023 8d4d90                   lea        ecx, [ebp - 0x70]
00486026 51                       push       ecx
00486027 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
0048602a e8c132faff               call       0x4292f0
0048602f 50                       push       eax
00486030 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00486033 83c164                   add        ecx, 0x64
00486036 e8a536faff               call       0x4296e0
0048603b 90                       nop        
0048603c e9f4000000               jmp        0x486135
00486041 51                       push       ecx
00486042 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
0048604a f30f110424               movss      dword ptr [esp], xmm0
0048604f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00486052 81c1d8040000             add        ecx, 0x4d8
00486058 e89365ffff               call       0x47c5f0
0048605d 51                       push       ecx
0048605e f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00486066 f30f110424               movss      dword ptr [esp], xmm0
0048606b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048606e 81c1e8040000             add        ecx, 0x4e8
00486074 e87765ffff               call       0x47c5f0
00486079 51                       push       ecx
0048607a f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00486082 f30f110424               movss      dword ptr [esp], xmm0
00486087 b9e4ef5a00               mov        ecx, 0x5aefe4
0048608c e80f32faff               call       0x4292a0
00486091 90                       nop        
00486092 8b55fc                   mov        edx, dword ptr [ebp - 4]
00486095 83c270                   add        edx, 0x70
00486098 8955b4                   mov        dword ptr [ebp - 0x4c], edx
0048609b b9e4ef5a00               mov        ecx, 0x5aefe4
004860a0 e83b32faff               call       0x4292e0
004860a5 51                       push       ecx
004860a6 d91c24                   fstp       dword ptr [esp]
004860a9 8d4584                   lea        eax, [ebp - 0x7c]
004860ac 50                       push       eax
004860ad 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004860b0 e83b32faff               call       0x4292f0
004860b5 8945b0                   mov        dword ptr [ebp - 0x50], eax
004860b8 51                       push       ecx
004860b9 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
004860c1 f30f110424               movss      dword ptr [esp], xmm0
004860c6 8d8d78ffffff             lea        ecx, [ebp - 0x88]
004860cc 51                       push       ecx
004860cd 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
004860d0 e81bcffcff               call       0x452ff0
004860d5 50                       push       eax
004860d6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004860d9 83c164                   add        ecx, 0x64
004860dc e8ff35faff               call       0x4296e0
004860e1 90                       nop        
004860e2 eb51                     jmp        0x486135
004860e4 51                       push       ecx
004860e5 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004860ed f30f110424               movss      dword ptr [esp], xmm0
004860f2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004860f5 81c1d8040000             add        ecx, 0x4d8
004860fb e8f064ffff               call       0x47c5f0
00486100 51                       push       ecx
00486101 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00486109 f30f110424               movss      dword ptr [esp], xmm0
0048610e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00486111 81c1e8040000             add        ecx, 0x4e8
00486117 e8d464ffff               call       0x47c5f0
0048611c 51                       push       ecx
0048611d f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00486125 f30f110424               movss      dword ptr [esp], xmm0
0048612a b9e4ef5a00               mov        ecx, 0x5aefe4
0048612f e86c31faff               call       0x4292a0
00486134 90                       nop        
00486135 8b55fc                   mov        edx, dword ptr [ebp - 4]
00486138 8b4a1c                   mov        ecx, dword ptr [edx + 0x1c]
0048613b e8901bfbff               call       0x437cd0
00486140 85c0                     test       eax, eax
00486142 0f84c4000000             je         0x48620c
00486148 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048614b 8b8890000000             mov        ecx, dword ptr [eax + 0x90]
00486151 c1e90c                   shr        ecx, 0xc
00486154 83e101                   and        ecx, 1
00486157 7409                     je         0x486162
00486159 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048615c e80fd1ffff               call       0x483270
00486161 90                       nop        
00486162 8b55fc                   mov        edx, dword ptr [ebp - 4]
00486165 8b8290000000             mov        eax, dword ptr [edx + 0x90]
0048616b c1e808                   shr        eax, 8
0048616e 83e001                   and        eax, 1
00486171 0f8595000000             jne        0x48620c
00486177 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048617a 83793001                 cmp        dword ptr [ecx + 0x30], 1
0048617e 0f8d88000000             jge        0x48620c
00486184 8b55fc                   mov        edx, dword ptr [ebp - 4]
00486187 8b4a1c                   mov        ecx, dword ptr [edx + 0x1c]
0048618a e8411bfbff               call       0x437cd0
0048618f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00486192 f30f104048               movss      xmm0, dword ptr [eax + 0x48]
00486197 f30f594148               mulss      xmm0, dword ptr [ecx + 0x48]
0048619c f30f1145ac               movss      dword ptr [ebp - 0x54], xmm0
004861a1 8b55fc                   mov        edx, dword ptr [ebp - 4]
004861a4 8b4a1c                   mov        ecx, dword ptr [edx + 0x1c]
004861a7 e8241bfbff               call       0x437cd0
004861ac 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004861af f30f10404c               movss      xmm0, dword ptr [eax + 0x4c]
004861b4 f30f594148               mulss      xmm0, dword ptr [ecx + 0x48]
004861b9 f30f1145a8               movss      dword ptr [ebp - 0x58], xmm0
004861be 51                       push       ecx
004861bf f30f1045ac               movss      xmm0, dword ptr [ebp - 0x54]
004861c4 f30f110424               movss      dword ptr [esp], xmm0
004861c9 51                       push       ecx
004861ca f30f1045a8               movss      xmm0, dword ptr [ebp - 0x58]
004861cf f30f110424               movss      dword ptr [esp], xmm0
004861d4 8b55fc                   mov        edx, dword ptr [ebp - 4]
004861d7 83c264                   add        edx, 0x64
004861da 52                       push       edx
004861db e8a0f3ffff               call       0x485580
004861e0 83c40c                   add        esp, 0xc
004861e3 85c0                     test       eax, eax
004861e5 7425                     je         0x48620c
004861e7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004861ea e85178ffff               call       0x47da40
004861ef 51                       push       ecx
004861f0 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004861f5 f30f110424               movss      dword ptr [esp], xmm0
004861fa b9e4ef5a00               mov        ecx, 0x5aefe4
004861ff e89c30faff               call       0x4292a0
00486204 83c8ff                   or         eax, 0xffffffff
00486207 e986000000               jmp        0x486292
0048620c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048620f 83781800                 cmp        dword ptr [eax + 0x18], 0
00486213 740f                     je         0x486224
00486215 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00486218 8b5118                   mov        edx, dword ptr [ecx + 0x18]
0048621b 83ea01                   sub        edx, 1
0048621e 8b45fc                   mov        eax, dword ptr [ebp - 4]
00486221 895018                   mov        dword ptr [eax + 0x18], edx
00486224 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00486227 83793000                 cmp        dword ptr [ecx + 0x30], 0
0048622b 7e0f                     jle        0x48623c
0048622d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00486230 8b4230                   mov        eax, dword ptr [edx + 0x30]
00486233 83e801                   sub        eax, 1
00486236 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00486239 894130                   mov        dword ptr [ecx + 0x30], eax
0048623c 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048623f 8b4214                   mov        eax, dword ptr [edx + 0x14]
00486242 c1e809                   shr        eax, 9
00486245 83e001                   and        eax, 1
00486248 7531                     jne        0x48627b
0048624a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048624d 8b491c                   mov        ecx, dword ptr [ecx + 0x1c]
00486250 e87b53faff               call       0x42b5d0
00486255 85c0                     test       eax, eax
00486257 7422                     je         0x48627b
00486259 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048625c e8df77ffff               call       0x47da40
00486261 51                       push       ecx
00486262 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00486267 f30f110424               movss      dword ptr [esp], xmm0
0048626c b9e4ef5a00               mov        ecx, 0x5aefe4
00486271 e82a30faff               call       0x4292a0
00486276 83c8ff                   or         eax, 0xffffffff
00486279 eb17                     jmp        0x486292
0048627b 51                       push       ecx
0048627c f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00486281 f30f110424               movss      dword ptr [esp], xmm0
00486286 b9e4ef5a00               mov        ecx, 0x5aefe4
0048628b e81030faff               call       0x4292a0
00486290 33c0                     xor        eax, eax
00486292 8be5                     mov        esp, ebp
00486294 5d                       pop        ebp
00486295 c3                       ret        
00486296 6690                     nop        
00486298 675d                     pop        ebp
0048629a 48                       dec        eax
0048629b 00c7                     add        bh, al
0048629d 5c                       pop        esp
0048629e 48                       dec        eax
0048629f 004160                   add        byte ptr [ecx + 0x60], al
004862a2 48                       dec        eax
004862a3 00e4                     add        ah, ah
004862a5 60                       pushal     
004862a6 48                       dec        eax
004862a7 00cc                     add        ah, cl
004862a9 cc                       int3       
004862aa cc                       int3       
004862ab cc                       int3       
004862ac cc                       int3       
004862ad cc                       int3       
004862ae cc                       int3       
004862af cc                       int3       
