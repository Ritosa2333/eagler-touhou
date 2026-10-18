00510870 55                       push       ebp
00510871 8bec                     mov        ebp, esp
00510873 51                       push       ecx
00510874 894dfc                   mov        dword ptr [ebp - 4], ecx
00510877 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051087a f30f104508               movss      xmm0, dword ptr [ebp + 8]
0051087f f30f1180c0050000         movss      dword ptr [eax + 0x5c0], xmm0
00510887 8be5                     mov        esp, ebp
00510889 5d                       pop        ebp
0051088a c20400                   ret        4
0051088d cc                       int3       
0051088e cc                       int3       
0051088f cc                       int3       
