0044fcd0 55                       push       ebp
0044fcd1 8bec                     mov        ebp, esp
0044fcd3 51                       push       ecx
0044fcd4 894dfc                   mov        dword ptr [ebp - 4], ecx
0044fcd7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044fcda 8b08                     mov        ecx, dword ptr [eax]
0044fcdc 51                       push       ecx
0044fcdd 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0044fce3 e8b8ffffff               call       0x44fca0
0044fce8 8b55fc                   mov        edx, dword ptr [ebp - 4]
0044fceb c70200000000             mov        dword ptr [edx], 0
0044fcf1 8be5                     mov        esp, ebp
0044fcf3 5d                       pop        ebp
0044fcf4 c3                       ret        
0044fcf5 cc                       int3       
0044fcf6 cc                       int3       
0044fcf7 cc                       int3       
0044fcf8 cc                       int3       
0044fcf9 cc                       int3       
0044fcfa cc                       int3       
0044fcfb cc                       int3       
0044fcfc cc                       int3       
0044fcfd cc                       int3       
0044fcfe cc                       int3       
0044fcff cc                       int3       
