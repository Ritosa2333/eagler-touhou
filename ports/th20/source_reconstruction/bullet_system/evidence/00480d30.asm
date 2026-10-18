00480d30 55                       push       ebp
00480d31 8bec                     mov        ebp, esp
00480d33 51                       push       ecx
00480d34 8b4508                   mov        eax, dword ptr [ebp + 8]
00480d37 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
00480d3a f30f1000                 movss      xmm0, dword ptr [eax]
00480d3e f30f594104               mulss      xmm0, dword ptr [ecx + 4]
00480d43 8b5508                   mov        edx, dword ptr [ebp + 8]
00480d46 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00480d49 f30f104a04               movss      xmm1, dword ptr [edx + 4]
00480d4e f30f5908                 mulss      xmm1, dword ptr [eax]
00480d52 f30f5cc1                 subss      xmm0, xmm1
00480d56 f30f1145fc               movss      dword ptr [ebp - 4], xmm0
00480d5b d945fc                   fld        dword ptr [ebp - 4]
00480d5e 8be5                     mov        esp, ebp
00480d60 5d                       pop        ebp
00480d61 c3                       ret        
00480d62 cc                       int3       
00480d63 cc                       int3       
00480d64 cc                       int3       
00480d65 cc                       int3       
00480d66 cc                       int3       
00480d67 cc                       int3       
00480d68 cc                       int3       
00480d69 cc                       int3       
00480d6a cc                       int3       
00480d6b cc                       int3       
00480d6c cc                       int3       
00480d6d cc                       int3       
00480d6e cc                       int3       
00480d6f cc                       int3       
