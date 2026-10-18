00535f70 55                       push       ebp
00535f71 8bec                     mov        ebp, esp
00535f73 83ec10                   sub        esp, 0x10
00535f76 894dfc                   mov        dword ptr [ebp - 4], ecx
00535f79 6a3c                     push       0x3c
00535f7b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00535f7e 83c148                   add        ecx, 0x48
00535f81 e89ad5eeff               call       0x423520
00535f86 90                       nop        
00535f87 6a00                     push       0
00535f89 e8a2a8f2ff               call       0x460830
00535f8e 83c404                   add        esp, 4
00535f91 8bc8                     mov        ecx, eax
00535f93 e878b8eeff               call       0x421810
00535f98 8945f4                   mov        dword ptr [ebp - 0xc], eax
00535f9b 6a00                     push       0
00535f9d e88ea8f2ff               call       0x460830
00535fa2 83c404                   add        esp, 4
00535fa5 8bc8                     mov        ecx, eax
00535fa7 e8a4a8f2ff               call       0x460850
00535fac 8945f8                   mov        dword ptr [ebp - 8], eax
00535faf 6a00                     push       0
00535fb1 6aff                     push       -1
00535fb3 51                       push       ecx
00535fb4 0f57c0                   xorps      xmm0, xmm0
00535fb7 f30f110424               movss      dword ptr [esp], xmm0
00535fbc 8b45f8                   mov        eax, dword ptr [ebp - 8]
00535fbf 50                       push       eax
00535fc0 6a1f                     push       0x1f
00535fc2 688cfb5600               push       0x56fb8c
00535fc7 8d4df0                   lea        ecx, [ebp - 0x10]
00535fca 51                       push       ecx
00535fcb 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00535fce e80d31f4ff               call       0x4790e0
00535fd3 8b10                     mov        edx, dword ptr [eax]
00535fd5 891594695c00             mov        dword ptr [0x5c6994], edx
00535fdb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00535fde e83dcfffff               call       0x532f20
00535fe3 8be5                     mov        esp, ebp
00535fe5 5d                       pop        ebp
00535fe6 c3                       ret        
00535fe7 cc                       int3       
00535fe8 cc                       int3       
00535fe9 cc                       int3       
00535fea cc                       int3       
00535feb cc                       int3       
00535fec cc                       int3       
00535fed cc                       int3       
00535fee cc                       int3       
00535fef cc                       int3       
