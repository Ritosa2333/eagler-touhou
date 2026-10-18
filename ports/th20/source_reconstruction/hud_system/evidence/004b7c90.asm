004b7c90 55                       push       ebp
004b7c91 8bec                     mov        ebp, esp
004b7c93 83ec0c                   sub        esp, 0xc
004b7c96 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004b7c9d c645ff77                 mov        byte ptr [ebp - 1], 0x77
004b7ca1 c645fe07                 mov        byte ptr [ebp - 2], 7
004b7ca5 8b4508                   mov        eax, dword ptr [ebp + 8]
004b7ca8 0fbe08                   movsx      ecx, byte ptr [eax]
004b7cab 0fb655ff                 movzx      edx, byte ptr [ebp - 1]
004b7caf 33ca                     xor        ecx, edx
004b7cb1 884dfd                   mov        byte ptr [ebp - 3], cl
004b7cb4 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004b7cb7 8a4dfd                   mov        cl, byte ptr [ebp - 3]
004b7cba 8888204a5c00             mov        byte ptr [eax + 0x5c4a20], cl
004b7cc0 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004b7cc3 83c201                   add        edx, 1
004b7cc6 8955f4                   mov        dword ptr [ebp - 0xc], edx
004b7cc9 8b4508                   mov        eax, dword ptr [ebp + 8]
004b7ccc 83c001                   add        eax, 1
004b7ccf 894508                   mov        dword ptr [ebp + 8], eax
004b7cd2 0fb64dff                 movzx      ecx, byte ptr [ebp - 1]
004b7cd6 0fb655fe                 movzx      edx, byte ptr [ebp - 2]
004b7cda 03ca                     add        ecx, edx
004b7cdc 884dff                   mov        byte ptr [ebp - 1], cl
004b7cdf 0fb645fe                 movzx      eax, byte ptr [ebp - 2]
004b7ce3 83c010                   add        eax, 0x10
004b7ce6 8845fe                   mov        byte ptr [ebp - 2], al
004b7ce9 0fb64dfd                 movzx      ecx, byte ptr [ebp - 3]
004b7ced 85c9                     test       ecx, ecx
004b7cef 75b4                     jne        0x4b7ca5
004b7cf1 c745f800000000           mov        dword ptr [ebp - 8], 0
004b7cf8 8b55f8                   mov        edx, dword ptr [ebp - 8]
004b7cfb 0fb682204a5c00           movzx      eax, byte ptr [edx + 0x5c4a20]
004b7d02 85c0                     test       eax, eax
004b7d04 7446                     je         0x4b7d4c
004b7d06 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004b7d09 0fb691204a5c00           movzx      edx, byte ptr [ecx + 0x5c4a20]
004b7d10 52                       push       edx
004b7d11 e84ae7ffff               call       0x4b6460
004b7d16 83c404                   add        esp, 4
004b7d19 85c0                     test       eax, eax
004b7d1b 740b                     je         0x4b7d28
004b7d1d 8b45f8                   mov        eax, dword ptr [ebp - 8]
004b7d20 83c001                   add        eax, 1
004b7d23 8945f8                   mov        dword ptr [ebp - 8], eax
004b7d26 eb19                     jmp        0x4b7d41
004b7d28 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004b7d2b 0fb691204a5c00           movzx      edx, byte ptr [ecx + 0x5c4a20]
004b7d32 83fa5f                   cmp        edx, 0x5f
004b7d35 750a                     jne        0x4b7d41
004b7d37 8b45f8                   mov        eax, dword ptr [ebp - 8]
004b7d3a c680204a5c0020           mov        byte ptr [eax + 0x5c4a20], 0x20
004b7d41 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004b7d44 83c101                   add        ecx, 1
004b7d47 894df8                   mov        dword ptr [ebp - 8], ecx
004b7d4a ebac                     jmp        0x4b7cf8
004b7d4c b8204a5c00               mov        eax, 0x5c4a20
004b7d51 8be5                     mov        esp, ebp
004b7d53 5d                       pop        ebp
004b7d54 c3                       ret        
004b7d55 cc                       int3       
004b7d56 cc                       int3       
004b7d57 cc                       int3       
004b7d58 cc                       int3       
004b7d59 cc                       int3       
004b7d5a cc                       int3       
004b7d5b cc                       int3       
004b7d5c cc                       int3       
004b7d5d cc                       int3       
004b7d5e cc                       int3       
004b7d5f cc                       int3       
