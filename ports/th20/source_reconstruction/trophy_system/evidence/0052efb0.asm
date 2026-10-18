0052efb0 55                       push       ebp
0052efb1 8bec                     mov        ebp, esp
0052efb3 83ec08                   sub        esp, 8
0052efb6 894dfc                   mov        dword ptr [ebp - 4], ecx
0052efb9 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052efbc 8945f8                   mov        dword ptr [ebp - 8], eax
0052efbf 8d4d08                   lea        ecx, [ebp + 8]
0052efc2 51                       push       ecx
0052efc3 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052efc6 e805080000               call       0x52f7d0
0052efcb 90                       nop        
0052efcc 8be5                     mov        esp, ebp
0052efce 5d                       pop        ebp
0052efcf c20400                   ret        4
0052efd2 cc                       int3       
0052efd3 cc                       int3       
0052efd4 cc                       int3       
0052efd5 cc                       int3       
0052efd6 cc                       int3       
0052efd7 cc                       int3       
0052efd8 cc                       int3       
0052efd9 cc                       int3       
0052efda cc                       int3       
0052efdb cc                       int3       
0052efdc cc                       int3       
0052efdd cc                       int3       
0052efde cc                       int3       
0052efdf cc                       int3       
