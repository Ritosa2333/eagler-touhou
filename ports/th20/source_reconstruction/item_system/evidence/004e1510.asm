004e1510 55                       push       ebp
004e1511 8bec                     mov        ebp, esp
004e1513 51                       push       ecx
004e1514 894dfc                   mov        dword ptr [ebp - 4], ecx
004e1517 6a01                     push       1
004e1519 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e151c e82ffdffff               call       0x4e1250
004e1521 85c0                     test       eax, eax
004e1523 7428                     je         0x4e154d
004e1525 6a00                     push       0
004e1527 6a11                     push       0x11
004e1529 b930a85b00               mov        ecx, 0x5ba830
004e152e e83d58f4ff               call       0x426d70
004e1533 90                       nop        
004e1534 833da4065c0000           cmp        dword ptr [0x5c06a4], 0
004e153b 7410                     je         0x4e154d
004e153d 6a00                     push       0
004e153f 6a04                     push       4
004e1541 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e1547 e8947bfdff               call       0x4b90e0
004e154c 90                       nop        
004e154d 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e1550 8b88c4000000             mov        ecx, dword ptr [eax + 0xc4]
004e1556 83c101                   add        ecx, 1
004e1559 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e155c 898ac4000000             mov        dword ptr [edx + 0xc4], ecx
004e1562 8be5                     mov        esp, ebp
004e1564 5d                       pop        ebp
004e1565 c3                       ret        
004e1566 cc                       int3       
004e1567 cc                       int3       
004e1568 cc                       int3       
004e1569 cc                       int3       
004e156a cc                       int3       
004e156b cc                       int3       
004e156c cc                       int3       
004e156d cc                       int3       
004e156e cc                       int3       
004e156f cc                       int3       
