00479280 55                       push       ebp
00479281 8bec                     mov        ebp, esp
00479283 6aff                     push       -1
00479285 68ad8f5600               push       0x568fad
0047928a 64a100000000             mov        eax, dword ptr fs:[0]
00479290 50                       push       eax
00479291 51                       push       ecx
00479292 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00479297 33c5                     xor        eax, ebp
00479299 50                       push       eax
0047929a 8d45f4                   lea        eax, [ebp - 0xc]
0047929d 64a300000000             mov        dword ptr fs:[0], eax
004792a3 894df0                   mov        dword ptr [ebp - 0x10], ecx
004792a6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004792a9 e882f1ffff               call       0x478430
004792ae c745fc00000000           mov        dword ptr [ebp - 4], 0
004792b5 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004792b8 c70098fb5600             mov        dword ptr [eax], 0x56fb98
004792be 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004792c1 81c1b8000000             add        ecx, 0xb8
004792c7 e824000000               call       0x4792f0
004792cc 90                       nop        
004792cd c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004792d4 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004792d7 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004792da 64890d00000000           mov        dword ptr fs:[0], ecx
004792e1 59                       pop        ecx
004792e2 8be5                     mov        esp, ebp
004792e4 5d                       pop        ebp
004792e5 c3                       ret        
004792e6 cc                       int3       
004792e7 cc                       int3       
004792e8 cc                       int3       
004792e9 cc                       int3       
004792ea cc                       int3       
004792eb cc                       int3       
004792ec cc                       int3       
004792ed cc                       int3       
004792ee cc                       int3       
004792ef cc                       int3       
