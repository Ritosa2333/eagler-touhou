0047c030 55                       push       ebp
0047c031 8bec                     mov        ebp, esp
0047c033 51                       push       ecx
0047c034 894dfc                   mov        dword ptr [ebp - 4], ecx
0047c037 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c03a e8d16dfaff               call       0x422e10
0047c03f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c042 83c10c                   add        ecx, 0xc
0047c045 e8c66dfaff               call       0x422e10
0047c04a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047c04d 0f57c0                   xorps      xmm0, xmm0
0047c050 f30f114018               movss      dword ptr [eax + 0x18], xmm0
0047c055 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c058 0f57c0                   xorps      xmm0, xmm0
0047c05b f30f11411c               movss      dword ptr [ecx + 0x1c], xmm0
0047c060 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047c063 0f57c0                   xorps      xmm0, xmm0
0047c066 f30f114220               movss      dword ptr [edx + 0x20], xmm0
0047c06b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047c06e 0f57c0                   xorps      xmm0, xmm0
0047c071 f30f114024               movss      dword ptr [eax + 0x24], xmm0
0047c076 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c079 0f57c0                   xorps      xmm0, xmm0
0047c07c f30f114128               movss      dword ptr [ecx + 0x28], xmm0
0047c081 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047c084 f30f1005bcd75600         movss      xmm0, dword ptr [0x56d7bc]
0047c08c f30f11422c               movss      dword ptr [edx + 0x2c], xmm0
0047c091 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047c094 c7403000000000           mov        dword ptr [eax + 0x30], 0
0047c09b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c09e c7413400000000           mov        dword ptr [ecx + 0x34], 0
0047c0a5 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047c0a8 c7423800000000           mov        dword ptr [edx + 0x38], 0
0047c0af 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047c0b2 c7403c00000000           mov        dword ptr [eax + 0x3c], 0
0047c0b9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c0bc c7414000000000           mov        dword ptr [ecx + 0x40], 0
0047c0c3 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047c0c6 c7424400000000           mov        dword ptr [edx + 0x44], 0
0047c0cd 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047c0d0 c7404800000000           mov        dword ptr [eax + 0x48], 0
0047c0d7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c0da 0f57c0                   xorps      xmm0, xmm0
0047c0dd f30f11414c               movss      dword ptr [ecx + 0x4c], xmm0
0047c0e2 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047c0e5 c7425000000000           mov        dword ptr [edx + 0x50], 0
0047c0ec 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047c0ef c7405400000000           mov        dword ptr [eax + 0x54], 0
0047c0f6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c0f9 c7415800000000           mov        dword ptr [ecx + 0x58], 0
0047c100 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047c103 c7425c00000000           mov        dword ptr [edx + 0x5c], 0
0047c10a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c10d 83c160                   add        ecx, 0x60
0047c110 e8ab80f9ff               call       0x4141c0
0047c115 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047c118 c7407000000000           mov        dword ptr [eax + 0x70], 0
0047c11f 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047c122 8be5                     mov        esp, ebp
0047c124 5d                       pop        ebp
0047c125 c3                       ret        
0047c126 cc                       int3       
0047c127 cc                       int3       
0047c128 cc                       int3       
0047c129 cc                       int3       
0047c12a cc                       int3       
0047c12b cc                       int3       
0047c12c cc                       int3       
0047c12d cc                       int3       
0047c12e cc                       int3       
0047c12f cc                       int3       
