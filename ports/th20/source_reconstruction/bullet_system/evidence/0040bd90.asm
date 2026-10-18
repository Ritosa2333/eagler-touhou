0040bd90 55                       push       ebp
0040bd91 8bec                     mov        ebp, esp
0040bd93 51                       push       ecx
0040bd94 8b4508                   mov        eax, dword ptr [ebp + 8]
0040bd97 8945fc                   mov        dword ptr [ebp - 4], eax
0040bd9a 8be5                     mov        esp, ebp
0040bd9c 5d                       pop        ebp
0040bd9d c3                       ret        
0040bd9e cc                       int3       
0040bd9f cc                       int3       
