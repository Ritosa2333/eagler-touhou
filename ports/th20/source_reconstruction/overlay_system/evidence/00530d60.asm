00530d60 55                       push       ebp
00530d61 8bec                     mov        ebp, esp
00530d63 83ec0c                   sub        esp, 0xc
00530d66 894df4                   mov        dword ptr [ebp - 0xc], ecx
00530d69 8b4508                   mov        eax, dword ptr [ebp + 8]
00530d6c c7801c010000800e5300     mov        dword ptr [eax + 0x11c], 0x530e80
00530d76 6a00                     push       0
00530d78 e80333f3ff               call       0x464080
00530d7d 83c404                   add        esp, 4
00530d80 8bc8                     mov        ecx, eax
00530d82 e81974f8ff               call       0x4b81a0
00530d87 85c0                     test       eax, eax
00530d89 7455                     je         0x530de0
00530d8b f30f2a450c               cvtsi2ss   xmm0, dword ptr [ebp + 0xc]
00530d90 f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
00530d98 f30f590dd0c85600         mulss      xmm1, dword ptr [0x56c8d0]
00530da0 6a00                     push       0
00530da2 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00530da7 f30f114dfc               movss      dword ptr [ebp - 4], xmm1
00530dac e8cf32f3ff               call       0x464080
00530db1 83c404                   add        esp, 4
00530db4 8bc8                     mov        ecx, eax
00530db6 e8e573f8ff               call       0x4b81a0
00530dbb f30f2ac0                 cvtsi2ss   xmm0, eax
00530dbf f30f104dfc               movss      xmm1, dword ptr [ebp - 4]
00530dc4 f30f5ec8                 divss      xmm1, xmm0
00530dc8 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00530dcd f30f59c1                 mulss      xmm0, xmm1
00530dd1 51                       push       ecx
00530dd2 f30f110424               movss      dword ptr [esp], xmm0
00530dd7 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00530dda e8e1f7ffff               call       0x5305c0
00530ddf 90                       nop        
00530de0 8be5                     mov        esp, ebp
00530de2 5d                       pop        ebp
00530de3 c20800                   ret        8
00530de6 cc                       int3       
00530de7 cc                       int3       
00530de8 cc                       int3       
00530de9 cc                       int3       
00530dea cc                       int3       
00530deb cc                       int3       
00530dec cc                       int3       
00530ded cc                       int3       
00530dee cc                       int3       
00530def cc                       int3       
