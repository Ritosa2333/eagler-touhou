00480d70 55                       push       ebp
00480d71 8bec                     mov        ebp, esp
00480d73 51                       push       ecx
00480d74 8b4508                   mov        eax, dword ptr [ebp + 8]
00480d77 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
00480d7a f30f1000                 movss      xmm0, dword ptr [eax]
00480d7e f30f5901                 mulss      xmm0, dword ptr [ecx]
00480d82 8b5508                   mov        edx, dword ptr [ebp + 8]
00480d85 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00480d88 f30f104a04               movss      xmm1, dword ptr [edx + 4]
00480d8d f30f594804               mulss      xmm1, dword ptr [eax + 4]
00480d92 f30f58c1                 addss      xmm0, xmm1
00480d96 f30f1145fc               movss      dword ptr [ebp - 4], xmm0
00480d9b d945fc                   fld        dword ptr [ebp - 4]
00480d9e 8be5                     mov        esp, ebp
00480da0 5d                       pop        ebp
00480da1 c3                       ret        
00480da2 cc                       int3       
00480da3 cc                       int3       
00480da4 cc                       int3       
00480da5 cc                       int3       
00480da6 cc                       int3       
00480da7 cc                       int3       
00480da8 cc                       int3       
00480da9 cc                       int3       
00480daa cc                       int3       
00480dab cc                       int3       
00480dac cc                       int3       
00480dad cc                       int3       
00480dae cc                       int3       
00480daf cc                       int3       
