00412280 55                       push       ebp
00412281 8bec                     mov        ebp, esp
00412283 83ec08                   sub        esp, 8
00412286 894df8                   mov        dword ptr [ebp - 8], ecx
00412289 c745fc00000000           mov        dword ptr [ebp - 4], 0
00412290 6a00                     push       0
00412292 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00412295 e8f6dcffff               call       0x40ff90
0041229a 50                       push       eax
0041229b 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0041229e e80df7ffff               call       0x4119b0
004122a3 8b45fc                   mov        eax, dword ptr [ebp - 4]
004122a6 83c801                   or         eax, 1
004122a9 8945fc                   mov        dword ptr [ebp - 4], eax
004122ac 8b4508                   mov        eax, dword ptr [ebp + 8]
004122af 8be5                     mov        esp, ebp
004122b1 5d                       pop        ebp
004122b2 c20400                   ret        4
004122b5 cc                       int3       
004122b6 cc                       int3       
004122b7 cc                       int3       
004122b8 cc                       int3       
004122b9 cc                       int3       
004122ba cc                       int3       
004122bb cc                       int3       
004122bc cc                       int3       
004122bd cc                       int3       
004122be cc                       int3       
004122bf cc                       int3       
