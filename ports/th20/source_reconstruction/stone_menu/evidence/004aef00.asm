004aef00 55                       push       ebp
004aef01 8bec                     mov        ebp, esp
004aef03 6aff                     push       -1
004aef05 6800765600               push       0x567600
004aef0a 64a100000000             mov        eax, dword ptr fs:[0]
004aef10 50                       push       eax
004aef11 51                       push       ecx
004aef12 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004aef17 33c5                     xor        eax, ebp
004aef19 50                       push       eax
004aef1a 8d45f4                   lea        eax, [ebp - 0xc]
004aef1d 64a300000000             mov        dword ptr fs:[0], eax
004aef23 894df0                   mov        dword ptr [ebp - 0x10], ecx
004aef26 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004aef29 c70000000000             mov        dword ptr [eax], 0
004aef2f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004aef32 c7410400000000           mov        dword ptr [ecx + 4], 0
004aef39 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004aef3c c74208e7030000           mov        dword ptr [edx + 8], 0x3e7
004aef43 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004aef46 c7400c00000000           mov        dword ptr [eax + 0xc], 0
004aef4d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004aef50 83c110                   add        ecx, 0x10
004aef53 e86852f6ff               call       0x4141c0
004aef58 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004aef5b 83c120                   add        ecx, 0x20
004aef5e e87dffffff               call       0x4aeee0
004aef63 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004aef66 83c134                   add        ecx, 0x34
004aef69 e872ffffff               call       0x4aeee0
004aef6e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004aef71 c7414801000000           mov        dword ptr [ecx + 0x48], 1
004aef78 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004aef7b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004aef7e 64890d00000000           mov        dword ptr fs:[0], ecx
004aef85 59                       pop        ecx
004aef86 8be5                     mov        esp, ebp
004aef88 5d                       pop        ebp
004aef89 c3                       ret        
004aef8a cc                       int3       
004aef8b cc                       int3       
004aef8c cc                       int3       
004aef8d cc                       int3       
004aef8e cc                       int3       
004aef8f cc                       int3       
