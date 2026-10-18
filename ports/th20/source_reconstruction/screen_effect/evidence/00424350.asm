00424350 55                       push       ebp
00424351 8bec                     mov        ebp, esp
00424353 83ec1c                   sub        esp, 0x1c
00424356 8b4508                   mov        eax, dword ptr [ebp + 8]
00424359 8b4824                   mov        ecx, dword ptr [eax + 0x24]
0042435c c1e918                   shr        ecx, 0x18
0042435f 81e1ff000000             and        ecx, 0xff
00424365 894df8                   mov        dword ptr [ebp - 8], ecx
00424368 833d18a55b0000           cmp        dword ptr [0x5ba518], 0
0042436f 740a                     je         0x42437b
00424371 b807000000               mov        eax, 7
00424376 e9d9000000               jmp        0x424454
0042437b 8b5508                   mov        edx, dword ptr [ebp + 8]
0042437e 83c230                   add        edx, 0x30
00424381 8955f4                   mov        dword ptr [ebp - 0xc], edx
00424384 8b4508                   mov        eax, dword ptr [ebp + 8]
00424387 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
0042438a 51                       push       ecx
0042438b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0042438e e8cdf1ffff               call       0x423560
00424393 0fb6d0                   movzx      edx, al
00424396 85d2                     test       edx, edx
00424398 746b                     je         0x424405
0042439a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0042439d 83c130                   add        ecx, 0x30
004243a0 e82bf8ffff               call       0x423bd0
004243a5 d95df0                   fstp       dword ptr [ebp - 0x10]
004243a8 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004243ad 8b45f8                   mov        eax, dword ptr [ebp - 8]
004243b0 8945fc                   mov        dword ptr [ebp - 4], eax
004243b3 f20f2a4dfc               cvtsi2sd   xmm1, dword ptr [ebp - 4]
004243b8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004243bb c1e91f                   shr        ecx, 0x1f
004243be f20f580ccdd0cd5600       addsd      xmm1, qword ptr [ecx*8 + 0x56cdd0]
004243c7 f20f114de4               movsd      qword ptr [ebp - 0x1c], xmm1
004243cc f20f5a4de4               cvtsd2ss   xmm1, qword ptr [ebp - 0x1c]
004243d1 f30f59c1                 mulss      xmm0, xmm1
004243d5 8b5508                   mov        edx, dword ptr [ebp + 8]
004243d8 f30f2a4a1c               cvtsi2ss   xmm1, dword ptr [edx + 0x1c]
004243dd f30f5ec1                 divss      xmm0, xmm1
004243e1 f30f2cc0                 cvttss2si  eax, xmm0
004243e5 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004243e8 2bc8                     sub        ecx, eax
004243ea 8b5508                   mov        edx, dword ptr [ebp + 8]
004243ed 894a18                   mov        dword ptr [edx + 0x18], ecx
004243f0 8b4508                   mov        eax, dword ptr [ebp + 8]
004243f3 83781800                 cmp        dword ptr [eax + 0x18], 0
004243f7 7d0a                     jge        0x424403
004243f9 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004243fc c7411800000000           mov        dword ptr [ecx + 0x18], 0
00424403 eb37                     jmp        0x42443c
00424405 8b5508                   mov        edx, dword ptr [ebp + 8]
00424408 c7421800000000           mov        dword ptr [edx + 0x18], 0
0042440f 8b4508                   mov        eax, dword ptr [ebp + 8]
00424412 8b4820                   mov        ecx, dword ptr [eax + 0x20]
00424415 83e901                   sub        ecx, 1
00424418 8b5508                   mov        edx, dword ptr [ebp + 8]
0042441b 894a20                   mov        dword ptr [edx + 0x20], ecx
0042441e 8b4508                   mov        eax, dword ptr [ebp + 8]
00424421 83782000                 cmp        dword ptr [eax + 0x20], 0
00424425 7f07                     jg         0x42442e
00424427 b807000000               mov        eax, 7
0042442c eb26                     jmp        0x424454
0042442e 6a00                     push       0
00424430 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424433 83c130                   add        ecx, 0x30
00424436 e8e5f0ffff               call       0x423520
0042443b 90                       nop        
0042443c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0042443f 83c130                   add        ecx, 0x30
00424442 894dec                   mov        dword ptr [ebp - 0x14], ecx
00424445 6a00                     push       0
00424447 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0042444a e8f1f0ffff               call       0x423540
0042444f b801000000               mov        eax, 1
00424454 8be5                     mov        esp, ebp
00424456 5d                       pop        ebp
00424457 c3                       ret        
00424458 cc                       int3       
00424459 cc                       int3       
0042445a cc                       int3       
0042445b cc                       int3       
0042445c cc                       int3       
0042445d cc                       int3       
0042445e cc                       int3       
0042445f cc                       int3       
