00449170 55                       push       ebp
00449171 8bec                     mov        ebp, esp
00449173 51                       push       ecx
00449174 894dfc                   mov        dword ptr [ebp - 4], ecx
00449177 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044917a e8919cfdff               call       0x422e10
0044917f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00449182 0f57c0                   xorps      xmm0, xmm0
00449185 f30f11400c               movss      dword ptr [eax + 0xc], xmm0
0044918a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044918d c74110ffffffff           mov        dword ptr [ecx + 0x10], 0xffffffff
00449194 8b55fc                   mov        edx, dword ptr [ebp - 4]
00449197 0f57c0                   xorps      xmm0, xmm0
0044919a f30f114214               movss      dword ptr [edx + 0x14], xmm0
0044919f 8b45fc                   mov        eax, dword ptr [ebp - 4]
004491a2 0f57c0                   xorps      xmm0, xmm0
004491a5 f30f114018               movss      dword ptr [eax + 0x18], xmm0
004491aa 8b45fc                   mov        eax, dword ptr [ebp - 4]
004491ad 8be5                     mov        esp, ebp
004491af 5d                       pop        ebp
004491b0 c3                       ret        
004491b1 cc                       int3       
004491b2 cc                       int3       
004491b3 cc                       int3       
004491b4 cc                       int3       
004491b5 cc                       int3       
004491b6 cc                       int3       
004491b7 cc                       int3       
004491b8 cc                       int3       
004491b9 cc                       int3       
004491ba cc                       int3       
004491bb cc                       int3       
004491bc cc                       int3       
004491bd cc                       int3       
004491be cc                       int3       
004491bf cc                       int3       
