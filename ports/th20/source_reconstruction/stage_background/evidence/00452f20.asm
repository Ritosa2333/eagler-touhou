00452f20 55                       push       ebp
00452f21 8bec                     mov        ebp, esp
00452f23 51                       push       ecx
00452f24 894dfc                   mov        dword ptr [ebp - 4], ecx
00452f27 51                       push       ecx
00452f28 f30f104508               movss      xmm0, dword ptr [ebp + 8]
00452f2d f30f110424               movss      dword ptr [esp], xmm0
00452f32 e80956feff               call       0x438540
00452f37 83c404                   add        esp, 4
00452f3a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00452f3d d918                     fstp       dword ptr [eax]
00452f3f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00452f42 8be5                     mov        esp, ebp
00452f44 5d                       pop        ebp
00452f45 c20400                   ret        4
00452f48 cc                       int3       
00452f49 cc                       int3       
00452f4a cc                       int3       
00452f4b cc                       int3       
00452f4c cc                       int3       
00452f4d cc                       int3       
00452f4e cc                       int3       
00452f4f cc                       int3       
