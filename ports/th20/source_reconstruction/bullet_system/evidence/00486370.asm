00486370 55                       push       ebp
00486371 8bec                     mov        ebp, esp
00486373 51                       push       ecx
00486374 894dfc                   mov        dword ptr [ebp - 4], ecx
00486377 6a03                     push       3
00486379 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048637c e86f25fbff               call       0x4388f0
00486381 90                       nop        
00486382 8be5                     mov        esp, ebp
00486384 5d                       pop        ebp
00486385 c3                       ret        
00486386 cc                       int3       
00486387 cc                       int3       
00486388 cc                       int3       
00486389 cc                       int3       
0048638a cc                       int3       
0048638b cc                       int3       
0048638c cc                       int3       
0048638d cc                       int3       
0048638e cc                       int3       
0048638f cc                       int3       
