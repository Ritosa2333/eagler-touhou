004bfee0 55                       push       ebp
004bfee1 8bec                     mov        ebp, esp
004bfee3 6aff                     push       -1
004bfee5 6800765600               push       0x567600
004bfeea 64a100000000             mov        eax, dword ptr fs:[0]
004bfef0 50                       push       eax
004bfef1 51                       push       ecx
004bfef2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004bfef7 33c5                     xor        eax, ebp
004bfef9 50                       push       eax
004bfefa 8d45f4                   lea        eax, [ebp - 0xc]
004bfefd 64a300000000             mov        dword ptr fs:[0], eax
004bff03 894df0                   mov        dword ptr [ebp - 0x10], ecx
004bff06 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bff09 e812fef5ff               call       0x41fd20
004bff0e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004bff11 c700880c5700             mov        dword ptr [eax], 0x570c88
004bff17 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bff1a 83c110                   add        ecx, 0x10
004bff1d e85effffff               call       0x4bfe80
004bff22 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bff25 c78110c4000000000000     mov        dword ptr [ecx + 0xc410], 0
004bff2f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bff32 81c114c40000             add        ecx, 0xc414
004bff38 e8138bf5ff               call       0x418a50
004bff3d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bff40 81c12cc40000             add        ecx, 0xc42c
004bff46 e8058bf5ff               call       0x418a50
004bff4b 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004bff4e c78244c4000000000000     mov        dword ptr [edx + 0xc444], 0
004bff58 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bff5b 81c148c40000             add        ecx, 0xc448
004bff61 e82a2ef6ff               call       0x422d90
004bff66 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004bff69 c78058c4000000000000     mov        dword ptr [eax + 0xc458], 0
004bff73 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bff76 c7815cc4000000000000     mov        dword ptr [ecx + 0xc45c], 0
004bff80 68940c5700               push       0x570c94
004bff85 e826c7f4ff               call       0x40c6b0
004bff8a 83c404                   add        esp, 4
004bff8d 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004bff90 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004bff93 64890d00000000           mov        dword ptr fs:[0], ecx
004bff9a 59                       pop        ecx
004bff9b 8be5                     mov        esp, ebp
004bff9d 5d                       pop        ebp
004bff9e c3                       ret        
004bff9f cc                       int3       
004bffa0 cc                       int3       
004bffa1 cc                       int3       
004bffa2 cc                       int3       
004bffa3 cc                       int3       
004bffa4 cc                       int3       
004bffa5 cc                       int3       
004bffa6 cc                       int3       
004bffa7 cc                       int3       
004bffa8 cc                       int3       
004bffa9 cc                       int3       
004bffaa cc                       int3       
004bffab cc                       int3       
004bffac cc                       int3       
004bffad cc                       int3       
004bffae cc                       int3       
004bffaf cc                       int3       
