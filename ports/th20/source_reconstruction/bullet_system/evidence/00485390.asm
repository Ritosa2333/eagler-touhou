00485390 55                       push       ebp
00485391 8bec                     mov        ebp, esp
00485393 51                       push       ecx
00485394 8b4508                   mov        eax, dword ptr [ebp + 8]
00485397 f30f1000                 movss      xmm0, dword ptr [eax]
0048539b f30f58450c               addss      xmm0, dword ptr [ebp + 0xc]
004853a0 f30f100d6cfd5600         movss      xmm1, dword ptr [0x56fd6c]
004853a8 f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
004853b0 0f2fc8                   comiss     xmm1, xmm0
004853b3 7355                     jae        0x48540a
004853b5 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004853b8 f30f1001                 movss      xmm0, dword ptr [ecx]
004853bc f30f5c450c               subss      xmm0, dword ptr [ebp + 0xc]
004853c1 f30f100da4cd5600         movss      xmm1, dword ptr [0x56cda4]
004853c9 f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
004853d1 0f2fc1                   comiss     xmm0, xmm1
004853d4 7334                     jae        0x48540a
004853d6 8b5508                   mov        edx, dword ptr [ebp + 8]
004853d9 f30f104204               movss      xmm0, dword ptr [edx + 4]
004853de f30f584510               addss      xmm0, dword ptr [ebp + 0x10]
004853e3 0f57c9                   xorps      xmm1, xmm1
004853e6 0f2fc8                   comiss     xmm1, xmm0
004853e9 731f                     jae        0x48540a
004853eb 8b4508                   mov        eax, dword ptr [ebp + 8]
004853ee f30f104004               movss      xmm0, dword ptr [eax + 4]
004853f3 f30f5c4510               subss      xmm0, dword ptr [ebp + 0x10]
004853f8 0f2f05c0d75600           comiss     xmm0, dword ptr [0x56d7c0]
004853ff 7309                     jae        0x48540a
00485401 c745fc00000000           mov        dword ptr [ebp - 4], 0
00485408 eb07                     jmp        0x485411
0048540a c745fc01000000           mov        dword ptr [ebp - 4], 1
00485411 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485414 8be5                     mov        esp, ebp
00485416 5d                       pop        ebp
00485417 c3                       ret        
00485418 cc                       int3       
00485419 cc                       int3       
0048541a cc                       int3       
0048541b cc                       int3       
0048541c cc                       int3       
0048541d cc                       int3       
0048541e cc                       int3       
0048541f cc                       int3       
