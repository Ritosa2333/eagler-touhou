0052ceb0 55                       push       ebp
0052ceb1 8bec                     mov        ebp, esp
0052ceb3 51                       push       ecx
0052ceb4 894dfc                   mov        dword ptr [ebp - 4], ecx
0052ceb7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052ceba 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052cebd 894874                   mov        dword ptr [eax + 0x74], ecx
0052cec0 8be5                     mov        esp, ebp
0052cec2 5d                       pop        ebp
0052cec3 c20400                   ret        4
0052cec6 cc                       int3       
0052cec7 cc                       int3       
0052cec8 cc                       int3       
0052cec9 cc                       int3       
0052ceca cc                       int3       
0052cecb cc                       int3       
0052cecc cc                       int3       
0052cecd cc                       int3       
0052cece cc                       int3       
0052cecf cc                       int3       
