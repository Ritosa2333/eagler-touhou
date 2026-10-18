00486310 55                       push       ebp
00486311 8bec                     mov        ebp, esp
00486313 51                       push       ecx
00486314 894dfc                   mov        dword ptr [ebp - 4], ecx
00486317 6a02                     push       2
00486319 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048631c e86f8cfcff               call       0x44ef90
00486321 90                       nop        
00486322 8be5                     mov        esp, ebp
00486324 5d                       pop        ebp
00486325 c3                       ret        
00486326 cc                       int3       
00486327 cc                       int3       
00486328 cc                       int3       
00486329 cc                       int3       
0048632a cc                       int3       
0048632b cc                       int3       
0048632c cc                       int3       
0048632d cc                       int3       
0048632e cc                       int3       
0048632f cc                       int3       
