004d5e50 55                       push       ebp
004d5e51 8bec                     mov        ebp, esp
004d5e53 83ec2c                   sub        esp, 0x2c
004d5e56 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004d5e5b 33c5                     xor        eax, ebp
004d5e5d 8945fc                   mov        dword ptr [ebp - 4], eax
004d5e60 56                       push       esi
004d5e61 57                       push       edi
004d5e62 894dec                   mov        dword ptr [ebp - 0x14], ecx
004d5e65 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d5e68 8b88d8060000             mov        ecx, dword ptr [eax + 0x6d8]
004d5e6e 894de8                   mov        dword ptr [ebp - 0x18], ecx
004d5e71 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
004d5e78 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d5e7b 8b421c                   mov        eax, dword ptr [edx + 0x1c]
004d5e7e 83c840                   or         eax, 0x40
004d5e81 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d5e84 89411c                   mov        dword ptr [ecx + 0x1c], eax
004d5e87 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d5e8a 8b4238                   mov        eax, dword ptr [edx + 0x38]
004d5e8d 83e0f7                   and        eax, 0xfffffff7
004d5e90 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004d5e93 894138                   mov        dword ptr [ecx + 0x38], eax
004d5e96 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d5e99 837a2004                 cmp        dword ptr [edx + 0x20], 4
004d5e9d 740d                     je         0x4d5eac
004d5e9f 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d5ea2 83782002                 cmp        dword ptr [eax + 0x20], 2
004d5ea6 0f85d2010000             jne        0x4d607e
004d5eac 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d5eaf f30f104174               movss      xmm0, dword ptr [ecx + 0x74]
004d5eb4 0f2f0590cd5600           comiss     xmm0, dword ptr [0x56cd90]
004d5ebb 0f86bd010000             jbe        0x4d607e
004d5ec1 8d4df0                   lea        ecx, [ebp - 0x10]
004d5ec4 e847cff4ff               call       0x422e10
004d5ec9 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d5ecc f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
004d5ed4 0f2f4278                 comiss     xmm0, dword ptr [edx + 0x78]
004d5ed8 7617                     jbe        0x4d5ef1
004d5eda 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d5edd f30f104078               movss      xmm0, dword ptr [eax + 0x78]
004d5ee2 f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
004d5eea f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
004d5eef eb29                     jmp        0x4d5f1a
004d5ef1 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d5ef4 f30f104178               movss      xmm0, dword ptr [ecx + 0x78]
004d5ef9 f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
004d5f01 f30f5e05d8c85600         divss      xmm0, dword ptr [0x56c8d8]
004d5f09 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d5f0c f30f104a78               movss      xmm1, dword ptr [edx + 0x78]
004d5f11 f30f5cc8                 subss      xmm1, xmm0
004d5f15 f30f114de0               movss      dword ptr [ebp - 0x20], xmm1
004d5f1a f30f1045e0               movss      xmm0, dword ptr [ebp - 0x20]
004d5f1f f30f1145d8               movss      dword ptr [ebp - 0x28], xmm0
004d5f24 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d5f27 f30f104074               movss      xmm0, dword ptr [eax + 0x74]
004d5f2c f30f59058c175700         mulss      xmm0, dword ptr [0x57178c]
004d5f34 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
004d5f39 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d5f3c 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
004d5f3f c1ea07                   shr        edx, 7
004d5f42 83e201                   and        edx, 1
004d5f45 743f                     je         0x4d5f86
004d5f47 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d5f4a f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
004d5f52 0f2f4078                 comiss     xmm0, dword ptr [eax + 0x78]
004d5f56 760f                     jbe        0x4d5f67
004d5f58 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
004d5f60 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
004d5f65 eb0d                     jmp        0x4d5f74
004d5f67 f30f1005e0c85600         movss      xmm0, dword ptr [0x56c8e0]
004d5f6f f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
004d5f74 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d5f77 f30f104178               movss      xmm0, dword ptr [ecx + 0x78]
004d5f7c f30f5c45dc               subss      xmm0, dword ptr [ebp - 0x24]
004d5f81 f30f1145d8               movss      dword ptr [ebp - 0x28], xmm0
004d5f86 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d5f89 f30f104274               movss      xmm0, dword ptr [edx + 0x74]
004d5f8e f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004d5f96 51                       push       ecx
004d5f97 f30f110424               movss      dword ptr [esp], xmm0
004d5f9c 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d5f9f 51                       push       ecx
004d5fa0 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004d5fa5 f30f110424               movss      dword ptr [esp], xmm0
004d5faa 8d4df0                   lea        ecx, [ebp - 0x10]
004d5fad 51                       push       ecx
004d5fae e87d33f6ff               call       0x439330
004d5fb3 83c40c                   add        esp, 0xc
004d5fb6 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d5fb9 83c258                   add        edx, 0x58
004d5fbc 52                       push       edx
004d5fbd 8d4df0                   lea        ecx, [ebp - 0x10]
004d5fc0 e81b37f5ff               call       0x4296e0
004d5fc5 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004d5fc8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004d5fcb 8908                     mov        dword ptr [eax], ecx
004d5fcd 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004d5fd0 895004                   mov        dword ptr [eax + 4], edx
004d5fd3 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004d5fd6 894808                   mov        dword ptr [eax + 8], ecx
004d5fd9 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d5fdc 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d5fdf 8b4870                   mov        ecx, dword ptr [eax + 0x70]
004d5fe2 894a24                   mov        dword ptr [edx + 0x24], ecx
004d5fe5 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d5fe8 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d5feb 8b4870                   mov        ecx, dword ptr [eax + 0x70]
004d5fee 894a28                   mov        dword ptr [edx + 0x28], ecx
004d5ff1 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d5ff4 8b4238                   mov        eax, dword ptr [edx + 0x38]
004d5ff7 83e0fe                   and        eax, 0xfffffffe
004d5ffa 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004d5ffd 894138                   mov        dword ptr [ecx + 0x38], eax
004d6000 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d6003 8b4238                   mov        eax, dword ptr [edx + 0x38]
004d6006 83e0fd                   and        eax, 0xfffffffd
004d6009 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004d600c 894138                   mov        dword ptr [ecx + 0x38], eax
004d600f 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d6012 8b4238                   mov        eax, dword ptr [edx + 0x38]
004d6015 83c804                   or         eax, 4
004d6018 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004d601b 894138                   mov        dword ptr [ecx + 0x38], eax
004d601e 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d6021 8b4238                   mov        eax, dword ptr [edx + 0x38]
004d6024 83c808                   or         eax, 8
004d6027 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004d602a 894138                   mov        dword ptr [ecx + 0x38], eax
004d602d 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004d6030 0f57c0                   xorps      xmm0, xmm0
004d6033 f30f11422c               movss      dword ptr [edx + 0x2c], xmm0
004d6038 f30f1045d4               movss      xmm0, dword ptr [ebp - 0x2c]
004d603d f30f5905ccc85600         mulss      xmm0, dword ptr [0x56c8cc]
004d6045 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004d6048 f30f114018               movss      dword ptr [eax + 0x18], xmm0
004d604d f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
004d6052 f30f5905ccc85600         mulss      xmm0, dword ptr [0x56c8cc]
004d605a 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004d605d f30f11411c               movss      dword ptr [ecx + 0x1c], xmm0
004d6062 837d0800                 cmp        dword ptr [ebp + 8], 0
004d6066 740d                     je         0x4d6075
004d6068 b90f000000               mov        ecx, 0xf
004d606d 8b75e8                   mov        esi, dword ptr [ebp - 0x18]
004d6070 8b7d08                   mov        edi, dword ptr [ebp + 8]
004d6073 f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
004d6075 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004d6078 83c201                   add        edx, 1
004d607b 8955e4                   mov        dword ptr [ebp - 0x1c], edx
004d607e 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004d6081 5f                       pop        edi
004d6082 5e                       pop        esi
004d6083 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d6086 33cd                     xor        ecx, ebp
004d6088 e861c90600               call       0x5429ee
004d608d 8be5                     mov        esp, ebp
004d608f 5d                       pop        ebp
004d6090 c20400                   ret        4
004d6093 cc                       int3       
004d6094 cc                       int3       
004d6095 cc                       int3       
004d6096 cc                       int3       
004d6097 cc                       int3       
004d6098 cc                       int3       
004d6099 cc                       int3       
004d609a cc                       int3       
004d609b cc                       int3       
004d609c cc                       int3       
004d609d cc                       int3       
004d609e cc                       int3       
004d609f cc                       int3       
