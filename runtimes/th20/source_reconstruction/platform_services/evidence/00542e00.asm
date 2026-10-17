00542e00 f60510265b0020           test       byte ptr [0x5b2610], 0x20
00542e07 7511                     jne        0x542e1a
00542e09 c5f96ec1                 vmovd      xmm0, ecx
00542e0d c4e37922c201             vpinsrd    xmm0, xmm0, edx, 1
00542e13 62f1fe08e6c0             vcvtqq2pd  xmm0, xmm0
00542e19 c3                       ret        
00542e1a 660f6ec1                 movd       xmm0, ecx
00542e1e 0f560530715900           orps       xmm0, xmmword ptr [0x597130]
00542e25 f20f5c0530715900         subsd      xmm0, qword ptr [0x597130]
00542e2d 85d2                     test       edx, edx
00542e2f 7410                     je         0x542e41
00542e31 f20f2aca                 cvtsi2sd   xmm1, edx
00542e35 f20f590d50715900         mulsd      xmm1, qword ptr [0x597150]
00542e3d f20f58c1                 addsd      xmm0, xmm1
00542e41 c3                       ret        
00542e42 55                       push       ebp
00542e43 8bec                     mov        ebp, esp
00542e45 eb0d                     jmp        0x542e54
00542e47 ff7508                   push       dword ptr [ebp + 8]
00542e4a e811fa0000               call       0x552860
00542e4f 59                       pop        ecx
00542e50 85c0                     test       eax, eax
00542e52 7413                     je         0x542e67
00542e54 ff750c                   push       dword ptr [ebp + 0xc]
00542e57 ff7508                   push       dword ptr [ebp + 8]
00542e5a e8d1140100               call       0x554330
00542e5f 59                       pop        ecx
