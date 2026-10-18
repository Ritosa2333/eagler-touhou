004bfcd0 55                       push       ebp
004bfcd1 8bec                     mov        ebp, esp
004bfcd3 83ec1c                   sub        esp, 0x1c
004bfcd6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004bfcdb 33c5                     xor        eax, ebp
004bfcdd 8945fc                   mov        dword ptr [ebp - 4], eax
004bfce0 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004bfce3 e888120000               call       0x4c0f70
004bfce8 8945ec                   mov        dword ptr [ebp - 0x14], eax
004bfceb 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004bfcee 8b08                     mov        ecx, dword ptr [eax]
004bfcf0 894df0                   mov        dword ptr [ebp - 0x10], ecx
004bfcf3 8b5004                   mov        edx, dword ptr [eax + 4]
004bfcf6 8955f4                   mov        dword ptr [ebp - 0xc], edx
004bfcf9 8b4008                   mov        eax, dword ptr [eax + 8]
004bfcfc 8945f8                   mov        dword ptr [ebp - 8], eax
004bfcff 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bfd02 83794c00                 cmp        dword ptr [ecx + 0x4c], 0
004bfd06 7435                     je         0x4bfd3d
004bfd08 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004bfd0b 8b424c                   mov        eax, dword ptr [edx + 0x4c]
004bfd0e 8945e8                   mov        dword ptr [ebp - 0x18], eax
004bfd11 51                       push       ecx
004bfd12 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
004bfd17 f30f110424               movss      dword ptr [esp], xmm0
004bfd1c 51                       push       ecx
004bfd1d f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004bfd22 f30f110424               movss      dword ptr [esp], xmm0
004bfd27 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
004bfd2a 51                       push       ecx
004bfd2b 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004bfd2e 52                       push       edx
004bfd2f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bfd32 ff55e8                   call       dword ptr [ebp - 0x18]
004bfd35 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004bfd38 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004bfd3b eb08                     jmp        0x4bfd45
004bfd3d 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bfd40 e8ab4d0400               call       0x504af0
004bfd45 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfd48 33cd                     xor        ecx, ebp
004bfd4a e89f2c0800               call       0x5429ee
004bfd4f 8be5                     mov        esp, ebp
004bfd51 5d                       pop        ebp
004bfd52 c3                       ret        
004bfd53 cc                       int3       
004bfd54 cc                       int3       
004bfd55 cc                       int3       
004bfd56 cc                       int3       
004bfd57 cc                       int3       
004bfd58 cc                       int3       
004bfd59 cc                       int3       
004bfd5a cc                       int3       
004bfd5b cc                       int3       
004bfd5c cc                       int3       
004bfd5d cc                       int3       
004bfd5e cc                       int3       
004bfd5f cc                       int3       
