00449140 55                       push       ebp
00449141 8bec                     mov        ebp, esp
00449143 51                       push       ecx
00449144 894dfc                   mov        dword ptr [ebp - 4], ecx
00449147 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044914a e8c19cfdff               call       0x422e10
0044914f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00449152 0f57c0                   xorps      xmm0, xmm0
00449155 f30f11400c               movss      dword ptr [eax + 0xc], xmm0
0044915a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044915d c74110ffffffff           mov        dword ptr [ecx + 0x10], 0xffffffff
00449164 8b45fc                   mov        eax, dword ptr [ebp - 4]
00449167 8be5                     mov        esp, ebp
00449169 5d                       pop        ebp
0044916a c3                       ret        
0044916b cc                       int3       
0044916c cc                       int3       
0044916d cc                       int3       
0044916e cc                       int3       
0044916f cc                       int3       
