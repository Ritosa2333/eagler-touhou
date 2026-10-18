004bfd60 55                       push       ebp
004bfd61 8bec                     mov        ebp, esp
004bfd63 83ec1c                   sub        esp, 0x1c
004bfd66 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004bfd6b 33c5                     xor        eax, ebp
004bfd6d 8945fc                   mov        dword ptr [ebp - 4], eax
004bfd70 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004bfd73 e8f8110000               call       0x4c0f70
004bfd78 8945ec                   mov        dword ptr [ebp - 0x14], eax
004bfd7b 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004bfd7e e8cd110000               call       0x4c0f50
004bfd83 8945e8                   mov        dword ptr [ebp - 0x18], eax
004bfd86 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004bfd89 50                       push       eax
004bfd8a 8d4df0                   lea        ecx, [ebp - 0x10]
004bfd8d 51                       push       ecx
004bfd8e 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004bfd91 e8da97f6ff               call       0x429570
004bfd96 51                       push       ecx
004bfd97 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
004bfd9f f30f110424               movss      dword ptr [esp], xmm0
004bfda4 8d4df0                   lea        ecx, [ebp - 0x10]
004bfda7 e894040000               call       0x4c0240
004bfdac 0f57c0                   xorps      xmm0, xmm0
004bfdaf f30f1145f8               movss      dword ptr [ebp - 8], xmm0
004bfdb4 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004bfdb7 8b82ac000000             mov        eax, dword ptr [edx + 0xac]
004bfdbd 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004bfdc0 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bfdc3 c781ac00000001000000     mov        dword ptr [ecx + 0xac], 1
004bfdcd 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004bfdd0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfdd3 33cd                     xor        ecx, ebp
004bfdd5 e8142c0800               call       0x5429ee
004bfdda 8be5                     mov        esp, ebp
004bfddc 5d                       pop        ebp
004bfddd c3                       ret        
004bfdde cc                       int3       
004bfddf cc                       int3       
