0047c470 55                       push       ebp
0047c471 8bec                     mov        ebp, esp
0047c473 51                       push       ecx
0047c474 894dfc                   mov        dword ptr [ebp - 4], ecx
0047c477 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c47a 81c198000000             add        ecx, 0x98
0047c480 e87bfeffff               call       0x47c300
0047c485 90                       nop        
0047c486 8be5                     mov        esp, ebp
0047c488 5d                       pop        ebp
0047c489 c3                       ret        
0047c48a cc                       int3       
0047c48b cc                       int3       
0047c48c cc                       int3       
0047c48d cc                       int3       
0047c48e cc                       int3       
0047c48f cc                       int3       
