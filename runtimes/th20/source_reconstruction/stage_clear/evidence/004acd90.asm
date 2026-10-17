004acd90 55                       push       ebp
004acd91 8bec                     mov        ebp, esp
004acd93 6aff                     push       -1
004acd95 688d7c5600               push       0x567c8d
004acd9a 64a100000000             mov        eax, dword ptr fs:[0]
004acda0 50                       push       eax
004acda1 51                       push       ecx
004acda2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004acda7 33c5                     xor        eax, ebp
004acda9 50                       push       eax
004acdaa 8d45f4                   lea        eax, [ebp - 0xc]
004acdad 64a300000000             mov        dword ptr fs:[0], eax
004acdb3 894df0                   mov        dword ptr [ebp - 0x10], ecx
004acdb6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004acdb9 e8c271f6ff               call       0x413f80
004acdbe c745fc00000000           mov        dword ptr [ebp - 4], 0
004acdc5 8b4508                   mov        eax, dword ptr [ebp + 8]
004acdc8 50                       push       eax
004acdc9 e8b2e4f5ff               call       0x40b280
004acdce 83c404                   add        esp, 4
004acdd1 50                       push       eax
004acdd2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004acdd5 e836130000               call       0x4ae110
004acdda 90                       nop        
004acddb c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004acde2 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004acde5 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004acde8 64890d00000000           mov        dword ptr fs:[0], ecx
004acdef 59                       pop        ecx
004acdf0 8be5                     mov        esp, ebp
004acdf2 5d                       pop        ebp
004acdf3 c20400                   ret        4
004acdf6 cc                       int3       
004acdf7 cc                       int3       
004acdf8 cc                       int3       
004acdf9 cc                       int3       
004acdfa cc                       int3       
004acdfb cc                       int3       
004acdfc cc                       int3       
004acdfd cc                       int3       
004acdfe cc                       int3       
004acdff cc                       int3       
