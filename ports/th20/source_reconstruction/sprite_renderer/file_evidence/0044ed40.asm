0044ed40 55                       push       ebp
0044ed41 8bec                     mov        ebp, esp
0044ed43 6aff                     push       -1
0044ed45 68ed765600               push       0x5676ed
0044ed4a 64a100000000             mov        eax, dword ptr fs:[0]
0044ed50 50                       push       eax
0044ed51 83ec18                   sub        esp, 0x18
0044ed54 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0044ed59 33c5                     xor        eax, ebp
0044ed5b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0044ed5e 50                       push       eax
0044ed5f 8d45f4                   lea        eax, [ebp - 0xc]
0044ed62 64a300000000             mov        dword ptr fs:[0], eax
0044ed68 894de0                   mov        dword ptr [ebp - 0x20], ecx
0044ed6b 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0044ed6e 50                       push       eax
0044ed6f 6878e35600               push       0x56e378
0044ed74 e837d9fbff               call       0x40c6b0
0044ed79 83c408                   add        esp, 8
0044ed7c 837d082a                 cmp        dword ptr [ebp + 8], 0x2a
0044ed80 7c19                     jl         0x44ed9b
0044ed82 688ce35600               push       0x56e38c
0044ed87 6878065c00               push       0x5c0678
0044ed8c e89f540000               call       0x454230
0044ed91 83c408                   add        esp, 8
0044ed94 33c0                     xor        eax, eax
0044ed96 e997000000               jmp        0x44ee32
0044ed9b 6a11                     push       0x11
0044ed9d b940025c00               mov        ecx, 0x5c0240
0044eda2 e869d5fbff               call       0x40c310
0044eda7 50                       push       eax
0044eda8 8d4dec                   lea        ecx, [ebp - 0x14]
0044edab e880c9fbff               call       0x40b730
0044edb0 c745fc00000000           mov        dword ptr [ebp - 4], 0
0044edb7 68b0e35600               push       0x56e3b0
0044edbc 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0044edc2 e8e989ffff               call       0x4477b0
0044edc7 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0044edca 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0044edcd 81c130070006             add        ecx, 0x6000730
0044edd3 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0044edd6 8b5508                   mov        edx, dword ptr [ebp + 8]
0044edd9 52                       push       edx
0044edda 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0044eddd e82efdfeff               call       0x43eb10
0044ede2 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0044ede5 8908                     mov        dword ptr [eax], ecx
0044ede7 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0044edea 8955e8                   mov        dword ptr [ebp - 0x18], edx
0044eded 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0044edf0 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044edf3 8908                     mov        dword ptr [eax], ecx
0044edf5 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0044edfc 8d4dec                   lea        ecx, [ebp - 0x14]
0044edff e8fccafbff               call       0x40b900
0044ee04 90                       nop        
0044ee05 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0044ee08 52                       push       edx
0044ee09 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0044ee0c e8efb8ffff               call       0x44a700
0044ee11 85c0                     test       eax, eax
0044ee13 741a                     je         0x44ee2f
0044ee15 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0044ee18 50                       push       eax
0044ee19 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0044ee1f e87c85ffff               call       0x4473a0
0044ee24 c745e800000000           mov        dword ptr [ebp - 0x18], 0
0044ee2b 33c0                     xor        eax, eax
0044ee2d eb03                     jmp        0x44ee32
0044ee2f 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0044ee32 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044ee35 64890d00000000           mov        dword ptr fs:[0], ecx
0044ee3c 59                       pop        ecx
0044ee3d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0044ee40 33cd                     xor        ecx, ebp
0044ee42 e8a73b0f00               call       0x5429ee
0044ee47 8be5                     mov        esp, ebp
0044ee49 5d                       pop        ebp
0044ee4a c20800                   ret        8
0044ee4d cc                       int3       
0044ee4e cc                       int3       
0044ee4f cc                       int3       
