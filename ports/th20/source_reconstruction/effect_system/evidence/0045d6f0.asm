0045d6f0 55                       push       ebp
0045d6f1 8bec                     mov        ebp, esp
0045d6f3 83ec18                   sub        esp, 0x18
0045d6f6 894df8                   mov        dword ptr [ebp - 8], ecx
0045d6f9 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0045d6fc 81c1cc030000             add        ecx, 0x3cc
0045d702 e829f9ffff               call       0x45d030
0045d707 85c0                     test       eax, eax
0045d709 7455                     je         0x45d760
0045d70b 8b45f8                   mov        eax, dword ptr [ebp - 8]
0045d70e 83c008                   add        eax, 8
0045d711 8945f4                   mov        dword ptr [ebp - 0xc], eax
0045d714 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045d717 e884e6faff               call       0x40bda0
0045d71c 8945fc                   mov        dword ptr [ebp - 4], eax
0045d71f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045d722 e8b9050000               call       0x45dce0
0045d727 8945f0                   mov        dword ptr [ebp - 0x10], eax
0045d72a eb0c                     jmp        0x45d738
0045d72c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045d72f 81c1c0030000             add        ecx, 0x3c0
0045d735 894dfc                   mov        dword ptr [ebp - 4], ecx
0045d738 8b55fc                   mov        edx, dword ptr [ebp - 4]
0045d73b 3b55f0                   cmp        edx, dword ptr [ebp - 0x10]
0045d73e 7420                     je         0x45d760
0045d740 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045d743 8945ec                   mov        dword ptr [ebp - 0x14], eax
0045d746 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0045d749 8b4904                   mov        ecx, dword ptr [ecx + 4]
0045d74c e86fa2fdff               call       0x4379c0
0045d751 51                       push       ecx
0045d752 d91c24                   fstp       dword ptr [esp]
0045d755 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0045d758 e823000000               call       0x45d780
0045d75d 90                       nop        
0045d75e ebcc                     jmp        0x45d72c
0045d760 8b55f8                   mov        edx, dword ptr [ebp - 8]
0045d763 81c2cc030000             add        edx, 0x3cc
0045d769 8955e8                   mov        dword ptr [ebp - 0x18], edx
0045d76c 6a00                     push       0
0045d76e 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0045d771 e8ca5dfcff               call       0x423540
0045d776 33c0                     xor        eax, eax
0045d778 8be5                     mov        esp, ebp
0045d77a 5d                       pop        ebp
0045d77b c3                       ret        
0045d77c cc                       int3       
0045d77d cc                       int3       
0045d77e cc                       int3       
0045d77f cc                       int3       
