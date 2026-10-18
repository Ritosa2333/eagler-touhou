0047bd90 55                       push       ebp
0047bd91 8bec                     mov        ebp, esp
0047bd93 51                       push       ecx
0047bd94 894dfc                   mov        dword ptr [ebp - 4], ecx
0047bd97 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047bd9a e8f16ffaff               call       0x422d90
0047bd9f 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047bda2 0f57c0                   xorps      xmm0, xmm0
0047bda5 f30f114010               movss      dword ptr [eax + 0x10], xmm0
0047bdaa 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047bdad 0f57c0                   xorps      xmm0, xmm0
0047bdb0 f30f114114               movss      dword ptr [ecx + 0x14], xmm0
0047bdb5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047bdb8 83c118                   add        ecx, 0x18
0047bdbb e85070faff               call       0x422e10
0047bdc0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047bdc3 83c124                   add        ecx, 0x24
0047bdc6 e84570faff               call       0x422e10
0047bdcb 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047bdce c7423000000000           mov        dword ptr [edx + 0x30], 0
0047bdd5 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047bdd8 c7403400000000           mov        dword ptr [eax + 0x34], 0
0047bddf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047bde2 c7413800000000           mov        dword ptr [ecx + 0x38], 0
0047bde9 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047bdec c7423c00000000           mov        dword ptr [edx + 0x3c], 0
0047bdf3 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047bdf6 8be5                     mov        esp, ebp
0047bdf8 5d                       pop        ebp
0047bdf9 c3                       ret        
0047bdfa cc                       int3       
0047bdfb cc                       int3       
0047bdfc cc                       int3       
0047bdfd cc                       int3       
0047bdfe cc                       int3       
0047bdff cc                       int3       
