0052ff70 55                       push       ebp
0052ff71 8bec                     mov        ebp, esp
0052ff73 83ec0c                   sub        esp, 0xc
0052ff76 894dfc                   mov        dword ptr [ebp - 4], ecx
0052ff79 e8a2050000               call       0x530520
0052ff7e 0fb6c0                   movzx      eax, al
0052ff81 85c0                     test       eax, eax
0052ff83 753a                     jne        0x52ffbf
0052ff85 6a00                     push       0
0052ff87 e8a408f3ff               call       0x460830
0052ff8c 83c404                   add        esp, 4
0052ff8f 8bc8                     mov        ecx, eax
0052ff91 e81af7fcff               call       0x4ff6b0
0052ff96 8945f4                   mov        dword ptr [ebp - 0xc], eax
0052ff99 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052ff9c 6b510414                 imul       edx, dword ptr [ecx + 4], 0x14
0052ffa0 8b4510                   mov        eax, dword ptr [ebp + 0x10]
0052ffa3 8d4c0205                 lea        ecx, [edx + eax + 5]
0052ffa7 894df8                   mov        dword ptr [ebp - 8], ecx
0052ffaa 8b55f8                   mov        edx, dword ptr [ebp - 8]
0052ffad 52                       push       edx
0052ffae 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0052ffb1 50                       push       eax
0052ffb2 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052ffb5 51                       push       ecx
0052ffb6 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052ffb9 e8824dfdff               call       0x504d40
0052ffbe 90                       nop        
0052ffbf 8be5                     mov        esp, ebp
0052ffc1 5d                       pop        ebp
0052ffc2 c20c00                   ret        0xc
0052ffc5 cc                       int3       
0052ffc6 cc                       int3       
0052ffc7 cc                       int3       
0052ffc8 cc                       int3       
0052ffc9 cc                       int3       
0052ffca cc                       int3       
0052ffcb cc                       int3       
0052ffcc cc                       int3       
0052ffcd cc                       int3       
0052ffce cc                       int3       
0052ffcf cc                       int3       
