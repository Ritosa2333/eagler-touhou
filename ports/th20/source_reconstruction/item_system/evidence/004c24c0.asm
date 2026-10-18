004c24c0 55                       push       ebp
004c24c1 8bec                     mov        ebp, esp
004c24c3 6aff                     push       -1
004c24c5 6800765600               push       0x567600
004c24ca 64a100000000             mov        eax, dword ptr fs:[0]
004c24d0 50                       push       eax
004c24d1 51                       push       ecx
004c24d2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c24d7 33c5                     xor        eax, ebp
004c24d9 50                       push       eax
004c24da 8d45f4                   lea        eax, [ebp - 0xc]
004c24dd 64a300000000             mov        dword ptr fs:[0], eax
004c24e3 894df0                   mov        dword ptr [ebp - 0x10], ecx
004c24e6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c24e9 c700400d5700             mov        dword ptr [eax], 0x570d40
004c24ef 68600d5700               push       0x570d60
004c24f4 e8b7a1f4ff               call       0x40c6b0
004c24f9 83c404                   add        esp, 4
004c24fc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c24ff 8b5108                   mov        edx, dword ptr [ecx + 8]
004c2502 52                       push       edx
004c2503 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004c2509 e8a2fff4ff               call       0x4124b0
004c250e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c2511 8b480c                   mov        ecx, dword ptr [eax + 0xc]
004c2514 51                       push       ecx
004c2515 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004c251b e890fff4ff               call       0x4124b0
004c2520 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c2523 8b4214                   mov        eax, dword ptr [edx + 0x14]
004c2526 50                       push       eax
004c2527 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004c252d e87efff4ff               call       0x4124b0
004c2532 90                       nop        
004c2533 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c2536 83c118                   add        ecx, 0x18
004c2539 e8f2feffff               call       0x4c2430
004c253e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c2541 e8aad8f5ff               call       0x41fdf0
004c2546 90                       nop        
004c2547 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c254a 64890d00000000           mov        dword ptr fs:[0], ecx
004c2551 59                       pop        ecx
004c2552 8be5                     mov        esp, ebp
004c2554 5d                       pop        ebp
004c2555 c3                       ret        
004c2556 cc                       int3       
004c2557 cc                       int3       
004c2558 cc                       int3       
004c2559 cc                       int3       
004c255a cc                       int3       
004c255b cc                       int3       
004c255c cc                       int3       
004c255d cc                       int3       
004c255e cc                       int3       
004c255f cc                       int3       
