00456130 55                       push       ebp
00456131 8bec                     mov        ebp, esp
00456133 51                       push       ecx
00456134 8b4508                   mov        eax, dword ptr [ebp + 8]
00456137 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0045613a f30f1000                 movss      xmm0, dword ptr [eax]
0045613e f30f5901                 mulss      xmm0, dword ptr [ecx]
00456142 8b5508                   mov        edx, dword ptr [ebp + 8]
00456145 8b4508                   mov        eax, dword ptr [ebp + 8]
00456148 f30f104a04               movss      xmm1, dword ptr [edx + 4]
0045614d f30f594804               mulss      xmm1, dword ptr [eax + 4]
00456152 f30f58c1                 addss      xmm0, xmm1
00456156 f30f1145fc               movss      dword ptr [ebp - 4], xmm0
0045615b d945fc                   fld        dword ptr [ebp - 4]
0045615e 8be5                     mov        esp, ebp
00456160 5d                       pop        ebp
00456161 c3                       ret        
00456162 cc                       int3       
00456163 cc                       int3       
00456164 cc                       int3       
00456165 cc                       int3       
00456166 cc                       int3       
00456167 cc                       int3       
00456168 cc                       int3       
00456169 cc                       int3       
0045616a cc                       int3       
0045616b cc                       int3       
0045616c cc                       int3       
0045616d cc                       int3       
0045616e cc                       int3       
0045616f cc                       int3       
