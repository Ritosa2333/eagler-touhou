004cf5f0 55                       push       ebp
004cf5f1 8bec                     mov        ebp, esp
004cf5f3 83ec08                   sub        esp, 8
004cf5f6 894dfc                   mov        dword ptr [ebp - 4], ecx
004cf5f9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004cf5fc 8b10                     mov        edx, dword ptr [eax]
004cf5fe 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cf601 8b4218                   mov        eax, dword ptr [edx + 0x18]
004cf604 ffd0                     call       eax
004cf606 85c0                     test       eax, eax
004cf608 7405                     je         0x4cf60f
004cf60a 83c8ff                   or         eax, 0xffffffff
004cf60d eb67                     jmp        0x4cf676
004cf60f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cf612 8b11                     mov        edx, dword ptr [ecx]
004cf614 8b424c                   mov        eax, dword ptr [edx + 0x4c]
004cf617 8945f8                   mov        dword ptr [ebp - 8], eax
004cf61a 6a00                     push       0
004cf61c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cf61f ff55f8                   call       dword ptr [ebp - 8]
004cf622 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cf625 81c154070000             add        ecx, 0x754
004cf62b e8a0bff5ff               call       0x42b5d0
004cf630 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cf633 81c1380d0000             add        ecx, 0xd38
004cf639 e892bff5ff               call       0x42b5d0
004cf63e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cf641 8b55fc                   mov        edx, dword ptr [ebp - 4]
004cf644 f30f108184000000         movss      xmm0, dword ptr [ecx + 0x84]
004cf64c f30f598288000000         mulss      xmm0, dword ptr [edx + 0x88]
004cf654 51                       push       ecx
004cf655 f30f110424               movss      dword ptr [esp], xmm0
004cf65a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cf65d 83c148                   add        ecx, 0x48
004cf660 e89b9affff               call       0x4c9100
004cf665 8b45fc                   mov        eax, dword ptr [ebp - 4]
004cf668 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004cf66b 83e1df                   and        ecx, 0xffffffdf
004cf66e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004cf671 894a1c                   mov        dword ptr [edx + 0x1c], ecx
004cf674 33c0                     xor        eax, eax
004cf676 8be5                     mov        esp, ebp
004cf678 5d                       pop        ebp
004cf679 c3                       ret        
004cf67a cc                       int3       
004cf67b cc                       int3       
004cf67c cc                       int3       
004cf67d cc                       int3       
004cf67e cc                       int3       
004cf67f cc                       int3       
