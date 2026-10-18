004d6bd0 55                       push       ebp
004d6bd1 8bec                     mov        ebp, esp
004d6bd3 51                       push       ecx
004d6bd4 894dfc                   mov        dword ptr [ebp - 4], ecx
004d6bd7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d6bda 51                       push       ecx
004d6bdb f30f104074               movss      xmm0, dword ptr [eax + 0x74]
004d6be0 f30f110424               movss      dword ptr [esp], xmm0
004d6be5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d6be8 51                       push       ecx
004d6be9 f30f104170               movss      xmm0, dword ptr [ecx + 0x70]
004d6bee f30f110424               movss      dword ptr [esp], xmm0
004d6bf3 8b5508                   mov        edx, dword ptr [ebp + 8]
004d6bf6 52                       push       edx
004d6bf7 e83427f6ff               call       0x439330
004d6bfc 83c40c                   add        esp, 0xc
004d6bff 8b4508                   mov        eax, dword ptr [ebp + 8]
004d6c02 0f57c0                   xorps      xmm0, xmm0
004d6c05 f30f114008               movss      dword ptr [eax + 8], xmm0
004d6c0a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004d6c0d 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d6c10 f30f1001                 movss      xmm0, dword ptr [ecx]
004d6c14 f30f584258               addss      xmm0, dword ptr [edx + 0x58]
004d6c19 8b4508                   mov        eax, dword ptr [ebp + 8]
004d6c1c f30f1100                 movss      dword ptr [eax], xmm0
004d6c20 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004d6c23 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d6c26 f30f104104               movss      xmm0, dword ptr [ecx + 4]
004d6c2b f30f58425c               addss      xmm0, dword ptr [edx + 0x5c]
004d6c30 8b4508                   mov        eax, dword ptr [ebp + 8]
004d6c33 f30f114004               movss      dword ptr [eax + 4], xmm0
004d6c38 8be5                     mov        esp, ebp
004d6c3a 5d                       pop        ebp
004d6c3b c20400                   ret        4
004d6c3e cc                       int3       
004d6c3f cc                       int3       
