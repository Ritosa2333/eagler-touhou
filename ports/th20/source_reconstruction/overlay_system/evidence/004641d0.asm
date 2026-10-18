004641d0 55                       push       ebp
004641d1 8bec                     mov        ebp, esp
004641d3 83ec08                   sub        esp, 8
004641d6 894dfc                   mov        dword ptr [ebp - 4], ecx
004641d9 8b4508                   mov        eax, dword ptr [ebp + 8]
004641dc 8945f8                   mov        dword ptr [ebp - 8], eax
004641df 837df803                 cmp        dword ptr [ebp - 8], 3
004641e3 770a                     ja         0x4641ef
004641e5 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004641e8 ff248d1c424600           jmp        dword ptr [ecx*4 + 0x46421c]
004641ef 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004641f2 e8b988fbff               call       0x41cab0
004641f7 eb1c                     jmp        0x464215
004641f9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004641fc e88f88fbff               call       0x41ca90
00464201 eb12                     jmp        0x464215
00464203 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00464206 e8f5d4faff               call       0x411700
0046420b eb08                     jmp        0x464215
0046420d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00464210 e8fbfdfbff               call       0x424010
00464215 8be5                     mov        esp, ebp
00464217 5d                       pop        ebp
00464218 c20400                   ret        4
0046421b 90                       nop        
0046421c ef                       out        dx, eax
0046421d 41                       inc        ecx
0046421e 46                       inc        esi
0046421f 00f9                     add        cl, bh
00464221 41                       inc        ecx
00464222 46                       inc        esi
00464223 0003                     add        byte ptr [ebx], al
00464225 42                       inc        edx
00464226 46                       inc        esi
00464227 000d424600cc             add        byte ptr [0xcc004642], cl
0046422d cc                       int3       
0046422e cc                       int3       
0046422f cc                       int3       
