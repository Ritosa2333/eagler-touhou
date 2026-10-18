0052ce50 55                       push       ebp
0052ce51 8bec                     mov        ebp, esp
0052ce53 51                       push       ecx
0052ce54 894dfc                   mov        dword ptr [ebp - 4], ecx
0052ce57 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052ce5a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052ce5d 894820                   mov        dword ptr [eax + 0x20], ecx
0052ce60 6a00                     push       0
0052ce62 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052ce65 81c154010000             add        ecx, 0x154
0052ce6b e8b066efff               call       0x423520
0052ce70 90                       nop        
0052ce71 8be5                     mov        esp, ebp
0052ce73 5d                       pop        ebp
0052ce74 c20400                   ret        4
0052ce77 cc                       int3       
0052ce78 cc                       int3       
0052ce79 cc                       int3       
0052ce7a cc                       int3       
0052ce7b cc                       int3       
0052ce7c cc                       int3       
0052ce7d cc                       int3       
0052ce7e cc                       int3       
0052ce7f cc                       int3       
