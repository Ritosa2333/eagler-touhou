004c45b0 55                       push       ebp
004c45b1 8bec                     mov        ebp, esp
004c45b3 83ec08                   sub        esp, 8
004c45b6 894df8                   mov        dword ptr [ebp - 8], ecx
004c45b9 837d0c00                 cmp        dword ptr [ebp + 0xc], 0
004c45bd 7e7e                     jle        0x4c463d
004c45bf 6aff                     push       -1
004c45c1 6a00                     push       0
004c45c3 6a00                     push       0
004c45c5 51                       push       ecx
004c45c6 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
004c45ce f30f110424               movss      dword ptr [esp], xmm0
004c45d3 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
004c45db f30f5e05b00d5700         divss      xmm0, dword ptr [0x570db0]
004c45e3 f30f590534e75600         mulss      xmm0, dword ptr [0x56e734]
004c45eb 51                       push       ecx
004c45ec f30f110424               movss      dword ptr [esp], xmm0
004c45f1 b9a8a45b00               mov        ecx, 0x5ba4a8
004c45f6 e8b552f6ff               call       0x4298b0
004c45fb d95dfc                   fstp       dword ptr [ebp - 4]
004c45fe f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
004c4603 f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
004c460b f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
004c4613 f30f5cc1                 subss      xmm0, xmm1
004c4617 51                       push       ecx
004c4618 f30f110424               movss      dword ptr [esp], xmm0
004c461d 6aff                     push       -1
004c461f 8b4508                   mov        eax, dword ptr [ebp + 8]
004c4622 50                       push       eax
004c4623 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
004c4626 51                       push       ecx
004c4627 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c462a e861f6ffff               call       0x4c3c90
004c462f 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004c4632 83ea01                   sub        edx, 1
004c4635 89550c                   mov        dword ptr [ebp + 0xc], edx
004c4638 e97cffffff               jmp        0x4c45b9
004c463d 8be5                     mov        esp, ebp
004c463f 5d                       pop        ebp
004c4640 c20c00                   ret        0xc
004c4643 cc                       int3       
004c4644 cc                       int3       
004c4645 cc                       int3       
004c4646 cc                       int3       
004c4647 cc                       int3       
004c4648 cc                       int3       
004c4649 cc                       int3       
004c464a cc                       int3       
004c464b cc                       int3       
004c464c cc                       int3       
004c464d cc                       int3       
004c464e cc                       int3       
004c464f cc                       int3       
