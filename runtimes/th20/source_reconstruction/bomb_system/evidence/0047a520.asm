0047a520 55                       push       ebp
0047a521 8bec                     mov        ebp, esp
0047a523 51                       push       ecx
0047a524 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a527 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a52a f30f104508               movss      xmm0, dword ptr [ebp + 8]
0047a52f f30f114024               movss      dword ptr [eax + 0x24], xmm0
0047a534 8be5                     mov        esp, ebp
0047a536 5d                       pop        ebp
0047a537 c20400                   ret        4
0047a53a cc                       int3       
0047a53b cc                       int3       
0047a53c cc                       int3       
0047a53d cc                       int3       
0047a53e cc                       int3       
0047a53f cc                       int3       
