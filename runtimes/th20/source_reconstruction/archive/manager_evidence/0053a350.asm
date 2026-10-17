0053a350 55                       push       ebp
0053a351 8bec                     mov        ebp, esp
0053a353 83ec0c                   sub        esp, 0xc
0053a356 894df8                   mov        dword ptr [ebp - 8], ecx
0053a359 8b45f8                   mov        eax, dword ptr [ebp - 8]
0053a35c 833800                   cmp        dword ptr [eax], 0
0053a35f 7504                     jne        0x53a365
0053a361 33c0                     xor        eax, eax
0053a363 eb4a                     jmp        0x53a3af
0053a365 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0053a368 8b11                     mov        edx, dword ptr [ecx]
0053a36a 8955fc                   mov        dword ptr [ebp - 4], edx
0053a36d 8b45f8                   mov        eax, dword ptr [ebp - 8]
0053a370 8b4804                   mov        ecx, dword ptr [eax + 4]
0053a373 894df4                   mov        dword ptr [ebp - 0xc], ecx
0053a376 eb12                     jmp        0x53a38a
0053a378 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0053a37b 83ea01                   sub        edx, 1
0053a37e 8955f4                   mov        dword ptr [ebp - 0xc], edx
0053a381 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053a384 83c010                   add        eax, 0x10
0053a387 8945fc                   mov        dword ptr [ebp - 4], eax
0053a38a 837df400                 cmp        dword ptr [ebp - 0xc], 0
0053a38e 7e1d                     jle        0x53a3ad
0053a390 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053a393 8b11                     mov        edx, dword ptr [ecx]
0053a395 52                       push       edx
0053a396 8b4508                   mov        eax, dword ptr [ebp + 8]
0053a399 50                       push       eax
0053a39a e8b1b30100               call       0x555750
0053a39f 83c408                   add        esp, 8
0053a3a2 85c0                     test       eax, eax
0053a3a4 7505                     jne        0x53a3ab
0053a3a6 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053a3a9 eb04                     jmp        0x53a3af
0053a3ab ebcb                     jmp        0x53a378
0053a3ad 33c0                     xor        eax, eax
0053a3af 8be5                     mov        esp, ebp
0053a3b1 5d                       pop        ebp
0053a3b2 c20400                   ret        4
0053a3b5 cc                       int3       
0053a3b6 cc                       int3       
0053a3b7 cc                       int3       
0053a3b8 cc                       int3       
0053a3b9 cc                       int3       
0053a3ba cc                       int3       
0053a3bb cc                       int3       
0053a3bc cc                       int3       
0053a3bd cc                       int3       
0053a3be cc                       int3       
0053a3bf cc                       int3       
