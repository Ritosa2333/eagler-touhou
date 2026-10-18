; timer_tick: original th20.exe code, SHA256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
; Recovered C++: native_recovered/native_core.hpp
; Called helpers remain visible in analysis/binary/disassembly.asm
004533b0  55                               push     ebp
004533b1  8bec                             mov      ebp, esp
004533b3  83ec18                           sub      esp, 0x18
004533b6  894dfc                           mov      dword ptr [ebp - 4], ecx
004533b9  8b45fc                           mov      eax, dword ptr [ebp - 4]
004533bc  8b480c                           mov      ecx, dword ptr [eax + 0xc]
004533bf  83e101                           and      ecx, 1
004533c2  7521                             jne      0x4533e5
004533c4  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
004533c7  e8840bfdff                       call     0x423f50
004533cc  6a00                             push     0
004533ce  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
004533d1  e80a0cfdff                       call     0x423fe0
004533d6  8b55fc                           mov      edx, dword ptr [ebp - 4]
004533d9  8b420c                           mov      eax, dword ptr [edx + 0xc]
004533dc  83c801                           or       eax, 1
004533df  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
004533e2  89410c                           mov      dword ptr [ecx + 0xc], eax
004533e5  8b55fc                           mov      edx, dword ptr [ebp - 4]
004533e8  8b420c                           mov      eax, dword ptr [edx + 0xc]
004533eb  d1e8                             shr      eax, 1
004533ed  83e003                           and      eax, 3
004533f0  83f801                           cmp      eax, 1
004533f3  720f                             jb       0x453404
004533f5  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
004533f8  8b510c                           mov      edx, dword ptr [ecx + 0xc]
004533fb  83e2f9                           and      edx, 0xfffffff9
004533fe  8b45fc                           mov      eax, dword ptr [ebp - 4]
00453401  89500c                           mov      dword ptr [eax + 0xc], edx
00453404  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
00453407  8b510c                           mov      edx, dword ptr [ecx + 0xc]
0045340a  d1ea                             shr      edx, 1
0045340c  83e203                           and      edx, 3
0045340f  8b0495e0ef5a00                   mov      eax, dword ptr [edx*4 + 0x5aefe0]
00453416  8945f8                           mov      dword ptr [ebp - 8], eax
00453419  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
0045341c  8b55fc                           mov      edx, dword ptr [ebp - 4]
0045341f  8b4204                           mov      eax, dword ptr [edx + 4]
00453422  8901                             mov      dword ptr [ecx], eax
00453424  837df800                         cmp      dword ptr [ebp - 8], 0
00453428  0f8495000000                     je       0x4534c3
0045342e  8b4df8                           mov      ecx, dword ptr [ebp - 8]
00453431  e8aa5efdff                       call     0x4292e0
00453436  d95df4                           fstp     dword ptr [ebp - 0xc]
00453439  f30f1045f4                       movss    xmm0, dword ptr [ebp - 0xc]
0045343e  0f2f052ce75600                   comiss   xmm0, dword ptr [0x56e72c]
00453445  7642                             jbe      0x453489
00453447  8b4df8                           mov      ecx, dword ptr [ebp - 8]
0045344a  e8915efdff                       call     0x4292e0
0045344f  d95df0                           fstp     dword ptr [ebp - 0x10]
00453452  f30f100530e75600                 movss    xmm0, dword ptr [0x56e730]
0045345a  0f2f45f0                         comiss   xmm0, dword ptr [ebp - 0x10]
0045345e  7629                             jbe      0x453489
00453460  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
00453463  8b5104                           mov      edx, dword ptr [ecx + 4]
00453466  83c201                           add      edx, 1
00453469  8b45fc                           mov      eax, dword ptr [ebp - 4]
0045346c  895004                           mov      dword ptr [eax + 4], edx
0045346f  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
00453472  f30f104108                       movss    xmm0, dword ptr [ecx + 8]
00453477  f30f5805ccc85600                 addss    xmm0, dword ptr [0x56c8cc]
0045347f  8b55fc                           mov      edx, dword ptr [ebp - 4]
00453482  f30f114208                       movss    dword ptr [edx + 8], xmm0
00453487  eb38                             jmp      0x4534c1
00453489  8b4df8                           mov      ecx, dword ptr [ebp - 8]
0045348c  e84f5efdff                       call     0x4292e0
00453491  d95de8                           fstp     dword ptr [ebp - 0x18]
00453494  8b45fc                           mov      eax, dword ptr [ebp - 4]
00453497  f30f104008                       movss    xmm0, dword ptr [eax + 8]
0045349c  f30f1145ec                       movss    dword ptr [ebp - 0x14], xmm0
004534a1  f30f1045ec                       movss    xmm0, dword ptr [ebp - 0x14]
004534a6  f30f5845e8                       addss    xmm0, dword ptr [ebp - 0x18]
004534ab  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
004534ae  f30f114108                       movss    dword ptr [ecx + 8], xmm0
004534b3  8b55fc                           mov      edx, dword ptr [ebp - 4]
004534b6  f30f2c4208                       cvttss2si eax, dword ptr [edx + 8]
004534bb  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
004534be  894104                           mov      dword ptr [ecx + 4], eax
004534c1  eb27                             jmp      0x4534ea
004534c3  8b55fc                           mov      edx, dword ptr [ebp - 4]
004534c6  8b4204                           mov      eax, dword ptr [edx + 4]
004534c9  83c001                           add      eax, 1
004534cc  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
004534cf  894104                           mov      dword ptr [ecx + 4], eax
004534d2  8b55fc                           mov      edx, dword ptr [ebp - 4]
004534d5  f30f104208                       movss    xmm0, dword ptr [edx + 8]
004534da  f30f5805ccc85600                 addss    xmm0, dword ptr [0x56c8cc]
004534e2  8b45fc                           mov      eax, dword ptr [ebp - 4]
004534e5  f30f114008                       movss    dword ptr [eax + 8], xmm0
004534ea  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
004534ed  8b4104                           mov      eax, dword ptr [ecx + 4]
004534f0  8be5                             mov      esp, ebp
004534f2  5d                               pop      ebp
004534f3  c3                               ret      
