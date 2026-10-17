004c03d0 55                       push       ebp
004c03d1 8bec                     mov        ebp, esp
004c03d3 83ec0c                   sub        esp, 0xc
004c03d6 894dfc                   mov        dword ptr [ebp - 4], ecx
004c03d9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c03dc 83c134                   add        ecx, 0x34
004c03df e80c9efbff               call       0x47a1f0
004c03e4 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c03e7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c03ea f30f104018               movss      xmm0, dword ptr [eax + 0x18]
004c03ef f30f584120               addss      xmm0, dword ptr [ecx + 0x20]
004c03f4 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c03f7 f30f114218               movss      dword ptr [edx + 0x18], xmm0
004c03fc 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c03ff 51                       push       ecx
004c0400 f30f104028               movss      xmm0, dword ptr [eax + 0x28]
004c0405 f30f110424               movss      dword ptr [esp], xmm0
004c040a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c040d 83c124                   add        ecx, 0x24
004c0410 e8ab2cf9ff               call       0x4530c0
004c0415 6a00                     push       0
004c0417 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c041a 81c1a4000000             add        ecx, 0xa4
004c0420 e87b13f5ff               call       0x4117a0
004c0425 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0428 83c17c                   add        ecx, 0x7c
004c042b 894df8                   mov        dword ptr [ebp - 8], ecx
004c042e 6a00                     push       0
004c0430 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c0433 e8e88ff6ff               call       0x429420
004c0438 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c043b 8b82a8000000             mov        eax, dword ptr [edx + 0xa8]
004c0441 83e801                   sub        eax, 1
004c0444 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0447 8981a8000000             mov        dword ptr [ecx + 0xa8], eax
004c044d 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c0450 83c27c                   add        edx, 0x7c
004c0453 8955f4                   mov        dword ptr [ebp - 0xc], edx
004c0456 6a00                     push       0
004c0458 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c045b e83031f6ff               call       0x423590
004c0460 0fb6c0                   movzx      eax, al
004c0463 85c0                     test       eax, eax
004c0465 7409                     je         0x4c0470
004c0467 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c046a e8c11a0000               call       0x4c1f30
004c046f 90                       nop        
004c0470 8be5                     mov        esp, ebp
004c0472 5d                       pop        ebp
004c0473 c3                       ret        
004c0474 cc                       int3       
004c0475 cc                       int3       
004c0476 cc                       int3       
004c0477 cc                       int3       
004c0478 cc                       int3       
004c0479 cc                       int3       
004c047a cc                       int3       
004c047b cc                       int3       
004c047c cc                       int3       
004c047d cc                       int3       
004c047e cc                       int3       
004c047f cc                       int3       
