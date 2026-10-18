005316c0 55                       push       ebp
005316c1 8bec                     mov        ebp, esp
005316c3 83ec10                   sub        esp, 0x10
005316c6 894df0                   mov        dword ptr [ebp - 0x10], ecx
005316c9 8b4508                   mov        eax, dword ptr [ebp + 8]
005316cc 8b88c8050000             mov        ecx, dword ptr [eax + 0x5c8]
005316d2 894dfc                   mov        dword ptr [ebp - 4], ecx
005316d5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005316d8 83c16c                   add        ecx, 0x6c
005316db e8308aefff               call       0x42a110
005316e0 8b55fc                   mov        edx, dword ptr [ebp - 4]
005316e3 d95a68                   fstp       dword ptr [edx + 0x68]
005316e6 e8752b0000               call       0x534260
005316eb f30f2ac0                 cvtsi2ss   xmm0, eax
005316ef f30f100d585d5700         movss      xmm1, dword ptr [0x575d58]
005316f7 f30f5ec8                 divss      xmm1, xmm0
005316fb f30f114df4               movss      dword ptr [ebp - 0xc], xmm1
00531700 8b45fc                   mov        eax, dword ptr [ebp - 4]
00531703 f30f104060               movss      xmm0, dword ptr [eax + 0x60]
00531708 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
0053170d f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00531712 f30f5c45f4               subss      xmm0, dword ptr [ebp - 0xc]
00531717 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053171a f30f114160               movss      dword ptr [ecx + 0x60], xmm0
0053171f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00531722 83c25c                   add        edx, 0x5c
00531725 52                       push       edx
00531726 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00531729 e832c6f2ff               call       0x45dd60
0053172e 51                       push       ecx
0053172f f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00531737 f30f110424               movss      dword ptr [esp], xmm0
0053173c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053173f 51                       push       ecx
00531740 f30f104068               movss      xmm0, dword ptr [eax + 0x68]
00531745 f30f110424               movss      dword ptr [esp], xmm0
0053174a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0053174d e8fef6f3ff               call       0x470e50
00531752 33c0                     xor        eax, eax
00531754 8be5                     mov        esp, ebp
00531756 5d                       pop        ebp
00531757 c20400                   ret        4
0053175a cc                       int3       
0053175b cc                       int3       
0053175c cc                       int3       
0053175d cc                       int3       
0053175e cc                       int3       
0053175f cc                       int3       
