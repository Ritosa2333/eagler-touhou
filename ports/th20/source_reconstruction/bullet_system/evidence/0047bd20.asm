0047bd20 55                       push       ebp
0047bd21 8bec                     mov        ebp, esp
0047bd23 51                       push       ecx
0047bd24 894dfc                   mov        dword ptr [ebp - 4], ecx
0047bd27 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047bd2a c70000000000             mov        dword ptr [eax], 0
0047bd30 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047bd33 c7410400000000           mov        dword ptr [ecx + 4], 0
0047bd3a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047bd3d 83c108                   add        ecx, 8
0047bd40 e8cb70faff               call       0x422e10
0047bd45 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047bd48 0f57c0                   xorps      xmm0, xmm0
0047bd4b f30f114214               movss      dword ptr [edx + 0x14], xmm0
0047bd50 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047bd53 0f57c0                   xorps      xmm0, xmm0
0047bd56 f30f114018               movss      dword ptr [eax + 0x18], xmm0
0047bd5b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047bd5e 0f57c0                   xorps      xmm0, xmm0
0047bd61 f30f11411c               movss      dword ptr [ecx + 0x1c], xmm0
0047bd66 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047bd69 0f57c0                   xorps      xmm0, xmm0
0047bd6c f30f114220               movss      dword ptr [edx + 0x20], xmm0
0047bd71 33c0                     xor        eax, eax
0047bd73 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047bd76 66894124                 mov        word ptr [ecx + 0x24], ax
0047bd7a 33d2                     xor        edx, edx
0047bd7c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047bd7f 66895026                 mov        word ptr [eax + 0x26], dx
0047bd83 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047bd86 8be5                     mov        esp, ebp
0047bd88 5d                       pop        ebp
0047bd89 c3                       ret        
0047bd8a cc                       int3       
0047bd8b cc                       int3       
0047bd8c cc                       int3       
0047bd8d cc                       int3       
0047bd8e cc                       int3       
0047bd8f cc                       int3       
