0047a210 55                       push       ebp
0047a211 8bec                     mov        ebp, esp
0047a213 51                       push       ecx
0047a214 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a217 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a21a f30f104020               movss      xmm0, dword ptr [eax + 0x20]
0047a21f f30f584508               addss      xmm0, dword ptr [ebp + 8]
0047a224 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a227 f30f114120               movss      dword ptr [ecx + 0x20], xmm0
0047a22c 8be5                     mov        esp, ebp
0047a22e 5d                       pop        ebp
0047a22f c20400                   ret        4
0047a232 cc                       int3       
0047a233 cc                       int3       
0047a234 cc                       int3       
0047a235 cc                       int3       
0047a236 cc                       int3       
0047a237 cc                       int3       
0047a238 cc                       int3       
0047a239 cc                       int3       
0047a23a cc                       int3       
0047a23b cc                       int3       
0047a23c cc                       int3       
0047a23d cc                       int3       
0047a23e cc                       int3       
0047a23f cc                       int3       
