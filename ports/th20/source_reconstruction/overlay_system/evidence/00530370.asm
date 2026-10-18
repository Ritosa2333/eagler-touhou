00530370 55                       push       ebp
00530371 8bec                     mov        ebp, esp
00530373 83ec30                   sub        esp, 0x30
00530376 894dfc                   mov        dword ptr [ebp - 4], ecx
00530379 8b4508                   mov        eax, dword ptr [ebp + 8]
0053037c 8945f8                   mov        dword ptr [ebp - 8], eax
0053037f 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00530382 83e901                   sub        ecx, 1
00530385 894df8                   mov        dword ptr [ebp - 8], ecx
00530388 837df803                 cmp        dword ptr [ebp - 8], 3
0053038c 770a                     ja         0x530398
0053038e 8b55f8                   mov        edx, dword ptr [ebp - 8]
00530391 ff2495f0045300           jmp        dword ptr [edx*4 + 0x5304f0]
00530398 6a00                     push       0
0053039a e89104f3ff               call       0x460830
0053039f 83c404                   add        esp, 4
005303a2 8bc8                     mov        ecx, eax
005303a4 e8975ffdff               call       0x506340
005303a9 05d4000000               add        eax, 0xd4
005303ae 8945f4                   mov        dword ptr [ebp - 0xc], eax
005303b1 8b45fc                   mov        eax, dword ptr [ebp - 4]
005303b4 694804a0000000           imul       ecx, dword ptr [eax + 4], 0xa0
005303bb 894df0                   mov        dword ptr [ebp - 0x10], ecx
005303be 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
005303c1 0345f0                   add        eax, dword ptr [ebp - 0x10]
005303c4 ba50000000               mov        edx, 0x50
005303c9 6bca00                   imul       ecx, edx, 0
005303cc 03c1                     add        eax, ecx
005303ce ba08000000               mov        edx, 8
005303d3 6bca00                   imul       ecx, edx, 0
005303d6 03c1                     add        eax, ecx
005303d8 e90d010000               jmp        0x5304ea
005303dd e908010000               jmp        0x5304ea
005303e2 6a00                     push       0
005303e4 e84704f3ff               call       0x460830
005303e9 83c404                   add        esp, 4
005303ec 8bc8                     mov        ecx, eax
005303ee e84d5ffdff               call       0x506340
005303f3 05d4000000               add        eax, 0xd4
005303f8 8945ec                   mov        dword ptr [ebp - 0x14], eax
005303fb 8b55fc                   mov        edx, dword ptr [ebp - 4]
005303fe 694204a0000000           imul       eax, dword ptr [edx + 4], 0xa0
00530405 8945e8                   mov        dword ptr [ebp - 0x18], eax
00530408 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0053040b 034de8                   add        ecx, dword ptr [ebp - 0x18]
0053040e ba50000000               mov        edx, 0x50
00530413 6bc200                   imul       eax, edx, 0
00530416 03c8                     add        ecx, eax
00530418 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0053041b 8d04d1                   lea        eax, [ecx + edx*8]
0053041e e9c7000000               jmp        0x5304ea
00530423 e9c2000000               jmp        0x5304ea
00530428 6a00                     push       0
0053042a e80104f3ff               call       0x460830
0053042f 83c404                   add        esp, 4
00530432 8bc8                     mov        ecx, eax
00530434 e8075ffdff               call       0x506340
00530439 05d4000000               add        eax, 0xd4
0053043e 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00530441 8b45fc                   mov        eax, dword ptr [ebp - 4]
00530444 694804a0000000           imul       ecx, dword ptr [eax + 4], 0xa0
0053044b 894de0                   mov        dword ptr [ebp - 0x20], ecx
0053044e 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
00530451 0355e0                   add        edx, dword ptr [ebp - 0x20]
00530454 b850000000               mov        eax, 0x50
00530459 6bc800                   imul       ecx, eax, 0
0053045c 03d1                     add        edx, ecx
0053045e 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00530461 8d44c208                 lea        eax, [edx + eax*8 + 8]
00530465 e980000000               jmp        0x5304ea
0053046a eb7e                     jmp        0x5304ea
0053046c 6a00                     push       0
0053046e e8bd03f3ff               call       0x460830
00530473 83c404                   add        esp, 4
00530476 8bc8                     mov        ecx, eax
00530478 e8c35efdff               call       0x506340
0053047d 05d4000000               add        eax, 0xd4
00530482 8945dc                   mov        dword ptr [ebp - 0x24], eax
00530485 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00530488 695104a0000000           imul       edx, dword ptr [ecx + 4], 0xa0
0053048f 8955d8                   mov        dword ptr [ebp - 0x28], edx
00530492 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
00530495 0345d8                   add        eax, dword ptr [ebp - 0x28]
00530498 b950000000               mov        ecx, 0x50
0053049d 6bd100                   imul       edx, ecx, 0
005304a0 03c2                     add        eax, edx
005304a2 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
005304a5 8d44c818                 lea        eax, [eax + ecx*8 + 0x18]
005304a9 eb3f                     jmp        0x5304ea
005304ab eb3d                     jmp        0x5304ea
005304ad 6a00                     push       0
005304af e87c03f3ff               call       0x460830
005304b4 83c404                   add        esp, 4
005304b7 8bc8                     mov        ecx, eax
005304b9 e8825efdff               call       0x506340
005304be 05d4000000               add        eax, 0xd4
005304c3 8945d4                   mov        dword ptr [ebp - 0x2c], eax
005304c6 8b55fc                   mov        edx, dword ptr [ebp - 4]
005304c9 694204a0000000           imul       eax, dword ptr [edx + 4], 0xa0
005304d0 8945d0                   mov        dword ptr [ebp - 0x30], eax
005304d3 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
005304d6 034dd0                   add        ecx, dword ptr [ebp - 0x30]
005304d9 ba50000000               mov        edx, 0x50
005304de 6bc200                   imul       eax, edx, 0
005304e1 03c8                     add        ecx, eax
005304e3 8b550c                   mov        edx, dword ptr [ebp + 0xc]
005304e6 8d44d130                 lea        eax, [ecx + edx*8 + 0x30]
005304ea 8be5                     mov        esp, ebp
005304ec 5d                       pop        ebp
005304ed c20800                   ret        8
005304f0 e203                     loop       0x5304f5
005304f2 53                       push       ebx
005304f3 0028                     add        byte ptr [eax], ch
005304f5 0453                     add        al, 0x53
005304f7 006c0453                 add        byte ptr [esp + eax + 0x53], ch
