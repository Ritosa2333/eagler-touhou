00534420 55                       push       ebp
00534421 8bec                     mov        ebp, esp
00534423 83ec10                   sub        esp, 0x10
00534426 894dfc                   mov        dword ptr [ebp - 4], ecx
00534429 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053442c 8b4828                   mov        ecx, dword ptr [eax + 0x28]
0053442f 51                       push       ecx
00534430 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00534436 e805e3ffff               call       0x532740
0053443b 8b55fc                   mov        edx, dword ptr [ebp - 4]
0053443e c7422800000000           mov        dword ptr [edx + 0x28], 0
00534445 6a00                     push       0
00534447 e834fcf2ff               call       0x464080
0053444c 83c404                   add        esp, 4
0053444f 8945f8                   mov        dword ptr [ebp - 8], eax
00534452 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00534455 50                       push       eax
00534456 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00534459 e862e9edff               call       0x412dc0
0053445e 6b4d0824                 imul       ecx, dword ptr [ebp + 8], 0x24
00534462 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00534465 8b8491b00a5b00           mov        eax, dword ptr [ecx + edx*4 + 0x5b0ab0]
0053446c ffd0                     call       eax
0053446e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534471 894128                   mov        dword ptr [ecx + 0x28], eax
00534474 8b55fc                   mov        edx, dword ptr [ebp - 4]
00534477 8b4228                   mov        eax, dword ptr [edx + 0x28]
0053447a 8945f4                   mov        dword ptr [ebp - 0xc], eax
0053447d 6a00                     push       0
0053447f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00534482 e839e9edff               call       0x412dc0
00534487 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053448a 8b5128                   mov        edx, dword ptr [ecx + 0x28]
0053448d 8955f0                   mov        dword ptr [ebp - 0x10], edx
00534490 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00534493 50                       push       eax
00534494 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00534497 e8d4d0edff               call       0x411570
0053449c 90                       nop        
0053449d 8be5                     mov        esp, ebp
0053449f 5d                       pop        ebp
005344a0 c20800                   ret        8
005344a3 cc                       int3       
005344a4 cc                       int3       
005344a5 cc                       int3       
005344a6 cc                       int3       
005344a7 cc                       int3       
005344a8 cc                       int3       
005344a9 cc                       int3       
005344aa cc                       int3       
005344ab cc                       int3       
005344ac cc                       int3       
005344ad cc                       int3       
005344ae cc                       int3       
005344af cc                       int3       
