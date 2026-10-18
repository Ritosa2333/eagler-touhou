0052f840 55                       push       ebp
0052f841 8bec                     mov        ebp, esp
0052f843 6aff                     push       -1
0052f845 68cd795600               push       0x5679cd
0052f84a 64a100000000             mov        eax, dword ptr fs:[0]
0052f850 50                       push       eax
0052f851 83ec10                   sub        esp, 0x10
0052f854 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052f859 33c5                     xor        eax, ebp
0052f85b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0052f85e 50                       push       eax
0052f85f 8d45f4                   lea        eax, [ebp - 0xc]
0052f862 64a300000000             mov        dword ptr fs:[0], eax
0052f868 894de8                   mov        dword ptr [ebp - 0x18], ecx
0052f86b 6a14                     push       0x14
0052f86d b940025c00               mov        ecx, 0x5c0240
0052f872 e899caedff               call       0x40c310
0052f877 50                       push       eax
0052f878 8d4dec                   lea        ecx, [ebp - 0x14]
0052f87b e8b0beedff               call       0x40b730
0052f880 c745fc00000000           mov        dword ptr [ebp - 4], 0
0052f887 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052f88a e85148f3ff               call       0x4640e0
0052f88f 83c068                   add        eax, 0x68
0052f892 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0052f895 8b4508                   mov        eax, dword ptr [ebp + 8]
0052f898 50                       push       eax
0052f899 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0052f89c e80fc9ffff               call       0x52c1b0
0052f8a1 c60001                   mov        byte ptr [eax], 1
0052f8a4 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052f8a7 e894f2f8ff               call       0x4beb40
0052f8ac c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0052f8b3 8d4dec                   lea        ecx, [ebp - 0x14]
0052f8b6 e845c0edff               call       0x40b900
0052f8bb 90                       nop        
0052f8bc 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052f8bf 64890d00000000           mov        dword ptr fs:[0], ecx
0052f8c6 59                       pop        ecx
0052f8c7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052f8ca 33cd                     xor        ecx, ebp
0052f8cc e81d310100               call       0x5429ee
0052f8d1 8be5                     mov        esp, ebp
0052f8d3 5d                       pop        ebp
0052f8d4 c20400                   ret        4
0052f8d7 cc                       int3       
0052f8d8 cc                       int3       
0052f8d9 cc                       int3       
0052f8da cc                       int3       
0052f8db cc                       int3       
0052f8dc cc                       int3       
0052f8dd cc                       int3       
0052f8de cc                       int3       
0052f8df cc                       int3       
