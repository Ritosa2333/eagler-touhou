0044ef90 55                       push       ebp
0044ef91 8bec                     mov        ebp, esp
0044ef93 51                       push       ecx
0044ef94 894dfc                   mov        dword ptr [ebp - 4], ecx
0044ef97 8b4508                   mov        eax, dword ptr [ebp + 8]
0044ef9a 50                       push       eax
0044ef9b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044ef9e 8b11                     mov        edx, dword ptr [ecx]
0044efa0 52                       push       edx
0044efa1 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0044efa7 e8e4feffff               call       0x44ee90
0044efac 90                       nop        
0044efad 8be5                     mov        esp, ebp
0044efaf 5d                       pop        ebp
0044efb0 c20400                   ret        4
0044efb3 cc                       int3       
0044efb4 cc                       int3       
0044efb5 cc                       int3       
0044efb6 cc                       int3       
0044efb7 cc                       int3       
0044efb8 cc                       int3       
0044efb9 cc                       int3       
0044efba cc                       int3       
0044efbb cc                       int3       
0044efbc cc                       int3       
0044efbd cc                       int3       
0044efbe cc                       int3       
0044efbf cc                       int3       
