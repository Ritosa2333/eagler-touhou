00533720 55                       push       ebp
00533721 8bec                     mov        ebp, esp
00533723 83ec08                   sub        esp, 8
00533726 894dfc                   mov        dword ptr [ebp - 4], ecx
00533729 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053372c 8b08                     mov        ecx, dword ptr [eax]
0053372e 034d0c                   add        ecx, dword ptr [ebp + 0xc]
00533731 8b55fc                   mov        edx, dword ptr [ebp - 4]
00533734 890a                     mov        dword ptr [edx], ecx
00533736 8b45fc                   mov        eax, dword ptr [ebp - 4]
00533739 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053373c 8b10                     mov        edx, dword ptr [eax]
0053373e 3b5104                   cmp        edx, dword ptr [ecx + 4]
00533741 7e31                     jle        0x533774
00533743 6a00                     push       0
00533745 e88658f6ff               call       0x498fd0
0053374a 83c404                   add        esp, 4
0053374d 8945f8                   mov        dword ptr [ebp - 8], eax
00533750 8b4510                   mov        eax, dword ptr [ebp + 0x10]
00533753 50                       push       eax
00533754 6a01                     push       1
00533756 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00533759 51                       push       ecx
0053375a 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0053375d e84e0ef9ff               call       0x4c45b0
00533762 8b55fc                   mov        edx, dword ptr [ebp - 4]
00533765 8b45fc                   mov        eax, dword ptr [ebp - 4]
00533768 8b0a                     mov        ecx, dword ptr [edx]
0053376a 2b4804                   sub        ecx, dword ptr [eax + 4]
0053376d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00533770 890a                     mov        dword ptr [edx], ecx
00533772 ebc2                     jmp        0x533736
00533774 8be5                     mov        esp, ebp
00533776 5d                       pop        ebp
00533777 c20c00                   ret        0xc
0053377a cc                       int3       
0053377b cc                       int3       
0053377c cc                       int3       
0053377d cc                       int3       
0053377e cc                       int3       
0053377f cc                       int3       
