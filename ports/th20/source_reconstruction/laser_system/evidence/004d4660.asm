004d4660 55                       push       ebp
004d4661 8bec                     mov        ebp, esp
004d4663 83ec14                   sub        esp, 0x14
004d4666 894dfc                   mov        dword ptr [ebp - 4], ecx
004d4669 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d466c 0590000000               add        eax, 0x90
004d4671 8945f8                   mov        dword ptr [ebp - 8], eax
004d4674 6a0b                     push       0xb
004d4676 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004d4679 e80280faff               call       0x47c680
004d467e 8945f4                   mov        dword ptr [ebp - 0xc], eax
004d4681 6a00                     push       0
004d4683 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004d4686 e8954df5ff               call       0x429420
004d468b 90                       nop        
004d468c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d468f 81c190000000             add        ecx, 0x90
004d4695 894df0                   mov        dword ptr [ebp - 0x10], ecx
004d4698 6a0b                     push       0xb
004d469a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004d469d e8de7ffaff               call       0x47c680
004d46a2 8945ec                   mov        dword ptr [ebp - 0x14], eax
004d46a5 6a00                     push       0
004d46a7 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d46aa e8e1eef4ff               call       0x423590
004d46af 0fb6d0                   movzx      edx, al
004d46b2 85d2                     test       edx, edx
004d46b4 741f                     je         0x4d46d5
004d46b6 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d46b9 8b8898060000             mov        ecx, dword ptr [eax + 0x698]
004d46bf 81e1fffeffff             and        ecx, 0xfffffeff
004d46c5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d46c8 898a98060000             mov        dword ptr [edx + 0x698], ecx
004d46ce b801000000               mov        eax, 1
004d46d3 eb02                     jmp        0x4d46d7
004d46d5 33c0                     xor        eax, eax
004d46d7 8be5                     mov        esp, ebp
004d46d9 5d                       pop        ebp
004d46da c3                       ret        
004d46db cc                       int3       
004d46dc cc                       int3       
004d46dd cc                       int3       
004d46de cc                       int3       
004d46df cc                       int3       
