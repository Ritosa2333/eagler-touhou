00532f40 55                       push       ebp
00532f41 8bec                     mov        ebp, esp
00532f43 51                       push       ecx
00532f44 894dfc                   mov        dword ptr [ebp - 4], ecx
00532f47 6a00                     push       0
00532f49 6a17                     push       0x17
00532f4b 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00532f51 e89a34f5ff               call       0x4863f0
00532f56 8b45fc                   mov        eax, dword ptr [ebp - 4]
00532f59 c7405400000000           mov        dword ptr [eax + 0x54], 0
00532f60 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00532f63 83c120                   add        ecx, 0x20
00532f66 e865cdf1ff               call       0x44fcd0
00532f6b 6a00                     push       0
00532f6d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00532f70 83c120                   add        ecx, 0x20
00532f73 e828e8edff               call       0x4117a0
00532f78 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00532f7b 8b5140                   mov        edx, dword ptr [ecx + 0x40]
00532f7e 52                       push       edx
00532f7f 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00532f85 e876f8f6ff               call       0x4a2800
00532f8a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00532f8d c7404000000000           mov        dword ptr [eax + 0x40], 0
00532f94 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00532f97 8b11                     mov        edx, dword ptr [ecx]
00532f99 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00532f9c 8b4208                   mov        eax, dword ptr [edx + 8]
00532f9f ffd0                     call       eax
00532fa1 33c0                     xor        eax, eax
00532fa3 8be5                     mov        esp, ebp
00532fa5 5d                       pop        ebp
00532fa6 c3                       ret        
00532fa7 cc                       int3       
00532fa8 cc                       int3       
00532fa9 cc                       int3       
00532faa cc                       int3       
00532fab cc                       int3       
00532fac cc                       int3       
00532fad cc                       int3       
00532fae cc                       int3       
00532faf cc                       int3       
