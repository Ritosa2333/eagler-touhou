0047a7d0 55                       push       ebp
0047a7d1 8bec                     mov        ebp, esp
0047a7d3 83ec08                   sub        esp, 8
0047a7d6 56                       push       esi
0047a7d7 57                       push       edi
0047a7d8 894df8                   mov        dword ptr [ebp - 8], ecx
0047a7db 8b45f8                   mov        eax, dword ptr [ebp - 8]
0047a7de 8b08                     mov        ecx, dword ptr [eax]
0047a7e0 51                       push       ecx
0047a7e1 6a2c                     push       0x2c
0047a7e3 e8183ff9ff               call       0x40e700
0047a7e8 83c408                   add        esp, 8
0047a7eb 8945fc                   mov        dword ptr [ebp - 4], eax
0047a7ee 8b5508                   mov        edx, dword ptr [ebp + 8]
0047a7f1 52                       push       edx
0047a7f2 e8890af9ff               call       0x40b280
0047a7f7 83c404                   add        esp, 4
0047a7fa b90b000000               mov        ecx, 0xb
0047a7ff 8bf0                     mov        esi, eax
0047a801 8b7dfc                   mov        edi, dword ptr [ebp - 4]
0047a804 f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
0047a806 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047a809 5f                       pop        edi
0047a80a 5e                       pop        esi
0047a80b 8be5                     mov        esp, ebp
0047a80d 5d                       pop        ebp
0047a80e c20400                   ret        4
0047a811 cc                       int3       
0047a812 cc                       int3       
0047a813 cc                       int3       
0047a814 cc                       int3       
0047a815 cc                       int3       
0047a816 cc                       int3       
0047a817 cc                       int3       
0047a818 cc                       int3       
0047a819 cc                       int3       
0047a81a cc                       int3       
0047a81b cc                       int3       
0047a81c cc                       int3       
0047a81d cc                       int3       
0047a81e cc                       int3       
0047a81f cc                       int3       
