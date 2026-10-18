004854d0 55                       push       ebp
004854d1 8bec                     mov        ebp, esp
004854d3 51                       push       ecx
004854d4 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004854d9 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004854e1 8b4508                   mov        eax, dword ptr [ebp + 8]
004854e4 f30f5800                 addss      xmm0, dword ptr [eax]
004854e8 f30f100d6cfd5600         movss      xmm1, dword ptr [0x56fd6c]
004854f0 f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
004854f8 0f2fc8                   comiss     xmm1, xmm0
004854fb 7375                     jae        0x485572
004854fd f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00485502 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0048550a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0048550d f30f1009                 movss      xmm1, dword ptr [ecx]
00485511 f30f5cc8                 subss      xmm1, xmm0
00485515 f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
0048551d f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00485525 0f2fc8                   comiss     xmm1, xmm0
00485528 7348                     jae        0x485572
0048552a f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
0048552f f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00485537 8b5508                   mov        edx, dword ptr [ebp + 8]
0048553a f30f584204               addss      xmm0, dword ptr [edx + 4]
0048553f 0f57c9                   xorps      xmm1, xmm1
00485542 0f2fc8                   comiss     xmm1, xmm0
00485545 732b                     jae        0x485572
00485547 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
0048554c f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00485554 8b4508                   mov        eax, dword ptr [ebp + 8]
00485557 f30f104804               movss      xmm1, dword ptr [eax + 4]
0048555c f30f5cc8                 subss      xmm1, xmm0
00485560 0f2f0dc0d75600           comiss     xmm1, dword ptr [0x56d7c0]
00485567 7309                     jae        0x485572
00485569 c745fc00000000           mov        dword ptr [ebp - 4], 0
00485570 eb07                     jmp        0x485579
00485572 c745fc01000000           mov        dword ptr [ebp - 4], 1
00485579 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048557c 8be5                     mov        esp, ebp
0048557e 5d                       pop        ebp
0048557f c3                       ret        
