00454d80 55                       push       ebp
00454d81 8bec                     mov        ebp, esp
00454d83 83ec38                   sub        esp, 0x38
00454d86 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00454d8b 33c5                     xor        eax, ebp
00454d8d 8945fc                   mov        dword ptr [ebp - 4], eax
00454d90 8d4df0                   lea        ecx, [ebp - 0x10]
00454d93 e878e0fcff               call       0x422e10
00454d98 8d4de4                   lea        ecx, [ebp - 0x1c]
00454d9b e870e0fcff               call       0x422e10
00454da0 90                       nop        
00454da1 8b4508                   mov        eax, dword ptr [ebp + 8]
00454da4 50                       push       eax
00454da5 8d4dd4                   lea        ecx, [ebp - 0x2c]
00454da8 51                       push       ecx
00454da9 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
00454dac e88f46fdff               call       0x429440
00454db1 8b10                     mov        edx, dword ptr [eax]
00454db3 8955f0                   mov        dword ptr [ebp - 0x10], edx
00454db6 8b4804                   mov        ecx, dword ptr [eax + 4]
00454db9 894df4                   mov        dword ptr [ebp - 0xc], ecx
00454dbc 8b5008                   mov        edx, dword ptr [eax + 8]
00454dbf 8955f8                   mov        dword ptr [ebp - 8], edx
00454dc2 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00454dc7 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00454dce 51                       push       ecx
00454dcf f30f110424               movss      dword ptr [esp], xmm0
00454dd4 8d45f0                   lea        eax, [ebp - 0x10]
00454dd7 50                       push       eax
00454dd8 8d4df0                   lea        ecx, [ebp - 0x10]
00454ddb 51                       push       ecx
00454ddc e8bf410000               call       0x458fa0
00454de1 83c40c                   add        esp, 0xc
00454de4 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00454de9 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00454df0 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00454df8 f30f104df0               movss      xmm1, dword ptr [ebp - 0x10]
00454dfd 0f2fc8                   comiss     xmm1, xmm0
00454e00 761f                     jbe        0x454e21
00454e02 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00454e07 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00454e0f 0f2f45f0                 comiss     xmm0, dword ptr [ebp - 0x10]
00454e13 760c                     jbe        0x454e21
00454e15 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00454e1a f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00454e1f eb44                     jmp        0x454e65
00454e21 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00454e26 0f2f05e8e05600           comiss     xmm0, dword ptr [0x56e0e8]
00454e2d 7614                     jbe        0x454e43
00454e2f f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00454e34 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00454e3c f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00454e41 eb22                     jmp        0x454e65
00454e43 0f57c0                   xorps      xmm0, xmm0
00454e46 0f2f45f0                 comiss     xmm0, dword ptr [ebp - 0x10]
00454e4a 7619                     jbe        0x454e65
00454e4c f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00454e51 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00454e58 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00454e60 f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00454e65 0f57c0                   xorps      xmm0, xmm0
00454e68 f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
00454e6d 0f57c0                   xorps      xmm0, xmm0
00454e70 f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
00454e75 8d55f0                   lea        edx, [ebp - 0x10]
00454e78 52                       push       edx
00454e79 8d45e4                   lea        eax, [ebp - 0x1c]
00454e7c 50                       push       eax
00454e7d e8ae110000               call       0x456030
00454e82 83c408                   add        esp, 8
00454e85 d95de0                   fstp       dword ptr [ebp - 0x20]
00454e88 51                       push       ecx
00454e89 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00454e8e f30f110424               movss      dword ptr [esp], xmm0
00454e93 8d4de4                   lea        ecx, [ebp - 0x1c]
00454e96 51                       push       ecx
00454e97 8d55e4                   lea        edx, [ebp - 0x1c]
00454e9a 52                       push       edx
00454e9b e800410000               call       0x458fa0
00454ea0 83c40c                   add        esp, 0xc
00454ea3 8d45e4                   lea        eax, [ebp - 0x1c]
00454ea6 50                       push       eax
00454ea7 8d4dc8                   lea        ecx, [ebp - 0x38]
00454eaa 51                       push       ecx
00454eab 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00454eae e8bd46fdff               call       0x429570
00454eb3 8b5518                   mov        edx, dword ptr [ebp + 0x18]
00454eb6 8b08                     mov        ecx, dword ptr [eax]
00454eb8 890a                     mov        dword ptr [edx], ecx
00454eba 8b4804                   mov        ecx, dword ptr [eax + 4]
00454ebd 894a04                   mov        dword ptr [edx + 4], ecx
00454ec0 8b4008                   mov        eax, dword ptr [eax + 8]
00454ec3 894208                   mov        dword ptr [edx + 8], eax
00454ec6 d945e0                   fld        dword ptr [ebp - 0x20]
00454ec9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00454ecc 33cd                     xor        ecx, ebp
00454ece e81bdb0e00               call       0x5429ee
00454ed3 8be5                     mov        esp, ebp
00454ed5 5d                       pop        ebp
00454ed6 c3                       ret        
00454ed7 cc                       int3       
00454ed8 cc                       int3       
00454ed9 cc                       int3       
00454eda cc                       int3       
00454edb cc                       int3       
00454edc cc                       int3       
00454edd cc                       int3       
00454ede cc                       int3       
00454edf cc                       int3       
00454ee0 cc                       int3       
00454ee1 cc                       int3       
00454ee2 cc                       int3       
00454ee3 cc                       int3       
00454ee4 cc                       int3       
00454ee5 cc                       int3       
00454ee6 cc                       int3       
00454ee7 cc                       int3       
00454ee8 cc                       int3       
00454ee9 cc                       int3       
00454eea cc                       int3       
00454eeb cc                       int3       
00454eec cc                       int3       
00454eed cc                       int3       
00454eee cc                       int3       
00454eef cc                       int3       
