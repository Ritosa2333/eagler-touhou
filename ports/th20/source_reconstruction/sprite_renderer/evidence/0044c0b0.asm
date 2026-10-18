0044c0b0 55                       push       ebp
0044c0b1 8bec                     mov        ebp, esp
0044c0b3 83ec10                   sub        esp, 0x10
0044c0b6 894df0                   mov        dword ptr [ebp - 0x10], ecx
0044c0b9 8b4508                   mov        eax, dword ptr [ebp + 8]
0044c0bc 8b4814                   mov        ecx, dword ptr [eax + 0x14]
0044c0bf 83e1fc                   and        ecx, 0xfffffffc
0044c0c2 83c902                   or         ecx, 2
0044c0c5 8b5508                   mov        edx, dword ptr [ebp + 8]
0044c0c8 894a14                   mov        dword ptr [edx + 0x14], ecx
0044c0cb b9404d5c00               mov        ecx, 0x5c4d40
0044c0d0 e85b66fcff               call       0x412730
0044c0d5 8945fc                   mov        dword ptr [ebp - 4], eax
0044c0d8 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044c0db 8b08                     mov        ecx, dword ptr [eax]
0044c0dd 8b515c                   mov        edx, dword ptr [ecx + 0x5c]
0044c0e0 8955f4                   mov        dword ptr [ebp - 0xc], edx
0044c0e3 8b4514                   mov        eax, dword ptr [ebp + 0x14]
0044c0e6 8b0c8508e25600           mov        ecx, dword ptr [eax*4 + 0x56e208]
0044c0ed 894df8                   mov        dword ptr [ebp - 8], ecx
0044c0f0 6a00                     push       0
0044c0f2 8b5508                   mov        edx, dword ptr [ebp + 8]
0044c0f5 52                       push       edx
0044c0f6 6a00                     push       0
0044c0f8 8b45f8                   mov        eax, dword ptr [ebp - 8]
0044c0fb 50                       push       eax
0044c0fc 6800020000               push       0x200
0044c101 6a01                     push       1
0044c103 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
0044c106 51                       push       ecx
0044c107 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0044c10a 52                       push       edx
0044c10b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044c10e 50                       push       eax
0044c10f ff55f4                   call       dword ptr [ebp - 0xc]
0044c112 85c0                     test       eax, eax
0044c114 740d                     je         0x44c123
0044c116 6850e25600               push       0x56e250
0044c11b e89005fcff               call       0x40c6b0
0044c120 83c404                   add        esp, 4
0044c123 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044c126 8b5514                   mov        edx, dword ptr [ebp + 0x14]
0044c129 8b04952ce25600           mov        eax, dword ptr [edx*4 + 0x56e22c]
0044c130 89410c                   mov        dword ptr [ecx + 0xc], eax
0044c133 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0044c136 0faf4510                 imul       eax, dword ptr [ebp + 0x10]
0044c13a 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
0044c13d 0faf048d2ce25600         imul       eax, dword ptr [ecx*4 + 0x56e22c]
0044c145 8be5                     mov        esp, ebp
0044c147 5d                       pop        ebp
0044c148 c21000                   ret        0x10
0044c14b cc                       int3       
0044c14c cc                       int3       
0044c14d cc                       int3       
0044c14e cc                       int3       
0044c14f cc                       int3       
