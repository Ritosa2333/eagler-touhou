00467380 55                       push       ebp
00467381 8bec                     mov        ebp, esp
00467383 83ec08                   sub        esp, 8
00467386 894dfc                   mov        dword ptr [ebp - 4], ecx
00467389 8b45fc                   mov        eax, dword ptr [ebp - 4]
0046738c f30f104008               movss      xmm0, dword ptr [eax + 8]
00467391 f30f5e4508               divss      xmm0, dword ptr [ebp + 8]
00467396 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
0046739b d945f8                   fld        dword ptr [ebp - 8]
0046739e 8be5                     mov        esp, ebp
004673a0 5d                       pop        ebp
004673a1 c20400                   ret        4
004673a4 cc                       int3       
004673a5 cc                       int3       
004673a6 cc                       int3       
004673a7 cc                       int3       
004673a8 cc                       int3       
004673a9 cc                       int3       
004673aa cc                       int3       
004673ab cc                       int3       
004673ac cc                       int3       
004673ad cc                       int3       
004673ae cc                       int3       
004673af cc                       int3       
