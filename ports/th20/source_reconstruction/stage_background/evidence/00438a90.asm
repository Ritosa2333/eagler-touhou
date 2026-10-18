00438a90 55                       push       ebp
00438a91 8bec                     mov        ebp, esp
00438a93 51                       push       ecx
00438a94 894dfc                   mov        dword ptr [ebp - 4], ecx
00438a97 8b45fc                   mov        eax, dword ptr [ebp - 4]
00438a9a 83c00c                   add        eax, 0xc
00438a9d 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00438aa0 8b11                     mov        edx, dword ptr [ecx]
00438aa2 8910                     mov        dword ptr [eax], edx
00438aa4 8b5104                   mov        edx, dword ptr [ecx + 4]
00438aa7 895004                   mov        dword ptr [eax + 4], edx
00438aaa 8b4908                   mov        ecx, dword ptr [ecx + 8]
00438aad 894808                   mov        dword ptr [eax + 8], ecx
00438ab0 8be5                     mov        esp, ebp
00438ab2 5d                       pop        ebp
00438ab3 c20400                   ret        4
00438ab6 cc                       int3       
00438ab7 cc                       int3       
00438ab8 cc                       int3       
00438ab9 cc                       int3       
00438aba cc                       int3       
00438abb cc                       int3       
00438abc cc                       int3       
00438abd cc                       int3       
00438abe cc                       int3       
00438abf cc                       int3       
