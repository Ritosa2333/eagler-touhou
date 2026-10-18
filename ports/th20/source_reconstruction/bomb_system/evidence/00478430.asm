00478430 55                       push       ebp
00478431 8bec                     mov        ebp, esp
00478433 51                       push       ecx
00478434 894dfc                   mov        dword ptr [ebp - 4], ecx
00478437 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047843a c70078fa5600             mov        dword ptr [eax], 0x56fa78
00478440 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478443 c7410400000000           mov        dword ptr [ecx + 4], 0
0047844a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047844d 83c108                   add        ecx, 8
00478450 e83ba9faff               call       0x422d90
00478455 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478458 83c118                   add        ecx, 0x18
0047845b e830a9faff               call       0x422d90
00478460 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478463 83c128                   add        ecx, 0x28
00478466 e8c5000000               call       0x478530
0047846b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047846e 83c170                   add        ecx, 0x70
00478471 e84ad8faff               call       0x425cc0
00478476 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478479 83c174                   add        ecx, 0x74
0047847c e83fd8faff               call       0x425cc0
00478481 8b55fc                   mov        edx, dword ptr [ebp - 4]
00478484 c7427800000000           mov        dword ptr [edx + 0x78], 0
0047848b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047848e c7407c00000000           mov        dword ptr [eax + 0x7c], 0
00478495 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478498 81c180000000             add        ecx, 0x80
0047849e e8ddf4fcff               call       0x447980
004784a3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004784a6 81c1ac000000             add        ecx, 0xac
004784ac e80fd8faff               call       0x425cc0
004784b1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004784b4 c781b000000000000000     mov        dword ptr [ecx + 0xb0], 0
004784be 8b55fc                   mov        edx, dword ptr [ebp - 4]
004784c1 c782b400000000000000     mov        dword ptr [edx + 0xb4], 0
004784cb 8b45fc                   mov        eax, dword ptr [ebp - 4]
004784ce 8be5                     mov        esp, ebp
004784d0 5d                       pop        ebp
004784d1 c3                       ret        
004784d2 cc                       int3       
004784d3 cc                       int3       
004784d4 cc                       int3       
004784d5 cc                       int3       
004784d6 cc                       int3       
004784d7 cc                       int3       
004784d8 cc                       int3       
004784d9 cc                       int3       
004784da cc                       int3       
004784db cc                       int3       
004784dc cc                       int3       
004784dd cc                       int3       
004784de cc                       int3       
004784df cc                       int3       
