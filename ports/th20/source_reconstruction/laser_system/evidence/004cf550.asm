004cf550 55                       push       ebp
004cf551 8bec                     mov        ebp, esp
004cf553 51                       push       ecx
004cf554 8b4508                   mov        eax, dword ptr [ebp + 8]
004cf557 8b88c8050000             mov        ecx, dword ptr [eax + 0x5c8]
004cf55d 894dfc                   mov        dword ptr [ebp - 4], ecx
004cf560 8b55fc                   mov        edx, dword ptr [ebp - 4]
004cf563 6982e406000058010000     imul       eax, dword ptr [edx + 0x6e4], 0x158
004cf56d b914000000               mov        ecx, 0x14
004cf572 6bd100                   imul       edx, ecx, 0
004cf575 8d8410ac065c00           lea        eax, [eax + edx + 0x5c06ac]
004cf57c b904000000               mov        ecx, 4
004cf581 6bd100                   imul       edx, ecx, 0
004cf584 833c1000                 cmp        dword ptr [eax + edx], 0
004cf588 7c28                     jl         0x4cf5b2
004cf58a 8b45fc                   mov        eax, dword ptr [ebp - 4]
004cf58d 6988e406000058010000     imul       ecx, dword ptr [eax + 0x6e4], 0x158
004cf597 8b55fc                   mov        edx, dword ptr [ebp - 4]
004cf59a 6b82e806000014           imul       eax, dword ptr [edx + 0x6e8], 0x14
004cf5a1 8d8c01ac065c00           lea        ecx, [ecx + eax + 0x5c06ac]
004cf5a8 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004cf5ab 8b0491                   mov        eax, dword ptr [ecx + edx*4]
004cf5ae eb05                     jmp        0x4cf5b5
004cf5b0 eb03                     jmp        0x4cf5b5
004cf5b2 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004cf5b5 8be5                     mov        esp, ebp
004cf5b7 5d                       pop        ebp
004cf5b8 c3                       ret        
004cf5b9 cc                       int3       
004cf5ba cc                       int3       
004cf5bb cc                       int3       
004cf5bc cc                       int3       
004cf5bd cc                       int3       
004cf5be cc                       int3       
004cf5bf cc                       int3       
