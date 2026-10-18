004c22e0 55                       push       ebp
004c22e1 8bec                     mov        ebp, esp
004c22e3 6aff                     push       -1
004c22e5 6800765600               push       0x567600
004c22ea 64a100000000             mov        eax, dword ptr fs:[0]
004c22f0 50                       push       eax
004c22f1 51                       push       ecx
004c22f2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c22f7 33c5                     xor        eax, ebp
004c22f9 50                       push       eax
004c22fa 8d45f4                   lea        eax, [ebp - 0xc]
004c22fd 64a300000000             mov        dword ptr fs:[0], eax
004c2303 894df0                   mov        dword ptr [ebp - 0x10], ecx
004c2306 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c2309 e812daf5ff               call       0x41fd20
004c230e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c2311 c700400d5700             mov        dword ptr [eax], 0x570d40
004c2317 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c231a c7411000000000           mov        dword ptr [ecx + 0x10], 0
004c2321 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c2324 c7421400000000           mov        dword ptr [edx + 0x14], 0
004c232b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c232e 83c118                   add        ecx, 0x18
004c2331 e84afeffff               call       0x4c2180
004c2336 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c2339 81c118c84900             add        ecx, 0x49c818
004c233f e80c67f5ff               call       0x418a50
004c2344 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c2347 81c130c84900             add        ecx, 0x49c830
004c234d e8fe66f5ff               call       0x418a50
004c2352 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c2355 81c148c84900             add        ecx, 0x49c848
004c235b e8f066f5ff               call       0x418a50
004c2360 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c2363 0f57c0                   xorps      xmm0, xmm0
004c2366 f30f118060c84900         movss      dword ptr [eax + 0x49c860], xmm0
004c236e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c2371 c78164c8490000000000     mov        dword ptr [ecx + 0x49c864], 0
004c237b 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c237e c78268c8490000000000     mov        dword ptr [edx + 0x49c868], 0
004c2388 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c238b c7806cc8490000000000     mov        dword ptr [eax + 0x49c86c], 0
004c2395 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c2398 c78170c8490000000000     mov        dword ptr [ecx + 0x49c870], 0
004c23a2 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c23a5 c78274c8490000000000     mov        dword ptr [edx + 0x49c874], 0
004c23af 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c23b2 c78078c8490000000000     mov        dword ptr [eax + 0x49c878], 0
004c23bc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c23bf c7817cc8490000000000     mov        dword ptr [ecx + 0x49c87c], 0
004c23c9 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c23cc c78280c8490000000000     mov        dword ptr [edx + 0x49c880], 0
004c23d6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c23d9 c78084c8490000000000     mov        dword ptr [eax + 0x49c884], 0
004c23e3 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c23e6 81c188c84900             add        ecx, 0x49c888
004c23ec e81f0af6ff               call       0x422e10
004c23f1 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c23f4 c78194c8490000000000     mov        dword ptr [ecx + 0x49c894], 0
004c23fe 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c2401 c78298c8490000000000     mov        dword ptr [edx + 0x49c898], 0
004c240b 684c0d5700               push       0x570d4c
004c2410 e89ba2f4ff               call       0x40c6b0
004c2415 83c404                   add        esp, 4
004c2418 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c241b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c241e 64890d00000000           mov        dword ptr fs:[0], ecx
004c2425 59                       pop        ecx
004c2426 8be5                     mov        esp, ebp
004c2428 5d                       pop        ebp
004c2429 c3                       ret        
004c242a cc                       int3       
004c242b cc                       int3       
004c242c cc                       int3       
004c242d cc                       int3       
004c242e cc                       int3       
004c242f cc                       int3       
