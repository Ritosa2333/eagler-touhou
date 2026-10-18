004e11a0 55                       push       ebp
004e11a1 8bec                     mov        ebp, esp
004e11a3 83ec10                   sub        esp, 0x10
004e11a6 894dfc                   mov        dword ptr [ebp - 4], ecx
004e11a9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e11ac e86f6cfdff               call       0x4b7e20
004e11b1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e11b4 3981cc000000             cmp        dword ptr [ecx + 0xcc], eax
004e11ba 7c0f                     jl         0x4e11cb
004e11bc 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e11bf c782d000000000000000     mov        dword ptr [edx + 0xd0], 0
004e11c9 eb7b                     jmp        0x4e1246
004e11cb 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e11ce 8b88d0000000             mov        ecx, dword ptr [eax + 0xd0]
004e11d4 034d08                   add        ecx, dword ptr [ebp + 8]
004e11d7 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e11da 898ad0000000             mov        dword ptr [edx + 0xd0], ecx
004e11e0 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e11e3 83b8d000000003           cmp        dword ptr [eax + 0xd0], 3
004e11ea 7c18                     jl         0x4e1204
004e11ec 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e11ef c781d000000000000000     mov        dword ptr [ecx + 0xd0], 0
004e11f9 6a01                     push       1
004e11fb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e11fe e8ddfeffff               call       0x4e10e0
004e1203 90                       nop        
004e1204 833da4065c0000           cmp        dword ptr [0x5c06a4], 0
004e120b 7439                     je         0x4e1246
004e120d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1210 e80b6cfdff               call       0x4b7e20
004e1215 8945f8                   mov        dword ptr [ebp - 8], eax
004e1218 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e121b e8506cfdff               call       0x4b7e70
004e1220 8945f4                   mov        dword ptr [ebp - 0xc], eax
004e1223 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1226 e8c56df9ff               call       0x477ff0
004e122b 8945f0                   mov        dword ptr [ebp - 0x10], eax
004e122e 8b55f8                   mov        edx, dword ptr [ebp - 8]
004e1231 52                       push       edx
004e1232 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004e1235 50                       push       eax
004e1236 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1239 51                       push       ecx
004e123a 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e1240 e80b74fdff               call       0x4b8650
004e1245 90                       nop        
004e1246 8be5                     mov        esp, ebp
004e1248 5d                       pop        ebp
004e1249 c20400                   ret        4
004e124c cc                       int3       
004e124d cc                       int3       
004e124e cc                       int3       
004e124f cc                       int3       
