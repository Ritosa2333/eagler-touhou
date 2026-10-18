0047a540 55                       push       ebp
0047a541 8bec                     mov        ebp, esp
0047a543 51                       push       ecx
0047a544 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a547 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a54a f30f104508               movss      xmm0, dword ptr [ebp + 8]
0047a54f f30f114020               movss      dword ptr [eax + 0x20], xmm0
0047a554 8be5                     mov        esp, ebp
0047a556 5d                       pop        ebp
0047a557 c20400                   ret        4
0047a55a cc                       int3       
0047a55b cc                       int3       
0047a55c cc                       int3       
0047a55d cc                       int3       
0047a55e cc                       int3       
0047a55f cc                       int3       
