004298e0 55                       push       ebp
004298e1 8bec                     mov        ebp, esp
004298e3 83ec20                   sub        esp, 0x20
004298e6 894df8                   mov        dword ptr [ebp - 8], ecx
004298e9 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004298ec e8efa5ffff               call       0x423ee0
004298f1 8945fc                   mov        dword ptr [ebp - 4], eax
004298f4 f20f2a45fc               cvtsi2sd   xmm0, dword ptr [ebp - 4]
004298f9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004298fc c1e81f                   shr        eax, 0x1f
004298ff f20f5804c5d0cd5600       addsd      xmm0, qword ptr [eax*8 + 0x56cdd0]
00429908 f20f1145e8               movsd      qword ptr [ebp - 0x18], xmm0
0042990d f20f5a45e8               cvtsd2ss   xmm0, qword ptr [ebp - 0x18]
00429912 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00429915 8b5110                   mov        edx, dword ptr [ecx + 0x10]
00429918 8955f4                   mov        dword ptr [ebp - 0xc], edx
0042991b f20f2a4df4               cvtsi2sd   xmm1, dword ptr [ebp - 0xc]
00429920 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00429923 c1e81f                   shr        eax, 0x1f
00429926 f20f580cc5d0cd5600       addsd      xmm1, qword ptr [eax*8 + 0x56cdd0]
0042992f f20f114de0               movsd      qword ptr [ebp - 0x20], xmm1
00429934 f20f5a4de0               cvtsd2ss   xmm1, qword ptr [ebp - 0x20]
00429939 f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
00429941 f30f5c0dccc85600         subss      xmm1, dword ptr [0x56c8cc]
00429949 f30f5ec1                 divss      xmm0, xmm1
0042994d f30f5c05ccc85600         subss      xmm0, dword ptr [0x56c8cc]
00429955 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
0042995a d945f0                   fld        dword ptr [ebp - 0x10]
0042995d 8be5                     mov        esp, ebp
0042995f 5d                       pop        ebp
00429960 c3                       ret        
00429961 cc                       int3       
00429962 cc                       int3       
00429963 cc                       int3       
00429964 cc                       int3       
00429965 cc                       int3       
00429966 cc                       int3       
00429967 cc                       int3       
00429968 cc                       int3       
00429969 cc                       int3       
0042996a cc                       int3       
0042996b cc                       int3       
0042996c cc                       int3       
0042996d cc                       int3       
0042996e cc                       int3       
0042996f cc                       int3       
