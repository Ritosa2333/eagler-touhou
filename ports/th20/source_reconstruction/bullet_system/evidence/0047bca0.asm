0047bca0 55                       push       ebp
0047bca1 8bec                     mov        ebp, esp
0047bca3 51                       push       ecx
0047bca4 894dfc                   mov        dword ptr [ebp - 4], ecx
0047bca7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047bcaa 0f57c0                   xorps      xmm0, xmm0
0047bcad f30f1100                 movss      dword ptr [eax], xmm0
0047bcb1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047bcb4 0f57c0                   xorps      xmm0, xmm0
0047bcb7 f30f114104               movss      dword ptr [ecx + 4], xmm0
0047bcbc 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047bcbf 0f57c0                   xorps      xmm0, xmm0
0047bcc2 f30f114208               movss      dword ptr [edx + 8], xmm0
0047bcc7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047bcca 0f57c0                   xorps      xmm0, xmm0
0047bccd f30f11400c               movss      dword ptr [eax + 0xc], xmm0
0047bcd2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047bcd5 c7411000000000           mov        dword ptr [ecx + 0x10], 0
0047bcdc 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047bcdf c7421400000000           mov        dword ptr [edx + 0x14], 0
0047bce6 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047bce9 c7401800000000           mov        dword ptr [eax + 0x18], 0
0047bcf0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047bcf3 c7411c00000000           mov        dword ptr [ecx + 0x1c], 0
0047bcfa 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047bcfd c7422000000000           mov        dword ptr [edx + 0x20], 0
0047bd04 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047bd07 c7402400000000           mov        dword ptr [eax + 0x24], 0
0047bd0e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047bd11 c7412800000000           mov        dword ptr [ecx + 0x28], 0
0047bd18 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047bd1b 8be5                     mov        esp, ebp
0047bd1d 5d                       pop        ebp
0047bd1e c3                       ret        
0047bd1f cc                       int3       
