0047a5e0 55                       push       ebp
0047a5e1 8bec                     mov        ebp, esp
0047a5e3 51                       push       ecx
0047a5e4 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a5e7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a5ea 83c038                   add        eax, 0x38
0047a5ed 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047a5f0 8b11                     mov        edx, dword ptr [ecx]
0047a5f2 8910                     mov        dword ptr [eax], edx
0047a5f4 8b5104                   mov        edx, dword ptr [ecx + 4]
0047a5f7 895004                   mov        dword ptr [eax + 4], edx
0047a5fa 8b4908                   mov        ecx, dword ptr [ecx + 8]
0047a5fd 894808                   mov        dword ptr [eax + 8], ecx
0047a600 8be5                     mov        esp, ebp
0047a602 5d                       pop        ebp
0047a603 c20400                   ret        4
0047a606 cc                       int3       
0047a607 cc                       int3       
0047a608 cc                       int3       
0047a609 cc                       int3       
0047a60a cc                       int3       
0047a60b cc                       int3       
0047a60c cc                       int3       
0047a60d cc                       int3       
0047a60e cc                       int3       
0047a60f cc                       int3       
