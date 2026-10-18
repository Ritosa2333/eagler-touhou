004bee70 55                       push       ebp
004bee71 8bec                     mov        ebp, esp
004bee73 6aff                     push       -1
004bee75 6800765600               push       0x567600
004bee7a 64a100000000             mov        eax, dword ptr fs:[0]
004bee80 50                       push       eax
004bee81 51                       push       ecx
004bee82 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004bee87 33c5                     xor        eax, ebp
004bee89 50                       push       eax
004bee8a 8d45f4                   lea        eax, [ebp - 0xc]
004bee8d 64a300000000             mov        dword ptr fs:[0], eax
004bee93 894df0                   mov        dword ptr [ebp - 0x10], ecx
004bee96 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bee99 e8820ef6ff               call       0x41fd20
004bee9e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004beea1 c700e40b5700             mov        dword ptr [eax], 0x570be4
004beea7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004beeaa c7411000000000           mov        dword ptr [ecx + 0x10], 0
004beeb1 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004beeb4 83c114                   add        ecx, 0x14
004beeb7 e8d43ef6ff               call       0x422d90
004beebc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004beebf 83c124                   add        ecx, 0x24
004beec2 e83900ffff               call       0x4aef00
004beec7 68c05c4200               push       0x425cc0
004beecc 6a0e                     push       0xe
004beece 6a04                     push       4
004beed0 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004beed3 83c270                   add        edx, 0x70
004beed6 52                       push       edx
004beed7 e844cdf4ff               call       0x40bc20
004beedc 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004beedf c780a800000000000000     mov        dword ptr [eax + 0xa8], 0
004beee9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004beeec 0f57c0                   xorps      xmm0, xmm0
004beeef f30f1181ac000000         movss      dword ptr [ecx + 0xac], xmm0
004beef7 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004beefa c782b000000000000000     mov        dword ptr [edx + 0xb0], 0
004bef04 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004bef07 c780b400000000000000     mov        dword ptr [eax + 0xb4], 0
004bef11 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bef14 c781b800000000000000     mov        dword ptr [ecx + 0xb8], 0
004bef1e 6880000000               push       0x80
004bef23 6a00                     push       0
004bef25 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004bef28 81c2bc000000             add        edx, 0xbc
004bef2e 52                       push       edx
004bef2f e82c580800               call       0x544760
004bef34 83c40c                   add        esp, 0xc
004bef37 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004bef3a c7803c01000000000000     mov        dword ptr [eax + 0x13c], 0
004bef44 68f00b5700               push       0x570bf0
004bef49 e862d7f4ff               call       0x40c6b0
004bef4e 83c404                   add        esp, 4
004bef51 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bef54 890d244d5c00             mov        dword ptr [0x5c4d24], ecx
004bef5a 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004bef5d 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004bef60 64890d00000000           mov        dword ptr fs:[0], ecx
004bef67 59                       pop        ecx
004bef68 8be5                     mov        esp, ebp
004bef6a 5d                       pop        ebp
004bef6b c3                       ret        
004bef6c cc                       int3       
004bef6d cc                       int3       
004bef6e cc                       int3       
004bef6f cc                       int3       
004bef70 cc                       int3       
004bef71 cc                       int3       
004bef72 cc                       int3       
004bef73 cc                       int3       
004bef74 cc                       int3       
004bef75 cc                       int3       
004bef76 cc                       int3       
004bef77 cc                       int3       
004bef78 cc                       int3       
004bef79 cc                       int3       
004bef7a cc                       int3       
004bef7b cc                       int3       
004bef7c cc                       int3       
004bef7d cc                       int3       
004bef7e cc                       int3       
004bef7f cc                       int3       
