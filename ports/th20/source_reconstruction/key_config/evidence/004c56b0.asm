004c56b0 55                       push       ebp
004c56b1 8bec                     mov        ebp, esp
004c56b3 6aff                     push       -1
004c56b5 6800765600               push       0x567600
004c56ba 64a100000000             mov        eax, dword ptr fs:[0]
004c56c0 50                       push       eax
004c56c1 51                       push       ecx
004c56c2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c56c7 33c5                     xor        eax, ebp
004c56c9 50                       push       eax
004c56ca 8d45f4                   lea        eax, [ebp - 0xc]
004c56cd 64a300000000             mov        dword ptr fs:[0], eax
004c56d3 894df0                   mov        dword ptr [ebp - 0x10], ecx
004c56d6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c56d9 c700d00d5700             mov        dword ptr [eax], 0x570dd0
004c56df 68f80d5700               push       0x570df8
004c56e4 e8c76ff4ff               call       0x40c6b0
004c56e9 83c404                   add        esp, 4
004c56ec 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c56ef 8b5108                   mov        edx, dword ptr [ecx + 8]
004c56f2 52                       push       edx
004c56f3 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004c56f9 e8b2cdf4ff               call       0x4124b0
004c56fe 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c5701 8b480c                   mov        ecx, dword ptr [eax + 0xc]
004c5704 51                       push       ecx
004c5705 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004c570b e8a0cdf4ff               call       0x4124b0
004c5710 c705284d5c0000000000     mov        dword ptr [0x5c4d28], 0
004c571a b9404d5c00               mov        ecx, 0x5c4d40
004c571f e8ec810100               call       0x4dd910
004c5724 90                       nop        
004c5725 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c5728 83c114                   add        ecx, 0x14
004c572b e840a3feff               call       0x4afa70
004c5730 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c5733 e8b8a6f5ff               call       0x41fdf0
004c5738 90                       nop        
004c5739 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c573c 64890d00000000           mov        dword ptr fs:[0], ecx
004c5743 59                       pop        ecx
004c5744 8be5                     mov        esp, ebp
004c5746 5d                       pop        ebp
004c5747 c3                       ret        
004c5748 cc                       int3       
004c5749 cc                       int3       
004c574a cc                       int3       
004c574b cc                       int3       
004c574c cc                       int3       
004c574d cc                       int3       
004c574e cc                       int3       
004c574f cc                       int3       
