0051fd80 55                       push       ebp
0051fd81 8bec                     mov        ebp, esp
0051fd83 83ec18                   sub        esp, 0x18
0051fd86 894df8                   mov        dword ptr [ebp - 8], ecx
0051fd89 c745fc00000000           mov        dword ptr [ebp - 4], 0
0051fd90 eb09                     jmp        0x51fd9b
0051fd92 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051fd95 83c001                   add        eax, 1
0051fd98 8945fc                   mov        dword ptr [ebp - 4], eax
0051fd9b 837dfc05                 cmp        dword ptr [ebp - 4], 5
0051fd9f 0f8df6000000             jge        0x51fe9b
0051fda5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051fda8 3b4d08                   cmp        ecx, dword ptr [ebp + 8]
0051fdab 7576                     jne        0x51fe23
0051fdad 8b55f8                   mov        edx, dword ptr [ebp - 8]
0051fdb0 81c294030000             add        edx, 0x394
0051fdb6 8955f4                   mov        dword ptr [ebp - 0xc], edx
0051fdb9 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051fdbc 50                       push       eax
0051fdbd 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0051fdc0 e8bb47efff               call       0x414580
0051fdc5 8bc8                     mov        ecx, eax
0051fdc7 e8445cf6ff               call       0x485a10
0051fdcc 85c0                     test       eax, eax
0051fdce 7420                     je         0x51fdf0
0051fdd0 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0051fdd3 81c194030000             add        ecx, 0x394
0051fdd9 894df0                   mov        dword ptr [ebp - 0x10], ecx
0051fddc 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051fddf 52                       push       edx
0051fde0 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051fde3 e89847efff               call       0x414580
0051fde8 8bc8                     mov        ecx, eax
0051fdea e82165f6ff               call       0x486310
0051fdef 90                       nop        
0051fdf0 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051fdf3 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0051fdf6 83bc81e458000000         cmp        dword ptr [ecx + eax*4 + 0x58e4], 0
0051fdfe 7c21                     jl         0x51fe21
0051fe00 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051fe03 8b45f8                   mov        eax, dword ptr [ebp - 8]
0051fe06 8b8c90e4580000           mov        ecx, dword ptr [eax + edx*4 + 0x58e4]
0051fe0d 8b148d58f05a00           mov        edx, dword ptr [ecx*4 + 0x5af058]
0051fe14 83c27f                   add        edx, 0x7f
0051fe17 52                       push       edx
0051fe18 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0051fe1b e8d0cd0000               call       0x52cbf0
0051fe20 90                       nop        
0051fe21 eb73                     jmp        0x51fe96
0051fe23 8b45f8                   mov        eax, dword ptr [ebp - 8]
0051fe26 0594030000               add        eax, 0x394
0051fe2b 8945ec                   mov        dword ptr [ebp - 0x14], eax
0051fe2e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051fe31 51                       push       ecx
0051fe32 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0051fe35 e84647efff               call       0x414580
0051fe3a 8bc8                     mov        ecx, eax
0051fe3c e8cf5bf6ff               call       0x485a10
0051fe41 85c0                     test       eax, eax
0051fe43 7420                     je         0x51fe65
0051fe45 8b55f8                   mov        edx, dword ptr [ebp - 8]
0051fe48 81c294030000             add        edx, 0x394
0051fe4e 8955e8                   mov        dword ptr [ebp - 0x18], edx
0051fe51 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051fe54 50                       push       eax
0051fe55 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0051fe58 e82347efff               call       0x414580
0051fe5d 8bc8                     mov        ecx, eax
0051fe5f e8ec64f6ff               call       0x486350
0051fe64 90                       nop        
0051fe65 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051fe68 8b55f8                   mov        edx, dword ptr [ebp - 8]
0051fe6b 83bc8ae458000000         cmp        dword ptr [edx + ecx*4 + 0x58e4], 0
0051fe73 7c21                     jl         0x51fe96
0051fe75 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051fe78 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0051fe7b 8b9481e4580000           mov        edx, dword ptr [ecx + eax*4 + 0x58e4]
0051fe82 8b049558f05a00           mov        eax, dword ptr [edx*4 + 0x5af058]
0051fe89 83c07f                   add        eax, 0x7f
0051fe8c 50                       push       eax
0051fe8d 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0051fe90 e8fbcd0000               call       0x52cc90
0051fe95 90                       nop        
0051fe96 e9f7feffff               jmp        0x51fd92
0051fe9b 33c0                     xor        eax, eax
0051fe9d 8be5                     mov        esp, ebp
0051fe9f 5d                       pop        ebp
0051fea0 c20400                   ret        4
0051fea3 cc                       int3       
0051fea4 cc                       int3       
0051fea5 cc                       int3       
0051fea6 cc                       int3       
0051fea7 cc                       int3       
0051fea8 cc                       int3       
0051fea9 cc                       int3       
0051feaa cc                       int3       
0051feab cc                       int3       
0051feac cc                       int3       
0051fead cc                       int3       
0051feae cc                       int3       
0051feaf cc                       int3       
