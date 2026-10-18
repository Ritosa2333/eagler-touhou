00456170 55                       push       ebp
00456171 8bec                     mov        ebp, esp
00456173 51                       push       ecx
00456174 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00456179 f30f5c4510               subss      xmm0, dword ptr [ebp + 0x10]
0045617e 51                       push       ecx
0045617f f30f110424               movss      dword ptr [esp], xmm0
00456184 e8f7f4feff               call       0x445680
00456189 83c404                   add        esp, 4
0045618c d95dfc                   fstp       dword ptr [ebp - 4]
0045618f f30f1005a0e15600         movss      xmm0, dword ptr [0x56e1a0]
00456197 0f2f45fc                 comiss     xmm0, dword ptr [ebp - 4]
0045619b 761d                     jbe        0x4561ba
0045619d 8b4508                   mov        eax, dword ptr [ebp + 8]
004561a0 0f57c0                   xorps      xmm0, xmm0
004561a3 f30f1100                 movss      dword ptr [eax], xmm0
004561a7 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004561aa f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004561af f30f1101                 movss      dword ptr [ecx], xmm0
004561b3 b801000000               mov        eax, 1
004561b8 eb4e                     jmp        0x456208
004561ba f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
004561bf f30f5c4514               subss      xmm0, dword ptr [ebp + 0x14]
004561c4 f30f104d18               movss      xmm1, dword ptr [ebp + 0x18]
004561c9 f30f5c4d10               subss      xmm1, dword ptr [ebp + 0x10]
004561ce f30f5ec1                 divss      xmm0, xmm1
004561d2 8b5508                   mov        edx, dword ptr [ebp + 8]
004561d5 f30f1102                 movss      dword ptr [edx], xmm0
004561d9 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
004561de f30f5c4514               subss      xmm0, dword ptr [ebp + 0x14]
004561e3 f30f594510               mulss      xmm0, dword ptr [ebp + 0x10]
004561e8 f30f104d18               movss      xmm1, dword ptr [ebp + 0x18]
004561ed f30f5c4d10               subss      xmm1, dword ptr [ebp + 0x10]
004561f2 f30f5ec1                 divss      xmm0, xmm1
004561f6 f30f104d14               movss      xmm1, dword ptr [ebp + 0x14]
004561fb f30f5cc8                 subss      xmm1, xmm0
004561ff 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00456202 f30f1108                 movss      dword ptr [eax], xmm1
00456206 33c0                     xor        eax, eax
00456208 8be5                     mov        esp, ebp
0045620a 5d                       pop        ebp
0045620b c3                       ret        
0045620c cc                       int3       
0045620d cc                       int3       
0045620e cc                       int3       
0045620f cc                       int3       
