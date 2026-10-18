0052ca70 55                       push       ebp
0052ca71 8bec                     mov        ebp, esp
0052ca73 6aff                     push       -1
0052ca75 68ed765600               push       0x5676ed
0052ca7a 64a100000000             mov        eax, dword ptr fs:[0]
0052ca80 50                       push       eax
0052ca81 83ec18                   sub        esp, 0x18
0052ca84 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052ca89 33c5                     xor        eax, ebp
0052ca8b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0052ca8e 50                       push       eax
0052ca8f 8d45f4                   lea        eax, [ebp - 0xc]
0052ca92 64a300000000             mov        dword ptr fs:[0], eax
0052ca98 894de8                   mov        dword ptr [ebp - 0x18], ecx
0052ca9b 6a14                     push       0x14
0052ca9d b940025c00               mov        ecx, 0x5c0240
0052caa2 e869f8edff               call       0x40c310
0052caa7 50                       push       eax
0052caa8 8d4dec                   lea        ecx, [ebp - 0x14]
0052caab e880ecedff               call       0x40b730
0052cab0 c745fc00000000           mov        dword ptr [ebp - 4], 0
0052cab7 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052caba e8f174f3ff               call       0x463fb0
0052cabf 90                       nop        
0052cac0 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052cac3 e81876f3ff               call       0x4640e0
0052cac8 83c068                   add        eax, 0x68
0052cacb 8945e0                   mov        dword ptr [ebp - 0x20], eax
0052cace 8b4508                   mov        eax, dword ptr [ebp + 8]
0052cad1 50                       push       eax
0052cad2 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052cad5 e8d6f6ffff               call       0x52c1b0
0052cada 0fb608                   movzx      ecx, byte ptr [eax]
0052cadd 85c9                     test       ecx, ecx
0052cadf 7409                     je         0x52caea
0052cae1 c745e401000000           mov        dword ptr [ebp - 0x1c], 1
0052cae8 eb07                     jmp        0x52caf1
0052caea c745e400000000           mov        dword ptr [ebp - 0x1c], 0
0052caf1 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0052caf4 8955dc                   mov        dword ptr [ebp - 0x24], edx
0052caf7 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0052cafe 8d4dec                   lea        ecx, [ebp - 0x14]
0052cb01 e8faededff               call       0x40b900
0052cb06 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0052cb09 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052cb0c 64890d00000000           mov        dword ptr fs:[0], ecx
0052cb13 59                       pop        ecx
0052cb14 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052cb17 33cd                     xor        ecx, ebp
0052cb19 e8d05e0100               call       0x5429ee
0052cb1e 8be5                     mov        esp, ebp
0052cb20 5d                       pop        ebp
0052cb21 c20400                   ret        4
0052cb24 cc                       int3       
0052cb25 cc                       int3       
0052cb26 cc                       int3       
0052cb27 cc                       int3       
0052cb28 cc                       int3       
0052cb29 cc                       int3       
0052cb2a cc                       int3       
0052cb2b cc                       int3       
0052cb2c cc                       int3       
0052cb2d cc                       int3       
0052cb2e cc                       int3       
0052cb2f cc                       int3       
