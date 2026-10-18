00513ce0 55                       push       ebp
00513ce1 8bec                     mov        ebp, esp
00513ce3 83ec10                   sub        esp, 0x10
00513ce6 894dfc                   mov        dword ptr [ebp - 4], ecx
00513ce9 8b45fc                   mov        eax, dword ptr [ebp - 4]
00513cec f30f2a4018               cvtsi2ss   xmm0, dword ptr [eax + 0x18]
00513cf1 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00513cf6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00513cf9 83c108                   add        ecx, 8
00513cfc e8cffef0ff               call       0x423bd0
00513d01 d95df4                   fstp       dword ptr [ebp - 0xc]
00513d04 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00513d07 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
00513d0a 8955f0                   mov        dword ptr [ebp - 0x10], edx
00513d0d 51                       push       ecx
00513d0e f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00513d13 f30f110424               movss      dword ptr [esp], xmm0
00513d18 51                       push       ecx
00513d19 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00513d1e f30f110424               movss      dword ptr [esp], xmm0
00513d23 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00513d26 50                       push       eax
00513d27 e8c411f4ff               call       0x454ef0
00513d2c 83c40c                   add        esp, 0xc
00513d2f 8be5                     mov        esp, ebp
00513d31 5d                       pop        ebp
00513d32 c3                       ret        
00513d33 cc                       int3       
00513d34 cc                       int3       
00513d35 cc                       int3       
00513d36 cc                       int3       
00513d37 cc                       int3       
00513d38 cc                       int3       
00513d39 cc                       int3       
00513d3a cc                       int3       
00513d3b cc                       int3       
00513d3c cc                       int3       
00513d3d cc                       int3       
00513d3e cc                       int3       
00513d3f cc                       int3       
