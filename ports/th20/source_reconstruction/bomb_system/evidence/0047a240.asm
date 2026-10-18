0047a240 55                       push       ebp
0047a241 8bec                     mov        ebp, esp
0047a243 6aff                     push       -1
0047a245 6800765600               push       0x567600
0047a24a 64a100000000             mov        eax, dword ptr fs:[0]
0047a250 50                       push       eax
0047a251 51                       push       ecx
0047a252 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0047a257 33c5                     xor        eax, ebp
0047a259 50                       push       eax
0047a25a 8d45f4                   lea        eax, [ebp - 0xc]
0047a25d 64a300000000             mov        dword ptr fs:[0], eax
0047a263 894df0                   mov        dword ptr [ebp - 0x10], ecx
0047a266 51                       push       ecx
0047a267 f30f104508               movss      xmm0, dword ptr [ebp + 8]
0047a26c f30f110424               movss      dword ptr [esp], xmm0
0047a271 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047a274 83c11c                   add        ecx, 0x1c
0047a277 e8448efdff               call       0x4530c0
0047a27c 90                       nop        
0047a27d 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0047a280 64890d00000000           mov        dword ptr fs:[0], ecx
0047a287 59                       pop        ecx
0047a288 8be5                     mov        esp, ebp
0047a28a 5d                       pop        ebp
0047a28b c20400                   ret        4
0047a28e cc                       int3       
0047a28f cc                       int3       
0047a290 cc                       int3       
0047a291 cc                       int3       
0047a292 cc                       int3       
0047a293 cc                       int3       
0047a294 cc                       int3       
0047a295 cc                       int3       
0047a296 cc                       int3       
0047a297 cc                       int3       
0047a298 cc                       int3       
0047a299 cc                       int3       
0047a29a cc                       int3       
0047a29b cc                       int3       
0047a29c cc                       int3       
0047a29d cc                       int3       
0047a29e cc                       int3       
0047a29f cc                       int3       
