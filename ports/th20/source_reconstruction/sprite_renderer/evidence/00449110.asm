00449110 55                       push       ebp
00449111 8bec                     mov        ebp, esp
00449113 51                       push       ecx
00449114 894dfc                   mov        dword ptr [ebp - 4], ecx
00449117 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044911a e8f19cfdff               call       0x422e10
0044911f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00449122 0f57c0                   xorps      xmm0, xmm0
00449125 f30f11400c               movss      dword ptr [eax + 0xc], xmm0
0044912a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044912d 0f57c0                   xorps      xmm0, xmm0
00449130 f30f114110               movss      dword ptr [ecx + 0x10], xmm0
00449135 8b45fc                   mov        eax, dword ptr [ebp - 4]
00449138 8be5                     mov        esp, ebp
0044913a 5d                       pop        ebp
0044913b c3                       ret        
0044913c cc                       int3       
0044913d cc                       int3       
0044913e cc                       int3       
0044913f cc                       int3       
