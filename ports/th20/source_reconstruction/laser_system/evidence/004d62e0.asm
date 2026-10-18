004d62e0 55                       push       ebp
004d62e1 8bec                     mov        ebp, esp
004d62e3 83ec08                   sub        esp, 8
004d62e6 894df8                   mov        dword ptr [ebp - 8], ecx
004d62e9 8b45f8                   mov        eax, dword ptr [ebp - 8]
004d62ec 8945fc                   mov        dword ptr [ebp - 4], eax
004d62ef 837dfc00                 cmp        dword ptr [ebp - 4], 0
004d62f3 0f848d000000             je         0x4d6386
004d62f9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d62fc f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
004d6301 0f2f4108                 comiss     xmm0, dword ptr [ecx + 8]
004d6305 7272                     jb         0x4d6379
004d6307 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d630a f30f10420c               movss      xmm0, dword ptr [edx + 0xc]
004d630f 0f2f4520                 comiss     xmm0, dword ptr [ebp + 0x20]
004d6313 7664                     jbe        0x4d6379
004d6315 837d2400                 cmp        dword ptr [ebp + 0x24], 0
004d6319 7522                     jne        0x4d633d
004d631b 51                       push       ecx
004d631c f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
004d6321 f30f110424               movss      dword ptr [esp], xmm0
004d6326 8b4510                   mov        eax, dword ptr [ebp + 0x10]
004d6329 50                       push       eax
004d632a 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004d632d 51                       push       ecx
004d632e 8b5508                   mov        edx, dword ptr [ebp + 8]
004d6331 52                       push       edx
004d6332 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d6335 e856000000               call       0x4d6390
004d633a 90                       nop        
004d633b eb3a                     jmp        0x4d6377
004d633d 51                       push       ecx
004d633e f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
004d6343 f30f110424               movss      dword ptr [esp], xmm0
004d6348 51                       push       ecx
004d6349 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
004d634e f30f110424               movss      dword ptr [esp], xmm0
004d6353 51                       push       ecx
004d6354 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
004d6359 f30f110424               movss      dword ptr [esp], xmm0
004d635e 8b4514                   mov        eax, dword ptr [ebp + 0x14]
004d6361 50                       push       eax
004d6362 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
004d6365 51                       push       ecx
004d6366 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004d6369 52                       push       edx
004d636a 8b4508                   mov        eax, dword ptr [ebp + 8]
004d636d 50                       push       eax
004d636e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d6371 e8ba040000               call       0x4d6830
004d6376 90                       nop        
004d6377 eb0d                     jmp        0x4d6386
004d6379 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d637c 8b11                     mov        edx, dword ptr [ecx]
004d637e 8955fc                   mov        dword ptr [ebp - 4], edx
004d6381 e969ffffff               jmp        0x4d62ef
004d6386 8be5                     mov        esp, ebp
004d6388 5d                       pop        ebp
004d6389 c22000                   ret        0x20
004d638c cc                       int3       
004d638d cc                       int3       
004d638e cc                       int3       
004d638f cc                       int3       
