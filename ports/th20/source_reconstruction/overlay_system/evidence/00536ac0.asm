00536ac0 55                       push       ebp
00536ac1 8bec                     mov        ebp, esp
00536ac3 83ec4c                   sub        esp, 0x4c
00536ac6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00536acb 33c5                     xor        eax, ebp
00536acd 8945fc                   mov        dword ptr [ebp - 4], eax
00536ad0 894dec                   mov        dword ptr [ebp - 0x14], ecx
00536ad3 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00536ad6 05e4000000               add        eax, 0xe4
00536adb 8945e8                   mov        dword ptr [ebp - 0x18], eax
00536ade 6a00                     push       0
00536ae0 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00536ae3 e8f81af4ff               call       0x4785e0
00536ae8 0fb6c8                   movzx      ecx, al
00536aeb 85c9                     test       ecx, ecx
00536aed 741d                     je         0x536b0c
00536aef b9a8a45b00               mov        ecx, 0x5ba4a8
00536af4 e8d72cefff               call       0x4297d0
00536af9 51                       push       ecx
00536afa d91c24                   fstp       dword ptr [esp]
00536afd 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00536b00 81c1d4000000             add        ecx, 0xd4
00536b06 e815c4f1ff               call       0x452f20
00536b0b 90                       nop        
00536b0c 8d4df0                   lea        ecx, [ebp - 0x10]
00536b0f e8fcc2eeff               call       0x422e10
00536b14 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00536b17 81c1d4000000             add        ecx, 0xd4
00536b1d e8be27efff               call       0x4292e0
00536b22 51                       push       ecx
00536b23 d91c24                   fstp       dword ptr [esp]
00536b26 e8052af0ff               call       0x439530
00536b2b 83c404                   add        esp, 4
00536b2e d95de4                   fstp       dword ptr [ebp - 0x1c]
00536b31 f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00536b36 f30f590560055700         mulss      xmm0, dword ptr [0x570560]
00536b3e f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00536b43 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00536b46 81c2d4000000             add        edx, 0xd4
00536b4c 8955e0                   mov        dword ptr [ebp - 0x20], edx
00536b4f 51                       push       ecx
00536b50 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
00536b58 f30f110424               movss      dword ptr [esp], xmm0
00536b5d 8d45c4                   lea        eax, [ebp - 0x3c]
00536b60 50                       push       eax
00536b61 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00536b64 e82728efff               call       0x429390
00536b69 8bc8                     mov        ecx, eax
00536b6b e87027efff               call       0x4292e0
00536b70 51                       push       ecx
00536b71 d91c24                   fstp       dword ptr [esp]
00536b74 e8b729f0ff               call       0x439530
00536b79 83c404                   add        esp, 4
00536b7c d95ddc                   fstp       dword ptr [ebp - 0x24]
00536b7f f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00536b84 f30f5905bcd75600         mulss      xmm0, dword ptr [0x56d7bc]
00536b8c f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00536b91 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00536b94 8b8928010000             mov        ecx, dword ptr [ecx + 0x128]
00536b9a e8f193edff               call       0x40ff90
00536b9f 8bc8                     mov        ecx, eax
00536ba1 e8ea8afcff               call       0x4ff690
00536ba6 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00536bab f30f590598cd5600         mulss      xmm0, dword ptr [0x56cd98]
00536bb3 f30f2cd0                 cvttss2si  edx, xmm0
00536bb7 0310                     add        edx, dword ptr [eax]
00536bb9 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00536bbc 895070                   mov        dword ptr [eax + 0x70], edx
00536bbf 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00536bc2 8b8928010000             mov        ecx, dword ptr [ecx + 0x128]
00536bc8 e8c393edff               call       0x40ff90
00536bcd 8bc8                     mov        ecx, eax
00536bcf e8bc8afcff               call       0x4ff690
00536bd4 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00536bd9 f30f590598cd5600         mulss      xmm0, dword ptr [0x56cd98]
00536be1 f30f2cd0                 cvttss2si  edx, xmm0
00536be5 035004                   add        edx, dword ptr [eax + 4]
00536be8 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00536beb 895074                   mov        dword ptr [eax + 0x74], edx
00536bee 6a00                     push       0
00536bf0 e83b9cf2ff               call       0x460830
00536bf5 83c404                   add        esp, 4
00536bf8 8bc8                     mov        ecx, eax
00536bfa e8e189fcff               call       0x4ff5e0
00536bff 0fb6c8                   movzx      ecx, al
00536c02 85c9                     test       ecx, ecx
00536c04 0f8591000000             jne        0x536c9b
00536c0a ba08000000               mov        edx, 8
00536c0f 6bc200                   imul       eax, edx, 0
00536c12 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00536c15 8d940180000000           lea        edx, [ecx + eax + 0x80]
00536c1c 52                       push       edx
00536c1d 8d45bc                   lea        eax, [ebp - 0x44]
00536c20 50                       push       eax
00536c21 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00536c24 83c170                   add        ecx, 0x70
00536c27 e8c404fcff               call       0x4f70f0
00536c2c 90                       nop        
00536c2d f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00536c35 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
00536c3d f30f5e05b0cd5600         divss      xmm0, dword ptr [0x56cdb0]
00536c45 b9a8a45b00               mov        ecx, 0x5ba4a8
00536c4a f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
00536c4f e8dc2befff               call       0x429830
00536c54 d95dd8                   fstp       dword ptr [ebp - 0x28]
00536c57 f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
00536c5c f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
00536c64 f30f590dd0c85600         mulss      xmm1, dword ptr [0x56c8d0]
00536c6c f30f59c1                 mulss      xmm0, xmm1
00536c70 f30f5e05b0cd5600         divss      xmm0, dword ptr [0x56cdb0]
00536c78 f30f104dd4               movss      xmm1, dword ptr [ebp - 0x2c]
00536c7d f30f58c8                 addss      xmm1, xmm0
00536c81 51                       push       ecx
00536c82 f30f110c24               movss      dword ptr [esp], xmm1
00536c87 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00536c8a 81c1d4000000             add        ecx, 0xd4
00536c90 e82bc4f1ff               call       0x4530c0
00536c95 90                       nop        
00536c96 e98c000000               jmp        0x536d27
00536c9b b908000000               mov        ecx, 8
00536ca0 c1e100                   shl        ecx, 0
00536ca3 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00536ca6 8d840a80000000           lea        eax, [edx + ecx + 0x80]
00536cad 50                       push       eax
00536cae 8d4db4                   lea        ecx, [ebp - 0x4c]
00536cb1 51                       push       ecx
00536cb2 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00536cb5 83c170                   add        ecx, 0x70
00536cb8 e83304fcff               call       0x4f70f0
00536cbd 90                       nop        
00536cbe f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00536cc6 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
00536cce f30f5e0598565700         divss      xmm0, dword ptr [0x575698]
00536cd6 b9a8a45b00               mov        ecx, 0x5ba4a8
00536cdb f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
00536ce0 e84b2befff               call       0x429830
00536ce5 d95dd0                   fstp       dword ptr [ebp - 0x30]
00536ce8 f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
00536ced f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
00536cf5 f30f590dd0c85600         mulss      xmm1, dword ptr [0x56c8d0]
00536cfd f30f59c1                 mulss      xmm0, xmm1
00536d01 f30f5e05b0cd5600         divss      xmm0, dword ptr [0x56cdb0]
00536d09 f30f104dcc               movss      xmm1, dword ptr [ebp - 0x34]
00536d0e f30f58c8                 addss      xmm1, xmm0
00536d12 51                       push       ecx
00536d13 f30f110c24               movss      dword ptr [esp], xmm1
00536d18 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00536d1b 81c1d4000000             add        ecx, 0xd4
00536d21 e89ac3f1ff               call       0x4530c0
00536d26 90                       nop        
00536d27 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00536d2a 81c2e4000000             add        edx, 0xe4
00536d30 8955c8                   mov        dword ptr [ebp - 0x38], edx
00536d33 6a00                     push       0
00536d35 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00536d38 e803c8eeff               call       0x423540
00536d3d 90                       nop        
00536d3e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00536d41 33cd                     xor        ecx, ebp
00536d43 e8a6bc0000               call       0x5429ee
00536d48 8be5                     mov        esp, ebp
00536d4a 5d                       pop        ebp
00536d4b c3                       ret        
00536d4c cc                       int3       
00536d4d cc                       int3       
00536d4e cc                       int3       
00536d4f cc                       int3       
