00534fd0 55                       push       ebp
00534fd1 8bec                     mov        ebp, esp
00534fd3 83ec14                   sub        esp, 0x14
00534fd6 894dfc                   mov        dword ptr [ebp - 4], ecx
00534fd9 6a00                     push       0
00534fdb e850f2f2ff               call       0x464230
00534fe0 83c404                   add        esp, 4
00534fe3 8bc8                     mov        ecx, eax
00534fe5 e8e6a6fcff               call       0x4ff6d0
00534fea 83f802                   cmp        eax, 2
00534fed 0f85a3000000             jne        0x535096
00534ff3 6a00                     push       0
00534ff5 e8863ef4ff               call       0x478e80
00534ffa 83c404                   add        esp, 4
00534ffd 8945f4                   mov        dword ptr [ebp - 0xc], eax
00535000 6a00                     push       0
00535002 e829b8f2ff               call       0x460830
00535007 83c404                   add        esp, 4
0053500a 8bc8                     mov        ecx, eax
0053500c e83fb8f2ff               call       0x460850
00535011 8945f8                   mov        dword ptr [ebp - 8], eax
00535014 6a00                     push       0
00535016 689f860100               push       0x1869f
0053501b 6a00                     push       0
0053501d 51                       push       ecx
0053501e f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
00535026 f30f110424               movss      dword ptr [esp], xmm0
0053502b 8b45f8                   mov        eax, dword ptr [ebp - 8]
0053502e 50                       push       eax
0053502f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00535032 e8297ff4ff               call       0x47cf60
00535037 6a00                     push       0
00535039 e8a23ef4ff               call       0x478ee0
0053503e 83c404                   add        esp, 4
00535041 8945ec                   mov        dword ptr [ebp - 0x14], eax
00535044 6a00                     push       0
00535046 e8e5b7f2ff               call       0x460830
0053504b 83c404                   add        esp, 4
0053504e 8bc8                     mov        ecx, eax
00535050 e8fbb7f2ff               call       0x460850
00535055 8945f0                   mov        dword ptr [ebp - 0x10], eax
00535058 6a01                     push       1
0053505a 6a00                     push       0
0053505c 51                       push       ecx
0053505d f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
00535065 f30f110424               movss      dword ptr [esp], xmm0
0053506a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0053506d 51                       push       ecx
0053506e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00535071 e85a5af9ff               call       0x4caad0
00535076 51                       push       ecx
00535077 f30f100598ec5600         movss      xmm0, dword ptr [0x56ec98]
0053507f f30f110424               movss      dword ptr [esp], xmm0
00535084 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00535087 83c114                   add        ecx, 0x14
0053508a e811ffffff               call       0x534fa0
0053508f b801000000               mov        eax, 1
00535094 eb12                     jmp        0x5350a8
00535096 b970695c00               mov        ecx, 0x5c6970
0053509b e830acf1ff               call       0x44fcd0
005350a0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005350a3 e878deffff               call       0x532f20
005350a8 8be5                     mov        esp, ebp
005350aa 5d                       pop        ebp
005350ab c3                       ret        
005350ac cc                       int3       
005350ad cc                       int3       
005350ae cc                       int3       
005350af cc                       int3       
