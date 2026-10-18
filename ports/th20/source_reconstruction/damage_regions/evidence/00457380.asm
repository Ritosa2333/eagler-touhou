00457380 55                       push       ebp
00457381 8bec                     mov        ebp, esp
00457383 83ec60                   sub        esp, 0x60
00457386 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0045738b 33c5                     xor        eax, ebp
0045738d 8945fc                   mov        dword ptr [ebp - 4], eax
00457390 68102e4200               push       0x422e10
00457395 6a02                     push       2
00457397 6a0c                     push       0xc
00457399 8d45e4                   lea        eax, [ebp - 0x1c]
0045739c 50                       push       eax
0045739d e87e48fbff               call       0x40bc20
004573a2 51                       push       ecx
004573a3 0f57c0                   xorps      xmm0, xmm0
004573a6 f30f110424               movss      dword ptr [esp], xmm0
004573ab 51                       push       ecx
004573ac 0f57c0                   xorps      xmm0, xmm0
004573af f30f110424               movss      dword ptr [esp], xmm0
004573b4 51                       push       ecx
004573b5 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
004573ba f30f110424               movss      dword ptr [esp], xmm0
004573bf 8d4dd8                   lea        ecx, [ebp - 0x28]
004573c2 e809bafcff               call       0x422dd0
004573c7 51                       push       ecx
004573c8 0f57c0                   xorps      xmm0, xmm0
004573cb f30f110424               movss      dword ptr [esp], xmm0
004573d0 51                       push       ecx
004573d1 0f57c0                   xorps      xmm0, xmm0
004573d4 f30f110424               movss      dword ptr [esp], xmm0
004573d9 51                       push       ecx
004573da f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
004573df f30f110424               movss      dword ptr [esp], xmm0
004573e4 8d4dcc                   lea        ecx, [ebp - 0x34]
004573e7 e8e4b9fcff               call       0x422dd0
004573ec 90                       nop        
004573ed c745c800000000           mov        dword ptr [ebp - 0x38], 0
004573f4 eb09                     jmp        0x4573ff
004573f6 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004573f9 83c101                   add        ecx, 1
004573fc 894dc8                   mov        dword ptr [ebp - 0x38], ecx
004573ff 8b5524                   mov        edx, dword ptr [ebp + 0x24]
00457402 d1e2                     shl        edx, 1
00457404 3955c8                   cmp        dword ptr [ebp - 0x38], edx
00457407 0f8dee010000             jge        0x4575fb
0045740d f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00457412 f30f1145bc               movss      dword ptr [ebp - 0x44], xmm0
00457417 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0045741a 2501000080               and        eax, 0x80000001
0045741f 7905                     jns        0x457426
00457421 48                       dec        eax
00457422 83c8fe                   or         eax, 0xfffffffe
00457425 40                       inc        eax
00457426 85c0                     test       eax, eax
00457428 7508                     jne        0x457432
0045742a 8d4dd8                   lea        ecx, [ebp - 0x28]
0045742d 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
00457430 eb06                     jmp        0x457438
00457432 8d55cc                   lea        edx, [ebp - 0x34]
00457435 8955c4                   mov        dword ptr [ebp - 0x3c], edx
00457438 b80c000000               mov        eax, 0xc
0045743d 6bc800                   imul       ecx, eax, 0
00457440 8d540de4                 lea        edx, [ebp + ecx - 0x1c]
00457444 51                       push       ecx
00457445 f30f1045bc               movss      xmm0, dword ptr [ebp - 0x44]
0045744a f30f110424               movss      dword ptr [esp], xmm0
0045744f 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
00457452 50                       push       eax
00457453 52                       push       edx
00457454 e8471b0000               call       0x458fa0
00457459 83c40c                   add        esp, 0xc
0045745c f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00457464 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
0045746c f30f2a4d24               cvtsi2ss   xmm1, dword ptr [ebp + 0x24]
00457471 f30f5ec1                 divss      xmm0, xmm1
00457475 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0045747d f30f584520               addss      xmm0, dword ptr [ebp + 0x20]
00457482 f30f114520               movss      dword ptr [ebp + 0x20], xmm0
00457487 51                       push       ecx
00457488 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
0045748d f30f110424               movss      dword ptr [esp], xmm0
00457492 e8a910feff               call       0x438540
00457497 83c404                   add        esp, 4
0045749a d95d20                   fstp       dword ptr [ebp + 0x20]
0045749d f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
004574a2 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
004574a7 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004574aa 81e101000080             and        ecx, 0x80000001
004574b0 7905                     jns        0x4574b7
004574b2 49                       dec        ecx
004574b3 83c9fe                   or         ecx, 0xfffffffe
004574b6 41                       inc        ecx
004574b7 85c9                     test       ecx, ecx
004574b9 7408                     je         0x4574c3
004574bb 8d55d8                   lea        edx, [ebp - 0x28]
004574be 8955c0                   mov        dword ptr [ebp - 0x40], edx
004574c1 eb06                     jmp        0x4574c9
004574c3 8d45cc                   lea        eax, [ebp - 0x34]
004574c6 8945c0                   mov        dword ptr [ebp - 0x40], eax
004574c9 b90c000000               mov        ecx, 0xc
004574ce c1e100                   shl        ecx, 0
004574d1 8d540de4                 lea        edx, [ebp + ecx - 0x1c]
004574d5 51                       push       ecx
004574d6 f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
004574db f30f110424               movss      dword ptr [esp], xmm0
004574e0 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
004574e3 50                       push       eax
004574e4 52                       push       edx
004574e5 e8b61a0000               call       0x458fa0
004574ea 83c40c                   add        esp, 0xc
004574ed 51                       push       ecx
004574ee 0f57c0                   xorps      xmm0, xmm0
004574f1 f30f110424               movss      dword ptr [esp], xmm0
004574f6 51                       push       ecx
004574f7 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004574fc f30f110424               movss      dword ptr [esp], xmm0
00457501 51                       push       ecx
00457502 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00457507 f30f110424               movss      dword ptr [esp], xmm0
0045750c 8d4dac                   lea        ecx, [ebp - 0x54]
0045750f e8bcb8fcff               call       0x422dd0
00457514 8d4dac                   lea        ecx, [ebp - 0x54]
00457517 51                       push       ecx
00457518 ba0c000000               mov        edx, 0xc
0045751d 6bc200                   imul       eax, edx, 0
00457520 8d4c05e4                 lea        ecx, [ebp + eax - 0x1c]
00457524 e8b721fdff               call       0x4296e0
00457529 90                       nop        
0045752a 51                       push       ecx
0045752b 0f57c0                   xorps      xmm0, xmm0
0045752e f30f110424               movss      dword ptr [esp], xmm0
00457533 51                       push       ecx
00457534 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00457539 f30f110424               movss      dword ptr [esp], xmm0
0045753e 51                       push       ecx
0045753f f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00457544 f30f110424               movss      dword ptr [esp], xmm0
00457549 8d4da0                   lea        ecx, [ebp - 0x60]
0045754c e87fb8fcff               call       0x422dd0
00457551 8d4da0                   lea        ecx, [ebp - 0x60]
00457554 51                       push       ecx
00457555 ba0c000000               mov        edx, 0xc
0045755a c1e200                   shl        edx, 0
0045755d 8d4c15e4                 lea        ecx, [ebp + edx - 0x1c]
00457561 e87a21fdff               call       0x4296e0
00457566 90                       nop        
00457567 51                       push       ecx
00457568 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
0045756d f30f110424               movss      dword ptr [esp], xmm0
00457572 51                       push       ecx
00457573 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00457578 f30f110424               movss      dword ptr [esp], xmm0
0045757d 51                       push       ecx
0045757e f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00457583 f30f110424               movss      dword ptr [esp], xmm0
00457588 51                       push       ecx
00457589 f30f104508               movss      xmm0, dword ptr [ebp + 8]
0045758e f30f110424               movss      dword ptr [esp], xmm0
00457593 b80c000000               mov        eax, 0xc
00457598 c1e000                   shl        eax, 0
0045759b 51                       push       ecx
0045759c f30f104405e8             movss      xmm0, dword ptr [ebp + eax - 0x18]
004575a2 f30f110424               movss      dword ptr [esp], xmm0
004575a7 b90c000000               mov        ecx, 0xc
004575ac c1e100                   shl        ecx, 0
004575af 51                       push       ecx
004575b0 f30f10440de4             movss      xmm0, dword ptr [ebp + ecx - 0x1c]
004575b6 f30f110424               movss      dword ptr [esp], xmm0
004575bb ba0c000000               mov        edx, 0xc
004575c0 6bc200                   imul       eax, edx, 0
004575c3 51                       push       ecx
004575c4 f30f104405e8             movss      xmm0, dword ptr [ebp + eax - 0x18]
004575ca f30f110424               movss      dword ptr [esp], xmm0
004575cf b90c000000               mov        ecx, 0xc
004575d4 6bd100                   imul       edx, ecx, 0
004575d7 51                       push       ecx
004575d8 f30f104415e4             movss      xmm0, dword ptr [ebp + edx - 0x1c]
004575de f30f110424               movss      dword ptr [esp], xmm0
004575e3 e838f3ffff               call       0x456920
004575e8 83c420                   add        esp, 0x20
004575eb 0fb6c0                   movzx      eax, al
004575ee 85c0                     test       eax, eax
004575f0 7404                     je         0x4575f6
004575f2 32c0                     xor        al, al
004575f4 eb07                     jmp        0x4575fd
004575f6 e9fbfdffff               jmp        0x4573f6
004575fb b001                     mov        al, 1
004575fd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00457600 33cd                     xor        ecx, ebp
00457602 e8e7b30e00               call       0x5429ee
00457607 8be5                     mov        esp, ebp
00457609 5d                       pop        ebp
0045760a c3                       ret        
0045760b cc                       int3       
0045760c cc                       int3       
0045760d cc                       int3       
0045760e cc                       int3       
0045760f cc                       int3       
