004c2490 55                       push       ebp
004c2491 8bec                     mov        ebp, esp
004c2493 51                       push       ecx
004c2494 894dfc                   mov        dword ptr [ebp - 4], ecx
004c2497 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c249a 81c1fc050000             add        ecx, 0x5fc
004c24a0 e8cb6ef8ff               call       0x449370
004c24a5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c24a8 83c118                   add        ecx, 0x18
004c24ab e8c06ef8ff               call       0x449370
004c24b0 90                       nop        
004c24b1 8be5                     mov        esp, ebp
004c24b3 5d                       pop        ebp
004c24b4 c3                       ret        
004c24b5 cc                       int3       
004c24b6 cc                       int3       
004c24b7 cc                       int3       
004c24b8 cc                       int3       
004c24b9 cc                       int3       
004c24ba cc                       int3       
004c24bb cc                       int3       
004c24bc cc                       int3       
004c24bd cc                       int3       
004c24be cc                       int3       
004c24bf cc                       int3       
