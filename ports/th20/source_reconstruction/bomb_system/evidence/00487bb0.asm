00487bb0 55                       push       ebp
00487bb1 8bec                     mov        ebp, esp
00487bb3 51                       push       ecx
00487bb4 894dfc                   mov        dword ptr [ebp - 4], ecx
00487bb7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487bba e8e10b0000               call       0x4887a0
00487bbf 0fb6c0                   movzx      eax, al
00487bc2 85c0                     test       eax, eax
00487bc4 7461                     je         0x487c27
00487bc6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487bc9 e8020c0000               call       0x4887d0
00487bce 85c0                     test       eax, eax
00487bd0 742a                     je         0x487bfc
00487bd2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487bd5 c7417c00000000           mov        dword ptr [ecx + 0x7c], 0
00487bdc 8b55fc                   mov        edx, dword ptr [ebp - 4]
00487bdf 8b4278                   mov        eax, dword ptr [edx + 0x78]
00487be2 83e0fd                   and        eax, 0xfffffffd
00487be5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487be8 894178                   mov        dword ptr [ecx + 0x78], eax
00487beb 8b55fc                   mov        edx, dword ptr [ebp - 4]
00487bee 8b4278                   mov        eax, dword ptr [edx + 0x78]
00487bf1 83e0df                   and        eax, 0xffffffdf
00487bf4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487bf7 894178                   mov        dword ptr [ecx + 0x78], eax
00487bfa eb2b                     jmp        0x487c27
00487bfc 8b55fc                   mov        edx, dword ptr [ebp - 4]
00487bff 8b8ac0000000             mov        ecx, dword ptr [edx + 0xc0]
00487c05 e806c4f9ff               call       0x424010
00487c0a 8bc8                     mov        ecx, eax
00487c0c e81f05ffff               call       0x478130
00487c11 0fb6c0                   movzx      eax, al
00487c14 85c0                     test       eax, eax
00487c16 740f                     je         0x487c27
00487c18 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487c1b 8b5178                   mov        edx, dword ptr [ecx + 0x78]
00487c1e 83ca20                   or         edx, 0x20
00487c21 8b45fc                   mov        eax, dword ptr [ebp - 4]
00487c24 895078                   mov        dword ptr [eax + 0x78], edx
00487c27 8be5                     mov        esp, ebp
00487c29 5d                       pop        ebp
00487c2a c3                       ret        
00487c2b cc                       int3       
00487c2c cc                       int3       
00487c2d cc                       int3       
00487c2e cc                       int3       
00487c2f cc                       int3       
