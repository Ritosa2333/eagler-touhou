00486350 55                       push       ebp
00486351 8bec                     mov        ebp, esp
00486353 51                       push       ecx
00486354 894dfc                   mov        dword ptr [ebp - 4], ecx
00486357 6a03                     push       3
00486359 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048635c e82f8cfcff               call       0x44ef90
00486361 90                       nop        
00486362 8be5                     mov        esp, ebp
00486364 5d                       pop        ebp
00486365 c3                       ret        
00486366 cc                       int3       
00486367 cc                       int3       
00486368 cc                       int3       
00486369 cc                       int3       
0048636a cc                       int3       
0048636b cc                       int3       
0048636c cc                       int3       
0048636d cc                       int3       
0048636e cc                       int3       
0048636f cc                       int3       
