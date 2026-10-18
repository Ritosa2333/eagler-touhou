00538290 55                       push       ebp
00538291 8bec                     mov        ebp, esp
00538293 83ec14                   sub        esp, 0x14
00538296 894dfc                   mov        dword ptr [ebp - 4], ecx
00538299 6a00                     push       0
0053829b e890bff2ff               call       0x464230
005382a0 83c404                   add        esp, 4
005382a3 8bc8                     mov        ecx, eax
005382a5 e82674fcff               call       0x4ff6d0
005382aa 83f802                   cmp        eax, 2
005382ad 0f85a3000000             jne        0x538356
005382b3 6a00                     push       0
005382b5 e8c60bf4ff               call       0x478e80
005382ba 83c404                   add        esp, 4
005382bd 8945f4                   mov        dword ptr [ebp - 0xc], eax
005382c0 6a00                     push       0
005382c2 e86985f2ff               call       0x460830
005382c7 83c404                   add        esp, 4
005382ca 8bc8                     mov        ecx, eax
005382cc e87f85f2ff               call       0x460850
005382d1 8945f8                   mov        dword ptr [ebp - 8], eax
005382d4 6a00                     push       0
005382d6 689f860100               push       0x1869f
005382db 6a00                     push       0
005382dd 51                       push       ecx
005382de f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
005382e6 f30f110424               movss      dword ptr [esp], xmm0
005382eb 8b45f8                   mov        eax, dword ptr [ebp - 8]
005382ee 50                       push       eax
005382ef 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
005382f2 e8694cf4ff               call       0x47cf60
005382f7 6a00                     push       0
005382f9 e8e20bf4ff               call       0x478ee0
005382fe 83c404                   add        esp, 4
00538301 8945ec                   mov        dword ptr [ebp - 0x14], eax
00538304 6a00                     push       0
00538306 e82585f2ff               call       0x460830
0053830b 83c404                   add        esp, 4
0053830e 8bc8                     mov        ecx, eax
00538310 e83b85f2ff               call       0x460850
00538315 8945f0                   mov        dword ptr [ebp - 0x10], eax
00538318 6a01                     push       1
0053831a 6a00                     push       0
0053831c 51                       push       ecx
0053831d f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
00538325 f30f110424               movss      dword ptr [esp], xmm0
0053832a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0053832d 51                       push       ecx
0053832e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00538331 e89a27f9ff               call       0x4caad0
00538336 51                       push       ecx
00538337 f30f100598ec5600         movss      xmm0, dword ptr [0x56ec98]
0053833f f30f110424               movss      dword ptr [esp], xmm0
00538344 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00538347 83c114                   add        ecx, 0x14
0053834a e851ccffff               call       0x534fa0
0053834f b801000000               mov        eax, 1
00538354 eb12                     jmp        0x538368
00538356 b9dc695c00               mov        ecx, 0x5c69dc
0053835b e87079f1ff               call       0x44fcd0
00538360 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00538363 e8b8abffff               call       0x532f20
00538368 8be5                     mov        esp, ebp
0053836a 5d                       pop        ebp
0053836b c3                       ret        
0053836c cc                       int3       
0053836d cc                       int3       
0053836e cc                       int3       
0053836f cc                       int3       
