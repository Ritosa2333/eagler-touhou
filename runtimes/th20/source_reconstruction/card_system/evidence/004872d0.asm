004872d0 55                       push       ebp
004872d1 8bec                     mov        ebp, esp
004872d3 83ec40                   sub        esp, 0x40
004872d6 894dfc                   mov        dword ptr [ebp - 4], ecx
004872d9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004872dc 8b4878                   mov        ecx, dword ptr [eax + 0x78]
004872df 83e101                   and        ecx, 1
004872e2 7435                     je         0x487319
004872e4 8b55fc                   mov        edx, dword ptr [ebp - 4]
004872e7 8b4278                   mov        eax, dword ptr [edx + 0x78]
004872ea c1e806                   shr        eax, 6
004872ed 83e001                   and        eax, 1
004872f0 7522                     jne        0x487314
004872f2 b958675b00               mov        ecx, 0x5b6758
004872f7 e81458f9ff               call       0x41cb10
004872fc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004872ff dd9998000000             fstp       qword ptr [ecx + 0x98]
00487305 8b55fc                   mov        edx, dword ptr [ebp - 4]
00487308 8b4278                   mov        eax, dword ptr [edx + 0x78]
0048730b 83c840                   or         eax, 0x40
0048730e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487311 894178                   mov        dword ptr [ecx + 0x78], eax
00487314 e938020000               jmp        0x487551
00487319 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048731c 8b4278                   mov        eax, dword ptr [edx + 0x78]
0048731f c1e806                   shr        eax, 6
00487322 83e001                   and        eax, 1
00487325 0f8426020000             je         0x487551
0048732b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048732e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00487331 8b828c000000             mov        eax, dword ptr [edx + 0x8c]
00487337 898190000000             mov        dword ptr [ecx + 0x90], eax
0048733d b958675b00               mov        ecx, 0x5b6758
00487342 e8c957f9ff               call       0x41cb10
00487347 dd5dc8                   fstp       qword ptr [ebp - 0x38]
0048734a f20f1045c8               movsd      xmm0, qword ptr [ebp - 0x38]
0048734f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487352 f20f5c8198000000         subsd      xmm0, qword ptr [ecx + 0x98]
0048735a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048735d f20f1182a0000000         movsd      qword ptr [edx + 0xa0], xmm0
00487365 83ec08                   sub        esp, 8
00487368 f20f100560fe5600         movsd      xmm0, qword ptr [0x56fe60]
00487370 f20f110424               movsd      qword ptr [esp], xmm0
00487375 8b45fc                   mov        eax, dword ptr [ebp - 4]
00487378 83ec08                   sub        esp, 8
0048737b f20f1080a0000000         movsd      xmm0, qword ptr [eax + 0xa0]
00487383 f20f110424               movsd      qword ptr [esp], xmm0
00487388 e8138c0c00               call       0x54ffa0
0048738d 83c410                   add        esp, 0x10
00487390 dd5dd0                   fstp       qword ptr [ebp - 0x30]
00487393 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487396 f20f1081a0000000         movsd      xmm0, qword ptr [ecx + 0xa0]
0048739e f20f5c45d0               subsd      xmm0, qword ptr [ebp - 0x30]
004873a3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004873a6 f20f1182a0000000         movsd      qword ptr [edx + 0xa0], xmm0
004873ae f20f100560fe5600         movsd      xmm0, qword ptr [0x56fe60]
004873b6 f20f5e0568fe5600         divsd      xmm0, qword ptr [0x56fe68]
004873be f20f104dd0               movsd      xmm1, qword ptr [ebp - 0x30]
004873c3 660f2fc8                 comisd     xmm1, xmm0
004873c7 721e                     jb         0x4873e7
004873c9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004873cc f20f1080a0000000         movsd      xmm0, qword ptr [eax + 0xa0]
004873d4 f20f580560fe5600         addsd      xmm0, qword ptr [0x56fe60]
004873dc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004873df f20f1181a0000000         movsd      qword ptr [ecx + 0xa0], xmm0
004873e7 8b55fc                   mov        edx, dword ptr [ebp - 4]
004873ea 83ec08                   sub        esp, 8
004873ed f20f1082a0000000         movsd      xmm0, qword ptr [edx + 0xa0]
004873f5 f20f110424               movsd      qword ptr [esp], xmm0
004873fa e8318d0c00               call       0x550130
004873ff 83c408                   add        esp, 8
00487402 e809bf0b00               call       0x543310
00487407 9b                       wait       
00487408 8945f8                   mov        dword ptr [ebp - 8], eax
0048740b 817df8e8030000           cmp        dword ptr [ebp - 8], 0x3e8
00487412 7c07                     jl         0x48741b
00487414 c745f8e7030000           mov        dword ptr [ebp - 8], 0x3e7
0048741b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048741e 83ec08                   sub        esp, 8
00487421 f20f1080a0000000         movsd      xmm0, qword ptr [eax + 0xa0]
00487429 f20f110424               movsd      qword ptr [esp], xmm0
0048742e e8fd8c0c00               call       0x550130
00487433 83c408                   add        esp, 8
00487436 dd5dc0                   fstp       qword ptr [ebp - 0x40]
00487439 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048743c f20f1081a0000000         movsd      xmm0, qword ptr [ecx + 0xa0]
00487444 f20f5c45c0               subsd      xmm0, qword ptr [ebp - 0x40]
00487449 f20f590570fe5600         mulsd      xmm0, qword ptr [0x56fe70]
00487451 f20f2cd0                 cvttsd2si  edx, xmm0
00487455 8955f4                   mov        dword ptr [ebp - 0xc], edx
00487458 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0048745b 50                       push       eax
0048745c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0048745f 51                       push       ecx
00487460 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487463 e828150000               call       0x488990
00487468 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048746b 0f57c0                   xorps      xmm0, xmm0
0048746e f20f1182a0000000         movsd      qword ptr [edx + 0xa0], xmm0
00487476 8b45fc                   mov        eax, dword ptr [ebp - 4]
00487479 8b4878                   mov        ecx, dword ptr [eax + 0x78]
0048747c 83e1bf                   and        ecx, 0xffffffbf
0048747f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00487482 894a78                   mov        dword ptr [edx + 0x78], ecx
00487485 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
0048748b e8a0130000               call       0x488830
00487490 85c0                     test       eax, eax
00487492 754a                     jne        0x4874de
00487494 8b45fc                   mov        eax, dword ptr [ebp - 4]
00487497 8b88a8000000             mov        ecx, dword ptr [eax + 0xa8]
0048749d 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004874a0 b968a55b00               mov        ecx, 0x5ba568
004874a5 e8b6d8feff               call       0x474d60
004874aa 8945f0                   mov        dword ptr [ebp - 0x10], eax
004874ad 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004874b0 52                       push       edx
004874b1 8b0dfc605c00             mov        ecx, dword ptr [0x5c60fc]
004874b7 e884120000               call       0x488740
004874bc 83c01c                   add        eax, 0x1c
004874bf 8945ec                   mov        dword ptr [ebp - 0x14], eax
004874c2 8b45fc                   mov        eax, dword ptr [ebp - 4]
004874c5 8b8888000000             mov        ecx, dword ptr [eax + 0x88]
004874cb c1e102                   shl        ecx, 2
004874ce 894de8                   mov        dword ptr [ebp - 0x18], ecx
004874d1 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004874d4 0355e8                   add        edx, dword ptr [ebp - 0x18]
004874d7 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004874da 8902                     mov        dword ptr [edx], eax
004874dc eb5e                     jmp        0x48753c
004874de b968a55b00               mov        ecx, 0x5ba568
004874e3 e878d8feff               call       0x474d60
004874e8 8945e0                   mov        dword ptr [ebp - 0x20], eax
004874eb 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004874ee 51                       push       ecx
004874ef 8b0dfc605c00             mov        ecx, dword ptr [0x5c60fc]
004874f5 e876120000               call       0x488770
004874fa 83c01c                   add        eax, 0x1c
004874fd 8945dc                   mov        dword ptr [ebp - 0x24], eax
00487500 8b55fc                   mov        edx, dword ptr [ebp - 4]
00487503 8b8288000000             mov        eax, dword ptr [edx + 0x88]
00487509 c1e002                   shl        eax, 2
0048750c 8945d8                   mov        dword ptr [ebp - 0x28], eax
0048750f 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00487512 034dd8                   add        ecx, dword ptr [ebp - 0x28]
00487515 8b55fc                   mov        edx, dword ptr [ebp - 4]
00487518 8b01                     mov        eax, dword ptr [ecx]
0048751a 8982a8000000             mov        dword ptr [edx + 0xa8], eax
00487520 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487523 e8b8100000               call       0x4885e0
00487528 85c0                     test       eax, eax
0048752a 7410                     je         0x48753c
0048752c 6a63                     push       0x63
0048752e 68e7030000               push       0x3e7
00487533 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487536 e855140000               call       0x488990
0048753b 90                       nop        
0048753c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048753f 8b9188000000             mov        edx, dword ptr [ecx + 0x88]
00487545 83c201                   add        edx, 1
00487548 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048754b 899088000000             mov        dword ptr [eax + 0x88], edx
00487551 8be5                     mov        esp, ebp
00487553 5d                       pop        ebp
00487554 c3                       ret        
00487555 cc                       int3       
00487556 cc                       int3       
00487557 cc                       int3       
00487558 cc                       int3       
00487559 cc                       int3       
0048755a cc                       int3       
0048755b cc                       int3       
0048755c cc                       int3       
0048755d cc                       int3       
0048755e cc                       int3       
0048755f cc                       int3       
