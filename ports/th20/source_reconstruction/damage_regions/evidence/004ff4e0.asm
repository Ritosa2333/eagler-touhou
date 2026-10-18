004ff4e0 55                       push       ebp
004ff4e1 8bec                     mov        ebp, esp
004ff4e3 83ec24                   sub        esp, 0x24
004ff4e6 894dfc                   mov        dword ptr [ebp - 4], ecx
004ff4e9 6a00                     push       0
004ff4eb e8404df6ff               call       0x464230
004ff4f0 83c404                   add        esp, 4
004ff4f3 8bc8                     mov        ecx, eax
004ff4f5 e846030000               call       0x4ff840
004ff4fa 0fb6c0                   movzx      eax, al
004ff4fd 85c0                     test       eax, eax
004ff4ff 743e                     je         0x4ff53f
004ff501 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ff504 8b9108220000             mov        edx, dword ptr [ecx + 0x2208]
004ff50a 83c228                   add        edx, 0x28
004ff50d 8955f0                   mov        dword ptr [ebp - 0x10], edx
004ff510 6a00                     push       0
004ff512 e8694bf6ff               call       0x464080
004ff517 83c404                   add        esp, 4
004ff51a 8bc8                     mov        ecx, eax
004ff51c e8df21f1ff               call       0x411700
004ff521 6bc00c                   imul       eax, eax, 0xc
004ff524 8945ec                   mov        dword ptr [ebp - 0x14], eax
004ff527 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004ff52a 034dec                   add        ecx, dword ptr [ebp - 0x14]
004ff52d ba04000000               mov        edx, 4
004ff532 d1e2                     shl        edx, 1
004ff534 8b0411                   mov        eax, dword ptr [ecx + edx]
004ff537 8945f4                   mov        dword ptr [ebp - 0xc], eax
004ff53a e98b000000               jmp        0x4ff5ca
004ff53f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ff542 e899000000               call       0x4ff5e0
004ff547 0fb6c8                   movzx      ecx, al
004ff54a 85c9                     test       ecx, ecx
004ff54c 743c                     je         0x4ff58a
004ff54e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ff551 8b8208220000             mov        eax, dword ptr [edx + 0x2208]
004ff557 83c028                   add        eax, 0x28
004ff55a 8945e8                   mov        dword ptr [ebp - 0x18], eax
004ff55d 6a00                     push       0
004ff55f e81c4bf6ff               call       0x464080
004ff564 83c404                   add        esp, 4
004ff567 8bc8                     mov        ecx, eax
004ff569 e89221f1ff               call       0x411700
004ff56e 6bc80c                   imul       ecx, eax, 0xc
004ff571 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004ff574 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004ff577 0355e4                   add        edx, dword ptr [ebp - 0x1c]
004ff57a b804000000               mov        eax, 4
004ff57f c1e000                   shl        eax, 0
004ff582 8b0c02                   mov        ecx, dword ptr [edx + eax]
004ff585 894df8                   mov        dword ptr [ebp - 8], ecx
004ff588 eb3a                     jmp        0x4ff5c4
004ff58a 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ff58d 8b8208220000             mov        eax, dword ptr [edx + 0x2208]
004ff593 83c028                   add        eax, 0x28
004ff596 8945e0                   mov        dword ptr [ebp - 0x20], eax
004ff599 6a00                     push       0
004ff59b e8e04af6ff               call       0x464080
004ff5a0 83c404                   add        esp, 4
004ff5a3 8bc8                     mov        ecx, eax
004ff5a5 e8e6d4f1ff               call       0x41ca90
004ff5aa 6bc80c                   imul       ecx, eax, 0xc
004ff5ad 894ddc                   mov        dword ptr [ebp - 0x24], ecx
004ff5b0 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004ff5b3 0355dc                   add        edx, dword ptr [ebp - 0x24]
004ff5b6 b804000000               mov        eax, 4
004ff5bb 6bc800                   imul       ecx, eax, 0
004ff5be 8b140a                   mov        edx, dword ptr [edx + ecx]
004ff5c1 8955f8                   mov        dword ptr [ebp - 8], edx
004ff5c4 8b45f8                   mov        eax, dword ptr [ebp - 8]
004ff5c7 8945f4                   mov        dword ptr [ebp - 0xc], eax
004ff5ca 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004ff5cd 8be5                     mov        esp, ebp
004ff5cf 5d                       pop        ebp
004ff5d0 c3                       ret        
004ff5d1 cc                       int3       
004ff5d2 cc                       int3       
004ff5d3 cc                       int3       
004ff5d4 cc                       int3       
004ff5d5 cc                       int3       
004ff5d6 cc                       int3       
004ff5d7 cc                       int3       
004ff5d8 cc                       int3       
004ff5d9 cc                       int3       
004ff5da cc                       int3       
004ff5db cc                       int3       
004ff5dc cc                       int3       
004ff5dd cc                       int3       
004ff5de cc                       int3       
004ff5df cc                       int3       
