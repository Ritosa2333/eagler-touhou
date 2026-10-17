0052c440 55                       push       ebp
0052c441 8bec                     mov        ebp, esp
0052c443 6aff                     push       -1
0052c445 688d805600               push       0x56808d
0052c44a 64a100000000             mov        eax, dword ptr fs:[0]
0052c450 50                       push       eax
0052c451 83ec14                   sub        esp, 0x14
0052c454 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052c459 33c5                     xor        eax, ebp
0052c45b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0052c45e 50                       push       eax
0052c45f 8d45f4                   lea        eax, [ebp - 0xc]
0052c462 64a300000000             mov        dword ptr fs:[0], eax
0052c468 894de8                   mov        dword ptr [ebp - 0x18], ecx
0052c46b 6a14                     push       0x14
0052c46d b940025c00               mov        ecx, 0x5c0240
0052c472 e899feedff               call       0x40c310
0052c477 50                       push       eax
0052c478 8d4dec                   lea        ecx, [ebp - 0x14]
0052c47b e8b0f2edff               call       0x40b730
0052c480 c745fc00000000           mov        dword ptr [ebp - 4], 0
0052c487 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c48a e8217bf3ff               call       0x463fb0
0052c48f 90                       nop        
0052c490 8b4510                   mov        eax, dword ptr [ebp + 0x10]
0052c493 50                       push       eax
0052c494 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0052c497 51                       push       ecx
0052c498 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c49b e8c00ff9ff               call       0x4bd460
0052c4a0 05d4760000               add        eax, 0x76d4
0052c4a5 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0052c4a8 8b5508                   mov        edx, dword ptr [ebp + 8]
0052c4ab 52                       push       edx
0052c4ac 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0052c4af e8cc80eeff               call       0x414580
0052c4b4 8b00                     mov        eax, dword ptr [eax]
0052c4b6 8945e0                   mov        dword ptr [ebp - 0x20], eax
0052c4b9 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0052c4c0 8d4dec                   lea        ecx, [ebp - 0x14]
0052c4c3 e838f4edff               call       0x40b900
0052c4c8 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0052c4cb 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052c4ce 64890d00000000           mov        dword ptr fs:[0], ecx
0052c4d5 59                       pop        ecx
0052c4d6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052c4d9 33cd                     xor        ecx, ebp
0052c4db e80e650100               call       0x5429ee
0052c4e0 8be5                     mov        esp, ebp
0052c4e2 5d                       pop        ebp
0052c4e3 c20c00                   ret        0xc
0052c4e6 cc                       int3       
0052c4e7 cc                       int3       
0052c4e8 cc                       int3       
0052c4e9 cc                       int3       
0052c4ea cc                       int3       
0052c4eb cc                       int3       
0052c4ec cc                       int3       
0052c4ed cc                       int3       
0052c4ee cc                       int3       
0052c4ef cc                       int3       
