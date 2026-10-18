0047d3d0 55                       push       ebp
0047d3d1 8bec                     mov        ebp, esp
0047d3d3 83ec14                   sub        esp, 0x14
0047d3d6 894dfc                   mov        dword ptr [ebp - 4], ecx
0047d3d9 837d0c00                 cmp        dword ptr [ebp + 0xc], 0
0047d3dd 0f848a010000             je         0x47d56d
0047d3e3 51                       push       ecx
0047d3e4 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
0047d3ec f30f110424               movss      dword ptr [esp], xmm0
0047d3f1 51                       push       ecx
0047d3f2 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
0047d3fa f30f110424               movss      dword ptr [esp], xmm0
0047d3ff 8b4508                   mov        eax, dword ptr [ebp + 8]
0047d402 50                       push       eax
0047d403 e8887f0000               call       0x485390
0047d408 83c40c                   add        esp, 0xc
0047d40b 85c0                     test       eax, eax
0047d40d 0f855a010000             jne        0x47d56d
0047d413 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047d416 8b8920050000             mov        ecx, dword ptr [ecx + 0x520]
0047d41c e8dfeef8ff               call       0x40c300
0047d421 05806d2800               add        eax, 0x286d80
0047d426 8945f8                   mov        dword ptr [ebp - 8], eax
0047d429 8b55f8                   mov        edx, dword ptr [ebp - 8]
0047d42c 8b02                     mov        eax, dword ptr [edx]
0047d42e 83c001                   add        eax, 1
0047d431 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0047d434 8901                     mov        dword ptr [ecx], eax
0047d436 837d0c01                 cmp        dword ptr [ebp + 0xc], 1
0047d43a 7527                     jne        0x47d463
0047d43c 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047d43f 8b8a20050000             mov        ecx, dword ptr [edx + 0x520]
0047d445 e866f6f9ff               call       0x41cab0
0047d44a 8945f4                   mov        dword ptr [ebp - 0xc], eax
0047d44d 6a0d                     push       0xd
0047d44f 6a01                     push       1
0047d451 8b4508                   mov        eax, dword ptr [ebp + 8]
0047d454 50                       push       eax
0047d455 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0047d458 e853710400               call       0x4c45b0
0047d45d 90                       nop        
0047d45e e90a010000               jmp        0x47d56d
0047d463 837d0c04                 cmp        dword ptr [ebp + 0xc], 4
0047d467 0f8500010000             jne        0x47d56d
0047d46d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047d470 8b8920050000             mov        ecx, dword ptr [ecx + 0x520]
0047d476 e885eef8ff               call       0x40c300
0047d47b 8b80806d2800             mov        eax, dword ptr [eax + 0x286d80]
0047d481 99                       cdq        
0047d482 b905000000               mov        ecx, 5
0047d487 f7f9                     idiv       ecx
0047d489 85d2                     test       edx, edx
0047d48b 7470                     je         0x47d4fd
0047d48d 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047d490 8b8a20050000             mov        ecx, dword ptr [edx + 0x520]
0047d496 e865eef8ff               call       0x40c300
0047d49b 8b80806d2800             mov        eax, dword ptr [eax + 0x286d80]
0047d4a1 99                       cdq        
0047d4a2 b905000000               mov        ecx, 5
0047d4a7 f7f9                     idiv       ecx
0047d4a9 83fa02                   cmp        edx, 2
0047d4ac 7517                     jne        0x47d4c5
0047d4ae b9a8a45b00               mov        ecx, 0x5ba4a8
0047d4b3 e8286afaff               call       0x423ee0
0047d4b8 33d2                     xor        edx, edx
0047d4ba b902000000               mov        ecx, 2
0047d4bf f7f1                     div        ecx
0047d4c1 85d2                     test       edx, edx
0047d4c3 7438                     je         0x47d4fd
0047d4c5 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047d4c8 8b8a20050000             mov        ecx, dword ptr [edx + 0x520]
0047d4ce e82deef8ff               call       0x40c300
0047d4d3 8b80806d2800             mov        eax, dword ptr [eax + 0x286d80]
0047d4d9 99                       cdq        
0047d4da b905000000               mov        ecx, 5
0047d4df f7f9                     idiv       ecx
0047d4e1 83fa04                   cmp        edx, 4
0047d4e4 7565                     jne        0x47d54b
0047d4e6 b9a8a45b00               mov        ecx, 0x5ba4a8
0047d4eb e8f069faff               call       0x423ee0
0047d4f0 33d2                     xor        edx, edx
0047d4f2 b903000000               mov        ecx, 3
0047d4f7 f7f1                     div        ecx
0047d4f9 85d2                     test       edx, edx
0047d4fb 754e                     jne        0x47d54b
0047d4fd 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047d500 8b8a20050000             mov        ecx, dword ptr [edx + 0x520]
0047d506 e8a5f5f9ff               call       0x41cab0
0047d50b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0047d50e 6aff                     push       -1
0047d510 6a00                     push       0
0047d512 6a00                     push       0
0047d514 51                       push       ecx
0047d515 f30f100534fd5600         movss      xmm0, dword ptr [0x56fd34]
0047d51d f30f110424               movss      dword ptr [esp], xmm0
0047d522 f30f1005f8e05600         movss      xmm0, dword ptr [0x56e0f8]
0047d52a f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0047d532 51                       push       ecx
0047d533 f30f110424               movss      dword ptr [esp], xmm0
0047d538 6aff                     push       -1
0047d53a 8b4508                   mov        eax, dword ptr [ebp + 8]
0047d53d 50                       push       eax
0047d53e 6a02                     push       2
0047d540 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047d543 e848670400               call       0x4c3c90
0047d548 90                       nop        
0047d549 eb22                     jmp        0x47d56d
0047d54b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047d54e 8b8920050000             mov        ecx, dword ptr [ecx + 0x520]
0047d554 e857f5f9ff               call       0x41cab0
0047d559 8945ec                   mov        dword ptr [ebp - 0x14], eax
0047d55c 6a0d                     push       0xd
0047d55e 6a01                     push       1
0047d560 8b5508                   mov        edx, dword ptr [ebp + 8]
0047d563 52                       push       edx
0047d564 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047d567 e844700400               call       0x4c45b0
0047d56c 90                       nop        
0047d56d 8be5                     mov        esp, ebp
0047d56f 5d                       pop        ebp
0047d570 c20800                   ret        8
0047d573 cc                       int3       
0047d574 cc                       int3       
0047d575 cc                       int3       
0047d576 cc                       int3       
0047d577 cc                       int3       
0047d578 cc                       int3       
0047d579 cc                       int3       
0047d57a cc                       int3       
0047d57b cc                       int3       
0047d57c cc                       int3       
0047d57d cc                       int3       
0047d57e cc                       int3       
0047d57f cc                       int3       
