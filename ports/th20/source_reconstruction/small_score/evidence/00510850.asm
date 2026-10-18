00510850 55                       push       ebp
00510851 8bec                     mov        ebp, esp
00510853 51                       push       ecx
00510854 894dfc                   mov        dword ptr [ebp - 4], ecx
00510857 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051085a f30f104508               movss      xmm0, dword ptr [ebp + 8]
0051085f f30f1180bc050000         movss      dword ptr [eax + 0x5bc], xmm0
00510867 8be5                     mov        esp, ebp
00510869 5d                       pop        ebp
0051086a c20400                   ret        4
0051086d cc                       int3       
0051086e cc                       int3       
0051086f cc                       int3       
