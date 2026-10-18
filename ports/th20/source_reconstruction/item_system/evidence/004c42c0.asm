004c42c0 55                       push       ebp
004c42c1 8bec                     mov        ebp, esp
004c42c3 83ec1c                   sub        esp, 0x1c
004c42c6 894dfc                   mov        dword ptr [ebp - 4], ecx
004c42c9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c42cc 83b8280c000004           cmp        dword ptr [eax + 0xc28], 4
004c42d3 7434                     je         0x4c4309
004c42d5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c42d8 83b9280c000006           cmp        dword ptr [ecx + 0xc28], 6
004c42df 7428                     je         0x4c4309
004c42e1 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c42e4 83ba280c00000e           cmp        dword ptr [edx + 0xc28], 0xe
004c42eb 741c                     je         0x4c4309
004c42ed 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c42f0 83b8280c000005           cmp        dword ptr [eax + 0xc28], 5
004c42f7 7410                     je         0x4c4309
004c42f9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c42fc 83b9280c000007           cmp        dword ptr [ecx + 0xc28], 7
004c4303 0f8585000000             jne        0x4c438e
004c4309 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c430c 8b8a480c0000             mov        ecx, dword ptr [edx + 0xc48]
004c4312 e8b987f5ff               call       0x41cad0
004c4317 8945f8                   mov        dword ptr [ebp - 8], eax
004c431a 6a00                     push       0
004c431c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c431f e8dc8df9ff               call       0x45d100
004c4324 8945f4                   mov        dword ptr [ebp - 0xc], eax
004c4327 6a00                     push       0
004c4329 6aff                     push       -1
004c432b 51                       push       ecx
004c432c 0f57c0                   xorps      xmm0, xmm0
004c432f f30f110424               movss      dword ptr [esp], xmm0
004c4334 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4337 05e40b0000               add        eax, 0xbe4
004c433c 50                       push       eax
004c433d 6a5e                     push       0x5e
004c433f 6884ec5600               push       0x56ec84
004c4344 8d4de8                   lea        ecx, [ebp - 0x18]
004c4347 51                       push       ecx
004c4348 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c434b e8904dfbff               call       0x4790e0
004c4350 90                       nop        
004c4351 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c4354 83ba280c000004           cmp        dword ptr [edx + 0xc28], 4
004c435b 740c                     je         0x4c4369
004c435d 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4360 83b8280c000005           cmp        dword ptr [eax + 0xc28], 5
004c4367 7511                     jne        0x4c437a
004c4369 6a00                     push       0
004c436b 6a4a                     push       0x4a
004c436d b930a85b00               mov        ecx, 0x5ba830
004c4372 e8f929f6ff               call       0x426d70
004c4377 90                       nop        
004c4378 eb0f                     jmp        0x4c4389
004c437a 6a00                     push       0
004c437c 6a30                     push       0x30
004c437e b930a85b00               mov        ecx, 0x5ba830
004c4383 e8e829f6ff               call       0x426d70
004c4388 90                       nop        
004c4389 e984000000               jmp        0x4c4412
004c438e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4391 83b9280c000001           cmp        dword ptr [ecx + 0xc28], 1
004c4398 740c                     je         0x4c43a6
004c439a 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c439d 83ba280c000002           cmp        dword ptr [edx + 0xc28], 2
004c43a4 756c                     jne        0x4c4412
004c43a6 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c43a9 8b88480c0000             mov        ecx, dword ptr [eax + 0xc48]
004c43af e81c87f5ff               call       0x41cad0
004c43b4 8945f0                   mov        dword ptr [ebp - 0x10], eax
004c43b7 6a00                     push       0
004c43b9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c43bc e83f8df9ff               call       0x45d100
004c43c1 8945ec                   mov        dword ptr [ebp - 0x14], eax
004c43c4 6a00                     push       0
004c43c6 6aff                     push       -1
004c43c8 51                       push       ecx
004c43c9 0f57c0                   xorps      xmm0, xmm0
004c43cc f30f110424               movss      dword ptr [esp], xmm0
004c43d1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c43d4 81c1e40b0000             add        ecx, 0xbe4
004c43da 51                       push       ecx
004c43db 6a5e                     push       0x5e
004c43dd 6884ec5600               push       0x56ec84
004c43e2 8d55e4                   lea        edx, [ebp - 0x1c]
004c43e5 52                       push       edx
004c43e6 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c43e9 e8f24cfbff               call       0x4790e0
004c43ee 90                       nop        
004c43ef 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c43f2 83b8400c000000           cmp        dword ptr [eax + 0xc40], 0
004c43f9 7c17                     jl         0x4c4412
004c43fb 6a00                     push       0
004c43fd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4400 8b91400c0000             mov        edx, dword ptr [ecx + 0xc40]
004c4406 52                       push       edx
004c4407 b930a85b00               mov        ecx, 0x5ba830
004c440c e85f29f6ff               call       0x426d70
004c4411 90                       nop        
004c4412 33c0                     xor        eax, eax
004c4414 8be5                     mov        esp, ebp
004c4416 5d                       pop        ebp
004c4417 c3                       ret        
004c4418 cc                       int3       
004c4419 cc                       int3       
004c441a cc                       int3       
004c441b cc                       int3       
004c441c cc                       int3       
004c441d cc                       int3       
004c441e cc                       int3       
004c441f cc                       int3       
