00467990 55                       push       ebp
00467991 8bec                     mov        ebp, esp
00467993 83ec68                   sub        esp, 0x68
00467996 894dfc                   mov        dword ptr [ebp - 4], ecx
00467999 8b45fc                   mov        eax, dword ptr [ebp - 4]
0046799c 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0046799f 898888010000             mov        dword ptr [eax + 0x188], ecx
004679a5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004679a8 83c208                   add        edx, 8
004679ab 8955e8                   mov        dword ptr [ebp - 0x18], edx
004679ae 6a00                     push       0
004679b0 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004679b3 e8b85cffff               call       0x45d670
004679b8 0f57c0                   xorps      xmm0, xmm0
004679bb f30f1100                 movss      dword ptr [eax], xmm0
004679bf 8b45fc                   mov        eax, dword ptr [ebp - 4]
004679c2 83c008                   add        eax, 8
004679c5 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004679c8 6a00                     push       0
004679ca 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004679cd e89e5cffff               call       0x45d670
004679d2 0f57c0                   xorps      xmm0, xmm0
004679d5 f30f114004               movss      dword ptr [eax + 4], xmm0
004679da 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004679dd 51                       push       ecx
004679de f30f104118               movss      xmm0, dword ptr [ecx + 0x18]
004679e3 f30f110424               movss      dword ptr [esp], xmm0
004679e8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004679eb 81c170010000             add        ecx, 0x170
004679f1 e82ab5feff               call       0x452f20
004679f6 6a01                     push       1
004679f8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004679fb 81c178010000             add        ecx, 0x178
00467a01 e81abbfbff               call       0x423520
00467a06 90                       nop        
00467a07 c745f4ff000000           mov        dword ptr [ebp - 0xc], 0xff
00467a0e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00467a11 81c2f8000000             add        edx, 0xf8
00467a17 8955f0                   mov        dword ptr [ebp - 0x10], edx
00467a1a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00467a1d e87e43faff               call       0x40bda0
00467a22 8945f8                   mov        dword ptr [ebp - 8], eax
00467a25 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00467a28 e883f5ffff               call       0x466fb0
00467a2d 8945e0                   mov        dword ptr [ebp - 0x20], eax
00467a30 eb09                     jmp        0x467a3b
00467a32 8b45f8                   mov        eax, dword ptr [ebp - 8]
00467a35 83c004                   add        eax, 4
00467a38 8945f8                   mov        dword ptr [ebp - 8], eax
00467a3b 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00467a3e 3b4de0                   cmp        ecx, dword ptr [ebp - 0x20]
00467a41 7425                     je         0x467a68
00467a43 8b55f8                   mov        edx, dword ptr [ebp - 8]
00467a46 8955ec                   mov        dword ptr [ebp - 0x14], edx
00467a49 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00467a4c 83e808                   sub        eax, 8
00467a4f 8945f4                   mov        dword ptr [ebp - 0xc], eax
00467a52 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00467a55 8b5120                   mov        edx, dword ptr [ecx + 0x20]
00467a58 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00467a5b 8910                     mov        dword ptr [eax], edx
00467a5d 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00467a60 8a55f4                   mov        dl, byte ptr [ebp - 0xc]
00467a63 885103                   mov        byte ptr [ecx + 3], dl
00467a66 ebca                     jmp        0x467a32
00467a68 8b45fc                   mov        eax, dword ptr [ebp - 4]
00467a6b 83c008                   add        eax, 8
00467a6e 8945dc                   mov        dword ptr [ebp - 0x24], eax
00467a71 51                       push       ecx
00467a72 0f57c0                   xorps      xmm0, xmm0
00467a75 f30f110424               movss      dword ptr [esp], xmm0
00467a7a 51                       push       ecx
00467a7b 0f57c0                   xorps      xmm0, xmm0
00467a7e f30f110424               movss      dword ptr [esp], xmm0
00467a83 8d4da8                   lea        ecx, [ebp - 0x58]
00467a86 e8c517fcff               call       0x429250
00467a8b 8d4da8                   lea        ecx, [ebp - 0x58]
00467a8e 51                       push       ecx
00467a8f 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00467a92 e8d9010000               call       0x467c70
00467a97 90                       nop        
00467a98 8b55fc                   mov        edx, dword ptr [ebp - 4]
00467a9b 8b4204                   mov        eax, dword ptr [edx + 4]
00467a9e 8945d8                   mov        dword ptr [ebp - 0x28], eax
00467aa1 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00467aa4 51                       push       ecx
00467aa5 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00467aa8 e8b362ffff               call       0x45dd60
00467aad 90                       nop        
00467aae b958675b00               mov        ecx, 0x5b6758
00467ab3 e8d807fdff               call       0x438290
00467ab8 d95dd4                   fstp       dword ptr [ebp - 0x2c]
00467abb 8b5508                   mov        edx, dword ptr [ebp + 8]
00467abe f30f1045d4               movss      xmm0, dword ptr [ebp - 0x2c]
00467ac3 f30f59422c               mulss      xmm0, dword ptr [edx + 0x2c]
00467ac8 f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
00467ad0 f30f1145c8               movss      dword ptr [ebp - 0x38], xmm0
00467ad5 b958675b00               mov        ecx, 0x5b6758
00467ada e8b107fdff               call       0x438290
00467adf d95dd0                   fstp       dword ptr [ebp - 0x30]
00467ae2 8b4508                   mov        eax, dword ptr [ebp + 8]
00467ae5 f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
00467aea f30f594030               mulss      xmm0, dword ptr [eax + 0x30]
00467aef f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
00467af7 f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
00467afc 51                       push       ecx
00467afd f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
00467b02 f30f110424               movss      dword ptr [esp], xmm0
00467b07 51                       push       ecx
00467b08 f30f1045c8               movss      xmm0, dword ptr [ebp - 0x38]
00467b0d f30f110424               movss      dword ptr [esp], xmm0
00467b12 8d4da0                   lea        ecx, [ebp - 0x60]
00467b15 e83617fcff               call       0x429250
00467b1a 8b08                     mov        ecx, dword ptr [eax]
00467b1c 8b5004                   mov        edx, dword ptr [eax + 4]
00467b1f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00467b22 89888c010000             mov        dword ptr [eax + 0x18c], ecx
00467b28 899090010000             mov        dword ptr [eax + 0x190], edx
00467b2e 51                       push       ecx
00467b2f 0f57c0                   xorps      xmm0, xmm0
00467b32 f30f110424               movss      dword ptr [esp], xmm0
00467b37 51                       push       ecx
00467b38 0f57c0                   xorps      xmm0, xmm0
00467b3b f30f110424               movss      dword ptr [esp], xmm0
00467b40 8d4d98                   lea        ecx, [ebp - 0x68]
00467b43 e80817fcff               call       0x429250
00467b48 8b08                     mov        ecx, dword ptr [eax]
00467b4a 8b5004                   mov        edx, dword ptr [eax + 4]
00467b4d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00467b50 898894010000             mov        dword ptr [eax + 0x194], ecx
00467b56 899098010000             mov        dword ptr [eax + 0x198], edx
00467b5c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00467b5f 8b5104                   mov        edx, dword ptr [ecx + 4]
00467b62 8955c4                   mov        dword ptr [ebp - 0x3c], edx
00467b65 6a05                     push       5
00467b67 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00467b6a e8e187feff               call       0x450350
00467b6f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00467b72 8b4804                   mov        ecx, dword ptr [eax + 4]
00467b75 894dc0                   mov        dword ptr [ebp - 0x40], ecx
00467b78 6a00                     push       0
00467b7a 68ff000000               push       0xff
00467b7f 6a00                     push       0
00467b81 6a1e                     push       0x1e
00467b83 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00467b86 e8d510fdff               call       0x438c60
00467b8b 8b55fc                   mov        edx, dword ptr [ebp - 4]
00467b8e 8b4204                   mov        eax, dword ptr [edx + 4]
00467b91 8945bc                   mov        dword ptr [ebp - 0x44], eax
00467b94 6a22                     push       0x22
00467b96 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00467b99 e8b287feff               call       0x450350
00467b9e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00467ba1 8b5104                   mov        edx, dword ptr [ecx + 4]
00467ba4 8955b8                   mov        dword ptr [ebp - 0x48], edx
00467ba7 6a02                     push       2
00467ba9 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00467bac e82f99ffff               call       0x4614e0
00467bb1 8b45fc                   mov        eax, dword ptr [ebp - 4]
00467bb4 8b4804                   mov        ecx, dword ptr [eax + 4]
00467bb7 894db4                   mov        dword ptr [ebp - 0x4c], ecx
00467bba 6a01                     push       1
00467bbc 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00467bbf e87c61ffff               call       0x45dd40
00467bc4 b9c4a45b00               mov        ecx, 0x5ba4c4
00467bc9 e8621cfcff               call       0x429830
00467bce d95db0                   fstp       dword ptr [ebp - 0x50]
00467bd1 f30f1045b0               movss      xmm0, dword ptr [ebp - 0x50]
00467bd6 f30f590590cd5600         mulss      xmm0, dword ptr [0x56cd90]
00467bde f30f5805bcd75600         addss      xmm0, dword ptr [0x56d7bc]
00467be6 8b55fc                   mov        edx, dword ptr [ebp - 4]
00467be9 f30f118274010000         movss      dword ptr [edx + 0x174], xmm0
00467bf1 b9c4a45b00               mov        ecx, 0x5ba4c4
00467bf6 e8d51bfcff               call       0x4297d0
00467bfb 51                       push       ecx
00467bfc d91c24                   fstp       dword ptr [esp]
00467bff 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00467c02 81c170010000             add        ecx, 0x170
00467c08 e813b3feff               call       0x452f20
00467c0d 33c0                     xor        eax, eax
00467c0f 8be5                     mov        esp, ebp
00467c11 5d                       pop        ebp
00467c12 c20800                   ret        8
00467c15 cc                       int3       
00467c16 cc                       int3       
00467c17 cc                       int3       
00467c18 cc                       int3       
00467c19 cc                       int3       
00467c1a cc                       int3       
00467c1b cc                       int3       
00467c1c cc                       int3       
00467c1d cc                       int3       
00467c1e cc                       int3       
00467c1f cc                       int3       
