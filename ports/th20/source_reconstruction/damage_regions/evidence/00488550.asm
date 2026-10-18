00488550 55                       push       ebp
00488551 8bec                     mov        ebp, esp
00488553 83ec0c                   sub        esp, 0xc
00488556 894dfc                   mov        dword ptr [ebp - 4], ecx
00488559 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048855c 0588000000               add        eax, 0x88
00488561 8945f8                   mov        dword ptr [ebp - 8], eax
00488564 6a00                     push       0
00488566 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00488569 e8b2010000               call       0x488720
0048856e 8945f4                   mov        dword ptr [ebp - 0xc], eax
00488571 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00488574 33d2                     xor        edx, edx
00488576 52                       push       edx
00488577 51                       push       ecx
00488578 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0048857b e8308f0500               call       0x4e14b0
00488580 90                       nop        
00488581 8be5                     mov        esp, ebp
00488583 5d                       pop        ebp
00488584 c20400                   ret        4
00488587 cc                       int3       
00488588 cc                       int3       
00488589 cc                       int3       
0048858a cc                       int3       
0048858b cc                       int3       
0048858c cc                       int3       
0048858d cc                       int3       
0048858e cc                       int3       
0048858f cc                       int3       
