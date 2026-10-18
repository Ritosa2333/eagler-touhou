0048b650 55                       push       ebp
0048b651 8bec                     mov        ebp, esp
0048b653 51                       push       ecx
0048b654 894dfc                   mov        dword ptr [ebp - 4], ecx
0048b657 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048b65a e8b177f9ff               call       0x422e10
0048b65f 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048b662 0f57c0                   xorps      xmm0, xmm0
0048b665 f30f11400c               movss      dword ptr [eax + 0xc], xmm0
0048b66a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048b66d 0f57c0                   xorps      xmm0, xmm0
0048b670 f30f114110               movss      dword ptr [ecx + 0x10], xmm0
0048b675 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048b678 0f57c0                   xorps      xmm0, xmm0
0048b67b f30f114214               movss      dword ptr [edx + 0x14], xmm0
0048b680 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048b683 c7401800000000           mov        dword ptr [eax + 0x18], 0
0048b68a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048b68d c7411c00000000           mov        dword ptr [ecx + 0x1c], 0
0048b694 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048b697 c7422000000000           mov        dword ptr [edx + 0x20], 0
0048b69e 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048b6a1 0f57c0                   xorps      xmm0, xmm0
0048b6a4 f30f114024               movss      dword ptr [eax + 0x24], xmm0
0048b6a9 33c9                     xor        ecx, ecx
0048b6ab 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048b6ae 83c228                   add        edx, 0x28
0048b6b1 890a                     mov        dword ptr [edx], ecx
0048b6b3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048b6b6 83c12c                   add        ecx, 0x2c
0048b6b9 e8028bf8ff               call       0x4141c0
0048b6be 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048b6c1 c7403c00000000           mov        dword ptr [eax + 0x3c], 0
0048b6c8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048b6cb c7414000000000           mov        dword ptr [ecx + 0x40], 0
0048b6d2 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048b6d5 c7424400000000           mov        dword ptr [edx + 0x44], 0
0048b6dc 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048b6df c7404800000000           mov        dword ptr [eax + 0x48], 0
0048b6e6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048b6e9 0f57c0                   xorps      xmm0, xmm0
0048b6ec f30f11414c               movss      dword ptr [ecx + 0x4c], xmm0
0048b6f1 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048b6f4 c7425000000000           mov        dword ptr [edx + 0x50], 0
0048b6fb 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048b6fe c7405400000000           mov        dword ptr [eax + 0x54], 0
0048b705 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048b708 c7415800000000           mov        dword ptr [ecx + 0x58], 0
0048b70f 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048b712 8be5                     mov        esp, ebp
0048b714 5d                       pop        ebp
0048b715 c3                       ret        
0048b716 cc                       int3       
0048b717 cc                       int3       
0048b718 cc                       int3       
0048b719 cc                       int3       
0048b71a cc                       int3       
0048b71b cc                       int3       
0048b71c cc                       int3       
0048b71d cc                       int3       
0048b71e cc                       int3       
0048b71f cc                       int3       
