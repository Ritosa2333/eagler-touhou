00429830 55                       push       ebp
00429831 8bec                     mov        ebp, esp
00429833 83ec20                   sub        esp, 0x20
00429836 894df8                   mov        dword ptr [ebp - 8], ecx
00429839 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0042983c e89fa6ffff               call       0x423ee0
00429841 8945fc                   mov        dword ptr [ebp - 4], eax
00429844 f20f2a45fc               cvtsi2sd   xmm0, dword ptr [ebp - 4]
00429849 8b45fc                   mov        eax, dword ptr [ebp - 4]
0042984c c1e81f                   shr        eax, 0x1f
0042984f f20f5804c5d0cd5600       addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
00429858 f20f1145e8               movsd      qword ptr [ebp - 0x18], xmm0
0042985d f20f5a45e8               cvtsd2ss   xmm0, qword ptr [ebp - 0x18]
00429862 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00429865 8b5110                   mov        edx, dword ptr [ecx + 0x10]
00429868 8955f4                   mov        dword ptr [ebp - 0xc], edx
0042986b f20f2a4df4               cvtsi2sd   xmm1, dword ptr [ebp - 0xc]
00429870 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00429873 c1e81f                   shr        eax, 0x1f
00429876 f20f580cc5d0cd5600       addsd      xmm1, qword ptr [eax*8 + 0x56cdd0]
0042987f f20f114de0               movsd      qword ptr [ebp - 0x20], xmm1
00429884 f20f5a4de0               cvtsd2ss   xmm1, qword ptr [ebp - 0x20]
00429889 f30f5c0dccc85600         subss      xmm1, dword ptr [0x56c8cc]
00429891 f30f5ec1                 divss      xmm0, xmm1
00429895 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
0042989a d945f0                   fld        dword ptr [ebp - 0x10]
0042989d 8be5                     mov        esp, ebp
0042989f 5d                       pop        ebp
004298a0 c3                       ret        
004298a1 cc                       int3       
004298a2 cc                       int3       
004298a3 cc                       int3       
004298a4 cc                       int3       
004298a5 cc                       int3       
004298a6 cc                       int3       
004298a7 cc                       int3       
004298a8 cc                       int3       
004298a9 cc                       int3       
004298aa cc                       int3       
004298ab cc                       int3       
004298ac cc                       int3       
004298ad cc                       int3       
004298ae cc                       int3       
004298af cc                       int3       
