004c82c0 55                       push       ebp
004c82c1 8bec                     mov        ebp, esp
004c82c3 6aff                     push       -1
004c82c5 68a0795600               push       0x5679a0
004c82ca 64a100000000             mov        eax, dword ptr fs:[0]
004c82d0 50                       push       eax
004c82d1 83ec08                   sub        esp, 8
004c82d4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c82d9 33c5                     xor        eax, ebp
004c82db 50                       push       eax
004c82dc 8d45f4                   lea        eax, [ebp - 0xc]
004c82df 64a300000000             mov        dword ptr fs:[0], eax
004c82e5 894df0                   mov        dword ptr [ebp - 0x10], ecx
004c82e8 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c82eb c70000000000             mov        dword ptr [eax], 0
004c82f1 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c82f4 c7410400000000           mov        dword ptr [ecx + 4], 0
004c82fb 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c82fe 0f57c0                   xorps      xmm0, xmm0
004c8301 f30f114208               movss      dword ptr [edx + 8], xmm0
004c8306 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c8309 0f57c0                   xorps      xmm0, xmm0
004c830c f30f11400c               movss      dword ptr [eax + 0xc], xmm0
004c8311 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8314 c7411000000000           mov        dword ptr [ecx + 0x10], 0
004c831b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c831e 83c114                   add        ecx, 0x14
004c8321 e8eaaaf5ff               call       0x422e10
004c8326 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8329 83c120                   add        ecx, 0x20
004c832c e8dfaaf5ff               call       0x422e10
004c8331 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c8334 83c22c                   add        edx, 0x2c
004c8337 8955ec                   mov        dword ptr [ebp - 0x14], edx
004c833a 51                       push       ecx
004c833b 0f57c0                   xorps      xmm0, xmm0
004c833e f30f110424               movss      dword ptr [esp], xmm0
004c8343 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c8346 e8c50ef6ff               call       0x429210
004c834b 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c834e 0f57c0                   xorps      xmm0, xmm0
004c8351 f30f114030               movss      dword ptr [eax + 0x30], xmm0
004c8356 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c8359 0f57c0                   xorps      xmm0, xmm0
004c835c f30f114134               movss      dword ptr [ecx + 0x34], xmm0
004c8361 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004c8364 0f57c0                   xorps      xmm0, xmm0
004c8367 f30f114238               movss      dword ptr [edx + 0x38], xmm0
004c836c 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c836f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c8372 64890d00000000           mov        dword ptr fs:[0], ecx
004c8379 59                       pop        ecx
004c837a 8be5                     mov        esp, ebp
004c837c 5d                       pop        ebp
004c837d c3                       ret        
004c837e cc                       int3       
004c837f cc                       int3       
004c8380 cc                       int3       
004c8381 cc                       int3       
004c8382 cc                       int3       
004c8383 cc                       int3       
004c8384 cc                       int3       
004c8385 cc                       int3       
004c8386 cc                       int3       
004c8387 cc                       int3       
004c8388 cc                       int3       
004c8389 cc                       int3       
004c838a cc                       int3       
004c838b cc                       int3       
004c838c cc                       int3       
004c838d cc                       int3       
004c838e cc                       int3       
004c838f cc                       int3       
