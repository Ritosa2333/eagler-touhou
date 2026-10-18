0049e080 55                       push       ebp
0049e081 8bec                     mov        ebp, esp
0049e083 51                       push       ecx
0049e084 894dfc                   mov        dword ptr [ebp - 4], ecx
0049e087 8b4514                   mov        eax, dword ptr [ebp + 0x14]
0049e08a 50                       push       eax
0049e08b 6a04                     push       4
0049e08d 6aff                     push       -1
0049e08f 51                       push       ecx
0049e090 0f57c0                   xorps      xmm0, xmm0
0049e093 f30f110424               movss      dword ptr [esp], xmm0
0049e098 6a00                     push       0
0049e09a 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
0049e09d 51                       push       ecx
0049e09e 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0049e0a1 52                       push       edx
0049e0a2 8b4508                   mov        eax, dword ptr [ebp + 8]
0049e0a5 50                       push       eax
0049e0a6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0049e0a9 e8022cfbff               call       0x450cb0
0049e0ae 8b4508                   mov        eax, dword ptr [ebp + 8]
0049e0b1 8be5                     mov        esp, ebp
0049e0b3 5d                       pop        ebp
0049e0b4 c21000                   ret        0x10
0049e0b7 cc                       int3       
0049e0b8 cc                       int3       
0049e0b9 cc                       int3       
0049e0ba cc                       int3       
0049e0bb cc                       int3       
0049e0bc cc                       int3       
0049e0bd cc                       int3       
0049e0be cc                       int3       
0049e0bf cc                       int3       
