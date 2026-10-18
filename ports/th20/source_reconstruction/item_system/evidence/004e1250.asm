004e1250 55                       push       ebp
004e1251 8bec                     mov        ebp, esp
004e1253 83ec10                   sub        esp, 0x10
004e1256 894dfc                   mov        dword ptr [ebp - 4], ecx
004e1259 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e125c e89f6efdff               call       0x4b8100
004e1261 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1264 3981b8000000             cmp        dword ptr [ecx + 0xb8], eax
004e126a 7c0d                     jl         0x4e1279
004e126c 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e126f c782c000000000000000     mov        dword ptr [edx + 0xc0], 0
004e1279 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e127c 8b88b8000000             mov        ecx, dword ptr [eax + 0xb8]
004e1282 034d08                   add        ecx, dword ptr [ebp + 8]
004e1285 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e1288 898ab8000000             mov        dword ptr [edx + 0xb8], ecx
004e128e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1291 e86a6efdff               call       0x4b8100
004e1296 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1299 3981b8000000             cmp        dword ptr [ecx + 0xb8], eax
004e129f 7e1e                     jle        0x4e12bf
004e12a1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e12a4 e8576efdff               call       0x4b8100
004e12a9 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e12ac 8982b8000000             mov        dword ptr [edx + 0xb8], eax
004e12b2 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e12b5 c780c000000000000000     mov        dword ptr [eax + 0xc0], 0
004e12bf 833da4065c0000           cmp        dword ptr [0x5c06a4], 0
004e12c6 7439                     je         0x4e1301
004e12c8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e12cb e8306efdff               call       0x4b8100
004e12d0 8945f8                   mov        dword ptr [ebp - 8], eax
004e12d3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e12d6 e8756efdff               call       0x4b8150
004e12db 8945f4                   mov        dword ptr [ebp - 0xc], eax
004e12de 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e12e1 e8ca6dfdff               call       0x4b80b0
004e12e6 8945f0                   mov        dword ptr [ebp - 0x10], eax
004e12e9 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004e12ec 51                       push       ecx
004e12ed 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004e12f0 52                       push       edx
004e12f1 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004e12f4 50                       push       eax
004e12f5 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e12fb e8f078fdff               call       0x4b8bf0
004e1300 90                       nop        
004e1301 b801000000               mov        eax, 1
004e1306 8be5                     mov        esp, ebp
004e1308 5d                       pop        ebp
004e1309 c20400                   ret        4
004e130c cc                       int3       
004e130d cc                       int3       
004e130e cc                       int3       
004e130f cc                       int3       
