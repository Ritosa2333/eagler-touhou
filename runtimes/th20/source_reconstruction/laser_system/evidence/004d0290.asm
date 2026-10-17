004d0290 55                       push       ebp
004d0291 8bec                     mov        ebp, esp
004d0293 83ec10                   sub        esp, 0x10
004d0296 894dfc                   mov        dword ptr [ebp - 4], ecx
004d0299 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d029c 054c070000               add        eax, 0x74c
004d02a1 8945f8                   mov        dword ptr [ebp - 8], eax
004d02a4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d02a7 83c158                   add        ecx, 0x58
004d02aa 51                       push       ecx
004d02ab 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004d02ae e83d08faff               call       0x470af0
004d02b3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d02b6 81c24c070000             add        edx, 0x74c
004d02bc 8955f4                   mov        dword ptr [ebp - 0xc], edx
004d02bf f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
004d02c7 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004d02cf 51                       push       ecx
004d02d0 f30f110424               movss      dword ptr [esp], xmm0
004d02d5 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d02d8 51                       push       ecx
004d02d9 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004d02de f30f110424               movss      dword ptr [esp], xmm0
004d02e3 e81883f6ff               call       0x438600
004d02e8 83c404                   add        esp, 4
004d02eb d91c24                   fstp       dword ptr [esp]
004d02ee 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004d02f1 e89a02f8ff               call       0x450590
004d02f6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d02f9 81c14c070000             add        ecx, 0x74c
004d02ff e86cc2f7ff               call       0x44c570
004d0304 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d0307 81c114130000             add        ecx, 0x1314
004d030d e84ec8f7ff               call       0x44cb60
004d0312 50                       push       eax
004d0313 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d0316 e8b5680000               call       0x4d6bd0
004d031b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d031e 81c114130000             add        ecx, 0x1314
004d0324 e847c2f7ff               call       0x44c570
004d0329 90                       nop        
004d032a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d032d f30f108180000000         movss      xmm0, dword ptr [ecx + 0x80]
004d0335 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
004d033c 9f                       lahf       
004d033d f6c444                   test       ah, 0x44
004d0340 7a2a                     jp         0x4d036c
004d0342 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d0345 81c2300d0000             add        edx, 0xd30
004d034b 8955f0                   mov        dword ptr [ebp - 0x10], edx
004d034e 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d0351 83c058                   add        eax, 0x58
004d0354 50                       push       eax
004d0355 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004d0358 e89307faff               call       0x470af0
004d035d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d0360 81c1300d0000             add        ecx, 0xd30
004d0366 e805c2f7ff               call       0x44c570
004d036b 90                       nop        
004d036c 33c0                     xor        eax, eax
004d036e 8be5                     mov        esp, ebp
004d0370 5d                       pop        ebp
004d0371 c3                       ret        
004d0372 cc                       int3       
004d0373 cc                       int3       
004d0374 cc                       int3       
004d0375 cc                       int3       
004d0376 cc                       int3       
004d0377 cc                       int3       
004d0378 cc                       int3       
004d0379 cc                       int3       
004d037a cc                       int3       
004d037b cc                       int3       
004d037c cc                       int3       
004d037d cc                       int3       
004d037e cc                       int3       
004d037f cc                       int3       
