0052f100 55                       push       ebp
0052f101 8bec                     mov        ebp, esp
0052f103 83ec0c                   sub        esp, 0xc
0052f106 c745f400000000           mov        dword ptr [ebp - 0xc], 0
0052f10d 6a00                     push       0
0052f10f 8d4d0c                   lea        ecx, [ebp + 0xc]
0052f112 e8a900f7ff               call       0x49f1c0
0052f117 8945f8                   mov        dword ptr [ebp - 8], eax
0052f11a c645ff77                 mov        byte ptr [ebp - 1], 0x77
0052f11e c645fe07                 mov        byte ptr [ebp - 2], 7
0052f122 8b45f8                   mov        eax, dword ptr [ebp - 8]
0052f125 8a08                     mov        cl, byte ptr [eax]
0052f127 884dfd                   mov        byte ptr [ebp - 3], cl
0052f12a 8b55f8                   mov        edx, dword ptr [ebp - 8]
0052f12d 0fb602                   movzx      eax, byte ptr [edx]
0052f130 0fb64dff                 movzx      ecx, byte ptr [ebp - 1]
0052f134 33c1                     xor        eax, ecx
0052f136 8b5508                   mov        edx, dword ptr [ebp + 8]
0052f139 8802                     mov        byte ptr [edx], al
0052f13b 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0052f13e 83c001                   add        eax, 1
0052f141 8945f4                   mov        dword ptr [ebp - 0xc], eax
0052f144 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052f147 83c101                   add        ecx, 1
0052f14a 894df8                   mov        dword ptr [ebp - 8], ecx
0052f14d 8b5508                   mov        edx, dword ptr [ebp + 8]
0052f150 83c201                   add        edx, 1
0052f153 895508                   mov        dword ptr [ebp + 8], edx
0052f156 0fb645ff                 movzx      eax, byte ptr [ebp - 1]
0052f15a 0fb64dfe                 movzx      ecx, byte ptr [ebp - 2]
0052f15e 03c1                     add        eax, ecx
0052f160 8845ff                   mov        byte ptr [ebp - 1], al
0052f163 0fb655fe                 movzx      edx, byte ptr [ebp - 2]
0052f167 83c210                   add        edx, 0x10
0052f16a 8855fe                   mov        byte ptr [ebp - 2], dl
0052f16d 0fb645fd                 movzx      eax, byte ptr [ebp - 3]
0052f171 85c0                     test       eax, eax
0052f173 75ad                     jne        0x52f122
0052f175 8d4d0c                   lea        ecx, [ebp + 0xc]
0052f178 e8c3a0f1ff               call       0x449240
0052f17d 90                       nop        
0052f17e 8be5                     mov        esp, ebp
0052f180 5d                       pop        ebp
0052f181 c3                       ret        
0052f182 cc                       int3       
0052f183 cc                       int3       
0052f184 cc                       int3       
0052f185 cc                       int3       
0052f186 cc                       int3       
0052f187 cc                       int3       
0052f188 cc                       int3       
0052f189 cc                       int3       
0052f18a cc                       int3       
0052f18b cc                       int3       
0052f18c cc                       int3       
0052f18d cc                       int3       
0052f18e cc                       int3       
0052f18f cc                       int3       
