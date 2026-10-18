0051bb90 55                       push       ebp
0051bb91 8bec                     mov        ebp, esp
0051bb93 83ec14                   sub        esp, 0x14
0051bb96 894dfc                   mov        dword ptr [ebp - 4], ecx
0051bb99 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051bb9c 83b8f410020004           cmp        dword ptr [eax + 0x210f4], 4
0051bba3 7520                     jne        0x51bbc5
0051bba5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051bba8 83c128                   add        ecx, 0x28
0051bbab 894df0                   mov        dword ptr [ebp - 0x10], ecx
0051bbae 6a01                     push       1
0051bbb0 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051bbb3 e8f8f5f4ff               call       0x46b1b0
0051bbb8 85c0                     test       eax, eax
0051bbba 7409                     je         0x51bbc5
0051bbbc c745f402000000           mov        dword ptr [ebp - 0xc], 2
0051bbc3 eb39                     jmp        0x51bbfe
0051bbc5 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051bbc8 83baf410020003           cmp        dword ptr [edx + 0x210f4], 3
0051bbcf 7520                     jne        0x51bbf1
0051bbd1 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051bbd4 83c028                   add        eax, 0x28
0051bbd7 8945ec                   mov        dword ptr [ebp - 0x14], eax
0051bbda 6a01                     push       1
0051bbdc 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0051bbdf e8ccf5f4ff               call       0x46b1b0
0051bbe4 85c0                     test       eax, eax
0051bbe6 7409                     je         0x51bbf1
0051bbe8 c745f801000000           mov        dword ptr [ebp - 8], 1
0051bbef eb07                     jmp        0x51bbf8
0051bbf1 c745f800000000           mov        dword ptr [ebp - 8], 0
0051bbf8 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0051bbfb 894df4                   mov        dword ptr [ebp - 0xc], ecx
0051bbfe 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0051bc01 8be5                     mov        esp, ebp
0051bc03 5d                       pop        ebp
0051bc04 c3                       ret        
0051bc05 cc                       int3       
0051bc06 cc                       int3       
0051bc07 cc                       int3       
0051bc08 cc                       int3       
0051bc09 cc                       int3       
0051bc0a cc                       int3       
0051bc0b cc                       int3       
0051bc0c cc                       int3       
0051bc0d cc                       int3       
0051bc0e cc                       int3       
0051bc0f cc                       int3       
