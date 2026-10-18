0047a400 55                       push       ebp
0047a401 8bec                     mov        ebp, esp
0047a403 83ec08                   sub        esp, 8
0047a406 894dfc                   mov        dword ptr [ebp - 4], ecx
0047a409 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
0047a40e f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0047a416 f30f104d08               movss      xmm1, dword ptr [ebp + 8]
0047a41b f30f5cc8                 subss      xmm1, xmm0
0047a41f 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a422 0f2f08                   comiss     xmm1, dword ptr [eax]
0047a425 7765                     ja         0x47a48c
0047a427 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
0047a42c f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0047a434 f30f584508               addss      xmm0, dword ptr [ebp + 8]
0047a439 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047a43c f30f1009                 movss      xmm1, dword ptr [ecx]
0047a440 0f2fc8                   comiss     xmm1, xmm0
0047a443 7747                     ja         0x47a48c
0047a445 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
0047a44a f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0047a452 f30f104d0c               movss      xmm1, dword ptr [ebp + 0xc]
0047a457 f30f5cc8                 subss      xmm1, xmm0
0047a45b 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047a45e 0f2f4a04                 comiss     xmm1, dword ptr [edx + 4]
0047a462 7728                     ja         0x47a48c
0047a464 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
0047a469 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0047a471 f30f58450c               addss      xmm0, dword ptr [ebp + 0xc]
0047a476 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a479 f30f104804               movss      xmm1, dword ptr [eax + 4]
0047a47e 0f2fc8                   comiss     xmm1, xmm0
0047a481 7709                     ja         0x47a48c
0047a483 c745f800000000           mov        dword ptr [ebp - 8], 0
0047a48a eb07                     jmp        0x47a493
0047a48c c745f801000000           mov        dword ptr [ebp - 8], 1
0047a493 8b45f8                   mov        eax, dword ptr [ebp - 8]
0047a496 8be5                     mov        esp, ebp
0047a498 5d                       pop        ebp
0047a499 c21000                   ret        0x10
0047a49c cc                       int3       
0047a49d cc                       int3       
0047a49e cc                       int3       
0047a49f cc                       int3       
