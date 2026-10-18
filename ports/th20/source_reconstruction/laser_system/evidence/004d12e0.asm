004d12e0 55                       push       ebp
004d12e1 8bec                     mov        ebp, esp
004d12e3 83ec18                   sub        esp, 0x18
004d12e6 894dfc                   mov        dword ptr [ebp - 4], ecx
004d12e9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d12ec 81c158070000             add        ecx, 0x758
004d12f2 e80953fbff               call       0x486600
004d12f7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d12fa 398190060000             cmp        dword ptr [ecx + 0x690], eax
004d1300 7205                     jb         0x4d1307
004d1302 e933010000               jmp        0x4d143a
004d1307 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d130a 83ba9006000018           cmp        dword ptr [edx + 0x690], 0x18
004d1311 7c05                     jl         0x4d1318
004d1313 e922010000               jmp        0x4d143a
004d1318 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d131b 0558070000               add        eax, 0x758
004d1320 8945e8                   mov        dword ptr [ebp - 0x18], eax
004d1323 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d1326 8b9190060000             mov        edx, dword ptr [ecx + 0x690]
004d132c 52                       push       edx
004d132d 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004d1330 e88bb3faff               call       0x47c6c0
004d1335 8945f8                   mov        dword ptr [ebp - 8], eax
004d1338 8b45f8                   mov        eax, dword ptr [ebp - 8]
004d133b 8b4820                   mov        ecx, dword ptr [eax + 0x20]
004d133e 894dec                   mov        dword ptr [ebp - 0x14], ecx
004d1341 837dec00                 cmp        dword ptr [ebp - 0x14], 0
004d1345 7505                     jne        0x4d134c
004d1347 e9ee000000               jmp        0x4d143a
004d134c 8b55f8                   mov        edx, dword ptr [ebp - 8]
004d134f 837a2400                 cmp        dword ptr [edx + 0x24], 0
004d1353 751f                     jne        0x4d1374
004d1355 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d1358 8945f0                   mov        dword ptr [ebp - 0x10], eax
004d135b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004d135e 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004d1361 8b8198060000             mov        eax, dword ptr [ecx + 0x698]
004d1367 0b829c060000             or         eax, dword ptr [edx + 0x69c]
004d136d 7405                     je         0x4d1374
004d136f e9c6000000               jmp        0x4d143a
004d1374 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d1377 894df4                   mov        dword ptr [ebp - 0xc], ecx
004d137a 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004d137d 83ea07                   sub        edx, 7
004d1380 8955f4                   mov        dword ptr [ebp - 0xc], edx
004d1383 837df41a                 cmp        dword ptr [ebp - 0xc], 0x1a
004d1387 0f8793000000             ja         0x4d1420
004d138d 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004d1390 0fb68854144d00           movzx      ecx, byte ptr [eax + 0x4d1454]
004d1397 ff248d40144d00           jmp        dword ptr [ecx*4 + 0x4d1440]
004d139e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d13a1 c7422003000000           mov        dword ptr [edx + 0x20], 3
004d13a8 eb76                     jmp        0x4d1420
004d13aa 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d13ad 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004d13b0 8b5110                   mov        edx, dword ptr [ecx + 0x10]
004d13b3 8990cc060000             mov        dword ptr [eax + 0x6cc], edx
004d13b9 eb65                     jmp        0x4d1420
004d13bb 8b45f8                   mov        eax, dword ptr [ebp - 8]
004d13be 83781000                 cmp        dword ptr [eax + 0x10], 0
004d13c2 7411                     je         0x4d13d5
004d13c4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d13c7 81c170070000             add        ecx, 0x770
004d13cd e8ded1f8ff               call       0x45e5b0
004d13d2 90                       nop        
004d13d3 eb0f                     jmp        0x4d13e4
004d13d5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d13d8 81c170070000             add        ecx, 0x770
004d13de e8add1f8ff               call       0x45e590
004d13e3 90                       nop        
004d13e4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d13e7 8b9190060000             mov        edx, dword ptr [ecx + 0x690]
004d13ed 83c201                   add        edx, 1
004d13f0 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d13f3 899090060000             mov        dword ptr [eax + 0x690], edx
004d13f9 e9ebfeffff               jmp        0x4d12e9
004d13fe 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004d1401 8b5110                   mov        edx, dword ptr [ecx + 0x10]
004d1404 83e201                   and        edx, 1
004d1407 d1e2                     shl        edx, 1
004d1409 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d140c 8b889c060000             mov        ecx, dword ptr [eax + 0x69c]
004d1412 83e1fd                   and        ecx, 0xfffffffd
004d1415 0bca                     or         ecx, edx
004d1417 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d141a 898a9c060000             mov        dword ptr [edx + 0x69c], ecx
004d1420 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d1423 8b8890060000             mov        ecx, dword ptr [eax + 0x690]
004d1429 83c101                   add        ecx, 1
004d142c 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d142f 898a90060000             mov        dword ptr [edx + 0x690], ecx
004d1435 e9affeffff               jmp        0x4d12e9
004d143a 8be5                     mov        esp, ebp
004d143c 5d                       pop        ebp
004d143d c3                       ret        
004d143e 6690                     nop        
004d1440 aa                       stosb      byte ptr es:[edi], al
004d1441 134d00                   adc        ecx, dword ptr [ebp]
004d1444 9e                       sahf       
004d1445 134d00                   adc        ecx, dword ptr [ebp]
004d1448 bb134d00fe               mov        ebx, 0xfe004d13
004d144d 134d00                   adc        ecx, dword ptr [ebp]
004d1450 20144d00000404           and        byte ptr [ecx*2 + 0x4040000], dl
004d1457 010404                   add        dword ptr [esp + eax], eax
004d145a 0404                     add        al, 4
004d145c 0404                     add        al, 4
004d145e 0404                     add        al, 4
004d1460 0402                     add        al, 2
004d1462 0404                     add        al, 4
004d1464 0404                     add        al, 4
004d1466 0404                     add        al, 4
004d1468 0404                     add        al, 4
004d146a 0404                     add        al, 4
004d146c 0404                     add        al, 4
004d146e 03cc                     add        ecx, esp
