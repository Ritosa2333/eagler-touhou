00506a00 55                       push       ebp
00506a01 8bec                     mov        ebp, esp
00506a03 51                       push       ecx
00506a04 894dfc                   mov        dword ptr [ebp - 4], ecx
00506a07 6a01                     push       1
00506a09 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00506a0c e8df1ef3ff               call       0x4388f0
00506a11 90                       nop        
00506a12 8be5                     mov        esp, ebp
00506a14 5d                       pop        ebp
00506a15 c3                       ret        
00506a16 cc                       int3       
00506a17 cc                       int3       
00506a18 cc                       int3       
00506a19 cc                       int3       
00506a1a cc                       int3       
00506a1b cc                       int3       
00506a1c cc                       int3       
00506a1d cc                       int3       
00506a1e cc                       int3       
00506a1f cc                       int3       
