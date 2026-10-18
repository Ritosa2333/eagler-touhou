004c0240 55                       push       ebp
004c0241 8bec                     mov        ebp, esp
004c0243 51                       push       ecx
004c0244 894dfc                   mov        dword ptr [ebp - 4], ecx
004c0247 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c024a f30f1000                 movss      xmm0, dword ptr [eax]
004c024e f30f5e4508               divss      xmm0, dword ptr [ebp + 8]
004c0253 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0256 f30f1101                 movss      dword ptr [ecx], xmm0
004c025a 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c025d f30f104204               movss      xmm0, dword ptr [edx + 4]
004c0262 f30f5e4508               divss      xmm0, dword ptr [ebp + 8]
004c0267 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c026a f30f114004               movss      dword ptr [eax + 4], xmm0
004c026f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0272 f30f104108               movss      xmm0, dword ptr [ecx + 8]
004c0277 f30f5e4508               divss      xmm0, dword ptr [ebp + 8]
004c027c 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c027f f30f114208               movss      dword ptr [edx + 8], xmm0
004c0284 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c0287 8be5                     mov        esp, ebp
004c0289 5d                       pop        ebp
004c028a c20400                   ret        4
004c028d cc                       int3       
004c028e cc                       int3       
004c028f cc                       int3       
