004e10e0 55                       push       ebp
004e10e1 8bec                     mov        ebp, esp
004e10e3 83ec10                   sub        esp, 0x10
004e10e6 894dfc                   mov        dword ptr [ebp - 4], ecx
004e10e9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e10ec 8b88cc000000             mov        ecx, dword ptr [eax + 0xcc]
004e10f2 034d08                   add        ecx, dword ptr [ebp + 8]
004e10f5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e10f8 898acc000000             mov        dword ptr [edx + 0xcc], ecx
004e10fe 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1101 e81a6dfdff               call       0x4b7e20
004e1106 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1109 3981cc000000             cmp        dword ptr [ecx + 0xcc], eax
004e110f 7c0d                     jl         0x4e111e
004e1111 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e1114 c782d000000000000000     mov        dword ptr [edx + 0xd0], 0
004e111e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1121 e8fa6cfdff               call       0x4b7e20
004e1126 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1129 3981cc000000             cmp        dword ptr [ecx + 0xcc], eax
004e112f 7e13                     jle        0x4e1144
004e1131 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1134 e8e76cfdff               call       0x4b7e20
004e1139 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e113c 8982cc000000             mov        dword ptr [edx + 0xcc], eax
004e1142 eb0f                     jmp        0x4e1153
004e1144 6a00                     push       0
004e1146 6a2e                     push       0x2e
004e1148 b930a85b00               mov        ecx, 0x5ba830
004e114d e81e5cf4ff               call       0x426d70
004e1152 90                       nop        
004e1153 833da4065c0000           cmp        dword ptr [0x5c06a4], 0
004e115a 7439                     je         0x4e1195
004e115c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e115f e8bc6cfdff               call       0x4b7e20
004e1164 8945f8                   mov        dword ptr [ebp - 8], eax
004e1167 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e116a e8016dfdff               call       0x4b7e70
004e116f 8945f4                   mov        dword ptr [ebp - 0xc], eax
004e1172 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1175 e8766ef9ff               call       0x477ff0
004e117a 8945f0                   mov        dword ptr [ebp - 0x10], eax
004e117d 8b45f8                   mov        eax, dword ptr [ebp - 8]
004e1180 50                       push       eax
004e1181 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004e1184 51                       push       ecx
004e1185 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004e1188 52                       push       edx
004e1189 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e118f e8bc74fdff               call       0x4b8650
004e1194 90                       nop        
004e1195 8be5                     mov        esp, ebp
004e1197 5d                       pop        ebp
004e1198 c20400                   ret        4
004e119b cc                       int3       
004e119c cc                       int3       
004e119d cc                       int3       
004e119e cc                       int3       
004e119f cc                       int3       
