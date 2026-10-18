004560d0 55                       push       ebp
004560d1 8bec                     mov        ebp, esp
004560d3 51                       push       ecx
004560d4 8b4508                   mov        eax, dword ptr [ebp + 8]
004560d7 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004560da f30f1000                 movss      xmm0, dword ptr [eax]
004560de f30f5c01                 subss      xmm0, dword ptr [ecx]
004560e2 8b5508                   mov        edx, dword ptr [ebp + 8]
004560e5 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004560e8 f30f100a                 movss      xmm1, dword ptr [edx]
004560ec f30f5c08                 subss      xmm1, dword ptr [eax]
004560f0 f30f59c1                 mulss      xmm0, xmm1
004560f4 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004560f7 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004560fa f30f104904               movss      xmm1, dword ptr [ecx + 4]
004560ff f30f5c4a04               subss      xmm1, dword ptr [edx + 4]
00456104 8b4508                   mov        eax, dword ptr [ebp + 8]
00456107 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0045610a f30f105004               movss      xmm2, dword ptr [eax + 4]
0045610f f30f5c5104               subss      xmm2, dword ptr [ecx + 4]
00456114 f30f59ca                 mulss      xmm1, xmm2
00456118 f30f58c1                 addss      xmm0, xmm1
0045611c f30f1145fc               movss      dword ptr [ebp - 4], xmm0
00456121 d945fc                   fld        dword ptr [ebp - 4]
00456124 8be5                     mov        esp, ebp
00456126 5d                       pop        ebp
00456127 c3                       ret        
00456128 cc                       int3       
00456129 cc                       int3       
0045612a cc                       int3       
0045612b cc                       int3       
0045612c cc                       int3       
0045612d cc                       int3       
0045612e cc                       int3       
0045612f cc                       int3       
