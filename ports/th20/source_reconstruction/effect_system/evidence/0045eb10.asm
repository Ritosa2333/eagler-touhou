0045eb10 55                       push       ebp
0045eb11 8bec                     mov        ebp, esp
0045eb13 83ec2c                   sub        esp, 0x2c
0045eb16 894dfc                   mov        dword ptr [ebp - 4], ecx
0045eb19 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045eb1c 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0045eb1f 89885c0b0000             mov        dword ptr [eax + 0xb5c], ecx
0045eb25 8b55fc                   mov        edx, dword ptr [ebp - 4]
0045eb28 8b4508                   mov        eax, dword ptr [ebp + 8]
0045eb2b 8b4824                   mov        ecx, dword ptr [eax + 0x24]
0045eb2e 898a480b0000             mov        dword ptr [edx + 0xb48], ecx
0045eb34 6a00                     push       0
0045eb36 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045eb39 81c14c0b0000             add        ecx, 0xb4c
0045eb3f e8dc49fcff               call       0x423520
0045eb44 8b55fc                   mov        edx, dword ptr [ebp - 4]
0045eb47 83c208                   add        edx, 8
0045eb4a 8955f8                   mov        dword ptr [ebp - 8], edx
0045eb4d 6a00                     push       0
0045eb4f 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0045eb52 e839ebffff               call       0x45d690
0045eb57 8945f4                   mov        dword ptr [ebp - 0xc], eax
0045eb5a 684040ffff               push       0xffff4040
0045eb5f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045eb62 e8a9000000               call       0x45ec10
0045eb67 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045eb6a 83c008                   add        eax, 8
0045eb6d 8945f0                   mov        dword ptr [ebp - 0x10], eax
0045eb70 6a01                     push       1
0045eb72 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0045eb75 e816ebffff               call       0x45d690
0045eb7a 8945ec                   mov        dword ptr [ebp - 0x14], eax
0045eb7d 68ff40c0ff               push       0xffc040ff
0045eb82 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0045eb85 e886000000               call       0x45ec10
0045eb8a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045eb8d 83c108                   add        ecx, 8
0045eb90 894de8                   mov        dword ptr [ebp - 0x18], ecx
0045eb93 6a02                     push       2
0045eb95 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0045eb98 e8f3eaffff               call       0x45d690
0045eb9d 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0045eba0 6840ffffff               push       0xffffff40
0045eba5 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0045eba8 e863000000               call       0x45ec10
0045ebad 8b55fc                   mov        edx, dword ptr [ebp - 4]
0045ebb0 8b4204                   mov        eax, dword ptr [edx + 4]
0045ebb3 8945e0                   mov        dword ptr [ebp - 0x20], eax
0045ebb6 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0045ebb9 51                       push       ecx
0045ebba 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0045ebbd e89ef1ffff               call       0x45dd60
0045ebc2 8b55fc                   mov        edx, dword ptr [ebp - 4]
0045ebc5 8b4204                   mov        eax, dword ptr [edx + 4]
0045ebc8 8945dc                   mov        dword ptr [ebp - 0x24], eax
0045ebcb 6a0b                     push       0xb
0045ebcd 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0045ebd0 e87b17ffff               call       0x450350
0045ebd5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045ebd8 8b5104                   mov        edx, dword ptr [ecx + 4]
0045ebdb 8955d8                   mov        dword ptr [ebp - 0x28], edx
0045ebde 6a00                     push       0
0045ebe0 68ff000000               push       0xff
0045ebe5 6a00                     push       0
0045ebe7 6a1e                     push       0x1e
0045ebe9 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0045ebec e86fa0fdff               call       0x438c60
0045ebf1 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045ebf4 8b4804                   mov        ecx, dword ptr [eax + 4]
0045ebf7 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
0045ebfa 6a01                     push       1
0045ebfc 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0045ebff e83cf1ffff               call       0x45dd40
0045ec04 33c0                     xor        eax, eax
0045ec06 8be5                     mov        esp, ebp
0045ec08 5d                       pop        ebp
0045ec09 c20800                   ret        8
0045ec0c cc                       int3       
0045ec0d cc                       int3       
0045ec0e cc                       int3       
0045ec0f cc                       int3       
