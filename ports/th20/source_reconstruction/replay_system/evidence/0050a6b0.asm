0050a6b0 55                       push       ebp
0050a6b1 8bec                     mov        ebp, esp
0050a6b3 83ec24                   sub        esp, 0x24
0050a6b6 894df8                   mov        dword ptr [ebp - 8], ecx
0050a6b9 8b45f8                   mov        eax, dword ptr [ebp - 8]
0050a6bc 83781000                 cmp        dword ptr [eax + 0x10], 0
0050a6c0 0f85b9000000             jne        0x50a77f
0050a6c6 68b8365700               push       0x5736b8
0050a6cb 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0050a6d1 e8aac9ffff               call       0x507080
0050a6d6 8945e8                   mov        dword ptr [ebp - 0x18], eax
0050a6d9 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0050a6dc 83c120                   add        ecx, 0x20
0050a6df 894dec                   mov        dword ptr [ebp - 0x14], ecx
0050a6e2 b968a55b00               mov        ecx, 0x5ba568
0050a6e7 e874a6f6ff               call       0x474d60
0050a6ec 50                       push       eax
0050a6ed 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0050a6f0 e88b9ef0ff               call       0x414580
0050a6f5 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0050a6f8 8910                     mov        dword ptr [eax], edx
0050a6fa 8b45f8                   mov        eax, dword ptr [ebp - 8]
0050a6fd 83c020                   add        eax, 0x20
0050a700 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0050a703 b968a55b00               mov        ecx, 0x5ba568
0050a708 e853a6f6ff               call       0x474d60
0050a70d 50                       push       eax
0050a70e 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0050a711 e86a9ef0ff               call       0x414580
0050a716 8b08                     mov        ecx, dword ptr [eax]
0050a718 894dfc                   mov        dword ptr [ebp - 4], ecx
0050a71b b9a8a45b00               mov        ecx, 0x5ba4a8
0050a720 e86b23f1ff               call       0x41ca90
0050a725 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050a728 894204                   mov        dword ptr [edx + 4], eax
0050a72b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050a72e 8b4804                   mov        ecx, dword ptr [eax + 4]
0050a731 51                       push       ecx
0050a732 b9c4a45b00               mov        ecx, 0x5ba4c4
0050a737 e804f2fcff               call       0x4d9940
0050a73c b9a8a45b00               mov        ecx, 0x5ba4a8
0050a741 e80a33f9ff               call       0x49da50
0050a746 b968a55b00               mov        ecx, 0x5ba568
0050a74b e810a6f6ff               call       0x474d60
0050a750 0fbfd0                   movsx      edx, ax
0050a753 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050a756 8910                     mov        dword ptr [eax], edx
0050a758 e8a318f9ff               call       0x49c000
0050a75d 8945e0                   mov        dword ptr [ebp - 0x20], eax
0050a760 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0050a763 83e101                   and        ecx, 1
0050a766 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050a769 8b8298020000             mov        eax, dword ptr [edx + 0x298]
0050a76f 83e0fe                   and        eax, 0xfffffffe
0050a772 0bc1                     or         eax, ecx
0050a774 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050a777 898198020000             mov        dword ptr [ecx + 0x298], eax
0050a77d eb68                     jmp        0x50a7e7
0050a77f 8b55f8                   mov        edx, dword ptr [ebp - 8]
0050a782 837a1001                 cmp        dword ptr [edx + 0x10], 1
0050a786 755f                     jne        0x50a7e7
0050a788 8b45f8                   mov        eax, dword ptr [ebp - 8]
0050a78b 05e8000000               add        eax, 0xe8
0050a790 8945dc                   mov        dword ptr [ebp - 0x24], eax
0050a793 b968a55b00               mov        ecx, 0x5ba568
0050a798 e8c3a5f6ff               call       0x474d60
0050a79d 50                       push       eax
0050a79e 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0050a7a1 e88ac6f7ff               call       0x486e30
0050a7a6 8945f4                   mov        dword ptr [ebp - 0xc], eax
0050a7a9 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0050a7ac 8b5110                   mov        edx, dword ptr [ecx + 0x10]
0050a7af 8955f0                   mov        dword ptr [ebp - 0x10], edx
0050a7b2 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0050a7b5 e856f6ffff               call       0x509e10
0050a7ba 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0050a7bd 8b4804                   mov        ecx, dword ptr [eax + 4]
0050a7c0 51                       push       ecx
0050a7c1 b9a8a45b00               mov        ecx, 0x5ba4a8
0050a7c6 e875f1fcff               call       0x4d9940
0050a7cb 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0050a7ce 8b4204                   mov        eax, dword ptr [edx + 4]
0050a7d1 50                       push       eax
0050a7d2 b9c4a45b00               mov        ecx, 0x5ba4c4
0050a7d7 e864f1fcff               call       0x4d9940
0050a7dc b9a8a45b00               mov        ecx, 0x5ba4a8
0050a7e1 e86a32f9ff               call       0x49da50
0050a7e6 90                       nop        
0050a7e7 8be5                     mov        esp, ebp
0050a7e9 5d                       pop        ebp
0050a7ea c3                       ret        
0050a7eb cc                       int3       
0050a7ec cc                       int3       
0050a7ed cc                       int3       
0050a7ee cc                       int3       
0050a7ef cc                       int3       
