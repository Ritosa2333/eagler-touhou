00457040 55                       push       ebp
00457041 8bec                     mov        ebp, esp
00457043 83ec10                   sub        esp, 0x10
00457046 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0045704b 33c5                     xor        eax, ebp
0045704d 8945fc                   mov        dword ptr [ebp - 4], eax
00457050 8d4df0                   lea        ecx, [ebp - 0x10]
00457053 e8b8bdfcff               call       0x422e10
00457058 f30f104508               movss      xmm0, dword ptr [ebp + 8]
0045705d f30f5c4510               subss      xmm0, dword ptr [ebp + 0x10]
00457062 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00457067 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
0045706c f30f5c4514               subss      xmm0, dword ptr [ebp + 0x14]
00457071 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00457076 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
0045707b 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00457082 51                       push       ecx
00457083 f30f110424               movss      dword ptr [esp], xmm0
00457088 8d45f0                   lea        eax, [ebp - 0x10]
0045708b 50                       push       eax
0045708c 8d4df0                   lea        ecx, [ebp - 0x10]
0045708f 51                       push       ecx
00457090 e80b1f0000               call       0x458fa0
00457095 83c40c                   add        esp, 0xc
00457098 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
0045709d f30f5945f0               mulss      xmm0, dword ptr [ebp - 0x10]
004570a2 f30f104d18               movss      xmm1, dword ptr [ebp + 0x18]
004570a7 f30f594d18               mulss      xmm1, dword ptr [ebp + 0x18]
004570ac f30f5ec1                 divss      xmm0, xmm1
004570b0 f30f104df4               movss      xmm1, dword ptr [ebp - 0xc]
004570b5 f30f594df4               mulss      xmm1, dword ptr [ebp - 0xc]
004570ba f30f10551c               movss      xmm2, dword ptr [ebp + 0x1c]
004570bf f30f59551c               mulss      xmm2, dword ptr [ebp + 0x1c]
004570c4 f30f5eca                 divss      xmm1, xmm2
004570c8 f30f58c1                 addss      xmm0, xmm1
004570cc f30f100dccc85600         movss      xmm1, dword ptr [0x56c8cc]
004570d4 0f2fc8                   comiss     xmm1, xmm0
004570d7 7204                     jb         0x4570dd
004570d9 b001                     mov        al, 1
004570db eb02                     jmp        0x4570df
004570dd 32c0                     xor        al, al
004570df 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004570e2 33cd                     xor        ecx, ebp
004570e4 e805b90e00               call       0x5429ee
004570e9 8be5                     mov        esp, ebp
004570eb 5d                       pop        ebp
004570ec c3                       ret        
004570ed cc                       int3       
004570ee cc                       int3       
004570ef cc                       int3       
