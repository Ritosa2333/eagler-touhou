0052fee0 55                       push       ebp
0052fee1 8bec                     mov        ebp, esp
0052fee3 83ec0c                   sub        esp, 0xc
0052fee6 894df4                   mov        dword ptr [ebp - 0xc], ecx
0052fee9 8b4508                   mov        eax, dword ptr [ebp + 8]
0052feec c7802001000010005300     mov        dword ptr [eax + 0x120], 0x530010
0052fef6 6a00                     push       0
0052fef8 e88341f3ff               call       0x464080
0052fefd 83c404                   add        esp, 4
0052ff00 8bc8                     mov        ecx, eax
0052ff02 e89982f8ff               call       0x4b81a0
0052ff07 85c0                     test       eax, eax
0052ff09 7455                     je         0x52ff60
0052ff0b f30f2a450c               cvtsi2ss   xmm0, dword ptr [ebp + 0xc]
0052ff10 f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
0052ff18 f30f590dd0c85600         mulss      xmm1, dword ptr [0x56c8d0]
0052ff20 6a00                     push       0
0052ff22 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
0052ff27 f30f114dfc               movss      dword ptr [ebp - 4], xmm1
0052ff2c e84f41f3ff               call       0x464080
0052ff31 83c404                   add        esp, 4
0052ff34 8bc8                     mov        ecx, eax
0052ff36 e86582f8ff               call       0x4b81a0
0052ff3b f30f2ac0                 cvtsi2ss   xmm0, eax
0052ff3f f30f104dfc               movss      xmm1, dword ptr [ebp - 4]
0052ff44 f30f5ec8                 divss      xmm1, xmm0
0052ff48 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
0052ff4d f30f59c1                 mulss      xmm0, xmm1
0052ff51 51                       push       ecx
0052ff52 f30f110424               movss      dword ptr [esp], xmm0
0052ff57 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052ff5a e861060000               call       0x5305c0
0052ff5f 90                       nop        
0052ff60 8be5                     mov        esp, ebp
0052ff62 5d                       pop        ebp
0052ff63 c20800                   ret        8
0052ff66 cc                       int3       
0052ff67 cc                       int3       
0052ff68 cc                       int3       
0052ff69 cc                       int3       
0052ff6a cc                       int3       
0052ff6b cc                       int3       
0052ff6c cc                       int3       
0052ff6d cc                       int3       
0052ff6e cc                       int3       
0052ff6f cc                       int3       
