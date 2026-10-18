00412540 55                       push       ebp
00412541 8bec                     mov        ebp, esp
00412543 51                       push       ecx
00412544 894dfc                   mov        dword ptr [ebp - 4], ecx
00412547 33c0                     xor        eax, eax
00412549 8be5                     mov        esp, ebp
0041254b 5d                       pop        ebp
0041254c c3                       ret        
0041254d cc                       int3       
0041254e cc                       int3       
0041254f cc                       int3       
