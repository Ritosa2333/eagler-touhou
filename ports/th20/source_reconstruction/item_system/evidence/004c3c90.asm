004c3c90 55                       push       ebp
004c3c91 8bec                     mov        ebp, esp
004c3c93 83ec54                   sub        esp, 0x54
004c3c96 894df8                   mov        dword ptr [ebp - 8], ecx
004c3c99 837d0801                 cmp        dword ptr [ebp + 8], 1
004c3c9d 0f853c010000             jne        0x4c3ddf
004c3ca3 6a00                     push       0
004c3ca5 e8d603faff               call       0x464080
004c3caa 83c404                   add        esp, 4
004c3cad 8bc8                     mov        ecx, eax
004c3caf e89c120000               call       0x4c4f50
004c3cb4 0fb6c0                   movzx      eax, al
004c3cb7 85c0                     test       eax, eax
004c3cb9 0f8420010000             je         0x4c3ddf
004c3cbf 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c3cc2 8b9178c84900             mov        edx, dword ptr [ecx + 0x49c878]
004c3cc8 83c201                   add        edx, 1
004c3ccb 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c3cce 899078c84900             mov        dword ptr [eax + 0x49c878], edx
004c3cd4 c745f00a000000           mov        dword ptr [ebp - 0x10], 0xa
004c3cdb 6a00                     push       0
004c3cdd e89e03faff               call       0x464080
004c3ce2 83c404                   add        esp, 4
004c3ce5 8bc8                     mov        ecx, eax
004c3ce7 e81407faff               call       0x464400
004c3cec 0fb6c8                   movzx      ecx, al
004c3cef 85c9                     test       ecx, ecx
004c3cf1 7407                     je         0x4c3cfa
004c3cf3 c745f00e000000           mov        dword ptr [ebp - 0x10], 0xe
004c3cfa 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c3cfd 8b8278c84900             mov        eax, dword ptr [edx + 0x49c878]
004c3d03 99                       cdq        
004c3d04 f77df0                   idiv       dword ptr [ebp - 0x10]
004c3d07 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c3d0a 83e801                   sub        eax, 1
004c3d0d 3bd0                     cmp        edx, eax
004c3d0f 0f85ca000000             jne        0x4c3ddf
004c3d15 6a00                     push       0
004c3d17 e81405faff               call       0x464230
004c3d1c 83c404                   add        esp, 4
004c3d1f 8bc8                     mov        ecx, eax
004c3d21 e8aa1cfcff               call       0x4859d0
004c3d26 8945ec                   mov        dword ptr [ebp - 0x14], eax
004c3d29 6a01                     push       1
004c3d2b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c3d2e e87d28fcff               call       0x4865b0
004c3d33 90                       nop        
004c3d34 b9a8a45b00               mov        ecx, 0x5ba4a8
004c3d39 e8a25bf6ff               call       0x4298e0
004c3d3e d95de8                   fstp       dword ptr [ebp - 0x18]
004c3d41 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
004c3d46 f30f590590cd5600         mulss      xmm0, dword ptr [0x56cd90]
004c3d4e 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004c3d51 f30f584104               addss      xmm0, dword ptr [ecx + 4]
004c3d56 f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
004c3d5b b9a8a45b00               mov        ecx, 0x5ba4a8
004c3d60 e87b5bf6ff               call       0x4298e0
004c3d65 d95de4                   fstp       dword ptr [ebp - 0x1c]
004c3d68 f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
004c3d6d f30f590590cd5600         mulss      xmm0, dword ptr [0x56cd90]
004c3d75 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004c3d78 f30f5802                 addss      xmm0, dword ptr [edx]
004c3d7c f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
004c3d81 8b4524                   mov        eax, dword ptr [ebp + 0x24]
004c3d84 50                       push       eax
004c3d85 8b4d20                   mov        ecx, dword ptr [ebp + 0x20]
004c3d88 51                       push       ecx
004c3d89 8b551c                   mov        edx, dword ptr [ebp + 0x1c]
004c3d8c 83c210                   add        edx, 0x10
004c3d8f 52                       push       edx
004c3d90 51                       push       ecx
004c3d91 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
004c3d96 f30f110424               movss      dword ptr [esp], xmm0
004c3d9b 51                       push       ecx
004c3d9c f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004c3da1 f30f110424               movss      dword ptr [esp], xmm0
004c3da6 8b4510                   mov        eax, dword ptr [ebp + 0x10]
004c3da9 50                       push       eax
004c3daa 51                       push       ecx
004c3dab 0f57c0                   xorps      xmm0, xmm0
004c3dae f30f110424               movss      dword ptr [esp], xmm0
004c3db3 51                       push       ecx
004c3db4 f30f1045e0               movss      xmm0, dword ptr [ebp - 0x20]
004c3db9 f30f110424               movss      dword ptr [esp], xmm0
004c3dbe 51                       push       ecx
004c3dbf f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
004c3dc4 f30f110424               movss      dword ptr [esp], xmm0
004c3dc9 8d4dac                   lea        ecx, [ebp - 0x54]
004c3dcc e8ffeff5ff               call       0x422dd0
004c3dd1 50                       push       eax
004c3dd2 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c3dd5 51                       push       ecx
004c3dd6 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c3dd9 e8b2feffff               call       0x4c3c90
004c3dde 90                       nop        
004c3ddf 837d0810                 cmp        dword ptr [ebp + 8], 0x10
004c3de3 7c07                     jl         0x4c3dec
004c3de5 33c0                     xor        eax, eax
004c3de7 e9c6040000               jmp        0x4c42b2
004c3dec 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c3def 8b8268c84900             mov        eax, dword ptr [edx + 0x49c868]
004c3df5 83c001                   add        eax, 1
004c3df8 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c3dfb 898168c84900             mov        dword ptr [ecx + 0x49c868], eax
004c3e01 837d0809                 cmp        dword ptr [ebp + 8], 9
004c3e05 741c                     je         0x4c3e23
004c3e07 837d080a                 cmp        dword ptr [ebp + 8], 0xa
004c3e0b 7416                     je         0x4c3e23
004c3e0d 837d080b                 cmp        dword ptr [ebp + 8], 0xb
004c3e11 7410                     je         0x4c3e23
004c3e13 837d080c                 cmp        dword ptr [ebp + 8], 0xc
004c3e17 740a                     je         0x4c3e23
004c3e19 837d080d                 cmp        dword ptr [ebp + 8], 0xd
004c3e1d 0f85f6010000             jne        0x4c4019
004c3e23 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c3e26 81c148c84900             add        ecx, 0x49c848
004c3e2c e8bf1bfcff               call       0x4859f0
004c3e31 8945f4                   mov        dword ptr [ebp - 0xc], eax
004c3e34 837df400                 cmp        dword ptr [ebp - 0xc], 0
004c3e38 0f84ce010000             je         0x4c400c
004c3e3e 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004c3e41 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c3e44 8b8874c84900             mov        ecx, dword ptr [eax + 0x49c874]
004c3e4a 898a380c0000             mov        dword ptr [edx + 0xc38], ecx
004c3e50 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c3e53 e888def4ff               call       0x411ce0
004c3e58 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c3e5b 81c218c84900             add        edx, 0x49c818
004c3e61 8955d8                   mov        dword ptr [ebp - 0x28], edx
004c3e64 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004c3e67 50                       push       eax
004c3e68 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c3e6b e84068f8ff               call       0x44a6b0
004c3e70 90                       nop        
004c3e71 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c3e74 81b970c8490000040000     cmp        dword ptr [ecx + 0x49c870], 0x400
004c3e7e 7c26                     jl         0x4c3ea6
004c3e80 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c3e83 8b8268c84900             mov        eax, dword ptr [edx + 0x49c868]
004c3e89 251f000080               and        eax, 0x8000001f
004c3e8e 7905                     jns        0x4c3e95
004c3e90 48                       dec        eax
004c3e91 83c8e0                   or         eax, 0xffffffe0
004c3e94 40                       inc        eax
004c3e95 83c010                   add        eax, 0x10
004c3e98 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c3e9b 8981340c0000             mov        dword ptr [ecx + 0xc34], eax
004c3ea1 e985000000               jmp        0x4c3f2b
004c3ea6 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c3ea9 81ba70c8490000020000     cmp        dword ptr [edx + 0x49c870], 0x200
004c3eb3 7c24                     jl         0x4c3ed9
004c3eb5 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c3eb8 8b8868c84900             mov        ecx, dword ptr [eax + 0x49c868]
004c3ebe 81e10f000080             and        ecx, 0x8000000f
004c3ec4 7905                     jns        0x4c3ecb
004c3ec6 49                       dec        ecx
004c3ec7 83c9f0                   or         ecx, 0xfffffff0
004c3eca 41                       inc        ecx
004c3ecb 83c108                   add        ecx, 8
004c3ece 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004c3ed1 898a340c0000             mov        dword ptr [edx + 0xc34], ecx
004c3ed7 eb52                     jmp        0x4c3f2b
004c3ed9 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c3edc 81b870c8490000010000     cmp        dword ptr [eax + 0x49c870], 0x100
004c3ee6 7c24                     jl         0x4c3f0c
004c3ee8 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c3eeb 8b9168c84900             mov        edx, dword ptr [ecx + 0x49c868]
004c3ef1 81e207000080             and        edx, 0x80000007
004c3ef7 7905                     jns        0x4c3efe
004c3ef9 4a                       dec        edx
004c3efa 83caf8                   or         edx, 0xfffffff8
004c3efd 42                       inc        edx
004c3efe 83c204                   add        edx, 4
004c3f01 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004c3f04 8990340c0000             mov        dword ptr [eax + 0xc34], edx
004c3f0a eb1f                     jmp        0x4c3f2b
004c3f0c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c3f0f 8b9168c84900             mov        edx, dword ptr [ecx + 0x49c868]
004c3f15 81e203000080             and        edx, 0x80000003
004c3f1b 7905                     jns        0x4c3f22
004c3f1d 4a                       dec        edx
004c3f1e 83cafc                   or         edx, 0xfffffffc
004c3f21 42                       inc        edx
004c3f22 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004c3f25 8990340c0000             mov        dword ptr [eax + 0xc34], edx
004c3f2b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c3f2e c781240c000005000000     mov        dword ptr [ecx + 0xc24], 5
004c3f38 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004c3f3b 8b4508                   mov        eax, dword ptr [ebp + 8]
004c3f3e 8982280c0000             mov        dword ptr [edx + 0xc28], eax
004c3f44 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c3f47 c7812c0c000000000000     mov        dword ptr [ecx + 0xc2c], 0
004c3f51 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004c3f54 81c2e40b0000             add        edx, 0xbe4
004c3f5a 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004c3f5d 8b08                     mov        ecx, dword ptr [eax]
004c3f5f 890a                     mov        dword ptr [edx], ecx
004c3f61 8b4804                   mov        ecx, dword ptr [eax + 4]
004c3f64 894a04                   mov        dword ptr [edx + 4], ecx
004c3f67 8b4008                   mov        eax, dword ptr [eax + 8]
004c3f6a 894208                   mov        dword ptr [edx + 8], eax
004c3f6d 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c3f70 0f57c0                   xorps      xmm0, xmm0
004c3f73 f30f1181ec0b0000         movss      dword ptr [ecx + 0xbec], xmm0
004c3f7b 51                       push       ecx
004c3f7c f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
004c3f81 f30f110424               movss      dword ptr [esp], xmm0
004c3f86 51                       push       ecx
004c3f87 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004c3f8c f30f110424               movss      dword ptr [esp], xmm0
004c3f91 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004c3f94 81c2f00b0000             add        edx, 0xbf0
004c3f9a 52                       push       edx
004c3f9b e89053f7ff               call       0x439330
004c3fa0 83c40c                   add        esp, 0xc
004c3fa3 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004c3fa6 0f57c0                   xorps      xmm0, xmm0
004c3fa9 f30f1180f80b0000         movss      dword ptr [eax + 0xbf8], xmm0
004c3fb1 6a00                     push       0
004c3fb3 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c3fb6 81c1040c0000             add        ecx, 0xc04
004c3fbc e85ff5f5ff               call       0x423520
004c3fc1 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c3fc4 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004c3fc9 f30f1181000c0000         movss      dword ptr [ecx + 0xc00], xmm0
004c3fd1 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004c3fd4 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
004c3fd9 f30f1182fc0b0000         movss      dword ptr [edx + 0xbfc], xmm0
004c3fe1 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004c3fe4 8b4d20                   mov        ecx, dword ptr [ebp + 0x20]
004c3fe7 89883c0c0000             mov        dword ptr [eax + 0xc3c], ecx
004c3fed 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004c3ff0 8b4524                   mov        eax, dword ptr [ebp + 0x24]
004c3ff3 8982400c0000             mov        dword ptr [edx + 0xc40], eax
004c3ff9 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c3ffc 8b9194c84900             mov        edx, dword ptr [ecx + 0x49c894]
004c4002 52                       push       edx
004c4003 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c4006 e8850f0000               call       0x4c4f90
004c400b 90                       nop        
004c400c 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004c400f e99e020000               jmp        0x4c42b2
004c4014 e999020000               jmp        0x4c42b2
004c4019 837d080f                 cmp        dword ptr [ebp + 8], 0xf
004c401d 7554                     jne        0x4c4073
004c401f e81c4ffdff               call       0x498f40
004c4024 8bc8                     mov        ecx, eax
004c4026 e89500faff               call       0x4640c0
004c402b 83c001                   add        eax, 1
004c402e 8945d0                   mov        dword ptr [ebp - 0x30], eax
004c4031 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c4034 8b886cc84900             mov        ecx, dword ptr [eax + 0x49c86c]
004c403a 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
004c403d 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
004c4040 0355d0                   add        edx, dword ptr [ebp - 0x30]
004c4043 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c4046 89906cc84900             mov        dword ptr [eax + 0x49c86c], edx
004c404c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c404f 83b96cc849000a           cmp        dword ptr [ecx + 0x49c86c], 0xa
004c4056 7d07                     jge        0x4c405f
004c4058 33c0                     xor        eax, eax
004c405a e953020000               jmp        0x4c42b2
004c405f 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c4062 c7826cc8490000000000     mov        dword ptr [edx + 0x49c86c], 0
004c406c c7450802000000           mov        dword ptr [ebp + 8], 2
004c4073 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c4076 81c130c84900             add        ecx, 0x49c830
004c407c e86f19fcff               call       0x4859f0
004c4081 8bc8                     mov        ecx, eax
004c4083 e87882f4ff               call       0x40c300
004c4088 8945fc                   mov        dword ptr [ebp - 4], eax
004c408b 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c408f 0f841a020000             je         0x4c42af
004c4095 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4098 e843dcf4ff               call       0x411ce0
004c409d 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c40a0 0518c84900               add        eax, 0x49c818
004c40a5 8945cc                   mov        dword ptr [ebp - 0x34], eax
004c40a8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c40ab 51                       push       ecx
004c40ac 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004c40af e8fc65f8ff               call       0x44a6b0
004c40b4 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c40b7 c782240c000001000000     mov        dword ptr [edx + 0xc24], 1
004c40c1 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c40c4 05e40b0000               add        eax, 0xbe4
004c40c9 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004c40cc 8b11                     mov        edx, dword ptr [ecx]
004c40ce 8910                     mov        dword ptr [eax], edx
004c40d0 8b5104                   mov        edx, dword ptr [ecx + 4]
004c40d3 895004                   mov        dword ptr [eax + 4], edx
004c40d6 8b4908                   mov        ecx, dword ptr [ecx + 8]
004c40d9 894808                   mov        dword ptr [eax + 8], ecx
004c40dc 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c40df 0f57c0                   xorps      xmm0, xmm0
004c40e2 f30f1182ec0b0000         movss      dword ptr [edx + 0xbec], xmm0
004c40ea 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c40ed f30f10056cfa5600         movss      xmm0, dword ptr [0x56fa6c]
004c40f5 0f2f80e40b0000           comiss     xmm0, dword ptr [eax + 0xbe4]
004c40fc 7215                     jb         0x4c4113
004c40fe 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4101 f30f10056cfa5600         movss      xmm0, dword ptr [0x56fa6c]
004c4109 f30f1181e40b0000         movss      dword ptr [ecx + 0xbe4], xmm0
004c4111 eb27                     jmp        0x4c413a
004c4113 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c4116 f30f1082e40b0000         movss      xmm0, dword ptr [edx + 0xbe4]
004c411e 0f2f0550e05600           comiss     xmm0, dword ptr [0x56e050]
004c4125 7213                     jb         0x4c413a
004c4127 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c412a f30f100550e05600         movss      xmm0, dword ptr [0x56e050]
004c4132 f30f1180e40b0000         movss      dword ptr [eax + 0xbe4], xmm0
004c413a 837d080e                 cmp        dword ptr [ebp + 8], 0xe
004c413e 7507                     jne        0x4c4147
004c4140 c7450806000000           mov        dword ptr [ebp + 8], 6
004c4147 51                       push       ecx
004c4148 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
004c414d f30f110424               movss      dword ptr [esp], xmm0
004c4152 51                       push       ecx
004c4153 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004c4158 f30f110424               movss      dword ptr [esp], xmm0
004c415d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4160 81c1f00b0000             add        ecx, 0xbf0
004c4166 51                       push       ecx
004c4167 e8c451f7ff               call       0x439330
004c416c 83c40c                   add        esp, 0xc
004c416f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c4172 0f57c0                   xorps      xmm0, xmm0
004c4175 f30f1182f80b0000         movss      dword ptr [edx + 0xbf8], xmm0
004c417d 6a00                     push       0
004c417f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4182 81c1040c0000             add        ecx, 0xc04
004c4188 e893f3f5ff               call       0x423520
004c418d 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4190 0f57c0                   xorps      xmm0, xmm0
004c4193 f30f1180fc0b0000         movss      dword ptr [eax + 0xbfc], xmm0
004c419b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c419e 0f57c0                   xorps      xmm0, xmm0
004c41a1 f30f1181300c0000         movss      dword ptr [ecx + 0xc30], xmm0
004c41a9 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c41ac 8b451c                   mov        eax, dword ptr [ebp + 0x1c]
004c41af 8982340c0000             mov        dword ptr [edx + 0xc34], eax
004c41b5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c41b8 8b5508                   mov        edx, dword ptr [ebp + 8]
004c41bb 8991280c0000             mov        dword ptr [ecx + 0xc28], edx
004c41c1 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c41c4 8b4d24                   mov        ecx, dword ptr [ebp + 0x24]
004c41c7 8988400c0000             mov        dword ptr [eax + 0xc40], ecx
004c41cd 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c41d0 8b8294c84900             mov        eax, dword ptr [edx + 0x49c894]
004c41d6 50                       push       eax
004c41d7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c41da e8b10d0000               call       0x4c4f90
004c41df 90                       nop        
004c41e0 837d1c00                 cmp        dword ptr [ebp + 0x1c], 0
004c41e4 7509                     jne        0x4c41ef
004c41e6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c41e9 e8d2000000               call       0x4c42c0
004c41ee 90                       nop        
004c41ef 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c41f2 c7812c0c000000000000     mov        dword ptr [ecx + 0xc2c], 0
004c41fc 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c41ff 8b8a98c84900             mov        ecx, dword ptr [edx + 0x49c898]
004c4205 e8f680f4ff               call       0x40c300
004c420a 8b808c6d2800             mov        eax, dword ptr [eax + 0x286d8c]
004c4210 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004c4213 b904000000               mov        ecx, 4
004c4218 6bd100                   imul       edx, ecx, 0
004c421b 8b4508                   mov        eax, dword ptr [ebp + 8]
004c421e 8b8cc250fd5a00           mov        ecx, dword ptr [edx + eax*8 + 0x5afd50]
004c4225 894dc8                   mov        dword ptr [ebp - 0x38], ecx
004c4228 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004c422b 52                       push       edx
004c422c 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c422f 83c018                   add        eax, 0x18
004c4232 50                       push       eax
004c4233 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004c4236 e84541f7ff               call       0x438380
004c423b 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c423e 8b8998c84900             mov        ecx, dword ptr [ecx + 0x49c898]
004c4244 e8b780f4ff               call       0x40c300
004c4249 8b908c6d2800             mov        edx, dword ptr [eax + 0x286d8c]
004c424f 8955bc                   mov        dword ptr [ebp - 0x44], edx
004c4252 b804000000               mov        eax, 4
004c4257 c1e000                   shl        eax, 0
004c425a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c425d 8b94c850fd5a00           mov        edx, dword ptr [eax + ecx*8 + 0x5afd50]
004c4264 8955c0                   mov        dword ptr [ebp - 0x40], edx
004c4267 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
004c426a 50                       push       eax
004c426b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c426e 81c1fc050000             add        ecx, 0x5fc
004c4274 51                       push       ecx
004c4275 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004c4278 e80341f7ff               call       0x438380
004c427d 6a00                     push       0
004c427f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4282 81c1e00b0000             add        ecx, 0xbe0
004c4288 e813d5f4ff               call       0x4117a0
004c428d 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c4290 83c218                   add        edx, 0x18
004c4293 8955b8                   mov        dword ptr [ebp - 0x48], edx
004c4296 8b4510                   mov        eax, dword ptr [ebp + 0x10]
004c4299 50                       push       eax
004c429a 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004c429d e86eb7f8ff               call       0x44fa10
004c42a2 90                       nop        
004c42a3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c42a6 8b5520                   mov        edx, dword ptr [ebp + 0x20]
004c42a9 89913c0c0000             mov        dword ptr [ecx + 0xc3c], edx
004c42af 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c42b2 8be5                     mov        esp, ebp
004c42b4 5d                       pop        ebp
004c42b5 c22000                   ret        0x20
004c42b8 cc                       int3       
004c42b9 cc                       int3       
004c42ba cc                       int3       
004c42bb cc                       int3       
004c42bc cc                       int3       
004c42bd cc                       int3       
004c42be cc                       int3       
004c42bf cc                       int3       
