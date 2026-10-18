00450270 55                       push       ebp
00450271 8bec                     mov        ebp, esp
00450273 83ec08                   sub        esp, 8
00450276 894df8                   mov        dword ptr [ebp - 8], ecx
00450279 8b4508                   mov        eax, dword ptr [ebp + 8]
0045027c 50                       push       eax
0045027d 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00450280 e87bcaffff               call       0x44cd00
00450285 8945fc                   mov        dword ptr [ebp - 4], eax
00450288 837dfc00                 cmp        dword ptr [ebp - 4], 0
0045028c 7502                     jne        0x450290
0045028e eb1c                     jmp        0x4502ac
00450290 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00450293 81c1bc050000             add        ecx, 0x5bc
00450299 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0045029c 8b02                     mov        eax, dword ptr [edx]
0045029e 8901                     mov        dword ptr [ecx], eax
004502a0 8b4204                   mov        eax, dword ptr [edx + 4]
004502a3 894104                   mov        dword ptr [ecx + 4], eax
004502a6 8b5208                   mov        edx, dword ptr [edx + 8]
004502a9 895108                   mov        dword ptr [ecx + 8], edx
004502ac 8be5                     mov        esp, ebp
004502ae 5d                       pop        ebp
004502af c20800                   ret        8
004502b2 cc                       int3       
004502b3 cc                       int3       
004502b4 cc                       int3       
004502b5 cc                       int3       
004502b6 cc                       int3       
004502b7 cc                       int3       
004502b8 cc                       int3       
004502b9 cc                       int3       
004502ba cc                       int3       
004502bb cc                       int3       
004502bc cc                       int3       
004502bd cc                       int3       
004502be cc                       int3       
004502bf cc                       int3       
