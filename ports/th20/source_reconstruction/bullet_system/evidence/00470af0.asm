00470af0 55                       push       ebp
00470af1 8bec                     mov        ebp, esp
00470af3 51                       push       ecx
00470af4 894dfc                   mov        dword ptr [ebp - 4], ecx
00470af7 8b45fc                   mov        eax, dword ptr [ebp - 4]
00470afa 83c02c                   add        eax, 0x2c
00470afd 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00470b00 8b11                     mov        edx, dword ptr [ecx]
00470b02 8910                     mov        dword ptr [eax], edx
00470b04 8b5104                   mov        edx, dword ptr [ecx + 4]
00470b07 895004                   mov        dword ptr [eax + 4], edx
00470b0a 8b4908                   mov        ecx, dword ptr [ecx + 8]
00470b0d 894808                   mov        dword ptr [eax + 8], ecx
00470b10 8be5                     mov        esp, ebp
00470b12 5d                       pop        ebp
00470b13 c20400                   ret        4
00470b16 cc                       int3       
00470b17 cc                       int3       
00470b18 cc                       int3       
00470b19 cc                       int3       
00470b1a cc                       int3       
00470b1b cc                       int3       
00470b1c cc                       int3       
00470b1d cc                       int3       
00470b1e cc                       int3       
00470b1f cc                       int3       
