00535c50 55                       push       ebp
00535c51 8bec                     mov        ebp, esp
00535c53 83ec0c                   sub        esp, 0xc
00535c56 894dfc                   mov        dword ptr [ebp - 4], ecx
00535c59 6a00                     push       0
00535c5b e8d0abf2ff               call       0x460830
00535c60 83c404                   add        esp, 4
00535c63 8bc8                     mov        ecx, eax
00535c65 e8469afcff               call       0x4ff6b0
00535c6a 8945f4                   mov        dword ptr [ebp - 0xc], eax
00535c6d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00535c70 8b08                     mov        ecx, dword ptr [eax]
00535c72 6b510414                 imul       edx, dword ptr [ecx + 4], 0x14
00535c76 83c20f                   add        edx, 0xf
00535c79 8955f8                   mov        dword ptr [ebp - 8], edx
00535c7c 8b4508                   mov        eax, dword ptr [ebp + 8]
00535c7f 50                       push       eax
00535c80 6a00                     push       0
00535c82 6a00                     push       0
00535c84 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00535c87 51                       push       ecx
00535c88 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00535c8b e8b001fdff               call       0x505e40
00535c90 b801000000               mov        eax, 1
00535c95 8be5                     mov        esp, ebp
00535c97 5d                       pop        ebp
00535c98 c20400                   ret        4
00535c9b cc                       int3       
00535c9c cc                       int3       
00535c9d cc                       int3       
00535c9e cc                       int3       
00535c9f cc                       int3       
