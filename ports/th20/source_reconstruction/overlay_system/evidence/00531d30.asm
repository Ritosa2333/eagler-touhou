00531d30 55                       push       ebp
00531d31 8bec                     mov        ebp, esp
00531d33 83ec10                   sub        esp, 0x10
00531d36 894df0                   mov        dword ptr [ebp - 0x10], ecx
00531d39 8b4508                   mov        eax, dword ptr [ebp + 8]
00531d3c 8b88c8050000             mov        ecx, dword ptr [eax + 0x5c8]
00531d42 894dfc                   mov        dword ptr [ebp - 4], ecx
00531d45 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531d48 83c160                   add        ecx, 0x60
00531d4b e8c083efff               call       0x42a110
00531d50 8b55fc                   mov        edx, dword ptr [ebp - 4]
00531d53 d95a5c                   fstp       dword ptr [edx + 0x5c]
00531d56 e805250000               call       0x534260
00531d5b f30f2ac0                 cvtsi2ss   xmm0, eax
00531d5f f30f100d585d5700         movss      xmm1, dword ptr [0x575d58]
00531d67 f30f5ec8                 divss      xmm1, xmm0
00531d6b f30f114df4               movss      dword ptr [ebp - 0xc], xmm1
00531d70 8b45fc                   mov        eax, dword ptr [ebp - 4]
00531d73 f30f108090000000         movss      xmm0, dword ptr [eax + 0x90]
00531d7b f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00531d80 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00531d85 f30f5c45f4               subss      xmm0, dword ptr [ebp - 0xc]
00531d8a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531d8d f30f118190000000         movss      dword ptr [ecx + 0x90], xmm0
00531d95 8b55fc                   mov        edx, dword ptr [ebp - 4]
00531d98 81c28c000000             add        edx, 0x8c
00531d9e 52                       push       edx
00531d9f 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00531da2 e8b9bff2ff               call       0x45dd60
00531da7 51                       push       ecx
00531da8 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00531db0 f30f110424               movss      dword ptr [esp], xmm0
00531db5 8b45fc                   mov        eax, dword ptr [ebp - 4]
00531db8 51                       push       ecx
00531db9 f30f10405c               movss      xmm0, dword ptr [eax + 0x5c]
00531dbe f30f110424               movss      dword ptr [esp], xmm0
00531dc3 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00531dc6 e885f0f3ff               call       0x470e50
00531dcb 33c0                     xor        eax, eax
00531dcd 8be5                     mov        esp, ebp
00531dcf 5d                       pop        ebp
00531dd0 c20400                   ret        4
00531dd3 cc                       int3       
00531dd4 cc                       int3       
00531dd5 cc                       int3       
00531dd6 cc                       int3       
00531dd7 cc                       int3       
00531dd8 cc                       int3       
00531dd9 cc                       int3       
00531dda cc                       int3       
00531ddb cc                       int3       
00531ddc cc                       int3       
00531ddd cc                       int3       
00531dde cc                       int3       
00531ddf cc                       int3       
