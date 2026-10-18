004e1310 55                       push       ebp
004e1311 8bec                     mov        ebp, esp
004e1313 83ec18                   sub        esp, 0x18
004e1316 894dfc                   mov        dword ptr [ebp - 4], ecx
004e1319 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e131c e8df6dfdff               call       0x4b8100
004e1321 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1324 3981b8000000             cmp        dword ptr [ecx + 0xb8], eax
004e132a 7c12                     jl         0x4e133e
004e132c 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e132f c782c000000000000000     mov        dword ptr [edx + 0xc0], 0
004e1339 e9c2000000               jmp        0x4e1400
004e133e 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e1341 8b88c0000000             mov        ecx, dword ptr [eax + 0xc0]
004e1347 034d08                   add        ecx, dword ptr [ebp + 8]
004e134a 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e134d 898ac0000000             mov        dword ptr [edx + 0xc0], ecx
004e1353 b968a55b00               mov        ecx, 0x5ba568
004e1358 e8432df8ff               call       0x4640a0
004e135d 83f804                   cmp        eax, 4
004e1360 7515                     jne        0x4e1377
004e1362 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e1365 8b88c4000000             mov        ecx, dword ptr [eax + 0xc4]
004e136b 8b148d54ff5a00           mov        edx, dword ptr [ecx*4 + 0x5aff54]
004e1372 8955f8                   mov        dword ptr [ebp - 8], edx
004e1375 eb13                     jmp        0x4e138a
004e1377 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e137a 8b88c4000000             mov        ecx, dword ptr [eax + 0xc4]
004e1380 8b148dd8fe5a00           mov        edx, dword ptr [ecx*4 + 0x5afed8]
004e1387 8955f8                   mov        dword ptr [ebp - 8], edx
004e138a 8b45f8                   mov        eax, dword ptr [ebp - 8]
004e138d 8945f4                   mov        dword ptr [ebp - 0xc], eax
004e1390 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1393 8b91c0000000             mov        edx, dword ptr [ecx + 0xc0]
004e1399 3b55f4                   cmp        edx, dword ptr [ebp - 0xc]
004e139c 7c20                     jl         0x4e13be
004e139e 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e13a1 8b88c0000000             mov        ecx, dword ptr [eax + 0xc0]
004e13a7 2b4df4                   sub        ecx, dword ptr [ebp - 0xc]
004e13aa 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e13ad 898ac0000000             mov        dword ptr [edx + 0xc0], ecx
004e13b3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e13b6 e855010000               call       0x4e1510
004e13bb 90                       nop        
004e13bc ebd2                     jmp        0x4e1390
004e13be 833da4065c0000           cmp        dword ptr [0x5c06a4], 0
004e13c5 7439                     je         0x4e1400
004e13c7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e13ca e8316dfdff               call       0x4b8100
004e13cf 8945f0                   mov        dword ptr [ebp - 0x10], eax
004e13d2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e13d5 e8766dfdff               call       0x4b8150
004e13da 8945ec                   mov        dword ptr [ebp - 0x14], eax
004e13dd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e13e0 e8cb6cfdff               call       0x4b80b0
004e13e5 8945e8                   mov        dword ptr [ebp - 0x18], eax
004e13e8 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004e13eb 50                       push       eax
004e13ec 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004e13ef 51                       push       ecx
004e13f0 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004e13f3 52                       push       edx
004e13f4 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e13fa e8f177fdff               call       0x4b8bf0
004e13ff 90                       nop        
004e1400 8be5                     mov        esp, ebp
004e1402 5d                       pop        ebp
004e1403 c20400                   ret        4
004e1406 cc                       int3       
004e1407 cc                       int3       
004e1408 cc                       int3       
004e1409 cc                       int3       
004e140a cc                       int3       
004e140b cc                       int3       
004e140c cc                       int3       
004e140d cc                       int3       
004e140e cc                       int3       
004e140f cc                       int3       
