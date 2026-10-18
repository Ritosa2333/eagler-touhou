0052cf80 55                       push       ebp
0052cf81 8bec                     mov        ebp, esp
0052cf83 83ec14                   sub        esp, 0x14
0052cf86 894dfc                   mov        dword ptr [ebp - 4], ecx
0052cf89 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052cf8c 8b4810                   mov        ecx, dword ptr [eax + 0x10]
0052cf8f 894df8                   mov        dword ptr [ebp - 8], ecx
0052cf92 6a00                     push       0
0052cf94 6aff                     push       -1
0052cf96 8b5508                   mov        edx, dword ptr [ebp + 8]
0052cf99 52                       push       edx
0052cf9a 68e44f5700               push       0x574fe4
0052cf9f 8d45ec                   lea        eax, [ebp - 0x14]
0052cfa2 50                       push       eax
0052cfa3 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052cfa6 e8c53cf2ff               call       0x450c70
0052cfab 8b08                     mov        ecx, dword ptr [eax]
0052cfad 894df0                   mov        dword ptr [ebp - 0x10], ecx
0052cfb0 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052cfb3 81c264010000             add        edx, 0x164
0052cfb9 8955f4                   mov        dword ptr [ebp - 0xc], edx
0052cfbc 8b4508                   mov        eax, dword ptr [ebp + 8]
0052cfbf 50                       push       eax
0052cfc0 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052cfc3 e8b875eeff               call       0x414580
0052cfc8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052cfcb 8908                     mov        dword ptr [eax], ecx
0052cfcd 8be5                     mov        esp, ebp
0052cfcf 5d                       pop        ebp
0052cfd0 c20400                   ret        4
0052cfd3 cc                       int3       
0052cfd4 cc                       int3       
0052cfd5 cc                       int3       
0052cfd6 cc                       int3       
0052cfd7 cc                       int3       
0052cfd8 cc                       int3       
0052cfd9 cc                       int3       
0052cfda cc                       int3       
0052cfdb cc                       int3       
0052cfdc cc                       int3       
0052cfdd cc                       int3       
0052cfde cc                       int3       
0052cfdf cc                       int3       
