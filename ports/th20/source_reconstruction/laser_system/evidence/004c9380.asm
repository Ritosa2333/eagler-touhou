004c9380 55                       push       ebp
004c9381 8bec                     mov        ebp, esp
004c9383 83ec2c                   sub        esp, 0x2c
004c9386 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c938b 33c5                     xor        eax, ebp
004c938d 8945fc                   mov        dword ptr [ebp - 4], eax
004c9390 894dec                   mov        dword ptr [ebp - 0x14], ecx
004c9393 837d0c00                 cmp        dword ptr [ebp + 0xc], 0
004c9397 7413                     je         0x4c93ac
004c9399 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004c939c 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004c93a3 7407                     je         0x4c93ac
004c93a5 33c0                     xor        eax, eax
004c93a7 e9ce000000               jmp        0x4c947a
004c93ac c745d800000000           mov        dword ptr [ebp - 0x28], 0
004c93b3 8d4df0                   lea        ecx, [ebp - 0x10]
004c93b6 e8559af5ff               call       0x422e10
004c93bb 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c93be 8b9128130000             mov        edx, dword ptr [ecx + 0x1328]
004c93c4 8955e4                   mov        dword ptr [ebp - 0x1c], edx
004c93c7 c745e800000000           mov        dword ptr [ebp - 0x18], 0
004c93ce eb09                     jmp        0x4c93d9
004c93d0 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004c93d3 83c001                   add        eax, 1
004c93d6 8945e8                   mov        dword ptr [ebp - 0x18], eax
004c93d9 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c93dc 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004c93df 3b9118070000             cmp        edx, dword ptr [ecx + 0x718]
004c93e5 0f8d83000000             jge        0x4c946e
004c93eb 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004c93ee 8b08                     mov        ecx, dword ptr [eax]
004c93f0 894df0                   mov        dword ptr [ebp - 0x10], ecx
004c93f3 8b5004                   mov        edx, dword ptr [eax + 4]
004c93f6 8955f4                   mov        dword ptr [ebp - 0xc], edx
004c93f9 8b4008                   mov        eax, dword ptr [eax + 8]
004c93fc 8945f8                   mov        dword ptr [ebp - 8], eax
004c93ff 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004c9402 99                       cdq        
004c9403 b903000000               mov        ecx, 3
004c9408 f7f9                     idiv       ecx
004c940a 85d2                     test       edx, edx
004c940c 7552                     jne        0x4c9460
004c940e 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004c9411 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004c9417 e8e42ef4ff               call       0x40c300
004c941c 8bc8                     mov        ecx, eax
004c941e e88dc5fbff               call       0x4859b0
004c9423 8945dc                   mov        dword ptr [ebp - 0x24], eax
004c9426 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004c9429 8b8814070000             mov        ecx, dword ptr [eax + 0x714]
004c942f 8d9409d4000000           lea        edx, [ecx + ecx + 0xd4]
004c9436 8955e0                   mov        dword ptr [ebp - 0x20], edx
004c9439 6a00                     push       0
004c943b 6aff                     push       -1
004c943d 51                       push       ecx
004c943e 0f57c0                   xorps      xmm0, xmm0
004c9441 f30f110424               movss      dword ptr [esp], xmm0
004c9446 8d45f0                   lea        eax, [ebp - 0x10]
004c9449 50                       push       eax
004c944a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c944d 51                       push       ecx
004c944e 6814fd5600               push       0x56fd14
004c9453 8d55d4                   lea        edx, [ebp - 0x2c]
004c9456 52                       push       edx
004c9457 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004c945a e881fcfaff               call       0x4790e0
004c945f 90                       nop        
004c9460 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004c9463 83c020                   add        eax, 0x20
004c9466 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004c9469 e962ffffff               jmp        0x4c93d0
004c946e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c9471 c7412001000000           mov        dword ptr [ecx + 0x20], 1
004c9478 33c0                     xor        eax, eax
004c947a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c947d 33cd                     xor        ecx, ebp
004c947f e86a950700               call       0x5429ee
004c9484 8be5                     mov        esp, ebp
004c9486 5d                       pop        ebp
004c9487 c20800                   ret        8
004c948a cc                       int3       
004c948b cc                       int3       
004c948c cc                       int3       
004c948d cc                       int3       
004c948e cc                       int3       
004c948f cc                       int3       
