0045e770 55                       push       ebp
0045e771 8bec                     mov        ebp, esp
0045e773 83ec18                   sub        esp, 0x18
0045e776 894df8                   mov        dword ptr [ebp - 8], ecx
0045e779 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0045e77c 81c14c0b0000             add        ecx, 0xb4c
0045e782 e8a9e8ffff               call       0x45d030
0045e787 85c0                     test       eax, eax
0045e789 7455                     je         0x45e7e0
0045e78b 8b45f8                   mov        eax, dword ptr [ebp - 8]
0045e78e 83c008                   add        eax, 8
0045e791 8945f4                   mov        dword ptr [ebp - 0xc], eax
0045e794 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045e797 e804d6faff               call       0x40bda0
0045e79c 8945fc                   mov        dword ptr [ebp - 4], eax
0045e79f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045e7a2 e8f9040000               call       0x45eca0
0045e7a7 8945f0                   mov        dword ptr [ebp - 0x10], eax
0045e7aa eb0c                     jmp        0x45e7b8
0045e7ac 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045e7af 81c1c0030000             add        ecx, 0x3c0
0045e7b5 894dfc                   mov        dword ptr [ebp - 4], ecx
0045e7b8 8b55fc                   mov        edx, dword ptr [ebp - 4]
0045e7bb 3b55f0                   cmp        edx, dword ptr [ebp - 0x10]
0045e7be 7420                     je         0x45e7e0
0045e7c0 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045e7c3 8945ec                   mov        dword ptr [ebp - 0x14], eax
0045e7c6 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0045e7c9 8b4904                   mov        ecx, dword ptr [ecx + 4]
0045e7cc e8ef91fdff               call       0x4379c0
0045e7d1 51                       push       ecx
0045e7d2 d91c24                   fstp       dword ptr [esp]
0045e7d5 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0045e7d8 e823000000               call       0x45e800
0045e7dd 90                       nop        
0045e7de ebcc                     jmp        0x45e7ac
0045e7e0 8b55f8                   mov        edx, dword ptr [ebp - 8]
0045e7e3 81c24c0b0000             add        edx, 0xb4c
0045e7e9 8955e8                   mov        dword ptr [ebp - 0x18], edx
0045e7ec 6a00                     push       0
0045e7ee 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0045e7f1 e84a4dfcff               call       0x423540
0045e7f6 33c0                     xor        eax, eax
0045e7f8 8be5                     mov        esp, ebp
0045e7fa 5d                       pop        ebp
0045e7fb c3                       ret        
0045e7fc cc                       int3       
0045e7fd cc                       int3       
0045e7fe cc                       int3       
0045e7ff cc                       int3       
