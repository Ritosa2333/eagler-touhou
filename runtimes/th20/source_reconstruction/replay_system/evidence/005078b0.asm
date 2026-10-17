005078b0 55                       push       ebp
005078b1 8bec                     mov        ebp, esp
005078b3 6aff                     push       -1
005078b5 68e0825600               push       0x5682e0
005078ba 64a100000000             mov        eax, dword ptr fs:[0]
005078c0 50                       push       eax
005078c1 83ec18                   sub        esp, 0x18
005078c4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
005078c9 33c5                     xor        eax, ebp
005078cb 50                       push       eax
005078cc 8d45f4                   lea        eax, [ebp - 0xc]
005078cf 64a300000000             mov        dword ptr fs:[0], eax
005078d5 894df0                   mov        dword ptr [ebp - 0x10], ecx
005078d8 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
005078db c700e4325700             mov        dword ptr [eax], 0x5732e4
005078e1 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005078e4 8b5118                   mov        edx, dword ptr [ecx + 0x18]
005078e7 52                       push       edx
005078e8 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
005078ee e87dc2f0ff               call       0x413b70
005078f3 90                       nop        
005078f4 c745e800000000           mov        dword ptr [ebp - 0x18], 0
005078fb eb09                     jmp        0x507906
005078fd 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00507900 83c001                   add        eax, 1
00507903 8945e8                   mov        dword ptr [ebp - 0x18], eax
00507906 837de808                 cmp        dword ptr [ebp - 0x18], 8
0050790a 7d0f                     jge        0x50791b
0050790c 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0050790f 51                       push       ecx
00507910 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00507913 e888260000               call       0x509fa0
00507918 90                       nop        
00507919 ebe2                     jmp        0x5078fd
0050791b 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0050791e 8b421c                   mov        eax, dword ptr [edx + 0x1c]
00507921 50                       push       eax
00507922 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00507928 e843c2f0ff               call       0x413b70
0050792d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00507930 c7411c00000000           mov        dword ptr [ecx + 0x1c], 0
00507937 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
0050793e eb09                     jmp        0x507949
00507940 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00507943 83c201                   add        edx, 1
00507946 8955ec                   mov        dword ptr [ebp - 0x14], edx
00507949 837dec08                 cmp        dword ptr [ebp - 0x14], 8
0050794d 7d44                     jge        0x507993
0050794f 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00507952 83c020                   add        eax, 0x20
00507955 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00507958 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0050795b 51                       push       ecx
0050795c 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0050795f e81cccf0ff               call       0x414580
00507964 8945e0                   mov        dword ptr [ebp - 0x20], eax
00507967 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0050796a 52                       push       edx
0050796b 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00507971 e82af5ffff               call       0x506ea0
00507976 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00507979 83c020                   add        eax, 0x20
0050797c 8945dc                   mov        dword ptr [ebp - 0x24], eax
0050797f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00507982 51                       push       ecx
00507983 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00507986 e8f5cbf0ff               call       0x414580
0050798b c70000000000             mov        dword ptr [eax], 0
00507991 ebad                     jmp        0x507940
00507993 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00507996 837a0800                 cmp        dword ptr [edx + 8], 0
0050799a 7413                     je         0x5079af
0050799c 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0050799f 8b4808                   mov        ecx, dword ptr [eax + 8]
005079a2 51                       push       ecx
005079a3 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
005079a9 e802abf0ff               call       0x4124b0
005079ae 90                       nop        
005079af 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
005079b2 83ba5402000000           cmp        dword ptr [edx + 0x254], 0
005079b9 7416                     je         0x5079d1
005079bb 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
005079be 8b8854020000             mov        ecx, dword ptr [eax + 0x254]
005079c4 51                       push       ecx
005079c5 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
005079cb e8e0aaf0ff               call       0x4124b0
005079d0 90                       nop        
005079d1 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
005079d4 837a0c00                 cmp        dword ptr [edx + 0xc], 0
005079d8 7413                     je         0x5079ed
005079da 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
005079dd 8b480c                   mov        ecx, dword ptr [eax + 0xc]
005079e0 51                       push       ecx
005079e1 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
005079e7 e8c4aaf0ff               call       0x4124b0
005079ec 90                       nop        
005079ed 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
005079f0 3b15fc605c00             cmp        edx, dword ptr [0x5c60fc]
005079f6 750a                     jne        0x507a02
005079f8 c705fc605c0000000000     mov        dword ptr [0x5c60fc], 0
00507a02 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00507a05 81c1e8000000             add        ecx, 0xe8
00507a0b e850feffff               call       0x507860
00507a10 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00507a13 e8d883f1ff               call       0x41fdf0
00507a18 90                       nop        
00507a19 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00507a1c 64890d00000000           mov        dword ptr fs:[0], ecx
00507a23 59                       pop        ecx
00507a24 8be5                     mov        esp, ebp
00507a26 5d                       pop        ebp
00507a27 c3                       ret        
00507a28 cc                       int3       
00507a29 cc                       int3       
00507a2a cc                       int3       
00507a2b cc                       int3       
00507a2c cc                       int3       
00507a2d cc                       int3       
00507a2e cc                       int3       
00507a2f cc                       int3       
