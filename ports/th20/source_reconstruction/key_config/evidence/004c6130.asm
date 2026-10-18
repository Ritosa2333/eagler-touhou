004c6130 55                       push       ebp
004c6131 8bec                     mov        ebp, esp
004c6133 51                       push       ecx
004c6134 56                       push       esi
004c6135 57                       push       edi
004c6136 894dfc                   mov        dword ptr [ebp - 4], ecx
004c6139 6b450830                 imul       eax, dword ptr [ebp + 8], 0x30
004c613d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c6140 8dbc01382e0000           lea        edi, [ecx + eax + 0x2e38]
004c6147 b90c000000               mov        ecx, 0xc
004c614c beb09e5b00               mov        esi, 0x5b9eb0
004c6151 f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
004c6153 5f                       pop        edi
004c6154 5e                       pop        esi
004c6155 8be5                     mov        esp, ebp
004c6157 5d                       pop        ebp
004c6158 c20400                   ret        4
004c615b cc                       int3       
004c615c cc                       int3       
004c615d cc                       int3       
004c615e cc                       int3       
004c615f cc                       int3       
