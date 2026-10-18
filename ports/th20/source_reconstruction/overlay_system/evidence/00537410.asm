00537410 55                       push       ebp
00537411 8bec                     mov        ebp, esp
00537413 83ec10                   sub        esp, 0x10
00537416 894dfc                   mov        dword ptr [ebp - 4], ecx
00537419 6a3c                     push       0x3c
0053741b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053741e 83c148                   add        ecx, 0x48
00537421 e8fac0eeff               call       0x423520
00537426 90                       nop        
00537427 6a00                     push       0
00537429 e80294f2ff               call       0x460830
0053742e 83c404                   add        esp, 4
00537431 8bc8                     mov        ecx, eax
00537433 e8d8a3eeff               call       0x421810
00537438 8945f4                   mov        dword ptr [ebp - 0xc], eax
0053743b 6a00                     push       0
0053743d e8ee93f2ff               call       0x460830
00537442 83c404                   add        esp, 4
00537445 8bc8                     mov        ecx, eax
00537447 e80494f2ff               call       0x460850
0053744c 8945f8                   mov        dword ptr [ebp - 8], eax
0053744f 6a00                     push       0
00537451 6aff                     push       -1
00537453 51                       push       ecx
00537454 0f57c0                   xorps      xmm0, xmm0
00537457 f30f110424               movss      dword ptr [esp], xmm0
0053745c 8b45f8                   mov        eax, dword ptr [ebp - 8]
0053745f 50                       push       eax
00537460 6a1b                     push       0x1b
00537462 6824fb5600               push       0x56fb24
00537467 8d4df0                   lea        ecx, [ebp - 0x10]
0053746a 51                       push       ecx
0053746b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0053746e e86d1cf4ff               call       0x4790e0
00537473 8b10                     mov        edx, dword ptr [eax]
00537475 8915b8695c00             mov        dword ptr [0x5c69b8], edx
0053747b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053747e e89dbaffff               call       0x532f20
00537483 8be5                     mov        esp, ebp
00537485 5d                       pop        ebp
00537486 c3                       ret        
00537487 cc                       int3       
00537488 cc                       int3       
00537489 cc                       int3       
0053748a cc                       int3       
0053748b cc                       int3       
0053748c cc                       int3       
0053748d cc                       int3       
0053748e cc                       int3       
0053748f cc                       int3       
