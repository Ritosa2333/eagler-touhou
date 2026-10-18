00435500 55                       push       ebp
00435501 8bec                     mov        ebp, esp
00435503 51                       push       ecx
00435504 894dfc                   mov        dword ptr [ebp - 4], ecx
00435507 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0043550a e8c160ffff               call       0x42b5d0
0043550f 8be5                     mov        esp, ebp
00435511 5d                       pop        ebp
00435512 c20400                   ret        4
00435515 cc                       int3       
00435516 cc                       int3       
00435517 cc                       int3       
00435518 cc                       int3       
00435519 cc                       int3       
0043551a cc                       int3       
0043551b cc                       int3       
0043551c cc                       int3       
0043551d cc                       int3       
0043551e cc                       int3       
0043551f cc                       int3       
