004c4b90 55                       push       ebp
004c4b91 8bec                     mov        ebp, esp
004c4b93 83ec28                   sub        esp, 0x28
004c4b96 53                       push       ebx
004c4b97 56                       push       esi
004c4b98 57                       push       edi
004c4b99 894df8                   mov        dword ptr [ebp - 8], ecx
004c4b9c 6a00                     push       0
004c4b9e e88dbcf9ff               call       0x460830
004c4ba3 83c404                   add        esp, 4
004c4ba6 8bc8                     mov        ecx, eax
004c4ba8 e8a3bcf9ff               call       0x460850
004c4bad f30f104004               movss      xmm0, dword ptr [eax + 4]
004c4bb2 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004c4bb7 6a00                     push       0
004c4bb9 e8c2f4f9ff               call       0x464080
004c4bbe 83c404                   add        esp, 4
004c4bc1 8bc8                     mov        ecx, eax
004c4bc3 e8e847fdff               call       0x4993b0
004c4bc8 8bf0                     mov        esi, eax
004c4bca 6a00                     push       0
004c4bcc e8aff4f9ff               call       0x464080
004c4bd1 83c404                   add        esp, 4
004c4bd4 8bc8                     mov        ecx, eax
004c4bd6 e83536ffff               call       0x4b8210
004c4bdb 3bf0                     cmp        esi, eax
004c4bdd 0f8c21010000             jl         0x4c4d04
004c4be3 6a00                     push       0
004c4be5 e846bcf9ff               call       0x460830
004c4bea 83c404                   add        esp, 4
004c4bed 8bc8                     mov        ecx, eax
004c4bef e85c34ffff               call       0x4b8050
004c4bf4 d95df0                   fstp       dword ptr [ebp - 0x10]
004c4bf7 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004c4bfc 0f2f45f4                 comiss     xmm0, dword ptr [ebp - 0xc]
004c4c00 730c                     jae        0x4c4c0e
004c4c02 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c4c05 83b8240c000003           cmp        dword ptr [eax + 0xc24], 3
004c4c0c 7539                     jne        0x4c4c47
004c4c0e 6a00                     push       0
004c4c10 e86bf4f9ff               call       0x464080
004c4c15 83c404                   add        esp, 4
004c4c18 8bc8                     mov        ecx, eax
004c4c1a e811ffffff               call       0x4c4b30
004c4c1f 8945fc                   mov        dword ptr [ebp - 4], eax
004c4c22 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4c25 99                       cdq        
004c4c26 b90a000000               mov        ecx, 0xa
004c4c2b f7f9                     idiv       ecx
004c4c2d 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4c30 2bc2                     sub        eax, edx
004c4c32 8945fc                   mov        dword ptr [ebp - 4], eax
004c4c35 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c4c39 7f07                     jg         0x4c4c42
004c4c3b c745fc0a000000           mov        dword ptr [ebp - 4], 0xa
004c4c42 e992000000               jmp        0x4c4cd9
004c4c47 6a00                     push       0
004c4c49 e832f4f9ff               call       0x464080
004c4c4e 83c404                   add        esp, 4
004c4c51 8bc8                     mov        ecx, eax
004c4c53 e8d8feffff               call       0x4c4b30
004c4c58 6bc009                   imul       eax, eax, 9
004c4c5b 99                       cdq        
004c4c5c b90a000000               mov        ecx, 0xa
004c4c61 f7f9                     idiv       ecx
004c4c63 8bf0                     mov        esi, eax
004c4c65 6a00                     push       0
004c4c67 e814f4f9ff               call       0x464080
004c4c6c 83c404                   add        esp, 4
004c4c6f 8bc8                     mov        ecx, eax
004c4c71 e8bafeffff               call       0x4c4b30
004c4c76 6bc009                   imul       eax, eax, 9
004c4c79 99                       cdq        
004c4c7a b90a000000               mov        ecx, 0xa
004c4c7f f7f9                     idiv       ecx
004c4c81 8bf8                     mov        edi, eax
004c4c83 f30f2c5df4               cvttss2si  ebx, dword ptr [ebp - 0xc]
004c4c88 6a00                     push       0
004c4c8a e8a1bbf9ff               call       0x460830
004c4c8f 83c404                   add        esp, 4
004c4c92 8bc8                     mov        ecx, eax
004c4c94 e8b733ffff               call       0x4b8050
004c4c99 d95dec                   fstp       dword ptr [ebp - 0x14]
004c4c9c f30f1045ec               movss      xmm0, dword ptr [ebp - 0x14]
004c4ca1 f30f2cd0                 cvttss2si  edx, xmm0
004c4ca5 2bda                     sub        ebx, edx
004c4ca7 0faffb                   imul       edi, ebx
004c4caa 8bc7                     mov        eax, edi
004c4cac 99                       cdq        
004c4cad b9c2010000               mov        ecx, 0x1c2
004c4cb2 f7f9                     idiv       ecx
004c4cb4 2bf0                     sub        esi, eax
004c4cb6 8975fc                   mov        dword ptr [ebp - 4], esi
004c4cb9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4cbc 99                       cdq        
004c4cbd b90a000000               mov        ecx, 0xa
004c4cc2 f7f9                     idiv       ecx
004c4cc4 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4cc7 2bc2                     sub        eax, edx
004c4cc9 8945fc                   mov        dword ptr [ebp - 4], eax
004c4ccc 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c4cd0 7f07                     jg         0x4c4cd9
004c4cd2 c745fc0a000000           mov        dword ptr [ebp - 4], 0xa
004c4cd9 6a00                     push       0
004c4cdb e8c088ffff               call       0x4bd5a0
004c4ce0 83c404                   add        esp, 4
004c4ce3 8945e8                   mov        dword ptr [ebp - 0x18], eax
004c4ce6 6aff                     push       -1
004c4ce8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4ceb 51                       push       ecx
004c4cec 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c4cef 81c2e40b0000             add        edx, 0xbe4
004c4cf5 52                       push       edx
004c4cf6 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004c4cf9 e812ba0400               call       0x510710
004c4cfe 90                       nop        
004c4cff e981000000               jmp        0x4c4d85
004c4d04 c745fc64000000           mov        dword ptr [ebp - 4], 0x64
004c4d0b 6a00                     push       0
004c4d0d e86ef3f9ff               call       0x464080
004c4d12 83c404                   add        esp, 4
004c4d15 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004c4d18 6a01                     push       1
004c4d1a 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004c4d1d e8eec60100               call       0x4e1410
004c4d22 8945e0                   mov        dword ptr [ebp - 0x20], eax
004c4d25 837de000                 cmp        dword ptr [ebp - 0x20], 0
004c4d29 745a                     je         0x4c4d85
004c4d2b 6a00                     push       0
004c4d2d e8febaf9ff               call       0x460830
004c4d32 83c404                   add        esp, 4
004c4d35 8945dc                   mov        dword ptr [ebp - 0x24], eax
004c4d38 6aff                     push       -1
004c4d3a 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004c4d3d e85e5f0300               call       0x4faca0
004c4d42 6a00                     push       0
004c4d44 e85788ffff               call       0x4bd5a0
004c4d49 83c404                   add        esp, 4
004c4d4c 8945d8                   mov        dword ptr [ebp - 0x28], eax
004c4d4f 6840ffffff               push       0xffffff40
004c4d54 6aff                     push       -1
004c4d56 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c4d59 05e40b0000               add        eax, 0xbe4
004c4d5e 50                       push       eax
004c4d5f 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c4d62 e8a9b90400               call       0x510710
004c4d67 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c4d6a 51                       push       ecx
004c4d6b f30f1081e40b0000         movss      xmm0, dword ptr [ecx + 0xbe4]
004c4d73 f30f110424               movss      dword ptr [esp], xmm0
004c4d78 6a0d                     push       0xd
004c4d7a b930a85b00               mov        ecx, 0x5ba830
004c4d7f e82c21f6ff               call       0x426eb0
004c4d84 90                       nop        
004c4d85 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c4d88 52                       push       edx
004c4d89 b968a55b00               mov        ecx, 0x5ba568
004c4d8e e8bd37fcff               call       0x488550
004c4d93 90                       nop        
004c4d94 5f                       pop        edi
004c4d95 5e                       pop        esi
004c4d96 5b                       pop        ebx
004c4d97 8be5                     mov        esp, ebp
004c4d99 5d                       pop        ebp
004c4d9a c3                       ret        
004c4d9b cc                       int3       
004c4d9c cc                       int3       
004c4d9d cc                       int3       
004c4d9e cc                       int3       
004c4d9f cc                       int3       
