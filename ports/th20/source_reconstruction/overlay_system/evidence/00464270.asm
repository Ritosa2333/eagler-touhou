00464270 55                       push       ebp
00464271 8bec                     mov        ebp, esp
00464273 83ec08                   sub        esp, 8
00464276 894dfc                   mov        dword ptr [ebp - 4], ecx
00464279 8b45fc                   mov        eax, dword ptr [ebp - 4]
0046427c 83782800                 cmp        dword ptr [eax + 0x28], 0
00464280 7423                     je         0x4642a5
00464282 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00464285 8b5128                   mov        edx, dword ptr [ecx + 0x28]
00464288 8b45fc                   mov        eax, dword ptr [ebp - 4]
0046428b 8b12                     mov        edx, dword ptr [edx]
0046428d 8b4828                   mov        ecx, dword ptr [eax + 0x28]
00464290 8b4268                   mov        eax, dword ptr [edx + 0x68]
00464293 ffd0                     call       eax
00464295 0fb6c8                   movzx      ecx, al
00464298 85c9                     test       ecx, ecx
0046429a 7409                     je         0x4642a5
0046429c c745f801000000           mov        dword ptr [ebp - 8], 1
004642a3 eb07                     jmp        0x4642ac
004642a5 c745f800000000           mov        dword ptr [ebp - 8], 0
004642ac 0fb645f8                 movzx      eax, byte ptr [ebp - 8]
004642b0 8be5                     mov        esp, ebp
004642b2 5d                       pop        ebp
004642b3 c3                       ret        
004642b4 cc                       int3       
004642b5 cc                       int3       
004642b6 cc                       int3       
004642b7 cc                       int3       
004642b8 cc                       int3       
004642b9 cc                       int3       
004642ba cc                       int3       
004642bb cc                       int3       
004642bc cc                       int3       
004642bd cc                       int3       
004642be cc                       int3       
004642bf cc                       int3       
