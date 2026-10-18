00535310 55                       push       ebp
00535311 8bec                     mov        ebp, esp
00535313 83ec0c                   sub        esp, 0xc
00535316 894df4                   mov        dword ptr [ebp - 0xc], ecx
00535319 6a00                     push       0
0053531b e860edf2ff               call       0x464080
00535320 83c404                   add        esp, 4
00535323 8945fc                   mov        dword ptr [ebp - 4], eax
00535326 6a01                     push       1
00535328 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053532b e81089f8ff               call       0x4bdc40
00535330 6a00                     push       0
00535332 e849edf2ff               call       0x464080
00535337 83c404                   add        esp, 4
0053533a 8945f8                   mov        dword ptr [ebp - 8], eax
0053533d 6a00                     push       0
0053533f 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00535342 e8e910f5ff               call       0x486430
00535347 90                       nop        
00535348 8be5                     mov        esp, ebp
0053534a 5d                       pop        ebp
0053534b c3                       ret        
0053534c cc                       int3       
0053534d cc                       int3       
0053534e cc                       int3       
0053534f cc                       int3       
