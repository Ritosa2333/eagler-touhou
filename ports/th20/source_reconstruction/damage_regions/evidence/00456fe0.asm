00456fe0 55                       push       ebp
00456fe1 8bec                     mov        ebp, esp
00456fe3 51                       push       ecx
00456fe4 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00456fe9 f30f5c4508               subss      xmm0, dword ptr [ebp + 8]
00456fee f30f104d10               movss      xmm1, dword ptr [ebp + 0x10]
00456ff3 f30f5c4d08               subss      xmm1, dword ptr [ebp + 8]
00456ff8 f30f59c1                 mulss      xmm0, xmm1
00456ffc f30f104d14               movss      xmm1, dword ptr [ebp + 0x14]
00457001 f30f5c4d0c               subss      xmm1, dword ptr [ebp + 0xc]
00457006 f30f105514               movss      xmm2, dword ptr [ebp + 0x14]
0045700b f30f5c550c               subss      xmm2, dword ptr [ebp + 0xc]
00457010 f30f59ca                 mulss      xmm1, xmm2
00457014 f30f58c1                 addss      xmm0, xmm1
00457018 f30f104d18               movss      xmm1, dword ptr [ebp + 0x18]
0045701d f30f594d18               mulss      xmm1, dword ptr [ebp + 0x18]
00457022 0f2fc8                   comiss     xmm1, xmm0
00457025 7209                     jb         0x457030
00457027 c745fc01000000           mov        dword ptr [ebp - 4], 1
0045702e eb07                     jmp        0x457037
00457030 c745fc00000000           mov        dword ptr [ebp - 4], 0
00457037 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
0045703b 8be5                     mov        esp, ebp
0045703d 5d                       pop        ebp
0045703e c3                       ret        
0045703f cc                       int3       
