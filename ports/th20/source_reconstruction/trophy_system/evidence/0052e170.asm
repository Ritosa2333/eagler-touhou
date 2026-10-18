0052e170 55                       push       ebp
0052e171 8bec                     mov        ebp, esp
0052e173 83ec0c                   sub        esp, 0xc
0052e176 894dfc                   mov        dword ptr [ebp - 4], ecx
0052e179 6800575700               push       0x575700
0052e17e 6a14                     push       0x14
0052e180 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0052e186 e8a509f2ff               call       0x44eb30
0052e18b 8945f8                   mov        dword ptr [ebp - 8], eax
0052e18e 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052e191 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052e194 894810                   mov        dword ptr [eax + 0x10], ecx
0052e197 837df800                 cmp        dword ptr [ebp - 8], 0
0052e19b 7517                     jne        0x52e1b4
0052e19d 685c1f5700               push       0x571f5c
0052e1a2 6878065c00               push       0x5c0678
0052e1a7 e8a45ff2ff               call       0x454150
0052e1ac 83c408                   add        esp, 8
0052e1af 83c8ff                   or         eax, 0xffffffff
0052e1b2 eb53                     jmp        0x52e207
0052e1b4 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052e1b7 52                       push       edx
0052e1b8 6830f05200               push       0x52f030
0052e1bd 6a11                     push       0x11
0052e1bf e8fc40eeff               call       0x4122c0
0052e1c4 83c40c                   add        esp, 0xc
0052e1c7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052e1ca 894108                   mov        dword ptr [ecx + 8], eax
0052e1cd 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052e1d0 52                       push       edx
0052e1d1 68a0de4900               push       0x49dea0
0052e1d6 6a5d                     push       0x5d
0052e1d8 e88341eeff               call       0x412360
0052e1dd 83c40c                   add        esp, 0xc
0052e1e0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052e1e3 89410c                   mov        dword ptr [ecx + 0xc], eax
0052e1e6 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052e1e9 83c214                   add        edx, 0x14
0052e1ec 8955f4                   mov        dword ptr [ebp - 0xc], edx
0052e1ef 8b4508                   mov        eax, dword ptr [ebp + 8]
0052e1f2 50                       push       eax
0052e1f3 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052e1f6 e8b50d0000               call       0x52efb0
0052e1fb 6a00                     push       0
0052e1fd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052e200 e8fb150000               call       0x52f800
0052e205 33c0                     xor        eax, eax
0052e207 8be5                     mov        esp, ebp
0052e209 5d                       pop        ebp
0052e20a c20400                   ret        4
0052e20d cc                       int3       
0052e20e cc                       int3       
0052e20f cc                       int3       
