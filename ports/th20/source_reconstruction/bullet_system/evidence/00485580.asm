00485580 55                       push       ebp
00485581 8bec                     mov        ebp, esp
00485583 51                       push       ecx
00485584 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00485589 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00485591 8b4508                   mov        eax, dword ptr [ebp + 8]
00485594 f30f5800                 addss      xmm0, dword ptr [eax]
00485598 f30f100d6cfd5600         movss      xmm1, dword ptr [0x56fd6c]
004855a0 f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
004855a8 0f2fc8                   comiss     xmm1, xmm0
004855ab 737a                     jae        0x485627
004855ad f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004855b2 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004855ba 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004855bd f30f1009                 movss      xmm1, dword ptr [ecx]
004855c1 f30f5cc8                 subss      xmm1, xmm0
004855c5 f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
004855cd f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004855d5 0f2fc8                   comiss     xmm1, xmm0
004855d8 734d                     jae        0x485627
004855da f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004855df f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004855e7 8b5508                   mov        edx, dword ptr [ebp + 8]
004855ea f30f584204               addss      xmm0, dword ptr [edx + 4]
004855ef f30f100d68fd5600         movss      xmm1, dword ptr [0x56fd68]
004855f7 0f2fc8                   comiss     xmm1, xmm0
004855fa 732b                     jae        0x485627
004855fc f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00485601 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00485609 8b4508                   mov        eax, dword ptr [ebp + 8]
0048560c f30f104804               movss      xmm1, dword ptr [eax + 4]
00485611 f30f5cc8                 subss      xmm1, xmm0
00485615 0f2f0dc0d75600           comiss     xmm1, dword ptr [0x56d7c0]
0048561c 7309                     jae        0x485627
0048561e c745fc00000000           mov        dword ptr [ebp - 4], 0
00485625 eb07                     jmp        0x48562e
00485627 c745fc01000000           mov        dword ptr [ebp - 4], 1
0048562e 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485631 8be5                     mov        esp, ebp
00485633 5d                       pop        ebp
00485634 c3                       ret        
00485635 cc                       int3       
00485636 cc                       int3       
00485637 cc                       int3       
00485638 cc                       int3       
00485639 cc                       int3       
0048563a cc                       int3       
0048563b cc                       int3       
0048563c cc                       int3       
0048563d cc                       int3       
0048563e cc                       int3       
0048563f cc                       int3       
