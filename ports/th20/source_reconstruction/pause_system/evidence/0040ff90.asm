0040ff90 55                       push       ebp
0040ff91 8bec                     mov        ebp, esp
0040ff93 51                       push       ecx
0040ff94 894dfc                   mov        dword ptr [ebp - 4], ecx
0040ff97 8b45fc                   mov        eax, dword ptr [ebp - 4]
0040ff9a 8b4004                   mov        eax, dword ptr [eax + 4]
0040ff9d 8be5                     mov        esp, ebp
0040ff9f 5d                       pop        ebp
0040ffa0 c3                       ret        
0040ffa1 cc                       int3       
0040ffa2 cc                       int3       
0040ffa3 cc                       int3       
0040ffa4 cc                       int3       
0040ffa5 cc                       int3       
0040ffa6 cc                       int3       
0040ffa7 cc                       int3       
0040ffa8 cc                       int3       
0040ffa9 cc                       int3       
0040ffaa cc                       int3       
0040ffab cc                       int3       
0040ffac cc                       int3       
0040ffad cc                       int3       
0040ffae cc                       int3       
0040ffaf cc                       int3       
