004591f0 55                       push       ebp
004591f1 8bec                     mov        ebp, esp
004591f3 51                       push       ecx
004591f4 894dfc                   mov        dword ptr [ebp - 4], ecx
004591f7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004591fa f30f104508               movss      xmm0, dword ptr [ebp + 8]
004591ff f30f114040               movss      dword ptr [eax + 0x40], xmm0
00459204 8be5                     mov        esp, ebp
00459206 5d                       pop        ebp
00459207 c20400                   ret        4
0045920a cc                       int3       
0045920b cc                       int3       
0045920c cc                       int3       
0045920d cc                       int3       
0045920e cc                       int3       
0045920f cc                       int3       
