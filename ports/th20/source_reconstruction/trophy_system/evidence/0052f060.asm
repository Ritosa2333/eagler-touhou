0052f060 55                       push       ebp
0052f061 8bec                     mov        ebp, esp
0052f063 83ec08                   sub        esp, 8
0052f066 c745f800000000           mov        dword ptr [ebp - 8], 0
0052f06d c645ff77                 mov        byte ptr [ebp - 1], 0x77
0052f071 c645fe07                 mov        byte ptr [ebp - 2], 7
0052f075 8b4508                   mov        eax, dword ptr [ebp + 8]
0052f078 0fb608                   movzx      ecx, byte ptr [eax]
0052f07b 0fb655ff                 movzx      edx, byte ptr [ebp - 1]
0052f07f 33ca                     xor        ecx, edx
0052f081 884dfd                   mov        byte ptr [ebp - 3], cl
0052f084 8b45f8                   mov        eax, dword ptr [ebp - 8]
0052f087 8a4dfd                   mov        cl, byte ptr [ebp - 3]
0052f08a 888860685c00             mov        byte ptr [eax + 0x5c6860], cl
0052f090 8b55f8                   mov        edx, dword ptr [ebp - 8]
0052f093 83c201                   add        edx, 1
0052f096 8955f8                   mov        dword ptr [ebp - 8], edx
0052f099 8b4508                   mov        eax, dword ptr [ebp + 8]
0052f09c 83c001                   add        eax, 1
0052f09f 894508                   mov        dword ptr [ebp + 8], eax
0052f0a2 0fb64dff                 movzx      ecx, byte ptr [ebp - 1]
0052f0a6 0fb655fe                 movzx      edx, byte ptr [ebp - 2]
0052f0aa 03ca                     add        ecx, edx
0052f0ac 884dff                   mov        byte ptr [ebp - 1], cl
0052f0af 0fb645fe                 movzx      eax, byte ptr [ebp - 2]
0052f0b3 83c010                   add        eax, 0x10
0052f0b6 8845fe                   mov        byte ptr [ebp - 2], al
0052f0b9 0fbe4dfd                 movsx      ecx, byte ptr [ebp - 3]
0052f0bd 85c9                     test       ecx, ecx
0052f0bf 75b4                     jne        0x52f075
0052f0c1 b860685c00               mov        eax, 0x5c6860
0052f0c6 8be5                     mov        esp, ebp
0052f0c8 5d                       pop        ebp
0052f0c9 c3                       ret        
0052f0ca cc                       int3       
0052f0cb cc                       int3       
0052f0cc cc                       int3       
0052f0cd cc                       int3       
0052f0ce cc                       int3       
0052f0cf cc                       int3       
