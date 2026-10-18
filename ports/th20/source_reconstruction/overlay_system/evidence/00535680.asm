00535680 55                       push       ebp
00535681 8bec                     mov        ebp, esp
00535683 83ec48                   sub        esp, 0x48
00535686 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0053568b 33c5                     xor        eax, ebp
0053568d 8945fc                   mov        dword ptr [ebp - 4], eax
00535690 894dc0                   mov        dword ptr [ebp - 0x40], ecx
00535693 e8c8ebffff               call       0x534260
00535698 50                       push       eax
00535699 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0053569c 83c114                   add        ecx, 0x14
0053569f e87cdeeeff               call       0x423520
005356a4 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
005356a7 c6403401                 mov        byte ptr [eax + 0x34], 1
005356ab 8d4dc4                   lea        ecx, [ebp - 0x3c]
005356ae e87d63f4ff               call       0x47ba30
005356b3 6a00                     push       0
005356b5 e876b1f2ff               call       0x460830
005356ba 83c404                   add        esp, 4
005356bd 8bc8                     mov        ecx, eax
005356bf e88cb1f2ff               call       0x460850
005356c4 8b08                     mov        ecx, dword ptr [eax]
005356c6 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
005356c9 8b5004                   mov        edx, dword ptr [eax + 4]
005356cc 8955c8                   mov        dword ptr [ebp - 0x38], edx
005356cf 8b4008                   mov        eax, dword ptr [eax + 8]
005356d2 8945cc                   mov        dword ptr [ebp - 0x34], eax
005356d5 b970695c00               mov        ecx, 0x5c6970
005356da e8f1a5f1ff               call       0x44fcd0
005356df 90                       nop        
005356e0 6a00                     push       0
005356e2 e8391ef0ff               call       0x437520
005356e7 83c404                   add        esp, 4
005356ea 8945bc                   mov        dword ptr [ebp - 0x44], eax
005356ed 6a00                     push       0
005356ef 8d4dc4                   lea        ecx, [ebp - 0x3c]
005356f2 51                       push       ecx
005356f3 6a07                     push       7
005356f5 8d55b8                   lea        edx, [ebp - 0x48]
005356f8 52                       push       edx
005356f9 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
005356fc e86f84f6ff               call       0x49db70
00535701 8b00                     mov        eax, dword ptr [eax]
00535703 a370695c00               mov        dword ptr [0x5c6970], eax
00535708 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0053570b 51                       push       ecx
0053570c b970695c00               mov        ecx, 0x5c6970
00535711 e83ab1f1ff               call       0x450850
00535716 68304e5300               push       0x534e30
0053571b b970695c00               mov        ecx, 0x5c6970
00535720 e8db99f1ff               call       0x44f100
00535725 90                       nop        
00535726 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00535729 33cd                     xor        ecx, ebp
0053572b e8bed20000               call       0x5429ee
00535730 8be5                     mov        esp, ebp
00535732 5d                       pop        ebp
00535733 c3                       ret        
00535734 cc                       int3       
00535735 cc                       int3       
00535736 cc                       int3       
00535737 cc                       int3       
00535738 cc                       int3       
00535739 cc                       int3       
0053573a cc                       int3       
0053573b cc                       int3       
0053573c cc                       int3       
0053573d cc                       int3       
0053573e cc                       int3       
0053573f cc                       int3       
