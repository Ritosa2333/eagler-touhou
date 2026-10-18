004c8240 55                       push       ebp
004c8241 8bec                     mov        ebp, esp
004c8243 51                       push       ecx
004c8244 894dfc                   mov        dword ptr [ebp - 4], ecx
004c8247 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c824a e8c1abf5ff               call       0x422e10
004c824f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8252 83c10c                   add        ecx, 0xc
004c8255 e8b6abf5ff               call       0x422e10
004c825a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c825d 83c118                   add        ecx, 0x18
004c8260 e83b16f7ff               call       0x4398a0
004c8265 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c8268 0f57c0                   xorps      xmm0, xmm0
004c826b f30f114020               movss      dword ptr [eax + 0x20], xmm0
004c8270 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8273 0f57c0                   xorps      xmm0, xmm0
004c8276 f30f114124               movss      dword ptr [ecx + 0x24], xmm0
004c827b 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c827e 0f57c0                   xorps      xmm0, xmm0
004c8281 f30f114228               movss      dword ptr [edx + 0x28], xmm0
004c8286 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c8289 0f57c0                   xorps      xmm0, xmm0
004c828c f30f11402c               movss      dword ptr [eax + 0x2c], xmm0
004c8291 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c8294 c7413000000000           mov        dword ptr [ecx + 0x30], 0
004c829b 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c829e c7423400000000           mov        dword ptr [edx + 0x34], 0
004c82a5 33c0                     xor        eax, eax
004c82a7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c82aa 83c138                   add        ecx, 0x38
004c82ad 8901                     mov        dword ptr [ecx], eax
004c82af 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c82b2 8be5                     mov        esp, ebp
004c82b4 5d                       pop        ebp
004c82b5 c3                       ret        
004c82b6 cc                       int3       
004c82b7 cc                       int3       
004c82b8 cc                       int3       
004c82b9 cc                       int3       
004c82ba cc                       int3       
004c82bb cc                       int3       
004c82bc cc                       int3       
004c82bd cc                       int3       
004c82be cc                       int3       
004c82bf cc                       int3       
