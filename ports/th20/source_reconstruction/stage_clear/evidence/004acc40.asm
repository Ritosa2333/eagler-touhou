004acc40 55                       push       ebp
004acc41 8bec                     mov        ebp, esp
004acc43 6aff                     push       -1
004acc45 688d7c5600               push       0x567c8d
004acc4a 64a100000000             mov        eax, dword ptr fs:[0]
004acc50 50                       push       eax
004acc51 51                       push       ecx
004acc52 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004acc57 33c5                     xor        eax, ebp
004acc59 50                       push       eax
004acc5a 8d45f4                   lea        eax, [ebp - 0xc]
004acc5d 64a300000000             mov        dword ptr fs:[0], eax
004acc63 894df0                   mov        dword ptr [ebp - 0x10], ecx
004acc66 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004acc69 e81273f6ff               call       0x413f80
004acc6e c745fc00000000           mov        dword ptr [ebp - 4], 0
004acc75 8b4508                   mov        eax, dword ptr [ebp + 8]
004acc78 50                       push       eax
004acc79 e802e6f5ff               call       0x40b280
004acc7e 83c404                   add        esp, 4
004acc81 50                       push       eax
004acc82 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004acc85 e866130000               call       0x4adff0
004acc8a 90                       nop        
004acc8b c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004acc92 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004acc95 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004acc98 64890d00000000           mov        dword ptr fs:[0], ecx
004acc9f 59                       pop        ecx
004acca0 8be5                     mov        esp, ebp
004acca2 5d                       pop        ebp
004acca3 c20400                   ret        4
004acca6 cc                       int3       
004acca7 cc                       int3       
004acca8 cc                       int3       
004acca9 cc                       int3       
004accaa cc                       int3       
004accab cc                       int3       
004accac cc                       int3       
004accad cc                       int3       
004accae cc                       int3       
004accaf cc                       int3       
