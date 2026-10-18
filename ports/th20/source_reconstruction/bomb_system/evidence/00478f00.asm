00478f00 55                       push       ebp
00478f01 8bec                     mov        ebp, esp
00478f03 83ec24                   sub        esp, 0x24
00478f06 894dfc                   mov        dword ptr [ebp - 4], ecx
00478f09 8b45fc                   mov        eax, dword ptr [ebp - 4]
00478f0c 05bc050000               add        eax, 0x5bc
00478f11 8945f8                   mov        dword ptr [ebp - 8], eax
00478f14 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478f17 83c12c                   add        ecx, 0x2c
00478f1a 51                       push       ecx
00478f1b 8d55e8                   lea        edx, [ebp - 0x18]
00478f1e 52                       push       edx
00478f1f 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00478f22 e84906fbff               call       0x429570
00478f27 8945f4                   mov        dword ptr [ebp - 0xc], eax
00478f2a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00478f2d 0584040000               add        eax, 0x484
00478f32 50                       push       eax
00478f33 8d4ddc                   lea        ecx, [ebp - 0x24]
00478f36 51                       push       ecx
00478f37 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00478f3a e83106fbff               call       0x429570
00478f3f 8b5508                   mov        edx, dword ptr [ebp + 8]
00478f42 8b08                     mov        ecx, dword ptr [eax]
00478f44 890a                     mov        dword ptr [edx], ecx
00478f46 8b4804                   mov        ecx, dword ptr [eax + 4]
00478f49 894a04                   mov        dword ptr [edx + 4], ecx
00478f4c 8b4008                   mov        eax, dword ptr [eax + 8]
00478f4f 894208                   mov        dword ptr [edx + 8], eax
00478f52 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00478f55 51                       push       ecx
00478f56 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478f59 e812000000               call       0x478f70
00478f5e 8b4508                   mov        eax, dword ptr [ebp + 8]
00478f61 8be5                     mov        esp, ebp
00478f63 5d                       pop        ebp
00478f64 c20400                   ret        4
00478f67 cc                       int3       
00478f68 cc                       int3       
00478f69 cc                       int3       
00478f6a cc                       int3       
00478f6b cc                       int3       
00478f6c cc                       int3       
00478f6d cc                       int3       
00478f6e cc                       int3       
00478f6f cc                       int3       
