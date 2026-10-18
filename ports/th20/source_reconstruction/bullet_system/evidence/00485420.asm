00485420 55                       push       ebp
00485421 8bec                     mov        ebp, esp
00485423 51                       push       ecx
00485424 8b4508                   mov        eax, dword ptr [ebp + 8]
00485427 f30f1000                 movss      xmm0, dword ptr [eax]
0048542b f30f58450c               addss      xmm0, dword ptr [ebp + 0xc]
00485430 f30f104d14               movss      xmm1, dword ptr [ebp + 0x14]
00485435 0f570dd0d75600           xorps      xmm1, xmmword ptr [0x56d7d0]
0048543c f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
00485444 0f2fc8                   comiss     xmm1, xmm0
00485447 7379                     jae        0x4854c2
00485449 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0048544c f30f1001                 movss      xmm0, dword ptr [ecx]
00485450 f30f5c450c               subss      xmm0, dword ptr [ebp + 0xc]
00485455 f30f104d14               movss      xmm1, dword ptr [ebp + 0x14]
0048545a f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
00485462 0f2fc1                   comiss     xmm0, xmm1
00485465 735b                     jae        0x4854c2
00485467 8b5508                   mov        edx, dword ptr [ebp + 8]
0048546a f30f104204               movss      xmm0, dword ptr [edx + 4]
0048546f f30f584510               addss      xmm0, dword ptr [ebp + 0x10]
00485474 f30f104d18               movss      xmm1, dword ptr [ebp + 0x18]
00485479 f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
00485481 f30f101514ed5600         movss      xmm2, dword ptr [0x56ed14]
00485489 f30f5cd1                 subss      xmm2, xmm1
0048548d 0f2fd0                   comiss     xmm2, xmm0
00485490 7330                     jae        0x4854c2
00485492 8b4508                   mov        eax, dword ptr [ebp + 8]
00485495 f30f104004               movss      xmm0, dword ptr [eax + 4]
0048549a f30f5c4510               subss      xmm0, dword ptr [ebp + 0x10]
0048549f f30f104d18               movss      xmm1, dword ptr [ebp + 0x18]
004854a4 f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
004854ac f30f580d14ed5600         addss      xmm1, dword ptr [0x56ed14]
004854b4 0f2fc1                   comiss     xmm0, xmm1
004854b7 7309                     jae        0x4854c2
004854b9 c745fc00000000           mov        dword ptr [ebp - 4], 0
004854c0 eb07                     jmp        0x4854c9
004854c2 c745fc01000000           mov        dword ptr [ebp - 4], 1
004854c9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004854cc 8be5                     mov        esp, ebp
004854ce 5d                       pop        ebp
004854cf c3                       ret        
