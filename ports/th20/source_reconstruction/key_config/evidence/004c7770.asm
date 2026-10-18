004c7770 55                       push       ebp
004c7771 8bec                     mov        ebp, esp
004c7773 83ec78                   sub        esp, 0x78
004c7776 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c777b 33c5                     xor        eax, ebp
004c777d 8945fc                   mov        dword ptr [ebp - 4], eax
004c7780 894de0                   mov        dword ptr [ebp - 0x20], ecx
004c7783 6a07                     push       7
004c7785 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c778b e87013fcff               call       0x488b00
004c7790 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c7793 05e0000000               add        eax, 0xe0
004c7798 8b08                     mov        ecx, dword ptr [eax]
004c779a 894df0                   mov        dword ptr [ebp - 0x10], ecx
004c779d 8b5004                   mov        edx, dword ptr [eax + 4]
004c77a0 8955f4                   mov        dword ptr [ebp - 0xc], edx
004c77a3 8b4008                   mov        eax, dword ptr [eax + 8]
004c77a6 8945f8                   mov        dword ptr [ebp - 8], eax
004c77a9 8d4de4                   lea        ecx, [ebp - 0x1c]
004c77ac e85fb6f5ff               call       0x422e10
004c77b1 6a01                     push       1
004c77b3 6a00                     push       0
004c77b5 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c77bb e8600effff               call       0x4b8620
004c77c0 6aff                     push       -1
004c77c2 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c77c8 e80393faff               call       0x470ad0
004c77cd 68000000ff               push       0xff000000
004c77d2 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c77d8 e8d392faff               call       0x470ab0
004c77dd f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004c77e2 f30f580558105700         addss      xmm0, dword ptr [0x571058]
004c77ea f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004c77ef 686c0e5700               push       0x570e6c
004c77f4 8d4df0                   lea        ecx, [ebp - 0x10]
004c77f7 51                       push       ecx
004c77f8 8b1598065c00             mov        edx, dword ptr [0x5c0698]
004c77fe 52                       push       edx
004c77ff e88c51faff               call       0x46c990
004c7804 83c40c                   add        esp, 0xc
004c7807 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
004c780c f30f580504fc5600         addss      xmm0, dword ptr [0x56fc04]
004c7814 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004c7819 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c781c 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004c781f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c7822 894de8                   mov        dword ptr [ebp - 0x18], ecx
004c7825 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c7828 8955ec                   mov        dword ptr [ebp - 0x14], edx
004c782b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c782e 81c1ec000000             add        ecx, 0xec
004c7834 e85787f4ff               call       0x40ff90
004c7839 8945d8                   mov        dword ptr [ebp - 0x28], eax
004c783c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c783f 81c1d0000000             add        ecx, 0xd0
004c7845 e84687f4ff               call       0x40ff90
004c784a 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004c784d 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004c7850 50                       push       eax
004c7851 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004c7854 51                       push       ecx
004c7855 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c7858 83c214                   add        edx, 0x14
004c785b 52                       push       edx
004c785c 6a00                     push       0
004c785e 8d45f0                   lea        eax, [ebp - 0x10]
004c7861 50                       push       eax
004c7862 e8293dfaff               call       0x46b590
004c7867 83c414                   add        esp, 0x14
004c786a 6a00                     push       0
004c786c 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c7872 e819a1f5ff               call       0x421990
004c7877 83f801                   cmp        eax, 1
004c787a 7d35                     jge        0x4c78b1
004c787c 6a00                     push       0
004c787e 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c7884 e807a1f5ff               call       0x421990
004c7889 8b0c8500fe5a00           mov        ecx, dword ptr [eax*4 + 0x5afe00]
004c7890 894dd0                   mov        dword ptr [ebp - 0x30], ecx
004c7893 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
004c7896 52                       push       edx
004c7897 68800e5700               push       0x570e80
004c789c 8d45f0                   lea        eax, [ebp - 0x10]
004c789f 50                       push       eax
004c78a0 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c78a6 51                       push       ecx
004c78a7 e8e450faff               call       0x46c990
004c78ac 83c410                   add        esp, 0x10
004c78af eb41                     jmp        0x4c78f2
004c78b1 6a00                     push       0
004c78b3 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
004c78b9 e8d2a0f5ff               call       0x421990
004c78be 8945cc                   mov        dword ptr [ebp - 0x34], eax
004c78c1 ba04000000               mov        edx, 4
004c78c6 c1e200                   shl        edx, 0
004c78c9 8b8200fe5a00             mov        eax, dword ptr [edx + 0x5afe00]
004c78cf 8945c8                   mov        dword ptr [ebp - 0x38], eax
004c78d2 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004c78d5 51                       push       ecx
004c78d6 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004c78d9 52                       push       edx
004c78da 68900e5700               push       0x570e90
004c78df 8d45f0                   lea        eax, [ebp - 0x10]
004c78e2 50                       push       eax
004c78e3 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c78e9 51                       push       ecx
004c78ea e8a150faff               call       0x46c990
004c78ef 83c414                   add        esp, 0x14
004c78f2 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
004c78f7 f30f580548fd5600         addss      xmm0, dword ptr [0x56fd48]
004c78ff f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
004c7904 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004c7907 8955f0                   mov        dword ptr [ebp - 0x10], edx
004c790a 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004c790d 8945f4                   mov        dword ptr [ebp - 0xc], eax
004c7910 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c7913 894df8                   mov        dword ptr [ebp - 8], ecx
004c7916 6aff                     push       -1
004c7918 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c791e e8ad91faff               call       0x470ad0
004c7923 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
004c7928 f30f580548fd5600         addss      xmm0, dword ptr [0x56fd48]
004c7930 f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
004c7935 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004c7938 8955f0                   mov        dword ptr [ebp - 0x10], edx
004c793b 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004c793e 8945f4                   mov        dword ptr [ebp - 0xc], eax
004c7941 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c7944 894df8                   mov        dword ptr [ebp - 8], ecx
004c7947 6aff                     push       -1
004c7949 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c794f e87c91faff               call       0x470ad0
004c7954 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c7957 81c1ec000000             add        ecx, 0xec
004c795d e82e86f4ff               call       0x40ff90
004c7962 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004c7965 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c7968 81c1d0000000             add        ecx, 0xd0
004c796e e81d86f4ff               call       0x40ff90
004c7973 8945c0                   mov        dword ptr [ebp - 0x40], eax
004c7976 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
004c7979 52                       push       edx
004c797a 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
004c797d 50                       push       eax
004c797e 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c7981 83c114                   add        ecx, 0x14
004c7984 51                       push       ecx
004c7985 6a01                     push       1
004c7987 8d55f0                   lea        edx, [ebp - 0x10]
004c798a 52                       push       edx
004c798b e8003cfaff               call       0x46b590
004c7990 83c414                   add        esp, 0x14
004c7993 68a00e5700               push       0x570ea0
004c7998 8d45f0                   lea        eax, [ebp - 0x10]
004c799b 50                       push       eax
004c799c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c79a2 51                       push       ecx
004c79a3 e8e84ffaff               call       0x46c990
004c79a8 83c40c                   add        esp, 0xc
004c79ab f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
004c79b0 f30f580548fd5600         addss      xmm0, dword ptr [0x56fd48]
004c79b8 f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
004c79bd 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004c79c0 8955f0                   mov        dword ptr [ebp - 0x10], edx
004c79c3 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004c79c6 8945f4                   mov        dword ptr [ebp - 0xc], eax
004c79c9 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c79cc 894df8                   mov        dword ptr [ebp - 8], ecx
004c79cf 6aff                     push       -1
004c79d1 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c79d7 e8f490faff               call       0x470ad0
004c79dc 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c79df 81c1ec000000             add        ecx, 0xec
004c79e5 e8a685f4ff               call       0x40ff90
004c79ea 8945bc                   mov        dword ptr [ebp - 0x44], eax
004c79ed 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c79f0 81c1d0000000             add        ecx, 0xd0
004c79f6 e89585f4ff               call       0x40ff90
004c79fb 8945b8                   mov        dword ptr [ebp - 0x48], eax
004c79fe 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004c7a01 52                       push       edx
004c7a02 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
004c7a05 50                       push       eax
004c7a06 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c7a09 83c114                   add        ecx, 0x14
004c7a0c 51                       push       ecx
004c7a0d 6a02                     push       2
004c7a0f 8d55f0                   lea        edx, [ebp - 0x10]
004c7a12 52                       push       edx
004c7a13 e8783bfaff               call       0x46b590
004c7a18 83c414                   add        esp, 0x14
004c7a1b 68b40e5700               push       0x570eb4
004c7a20 8d45f0                   lea        eax, [ebp - 0x10]
004c7a23 50                       push       eax
004c7a24 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7a2a 51                       push       ecx
004c7a2b e8604ffaff               call       0x46c990
004c7a30 83c40c                   add        esp, 0xc
004c7a33 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
004c7a38 f30f580528fa5600         addss      xmm0, dword ptr [0x56fa28]
004c7a40 f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
004c7a45 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004c7a48 8955f0                   mov        dword ptr [ebp - 0x10], edx
004c7a4b 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004c7a4e 8945f4                   mov        dword ptr [ebp - 0xc], eax
004c7a51 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c7a54 894df8                   mov        dword ptr [ebp - 8], ecx
004c7a57 68b0a0a0ff               push       0xffa0a0b0
004c7a5c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7a62 e86990faff               call       0x470ad0
004c7a67 68000000ff               push       0xff000000
004c7a6c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7a72 e83990faff               call       0x470ab0
004c7a77 6a00                     push       0
004c7a79 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7a7f e87c10fcff               call       0x488b00
004c7a84 6aff                     push       -1
004c7a86 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7a8c e83f90faff               call       0x470ad0
004c7a91 6a01                     push       1
004c7a93 6a01                     push       1
004c7a95 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7a9b e8800bffff               call       0x4b8620
004c7aa0 6a03                     push       3
004c7aa2 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7aa8 e8d3e6ffff               call       0x4c6180
004c7aad 6880c0c0ff               push       0xffc0c080
004c7ab2 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7ab8 e81390faff               call       0x470ad0
004c7abd 6aff                     push       -1
004c7abf 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7ac5 e896e6ffff               call       0x4c6160
004c7aca 68000000ff               push       0xff000000
004c7acf 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7ad5 e8d68ffaff               call       0x470ab0
004c7ada 6a00                     push       0
004c7adc 6a00                     push       0
004c7ade 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7ae4 e8370bffff               call       0x4b8620
004c7ae9 90                       nop        
004c7aea 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c7aed 83c114                   add        ecx, 0x14
004c7af0 e80b48f4ff               call       0x40c300
004c7af5 8945dc                   mov        dword ptr [ebp - 0x24], eax
004c7af8 837ddc00                 cmp        dword ptr [ebp - 0x24], 0
004c7afc 7415                     je         0x4c7b13
004c7afe 837ddc01                 cmp        dword ptr [ebp - 0x24], 1
004c7b02 747c                     je         0x4c7b80
004c7b04 837ddc02                 cmp        dword ptr [ebp - 0x24], 2
004c7b08 0f84db000000             je         0x4c7be9
004c7b0e e93e010000               jmp        0x4c7c51
004c7b13 51                       push       ecx
004c7b14 0f57c0                   xorps      xmm0, xmm0
004c7b17 f30f110424               movss      dword ptr [esp], xmm0
004c7b1c 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c7b1f f30f1082e4000000         movss      xmm0, dword ptr [edx + 0xe4]
004c7b27 f30f58050cf15600         addss      xmm0, dword ptr [0x56f10c]
004c7b2f 51                       push       ecx
004c7b30 f30f110424               movss      dword ptr [esp], xmm0
004c7b35 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c7b38 f30f1080e0000000         movss      xmm0, dword ptr [eax + 0xe0]
004c7b40 f30f580558105700         addss      xmm0, dword ptr [0x571058]
004c7b48 51                       push       ecx
004c7b49 f30f110424               movss      dword ptr [esp], xmm0
004c7b4e 8d4da0                   lea        ecx, [ebp - 0x60]
004c7b51 e87ab2f5ff               call       0x422dd0
004c7b56 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004c7b59 83ec08                   sub        esp, 8
004c7b5c 8bcc                     mov        ecx, esp
004c7b5e 68bc0e5700               push       0x570ebc
004c7b63 e86862f4ff               call       0x40ddd0
004c7b68 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004c7b6b 51                       push       ecx
004c7b6c 8b1598065c00             mov        edx, dword ptr [0x5c0698]
004c7b72 52                       push       edx
004c7b73 e89846faff               call       0x46c210
004c7b78 83c410                   add        esp, 0x10
004c7b7b e9d1000000               jmp        0x4c7c51
004c7b80 51                       push       ecx
004c7b81 0f57c0                   xorps      xmm0, xmm0
004c7b84 f30f110424               movss      dword ptr [esp], xmm0
004c7b89 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c7b8c f30f1080e4000000         movss      xmm0, dword ptr [eax + 0xe4]
004c7b94 f30f58059cec5600         addss      xmm0, dword ptr [0x56ec9c]
004c7b9c 51                       push       ecx
004c7b9d f30f110424               movss      dword ptr [esp], xmm0
004c7ba2 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c7ba5 f30f1081e0000000         movss      xmm0, dword ptr [ecx + 0xe0]
004c7bad f30f580558105700         addss      xmm0, dword ptr [0x571058]
004c7bb5 51                       push       ecx
004c7bb6 f30f110424               movss      dword ptr [esp], xmm0
004c7bbb 8d4d94                   lea        ecx, [ebp - 0x6c]
004c7bbe e80db2f5ff               call       0x422dd0
004c7bc3 8945b0                   mov        dword ptr [ebp - 0x50], eax
004c7bc6 83ec08                   sub        esp, 8
004c7bc9 8bcc                     mov        ecx, esp
004c7bcb 68dc0e5700               push       0x570edc
004c7bd0 e8fb61f4ff               call       0x40ddd0
004c7bd5 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
004c7bd8 52                       push       edx
004c7bd9 a198065c00               mov        eax, dword ptr [0x5c0698]
004c7bde 50                       push       eax
004c7bdf e82c46faff               call       0x46c210
004c7be4 83c410                   add        esp, 0x10
004c7be7 eb68                     jmp        0x4c7c51
004c7be9 51                       push       ecx
004c7bea 0f57c0                   xorps      xmm0, xmm0
004c7bed f30f110424               movss      dword ptr [esp], xmm0
004c7bf2 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c7bf5 f30f1081e4000000         movss      xmm0, dword ptr [ecx + 0xe4]
004c7bfd f30f58059cec5600         addss      xmm0, dword ptr [0x56ec9c]
004c7c05 51                       push       ecx
004c7c06 f30f110424               movss      dword ptr [esp], xmm0
004c7c0b 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c7c0e f30f1082e0000000         movss      xmm0, dword ptr [edx + 0xe0]
004c7c16 f30f580558105700         addss      xmm0, dword ptr [0x571058]
004c7c1e 51                       push       ecx
004c7c1f f30f110424               movss      dword ptr [esp], xmm0
004c7c24 8d4d88                   lea        ecx, [ebp - 0x78]
004c7c27 e8a4b1f5ff               call       0x422dd0
004c7c2c 8945ac                   mov        dword ptr [ebp - 0x54], eax
004c7c2f 83ec08                   sub        esp, 8
004c7c32 8bcc                     mov        ecx, esp
004c7c34 68fc0e5700               push       0x570efc
004c7c39 e89261f4ff               call       0x40ddd0
004c7c3e 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
004c7c41 50                       push       eax
004c7c42 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7c48 51                       push       ecx
004c7c49 e8c245faff               call       0x46c210
004c7c4e 83c410                   add        esp, 0x10
004c7c51 6aff                     push       -1
004c7c53 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7c59 e8728efaff               call       0x470ad0
004c7c5e 51                       push       ecx
004c7c5f f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004c7c67 f30f110424               movss      dword ptr [esp], xmm0
004c7c6c 51                       push       ecx
004c7c6d f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004c7c75 f30f110424               movss      dword ptr [esp], xmm0
004c7c7a 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7c80 e80b11ffff               call       0x4b8d90
004c7c85 6a01                     push       1
004c7c87 6a01                     push       1
004c7c89 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c7c8f e88c09ffff               call       0x4b8620
004c7c94 b801000000               mov        eax, 1
004c7c99 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c7c9c 33cd                     xor        ecx, ebp
004c7c9e e84bad0700               call       0x5429ee
004c7ca3 8be5                     mov        esp, ebp
004c7ca5 5d                       pop        ebp
004c7ca6 c3                       ret        
004c7ca7 cc                       int3       
004c7ca8 cc                       int3       
004c7ca9 cc                       int3       
004c7caa cc                       int3       
004c7cab cc                       int3       
004c7cac cc                       int3       
004c7cad cc                       int3       
004c7cae cc                       int3       
004c7caf cc                       int3       
