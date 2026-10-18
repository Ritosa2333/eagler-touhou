004e1410 55                       push       ebp
004e1411 8bec                     mov        ebp, esp
004e1413 83ec08                   sub        esp, 8
004e1416 56                       push       esi
004e1417 894dfc                   mov        dword ptr [ebp - 4], ecx
004e141a 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e141d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1420 8b5030                   mov        edx, dword ptr [eax + 0x30]
004e1423 3b5134                   cmp        edx, dword ptr [ecx + 0x34]
004e1426 7c04                     jl         0x4e142c
004e1428 33c0                     xor        eax, eax
004e142a eb6f                     jmp        0x4e149b
004e142c 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e142f 8b4830                   mov        ecx, dword ptr [eax + 0x30]
004e1432 034d08                   add        ecx, dword ptr [ebp + 8]
004e1435 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e1438 894a30                   mov        dword ptr [edx + 0x30], ecx
004e143b 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e143e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1441 8b5030                   mov        edx, dword ptr [eax + 0x30]
004e1444 3b5134                   cmp        edx, dword ptr [ecx + 0x34]
004e1447 7e1c                     jle        0x4e1465
004e1449 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e144c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e144f 8b5134                   mov        edx, dword ptr [ecx + 0x34]
004e1452 895030                   mov        dword ptr [eax + 0x30], edx
004e1455 6a00                     push       0
004e1457 6a02                     push       2
004e1459 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e145f e87c7cfdff               call       0x4b90e0
004e1464 90                       nop        
004e1465 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e1468 8b4030                   mov        eax, dword ptr [eax + 0x30]
004e146b 2b4508                   sub        eax, dword ptr [ebp + 8]
004e146e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1471 99                       cdq        
004e1472 f77938                   idiv       dword ptr [ecx + 0x38]
004e1475 8bc8                     mov        ecx, eax
004e1477 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e147a 8b75fc                   mov        esi, dword ptr [ebp - 4]
004e147d 8b4230                   mov        eax, dword ptr [edx + 0x30]
004e1480 99                       cdq        
004e1481 f77e38                   idiv       dword ptr [esi + 0x38]
004e1484 3bc8                     cmp        ecx, eax
004e1486 7409                     je         0x4e1491
004e1488 c745f801000000           mov        dword ptr [ebp - 8], 1
004e148f eb07                     jmp        0x4e1498
004e1491 c745f800000000           mov        dword ptr [ebp - 8], 0
004e1498 8b45f8                   mov        eax, dword ptr [ebp - 8]
004e149b 5e                       pop        esi
004e149c 8be5                     mov        esp, ebp
004e149e 5d                       pop        ebp
004e149f c20400                   ret        4
004e14a2 cc                       int3       
004e14a3 cc                       int3       
004e14a4 cc                       int3       
004e14a5 cc                       int3       
004e14a6 cc                       int3       
004e14a7 cc                       int3       
004e14a8 cc                       int3       
004e14a9 cc                       int3       
004e14aa cc                       int3       
004e14ab cc                       int3       
004e14ac cc                       int3       
004e14ad cc                       int3       
004e14ae cc                       int3       
004e14af cc                       int3       
