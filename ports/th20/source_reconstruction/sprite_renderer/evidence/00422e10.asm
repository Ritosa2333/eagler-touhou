00422e10 55                       push       ebp
00422e11 8bec                     mov        ebp, esp
00422e13 51                       push       ecx
00422e14 894dfc                   mov        dword ptr [ebp - 4], ecx
00422e17 8b45fc                   mov        eax, dword ptr [ebp - 4]
00422e1a 0f57c0                   xorps      xmm0, xmm0
00422e1d f30f1100                 movss      dword ptr [eax], xmm0
00422e21 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00422e24 0f57c0                   xorps      xmm0, xmm0
00422e27 f30f114104               movss      dword ptr [ecx + 4], xmm0
00422e2c 8b55fc                   mov        edx, dword ptr [ebp - 4]
00422e2f 0f57c0                   xorps      xmm0, xmm0
00422e32 f30f114208               movss      dword ptr [edx + 8], xmm0
00422e37 8b45fc                   mov        eax, dword ptr [ebp - 4]
00422e3a 8be5                     mov        esp, ebp
00422e3c 5d                       pop        ebp
00422e3d c3                       ret        
00422e3e cc                       int3       
00422e3f cc                       int3       
