0048b5b0 55                       push       ebp
0048b5b1 8bec                     mov        ebp, esp
0048b5b3 51                       push       ecx
0048b5b4 894dfc                   mov        dword ptr [ebp - 4], ecx
0048b5b7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048b5ba e85178f9ff               call       0x422e10
0048b5bf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048b5c2 83c10c                   add        ecx, 0xc
0048b5c5 e84678f9ff               call       0x422e10
0048b5ca 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048b5cd 0f57c0                   xorps      xmm0, xmm0
0048b5d0 f30f114018               movss      dword ptr [eax + 0x18], xmm0
0048b5d5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048b5d8 0f57c0                   xorps      xmm0, xmm0
0048b5db f30f11411c               movss      dword ptr [ecx + 0x1c], xmm0
0048b5e0 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048b5e3 0f57c0                   xorps      xmm0, xmm0
0048b5e6 f30f114220               movss      dword ptr [edx + 0x20], xmm0
0048b5eb 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048b5ee 0f57c0                   xorps      xmm0, xmm0
0048b5f1 f30f114024               movss      dword ptr [eax + 0x24], xmm0
0048b5f6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048b5f9 c7412800000000           mov        dword ptr [ecx + 0x28], 0
0048b600 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048b603 c7422c00000000           mov        dword ptr [edx + 0x2c], 0
0048b60a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048b60d 0f57c0                   xorps      xmm0, xmm0
0048b610 f30f114030               movss      dword ptr [eax + 0x30], xmm0
0048b615 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048b618 c7413400000000           mov        dword ptr [ecx + 0x34], 0
0048b61f 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048b622 c7423800000000           mov        dword ptr [edx + 0x38], 0
0048b629 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048b62c 83c13c                   add        ecx, 0x3c
0048b62f e88c8bf8ff               call       0x4141c0
0048b634 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048b637 c7404c00000000           mov        dword ptr [eax + 0x4c], 0
0048b63e 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048b641 8be5                     mov        esp, ebp
0048b643 5d                       pop        ebp
0048b644 c3                       ret        
0048b645 cc                       int3       
0048b646 cc                       int3       
0048b647 cc                       int3       
0048b648 cc                       int3       
0048b649 cc                       int3       
0048b64a cc                       int3       
0048b64b cc                       int3       
0048b64c cc                       int3       
0048b64d cc                       int3       
0048b64e cc                       int3       
0048b64f cc                       int3       
