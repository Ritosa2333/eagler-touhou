0047c130 55                       push       ebp
0047c131 8bec                     mov        ebp, esp
0047c133 51                       push       ecx
0047c134 894dfc                   mov        dword ptr [ebp - 4], ecx
0047c137 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c13a e8d16cfaff               call       0x422e10
0047c13f 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047c142 0f57c0                   xorps      xmm0, xmm0
0047c145 f30f11400c               movss      dword ptr [eax + 0xc], xmm0
0047c14a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c14d 0f57c0                   xorps      xmm0, xmm0
0047c150 f30f114110               movss      dword ptr [ecx + 0x10], xmm0
0047c155 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047c158 0f57c0                   xorps      xmm0, xmm0
0047c15b f30f114214               movss      dword ptr [edx + 0x14], xmm0
0047c160 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047c163 0f57c0                   xorps      xmm0, xmm0
0047c166 f30f114018               movss      dword ptr [eax + 0x18], xmm0
0047c16b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c16e 0f57c0                   xorps      xmm0, xmm0
0047c171 f30f11411c               movss      dword ptr [ecx + 0x1c], xmm0
0047c176 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047c179 0f57c0                   xorps      xmm0, xmm0
0047c17c f30f114220               movss      dword ptr [edx + 0x20], xmm0
0047c181 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047c184 c7402400000000           mov        dword ptr [eax + 0x24], 0
0047c18b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c18e c7412800000000           mov        dword ptr [ecx + 0x28], 0
0047c195 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047c198 0f57c0                   xorps      xmm0, xmm0
0047c19b f30f11422c               movss      dword ptr [edx + 0x2c], xmm0
0047c1a0 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047c1a3 c7403000000000           mov        dword ptr [eax + 0x30], 0
0047c1aa 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c1ad c7413400000000           mov        dword ptr [ecx + 0x34], 0
0047c1b4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c1b7 83c138                   add        ecx, 0x38
0047c1ba e80180f9ff               call       0x4141c0
0047c1bf 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047c1c2 c7424800000000           mov        dword ptr [edx + 0x48], 0
0047c1c9 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047c1cc c7404c00000000           mov        dword ptr [eax + 0x4c], 0
0047c1d3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c1d6 c7415000000000           mov        dword ptr [ecx + 0x50], 0
0047c1dd 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047c1e0 8be5                     mov        esp, ebp
0047c1e2 5d                       pop        ebp
0047c1e3 c3                       ret        
0047c1e4 cc                       int3       
0047c1e5 cc                       int3       
0047c1e6 cc                       int3       
0047c1e7 cc                       int3       
0047c1e8 cc                       int3       
0047c1e9 cc                       int3       
0047c1ea cc                       int3       
0047c1eb cc                       int3       
0047c1ec cc                       int3       
0047c1ed cc                       int3       
0047c1ee cc                       int3       
0047c1ef cc                       int3       
