0052c320 55                       push       ebp
0052c321 8bec                     mov        ebp, esp
0052c323 6aff                     push       -1
0052c325 68ed765600               push       0x5676ed
0052c32a 64a100000000             mov        eax, dword ptr fs:[0]
0052c330 50                       push       eax
0052c331 83ec18                   sub        esp, 0x18
0052c334 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052c339 33c5                     xor        eax, ebp
0052c33b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0052c33e 50                       push       eax
0052c33f 8d45f4                   lea        eax, [ebp - 0xc]
0052c342 64a300000000             mov        dword ptr fs:[0], eax
0052c348 894de8                   mov        dword ptr [ebp - 0x18], ecx
0052c34b 6a14                     push       0x14
0052c34d b940025c00               mov        ecx, 0x5c0240
0052c352 e8b9ffedff               call       0x40c310
0052c357 50                       push       eax
0052c358 8d4dec                   lea        ecx, [ebp - 0x14]
0052c35b e8d0f3edff               call       0x40b730
0052c360 c745fc00000000           mov        dword ptr [ebp - 4], 0
0052c367 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0052c36a 8b8880421200             mov        ecx, dword ptr [eax + 0x124280]
0052c370 83e901                   sub        ecx, 1
0052c373 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0052c376 898a80421200             mov        dword ptr [edx + 0x124280], ecx
0052c37c 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0052c37f 0584421200               add        eax, 0x124284
0052c384 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0052c387 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c38a 8b9180421200             mov        edx, dword ptr [ecx + 0x124280]
0052c390 52                       push       edx
0052c391 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0052c394 e8e785f5ff               call       0x484980
0052c399 8b00                     mov        eax, dword ptr [eax]
0052c39b 8945e0                   mov        dword ptr [ebp - 0x20], eax
0052c39e 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c3a1 e89a27f9ff               call       0x4beb40
0052c3a6 90                       nop        
0052c3a7 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052c3aa 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0052c3ad c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0052c3b4 8d4dec                   lea        ecx, [ebp - 0x14]
0052c3b7 e844f5edff               call       0x40b900
0052c3bc 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0052c3bf 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052c3c2 64890d00000000           mov        dword ptr fs:[0], ecx
0052c3c9 59                       pop        ecx
0052c3ca 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052c3cd 33cd                     xor        ecx, ebp
0052c3cf e81a660100               call       0x5429ee
0052c3d4 8be5                     mov        esp, ebp
0052c3d6 5d                       pop        ebp
0052c3d7 c3                       ret        
0052c3d8 cc                       int3       
0052c3d9 cc                       int3       
0052c3da cc                       int3       
0052c3db cc                       int3       
0052c3dc cc                       int3       
0052c3dd cc                       int3       
0052c3de cc                       int3       
0052c3df cc                       int3       
