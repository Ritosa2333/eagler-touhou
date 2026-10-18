00531460 55                       push       ebp
00531461 8bec                     mov        ebp, esp
00531463 83ec50                   sub        esp, 0x50
00531466 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0053146b 33c5                     xor        eax, ebp
0053146d 8945fc                   mov        dword ptr [ebp - 4], eax
00531470 894dc0                   mov        dword ptr [ebp - 0x40], ecx
00531473 e8e82d0000               call       0x534260
00531478 50                       push       eax
00531479 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0053147c 83c114                   add        ecx, 0x14
0053147f e89c20efff               call       0x423520
00531484 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
00531487 c6403401                 mov        byte ptr [eax + 0x34], 1
0053148b 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0053148e 83c138                   add        ecx, 0x38
00531491 e83ae8f1ff               call       0x44fcd0
00531496 8d4dc4                   lea        ecx, [ebp - 0x3c]
00531499 e892a5f4ff               call       0x47ba30
0053149e 6a00                     push       0
005314a0 e88bf3f2ff               call       0x460830
005314a5 83c404                   add        esp, 4
005314a8 8bc8                     mov        ecx, eax
005314aa e8a1f3f2ff               call       0x460850
005314af 8b08                     mov        ecx, dword ptr [eax]
005314b1 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
005314b4 8b5004                   mov        edx, dword ptr [eax + 4]
005314b7 8955c8                   mov        dword ptr [ebp - 0x38], edx
005314ba 8b4008                   mov        eax, dword ptr [eax + 8]
005314bd 8945cc                   mov        dword ptr [ebp - 0x34], eax
005314c0 6a00                     push       0
005314c2 e85960f0ff               call       0x437520
005314c7 83c404                   add        esp, 4
005314ca 8945b8                   mov        dword ptr [ebp - 0x48], eax
005314cd 6a00                     push       0
005314cf 8d4dc4                   lea        ecx, [ebp - 0x3c]
005314d2 51                       push       ecx
005314d3 6a08                     push       8
005314d5 8d55b0                   lea        edx, [ebp - 0x50]
005314d8 52                       push       edx
005314d9 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
005314dc e88fc6f6ff               call       0x49db70
005314e1 8b00                     mov        eax, dword ptr [eax]
005314e3 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
005314e6 894138                   mov        dword ptr [ecx + 0x38], eax
005314e9 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
005314ec 83c238                   add        edx, 0x38
005314ef 8955b4                   mov        dword ptr [ebp - 0x4c], edx
005314f2 51                       push       ecx
005314f3 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
005314fb f30f110424               movss      dword ptr [esp], xmm0
00531500 51                       push       ecx
00531501 f30f100580fb5600         movss      xmm0, dword ptr [0x56fb80]
00531509 f30f110424               movss      dword ptr [esp], xmm0
0053150e 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00531511 e89af1f1ff               call       0x4506b0
00531516 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00531519 83c138                   add        ecx, 0x38
0053151c e8afb9f1ff               call       0x44ced0
00531521 8945bc                   mov        dword ptr [ebp - 0x44], eax
00531524 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
00531527 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0053152a 8988c8050000             mov        dword ptr [eax + 0x5c8], ecx
00531530 6850105300               push       0x531050
00531535 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00531538 e81375f0ff               call       0x438a50
0053153d 90                       nop        
0053153e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531541 33cd                     xor        ecx, ebp
00531543 e8a6140100               call       0x5429ee
00531548 8be5                     mov        esp, ebp
0053154a 5d                       pop        ebp
0053154b c3                       ret        
0053154c cc                       int3       
0053154d cc                       int3       
0053154e cc                       int3       
0053154f cc                       int3       
