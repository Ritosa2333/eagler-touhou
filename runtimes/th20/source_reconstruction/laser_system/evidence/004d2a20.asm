004d2a20 55                       push       ebp
004d2a21 8bec                     mov        ebp, esp
004d2a23 81eca8000000             sub        esp, 0xa8
004d2a29 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004d2a2e 33c5                     xor        eax, ebp
004d2a30 8945fc                   mov        dword ptr [ebp - 4], eax
004d2a33 894dbc                   mov        dword ptr [ebp - 0x44], ecx
004d2a36 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004d2a39 8b88d8060000             mov        ecx, dword ptr [eax + 0x6d8]
004d2a3f 894db8                   mov        dword ptr [ebp - 0x48], ecx
004d2a42 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d2a45 81c270070000             add        edx, 0x770
004d2a4b 8955ac                   mov        dword ptr [ebp - 0x54], edx
004d2a4e 6a00                     push       0
004d2a50 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
004d2a53 e86839fbff               call       0x4863c0
004d2a58 90                       nop        
004d2a59 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004d2a5c 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004d2a5f c1e906                   shr        ecx, 6
004d2a62 83e101                   and        ecx, 1
004d2a65 7520                     jne        0x4d2a87
004d2a67 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d2a6a 8b02                     mov        eax, dword ptr [edx]
004d2a6c 8b4810                   mov        ecx, dword ptr [eax + 0x10]
004d2a6f 894da8                   mov        dword ptr [ebp - 0x58], ecx
004d2a72 6a00                     push       0
004d2a74 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d2a77 ff55a8                   call       dword ptr [ebp - 0x58]
004d2a7a 85c0                     test       eax, eax
004d2a7c 7507                     jne        0x4d2a85
004d2a7e 33c0                     xor        eax, eax
004d2a80 e92d030000               jmp        0x4d2db2
004d2a85 eb15                     jmp        0x4d2a9c
004d2a87 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
004d2a8a 8b4238                   mov        eax, dword ptr [edx + 0x38]
004d2a8d c1e803                   shr        eax, 3
004d2a90 83e001                   and        eax, 1
004d2a93 7507                     jne        0x4d2a9c
004d2a95 33c0                     xor        eax, eax
004d2a97 e916030000               jmp        0x4d2db2
004d2a9c 8d4df0                   lea        ecx, [ebp - 0x10]
004d2a9f e86c03f5ff               call       0x422e10
004d2aa4 8d4de4                   lea        ecx, [ebp - 0x1c]
004d2aa7 e86403f5ff               call       0x422e10
004d2aac 0f57c0                   xorps      xmm0, xmm0
004d2aaf f30f114594               movss      dword ptr [ebp - 0x6c], xmm0
004d2ab4 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004d2ab7 8b11                     mov        edx, dword ptr [ecx]
004d2ab9 8955f0                   mov        dword ptr [ebp - 0x10], edx
004d2abc 8b4104                   mov        eax, dword ptr [ecx + 4]
004d2abf 8945f4                   mov        dword ptr [ebp - 0xc], eax
004d2ac2 8b4908                   mov        ecx, dword ptr [ecx + 8]
004d2ac5 894df8                   mov        dword ptr [ebp - 8], ecx
004d2ac8 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d2acb 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004d2ad1 e8bad4f3ff               call       0x40ff90
004d2ad6 894598                   mov        dword ptr [ebp - 0x68], eax
004d2ad9 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
004d2adc f30f104018               movss      xmm0, dword ptr [eax + 0x18]
004d2ae1 f30f1145a4               movss      dword ptr [ebp - 0x5c], xmm0
004d2ae6 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004d2ae9 f30f10411c               movss      xmm0, dword ptr [ecx + 0x1c]
004d2aee f30f1145a0               movss      dword ptr [ebp - 0x60], xmm0
004d2af3 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
004d2af6 f30f104224               movss      xmm0, dword ptr [edx + 0x24]
004d2afb f30f11459c               movss      dword ptr [ebp - 0x64], xmm0
004d2b00 8b4508                   mov        eax, dword ptr [ebp + 8]
004d2b03 50                       push       eax
004d2b04 51                       push       ecx
004d2b05 f30f1045a4               movss      xmm0, dword ptr [ebp - 0x5c]
004d2b0a f30f110424               movss      dword ptr [esp], xmm0
004d2b0f 51                       push       ecx
004d2b10 f30f1045a0               movss      xmm0, dword ptr [ebp - 0x60]
004d2b15 f30f110424               movss      dword ptr [esp], xmm0
004d2b1a 51                       push       ecx
004d2b1b f30f10459c               movss      xmm0, dword ptr [ebp - 0x64]
004d2b20 f30f110424               movss      dword ptr [esp], xmm0
004d2b25 8d4df0                   lea        ecx, [ebp - 0x10]
004d2b28 51                       push       ecx
004d2b29 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
004d2b2c e89f660200               call       0x4f91d0
004d2b31 8945b0                   mov        dword ptr [ebp - 0x50], eax
004d2b34 837db002                 cmp        dword ptr [ebp - 0x50], 2
004d2b38 7543                     jne        0x4d2b7d
004d2b3a 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d2b3d 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004d2b43 e848d4f3ff               call       0x40ff90
004d2b48 8bc8                     mov        ecx, eax
004d2b4a e801ddf8ff               call       0x460850
004d2b4f 8b08                     mov        ecx, dword ptr [eax]
004d2b51 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004d2b54 8b5004                   mov        edx, dword ptr [eax + 4]
004d2b57 8955e8                   mov        dword ptr [ebp - 0x18], edx
004d2b5a 8b4008                   mov        eax, dword ptr [eax + 8]
004d2b5d 8945ec                   mov        dword ptr [ebp - 0x14], eax
004d2b60 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004d2b63 f30f10411c               movss      xmm0, dword ptr [ecx + 0x1c]
004d2b68 f30f5805d4ed5600         addss      xmm0, dword ptr [0x56edd4]
004d2b70 f30f580580f25600         addss      xmm0, dword ptr [0x56f280]
004d2b78 f30f114594               movss      dword ptr [ebp - 0x6c], xmm0
004d2b7d 837db001                 cmp        dword ptr [ebp - 0x50], 1
004d2b81 7571                     jne        0x4d2bf4
004d2b83 51                       push       ecx
004d2b84 0f57c0                   xorps      xmm0, xmm0
004d2b87 f30f110424               movss      dword ptr [esp], xmm0
004d2b8c 51                       push       ecx
004d2b8d f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
004d2b95 f30f110424               movss      dword ptr [esp], xmm0
004d2b9a 51                       push       ecx
004d2b9b f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
004d2ba3 f30f110424               movss      dword ptr [esp], xmm0
004d2ba8 8d4dc0                   lea        ecx, [ebp - 0x40]
004d2bab e82002f5ff               call       0x422dd0
004d2bb0 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d2bb3 8b02                     mov        eax, dword ptr [edx]
004d2bb5 8b482c                   mov        ecx, dword ptr [eax + 0x2c]
004d2bb8 894d8c                   mov        dword ptr [ebp - 0x74], ecx
004d2bbb 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d2bbe 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004d2bc4 e8c7d3f3ff               call       0x40ff90
004d2bc9 8bc8                     mov        ecx, eax
004d2bcb e880dcf8ff               call       0x460850
004d2bd0 894590                   mov        dword ptr [ebp - 0x70], eax
004d2bd3 6a01                     push       1
004d2bd5 6a00                     push       0
004d2bd7 51                       push       ecx
004d2bd8 0f57c0                   xorps      xmm0, xmm0
004d2bdb f30f110424               movss      dword ptr [esp], xmm0
004d2be0 8d45c0                   lea        eax, [ebp - 0x40]
004d2be3 50                       push       eax
004d2be4 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
004d2be7 51                       push       ecx
004d2be8 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d2beb ff558c                   call       dword ptr [ebp - 0x74]
004d2bee 90                       nop        
004d2bef e9bc010000               jmp        0x4d2db0
004d2bf4 837db002                 cmp        dword ptr [ebp - 0x50], 2
004d2bf8 0f85a5010000             jne        0x4d2da3
004d2bfe 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d2c01 8b82d4060000             mov        eax, dword ptr [edx + 0x6d4]
004d2c07 83c001                   add        eax, 1
004d2c0a 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d2c0d 8981d4060000             mov        dword ptr [ecx + 0x6d4], eax
004d2c13 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d2c16 83c238                   add        edx, 0x38
004d2c19 895588                   mov        dword ptr [ebp - 0x78], edx
004d2c1c 6a08                     push       8
004d2c1e 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004d2c21 e81af4f9ff               call       0x472040
004d2c26 85c0                     test       eax, eax
004d2c28 0f85cc000000             jne        0x4d2cfa
004d2c2e 8d4dcc                   lea        ecx, [ebp - 0x34]
004d2c31 e8da01f5ff               call       0x422e10
004d2c36 8d4dd8                   lea        ecx, [ebp - 0x28]
004d2c39 e8d201f5ff               call       0x422e10
004d2c3e 8d45cc                   lea        eax, [ebp - 0x34]
004d2c41 50                       push       eax
004d2c42 8d4de4                   lea        ecx, [ebp - 0x1c]
004d2c45 51                       push       ecx
004d2c46 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
004d2c49 51                       push       ecx
004d2c4a f30f104224               movss      xmm0, dword ptr [edx + 0x24]
004d2c4f f30f110424               movss      dword ptr [esp], xmm0
004d2c54 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
004d2c57 51                       push       ecx
004d2c58 f30f104018               movss      xmm0, dword ptr [eax + 0x18]
004d2c5d f30f110424               movss      dword ptr [esp], xmm0
004d2c62 8d4df0                   lea        ecx, [ebp - 0x10]
004d2c65 51                       push       ecx
004d2c66 e81521f8ff               call       0x454d80
004d2c6b ddd8                     fstp       st(0)
004d2c6d 83c414                   add        esp, 0x14
004d2c70 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d2c73 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004d2c79 e812d3f3ff               call       0x40ff90
004d2c7e 8bc8                     mov        ecx, eax
004d2c80 e8cbdbf8ff               call       0x460850
004d2c85 50                       push       eax
004d2c86 8d8564ffffff             lea        eax, [ebp - 0x9c]
004d2c8c 50                       push       eax
004d2c8d 8d4dcc                   lea        ecx, [ebp - 0x34]
004d2c90 e8db68f5ff               call       0x429570
004d2c95 894584                   mov        dword ptr [ebp - 0x7c], eax
004d2c98 51                       push       ecx
004d2c99 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
004d2ca1 f30f110424               movss      dword ptr [esp], xmm0
004d2ca6 8d8d58ffffff             lea        ecx, [ebp - 0xa8]
004d2cac 51                       push       ecx
004d2cad 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
004d2cb0 e83b03f8ff               call       0x452ff0
004d2cb5 8b10                     mov        edx, dword ptr [eax]
004d2cb7 8955d8                   mov        dword ptr [ebp - 0x28], edx
004d2cba 8b4804                   mov        ecx, dword ptr [eax + 4]
004d2cbd 894ddc                   mov        dword ptr [ebp - 0x24], ecx
004d2cc0 8b5008                   mov        edx, dword ptr [eax + 8]
004d2cc3 8955e0                   mov        dword ptr [ebp - 0x20], edx
004d2cc6 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
004d2cc9 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004d2ccf e8bcd2f3ff               call       0x40ff90
004d2cd4 89857cffffff             mov        dword ptr [ebp - 0x84], eax
004d2cda 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d2cdd 8b91d0060000             mov        edx, dword ptr [ecx + 0x6d0]
004d2ce3 895580                   mov        dword ptr [ebp - 0x80], edx
004d2ce6 8b4580                   mov        eax, dword ptr [ebp - 0x80]
004d2ce9 50                       push       eax
004d2cea 8d4dd8                   lea        ecx, [ebp - 0x28]
004d2ced 51                       push       ecx
004d2cee 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004d2cf4 e8975e0200               call       0x4f8b90
004d2cf9 90                       nop        
004d2cfa 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d2cfd 8b829c060000             mov        eax, dword ptr [edx + 0x69c]
004d2d03 d1e8                     shr        eax, 1
004d2d05 83e001                   and        eax, 1
004d2d08 757d                     jne        0x4d2d87
004d2d0a 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d2d0d 8b91d4060000             mov        edx, dword ptr [ecx + 0x6d4]
004d2d13 d1e2                     shl        edx, 1
004d2d15 b8d0000000               mov        eax, 0xd0
004d2d1a 2bc2                     sub        eax, edx
004d2d1c 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004d2d1f 817db4f0000000           cmp        dword ptr [ebp - 0x4c], 0xf0
004d2d26 7e07                     jle        0x4d2d2f
004d2d28 c745b4f0000000           mov        dword ptr [ebp - 0x4c], 0xf0
004d2d2f 837db460                 cmp        dword ptr [ebp - 0x4c], 0x60
004d2d33 7d07                     jge        0x4d2d3c
004d2d35 c745b460000000           mov        dword ptr [ebp - 0x4c], 0x60
004d2d3c 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d2d3f 81c170070000             add        ecx, 0x770
004d2d45 898d78ffffff             mov        dword ptr [ebp - 0x88], ecx
004d2d4b 6a01                     push       1
004d2d4d 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
004d2d53 e86836fbff               call       0x4863c0
004d2d58 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d2d5b 81c270070000             add        edx, 0x770
004d2d61 899574ffffff             mov        dword ptr [ebp - 0x8c], edx
004d2d67 68ff000000               push       0xff
004d2d6c 6880000000               push       0x80
004d2d71 0fb645b4                 movzx      eax, byte ptr [ebp - 0x4c]
004d2d75 50                       push       eax
004d2d76 68ff000000               push       0xff
004d2d7b 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
004d2d81 e82a37fbff               call       0x4864b0
004d2d86 90                       nop        
004d2d87 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004d2d8a 83c138                   add        ecx, 0x38
004d2d8d 898d70ffffff             mov        dword ptr [ebp - 0x90], ecx
004d2d93 6a00                     push       0
004d2d95 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
004d2d9b e8a007f5ff               call       0x423540
004d2da0 90                       nop        
004d2da1 eb0d                     jmp        0x4d2db0
004d2da3 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
004d2da6 c782d406000000000000     mov        dword ptr [edx + 0x6d4], 0
004d2db0 33c0                     xor        eax, eax
004d2db2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d2db5 33cd                     xor        ecx, ebp
004d2db7 e832fc0600               call       0x5429ee
004d2dbc 8be5                     mov        esp, ebp
004d2dbe 5d                       pop        ebp
004d2dbf c20400                   ret        4
004d2dc2 cc                       int3       
004d2dc3 cc                       int3       
004d2dc4 cc                       int3       
004d2dc5 cc                       int3       
004d2dc6 cc                       int3       
004d2dc7 cc                       int3       
004d2dc8 cc                       int3       
004d2dc9 cc                       int3       
004d2dca cc                       int3       
004d2dcb cc                       int3       
004d2dcc cc                       int3       
004d2dcd cc                       int3       
004d2dce cc                       int3       
004d2dcf cc                       int3       
