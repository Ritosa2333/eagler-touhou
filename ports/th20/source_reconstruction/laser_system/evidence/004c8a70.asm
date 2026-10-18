004c8a70 55                       push       ebp
004c8a71 8bec                     mov        ebp, esp
004c8a73 6aff                     push       -1
004c8a75 6800765600               push       0x567600
004c8a7a 64a100000000             mov        eax, dword ptr fs:[0]
004c8a80 50                       push       eax
004c8a81 51                       push       ecx
004c8a82 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c8a87 33c5                     xor        eax, ebp
004c8a89 50                       push       eax
004c8a8a 8d45f4                   lea        eax, [ebp - 0xc]
004c8a8d 64a300000000             mov        dword ptr fs:[0], eax
004c8a93 894df0                   mov        dword ptr [ebp - 0x10], ecx
004c8a96 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c8a99 c70014125700             mov        dword ptr [eax], 0x571214
004c8a9f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8aa2 e8b9980000               call       0x4d2360
004c8aa7 90                       nop        
004c8aa8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8aab 81c1380d0000             add        ecx, 0xd38
004c8ab1 e8ba08f8ff               call       0x449370
004c8ab6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8ab9 81c154070000             add        ecx, 0x754
004c8abf e8ac08f8ff               call       0x449370
004c8ac4 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8ac7 81c1f8060000             add        ecx, 0x6f8
004c8acd e8de2dfcff               call       0x48b8b0
004c8ad2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8ad5 e816000000               call       0x4c8af0
004c8ada 90                       nop        
004c8adb 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c8ade 64890d00000000           mov        dword ptr fs:[0], ecx
004c8ae5 59                       pop        ecx
004c8ae6 8be5                     mov        esp, ebp
004c8ae8 5d                       pop        ebp
004c8ae9 c3                       ret        
004c8aea cc                       int3       
004c8aeb cc                       int3       
004c8aec cc                       int3       
004c8aed cc                       int3       
004c8aee cc                       int3       
004c8aef cc                       int3       
