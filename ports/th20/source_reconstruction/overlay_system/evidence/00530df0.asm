00530df0 55                       push       ebp
00530df1 8bec                     mov        ebp, esp
00530df3 83ec0c                   sub        esp, 0xc
00530df6 894df4                   mov        dword ptr [ebp - 0xc], ecx
00530df9 8b4508                   mov        eax, dword ptr [ebp + 8]
00530dfc c78020010000800e5300     mov        dword ptr [eax + 0x120], 0x530e80
00530e06 6a00                     push       0
00530e08 e87332f3ff               call       0x464080
00530e0d 83c404                   add        esp, 4
00530e10 8bc8                     mov        ecx, eax
00530e12 e88973f8ff               call       0x4b81a0
00530e17 85c0                     test       eax, eax
00530e19 7455                     je         0x530e70
00530e1b f30f2a450c               cvtsi2ss   xmm0, dword ptr [ebp + 0xc]
00530e20 f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
00530e28 f30f590dd0c85600         mulss      xmm1, dword ptr [0x56c8d0]
00530e30 6a00                     push       0
00530e32 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00530e37 f30f114dfc               movss      dword ptr [ebp - 4], xmm1
00530e3c e83f32f3ff               call       0x464080
00530e41 83c404                   add        esp, 4
00530e44 8bc8                     mov        ecx, eax
00530e46 e85573f8ff               call       0x4b81a0
00530e4b f30f2ac0                 cvtsi2ss   xmm0, eax
00530e4f f30f104dfc               movss      xmm1, dword ptr [ebp - 4]
00530e54 f30f5ec8                 divss      xmm1, xmm0
00530e58 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00530e5d f30f59c1                 mulss      xmm0, xmm1
00530e61 51                       push       ecx
00530e62 f30f110424               movss      dword ptr [esp], xmm0
00530e67 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00530e6a e851f7ffff               call       0x5305c0
00530e6f 90                       nop        
00530e70 8be5                     mov        esp, ebp
00530e72 5d                       pop        ebp
00530e73 c20800                   ret        8
00530e76 cc                       int3       
00530e77 cc                       int3       
00530e78 cc                       int3       
00530e79 cc                       int3       
00530e7a cc                       int3       
00530e7b cc                       int3       
00530e7c cc                       int3       
00530e7d cc                       int3       
00530e7e cc                       int3       
00530e7f cc                       int3       
