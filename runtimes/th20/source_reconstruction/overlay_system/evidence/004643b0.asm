004643b0 55                       push       ebp
004643b1 8bec                     mov        ebp, esp
004643b3 83ec08                   sub        esp, 8
004643b6 894dfc                   mov        dword ptr [ebp - 4], ecx
004643b9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004643bc 83783400                 cmp        dword ptr [eax + 0x34], 0
004643c0 7423                     je         0x4643e5
004643c2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004643c5 8b5134                   mov        edx, dword ptr [ecx + 0x34]
004643c8 8b45fc                   mov        eax, dword ptr [ebp - 4]
004643cb 8b12                     mov        edx, dword ptr [edx]
004643cd 8b4834                   mov        ecx, dword ptr [eax + 0x34]
004643d0 8b4274                   mov        eax, dword ptr [edx + 0x74]
004643d3 ffd0                     call       eax
004643d5 0fb6c8                   movzx      ecx, al
004643d8 85c9                     test       ecx, ecx
004643da 7409                     je         0x4643e5
004643dc c745f801000000           mov        dword ptr [ebp - 8], 1
004643e3 eb07                     jmp        0x4643ec
004643e5 c745f800000000           mov        dword ptr [ebp - 8], 0
004643ec 0fb645f8                 movzx      eax, byte ptr [ebp - 8]
004643f0 8be5                     mov        esp, ebp
004643f2 5d                       pop        ebp
004643f3 c3                       ret        
004643f4 cc                       int3       
004643f5 cc                       int3       
004643f6 cc                       int3       
004643f7 cc                       int3       
004643f8 cc                       int3       
004643f9 cc                       int3       
004643fa cc                       int3       
004643fb cc                       int3       
004643fc cc                       int3       
004643fd cc                       int3       
004643fe cc                       int3       
004643ff cc                       int3       
