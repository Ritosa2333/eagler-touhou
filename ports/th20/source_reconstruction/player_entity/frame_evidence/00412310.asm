00412310 55                       push       ebp
00412311 8bec                     mov        ebp, esp
00412313 51                       push       ecx
00412314 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00412317 50                       push       eax
00412318 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
0041231e e88d040000               call       0x4127b0
00412323 8945fc                   mov        dword ptr [ebp - 4], eax
00412326 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
00412329 51                       push       ecx
0041232a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0041232d e8de090000               call       0x412d10
00412332 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00412335 e8b6040000               call       0x4127f0
0041233a 8b5508                   mov        edx, dword ptr [ebp + 8]
0041233d 52                       push       edx
0041233e 8b45fc                   mov        eax, dword ptr [ebp - 4]
00412341 50                       push       eax
00412342 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
00412348 e833fcffff               call       0x411f80
0041234d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00412350 8be5                     mov        esp, ebp
00412352 5d                       pop        ebp
00412353 c3                       ret        
00412354 cc                       int3       
00412355 cc                       int3       
00412356 cc                       int3       
00412357 cc                       int3       
00412358 cc                       int3       
00412359 cc                       int3       
0041235a cc                       int3       
0041235b cc                       int3       
0041235c cc                       int3       
0041235d cc                       int3       
0041235e cc                       int3       
0041235f cc                       int3       
