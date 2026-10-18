004f8af0 55                       push       ebp
004f8af1 8bec                     mov        ebp, esp
004f8af3 51                       push       ecx
004f8af4 894dfc                   mov        dword ptr [ebp - 4], ecx
004f8af7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004f8afa e8018cf1ff               call       0x411700
004f8aff 83f802                   cmp        eax, 2
004f8b02 7507                     jne        0x4f8b0b
004f8b04 b801000000               mov        eax, 1
004f8b09 eb72                     jmp        0x4f8b7d
004f8b0b 8b45fc                   mov        eax, dword ptr [ebp - 4]
004f8b0e 8b8854480100             mov        ecx, dword ptr [eax + 0x14854]
004f8b14 51                       push       ecx
004f8b15 6a0c                     push       0xc
004f8b17 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004f8b1d e8ae68f5ff               call       0x44f3d0
004f8b22 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004f8b25 e8267df6ff               call       0x460850
004f8b2a 8b55fc                   mov        edx, dword ptr [ebp - 4]
004f8b2d 81c2e4050000             add        edx, 0x5e4
004f8b33 8b08                     mov        ecx, dword ptr [eax]
004f8b35 890a                     mov        dword ptr [edx], ecx
004f8b37 8b4804                   mov        ecx, dword ptr [eax + 4]
004f8b3a 894a04                   mov        dword ptr [edx + 4], ecx
004f8b3d 8b4008                   mov        eax, dword ptr [eax + 8]
004f8b40 894208                   mov        dword ptr [edx + 8], eax
004f8b43 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004f8b46 8a91cb040000             mov        dl, byte ptr [ecx + 0x4cb]
004f8b4c 80e2fc                   and        dl, 0xfc
004f8b4f 80ca01                   or         dl, 1
004f8b52 8b45fc                   mov        eax, dword ptr [ebp - 4]
004f8b55 8890cb040000             mov        byte ptr [eax + 0x4cb], dl
004f8b5b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004f8b5e 83c128                   add        ecx, 0x28
004f8b61 51                       push       ecx
004f8b62 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004f8b68 e813adf4ff               call       0x443880
004f8b6d 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004f8b73 e848caf4ff               call       0x4455c0
004f8b78 b801000000               mov        eax, 1
004f8b7d 8be5                     mov        esp, ebp
004f8b7f 5d                       pop        ebp
004f8b80 c3                       ret        
004f8b81 cc                       int3       
004f8b82 cc                       int3       
004f8b83 cc                       int3       
004f8b84 cc                       int3       
004f8b85 cc                       int3       
004f8b86 cc                       int3       
004f8b87 cc                       int3       
004f8b88 cc                       int3       
004f8b89 cc                       int3       
004f8b8a cc                       int3       
004f8b8b cc                       int3       
004f8b8c cc                       int3       
004f8b8d cc                       int3       
004f8b8e cc                       int3       
004f8b8f cc                       int3       
