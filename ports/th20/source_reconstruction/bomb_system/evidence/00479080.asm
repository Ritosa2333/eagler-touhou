00479080 55                       push       ebp
00479081 8bec                     mov        ebp, esp
00479083 51                       push       ecx
00479084 894dfc                   mov        dword ptr [ebp - 4], ecx
00479087 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047908a f30f104508               movss      xmm0, dword ptr [ebp + 8]
0047908f f30f1180ec200000         movss      dword ptr [eax + 0x20ec], xmm0
00479097 8be5                     mov        esp, ebp
00479099 5d                       pop        ebp
0047909a c20400                   ret        4
0047909d cc                       int3       
0047909e cc                       int3       
0047909f cc                       int3       
