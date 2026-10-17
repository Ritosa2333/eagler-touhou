004488f0 55                       push       ebp
004488f1 8bec                     mov        ebp, esp
004488f3 51                       push       ecx
004488f4 894dfc                   mov        dword ptr [ebp - 4], ecx
004488f7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004488fa c70000000000             mov        dword ptr [eax], 0
00448900 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00448903 c7410400000000           mov        dword ptr [ecx + 4], 0
0044890a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0044890d c7420800000000           mov        dword ptr [edx + 8], 0
00448914 8b45fc                   mov        eax, dword ptr [ebp - 4]
00448917 0f57c0                   xorps      xmm0, xmm0
0044891a f30f11400c               movss      dword ptr [eax + 0xc], xmm0
0044891f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00448922 0f57c0                   xorps      xmm0, xmm0
00448925 f30f114110               movss      dword ptr [ecx + 0x10], xmm0
0044892a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0044892d 0f57c0                   xorps      xmm0, xmm0
00448930 f30f114214               movss      dword ptr [edx + 0x14], xmm0
00448935 8b45fc                   mov        eax, dword ptr [ebp - 4]
00448938 0f57c0                   xorps      xmm0, xmm0
0044893b f30f114018               movss      dword ptr [eax + 0x18], xmm0
00448940 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00448943 0f57c0                   xorps      xmm0, xmm0
00448946 f30f11411c               movss      dword ptr [ecx + 0x1c], xmm0
0044894b 8b55fc                   mov        edx, dword ptr [ebp - 4]
0044894e 0f57c0                   xorps      xmm0, xmm0
00448951 f30f114220               movss      dword ptr [edx + 0x20], xmm0
00448956 8b45fc                   mov        eax, dword ptr [ebp - 4]
00448959 0f57c0                   xorps      xmm0, xmm0
0044895c f30f114024               movss      dword ptr [eax + 0x24], xmm0
00448961 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00448964 0f57c0                   xorps      xmm0, xmm0
00448967 f30f114128               movss      dword ptr [ecx + 0x28], xmm0
0044896c 8b55fc                   mov        edx, dword ptr [ebp - 4]
0044896f 0f57c0                   xorps      xmm0, xmm0
00448972 f30f11422c               movss      dword ptr [edx + 0x2c], xmm0
00448977 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044897a 0f57c0                   xorps      xmm0, xmm0
0044897d f30f114030               movss      dword ptr [eax + 0x30], xmm0
00448982 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00448985 0f57c0                   xorps      xmm0, xmm0
00448988 f30f114134               movss      dword ptr [ecx + 0x34], xmm0
0044898d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00448990 0f57c0                   xorps      xmm0, xmm0
00448993 f30f114238               movss      dword ptr [edx + 0x38], xmm0
00448998 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044899b 0f57c0                   xorps      xmm0, xmm0
0044899e f30f11403c               movss      dword ptr [eax + 0x3c], xmm0
004489a3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004489a6 0f57c0                   xorps      xmm0, xmm0
004489a9 f30f114140               movss      dword ptr [ecx + 0x40], xmm0
004489ae 8b55fc                   mov        edx, dword ptr [ebp - 4]
004489b1 0f57c0                   xorps      xmm0, xmm0
004489b4 f30f114244               movss      dword ptr [edx + 0x44], xmm0
004489b9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004489bc 0f57c0                   xorps      xmm0, xmm0
004489bf f30f114048               movss      dword ptr [eax + 0x48], xmm0
004489c4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004489c7 0f57c0                   xorps      xmm0, xmm0
004489ca f30f11414c               movss      dword ptr [ecx + 0x4c], xmm0
004489cf 8b55fc                   mov        edx, dword ptr [ebp - 4]
004489d2 0f57c0                   xorps      xmm0, xmm0
004489d5 f30f114250               movss      dword ptr [edx + 0x50], xmm0
004489da 8b45fc                   mov        eax, dword ptr [ebp - 4]
004489dd 0f57c0                   xorps      xmm0, xmm0
004489e0 f30f114054               movss      dword ptr [eax + 0x54], xmm0
004489e5 8b45fc                   mov        eax, dword ptr [ebp - 4]
004489e8 8be5                     mov        esp, ebp
004489ea 5d                       pop        ebp
004489eb c3                       ret        
004489ec cc                       int3       
004489ed cc                       int3       
004489ee cc                       int3       
004489ef cc                       int3       
