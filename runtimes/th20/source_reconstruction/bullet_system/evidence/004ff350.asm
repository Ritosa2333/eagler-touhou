004ff350 55                       push       ebp
004ff351 8bec                     mov        ebp, esp
004ff353 83ec10                   sub        esp, 0x10
004ff356 894dfc                   mov        dword ptr [ebp - 4], ecx
004ff359 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ff35c e8ef14f6ff               call       0x460850
004ff361 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004ff364 f30f1000                 movss      xmm0, dword ptr [eax]
004ff368 f30f5c01                 subss      xmm0, dword ptr [ecx]
004ff36c f30f1145f8               movss      dword ptr [ebp - 8], xmm0
004ff371 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ff374 e8d714f6ff               call       0x460850
004ff379 8b5508                   mov        edx, dword ptr [ebp + 8]
004ff37c f30f104004               movss      xmm0, dword ptr [eax + 4]
004ff381 f30f5c4204               subss      xmm0, dword ptr [edx + 4]
004ff386 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004ff38b f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
004ff390 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
004ff397 9f                       lahf       
004ff398 f6c444                   test       ah, 0x44
004ff39b 7a2c                     jp         0x4ff3c9
004ff39d f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
004ff3a2 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
004ff3a9 9f                       lahf       
004ff3aa f6c444                   test       ah, 0x44
004ff3ad 7a1a                     jp         0x4ff3c9
004ff3af f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
004ff3b7 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004ff3bf f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004ff3c4 d945f0                   fld        dword ptr [ebp - 0x10]
004ff3c7 eb1e                     jmp        0x4ff3e7
004ff3c9 51                       push       ecx
004ff3ca f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
004ff3cf f30f110424               movss      dword ptr [esp], xmm0
004ff3d4 51                       push       ecx
004ff3d5 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
004ff3da f30f110424               movss      dword ptr [esp], xmm0
004ff3df e89c9ef5ff               call       0x459280
004ff3e4 83c408                   add        esp, 8
004ff3e7 8be5                     mov        esp, ebp
004ff3e9 5d                       pop        ebp
004ff3ea c20400                   ret        4
004ff3ed cc                       int3       
004ff3ee cc                       int3       
004ff3ef cc                       int3       
