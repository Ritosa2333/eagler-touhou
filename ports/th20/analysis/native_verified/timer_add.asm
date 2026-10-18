; timer_add: original th20.exe code, SHA256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
; Recovered C++: native_recovered/native_core.hpp
; Called helpers remain visible in analysis/binary/disassembly.asm
004530f0  55                               push     ebp
004530f1  8bec                             mov      ebp, esp
004530f3  83ec18                           sub      esp, 0x18
004530f6  894dfc                           mov      dword ptr [ebp - 4], ecx
004530f9  8b45fc                           mov      eax, dword ptr [ebp - 4]
004530fc  8b480c                           mov      ecx, dword ptr [eax + 0xc]
004530ff  83e101                           and      ecx, 1
00453102  7521                             jne      0x453125
00453104  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
00453107  e8440efdff                       call     0x423f50
0045310c  6a00                             push     0
0045310e  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
00453111  e8ca0efdff                       call     0x423fe0
00453116  8b55fc                           mov      edx, dword ptr [ebp - 4]
00453119  8b420c                           mov      eax, dword ptr [edx + 0xc]
0045311c  83c801                           or       eax, 1
0045311f  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
00453122  89410c                           mov      dword ptr [ecx + 0xc], eax
00453125  8b55fc                           mov      edx, dword ptr [ebp - 4]
00453128  8b420c                           mov      eax, dword ptr [edx + 0xc]
0045312b  d1e8                             shr      eax, 1
0045312d  83e003                           and      eax, 3
00453130  83f801                           cmp      eax, 1
00453133  720f                             jb       0x453144
00453135  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
00453138  8b510c                           mov      edx, dword ptr [ecx + 0xc]
0045313b  83e2f9                           and      edx, 0xfffffff9
0045313e  8b45fc                           mov      eax, dword ptr [ebp - 4]
00453141  89500c                           mov      dword ptr [eax + 0xc], edx
00453144  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
00453147  8b510c                           mov      edx, dword ptr [ecx + 0xc]
0045314a  d1ea                             shr      edx, 1
0045314c  83e203                           and      edx, 3
0045314f  8b0495e0ef5a00                   mov      eax, dword ptr [edx*4 + 0x5aefe0]
00453156  8945f8                           mov      dword ptr [ebp - 8], eax
00453159  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
0045315c  8b55fc                           mov      edx, dword ptr [ebp - 4]
0045315f  8b4204                           mov      eax, dword ptr [edx + 4]
00453162  8901                             mov      dword ptr [ecx], eax
00453164  837df800                         cmp      dword ptr [ebp - 8], 0
00453168  0f8480000000                     je       0x4531ee
0045316e  8b4df8                           mov      ecx, dword ptr [ebp - 8]
00453171  e86a61fdff                       call     0x4292e0
00453176  d95df4                           fstp     dword ptr [ebp - 0xc]
00453179  f30f1045f4                       movss    xmm0, dword ptr [ebp - 0xc]
0045317e  0f2f052ce75600                   comiss   xmm0, dword ptr [0x56e72c]
00453185  7630                             jbe      0x4531b7
00453187  8b4df8                           mov      ecx, dword ptr [ebp - 8]
0045318a  e85161fdff                       call     0x4292e0
0045318f  d95df0                           fstp     dword ptr [ebp - 0x10]
00453192  f30f100530e75600                 movss    xmm0, dword ptr [0x56e730]
0045319a  0f2f45f0                         comiss   xmm0, dword ptr [ebp - 0x10]
0045319e  7617                             jbe      0x4531b7
004531a0  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
004531a3  f30f104108                       movss    xmm0, dword ptr [ecx + 8]
004531a8  f30f584508                       addss    xmm0, dword ptr [ebp + 8]
004531ad  8b55fc                           mov      edx, dword ptr [ebp - 4]
004531b0  f30f114208                       movss    dword ptr [edx + 8], xmm0
004531b5  eb35                             jmp      0x4531ec
004531b7  51                               push     ecx
004531b8  f30f104508                       movss    xmm0, dword ptr [ebp + 8]
004531bd  f30f110424                       movss    dword ptr [esp], xmm0
004531c2  8b4df8                           mov      ecx, dword ptr [ebp - 8]
004531c5  e886fdffff                       call     0x452f50
004531ca  d95de8                           fstp     dword ptr [ebp - 0x18]
004531cd  8b45fc                           mov      eax, dword ptr [ebp - 4]
004531d0  f30f104008                       movss    xmm0, dword ptr [eax + 8]
004531d5  f30f1145ec                       movss    dword ptr [ebp - 0x14], xmm0
004531da  f30f1045ec                       movss    xmm0, dword ptr [ebp - 0x14]
004531df  f30f5845e8                       addss    xmm0, dword ptr [ebp - 0x18]
004531e4  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
004531e7  f30f114108                       movss    dword ptr [ecx + 8], xmm0
004531ec  eb15                             jmp      0x453203
004531ee  8b55fc                           mov      edx, dword ptr [ebp - 4]
004531f1  f30f104208                       movss    xmm0, dword ptr [edx + 8]
004531f6  f30f584508                       addss    xmm0, dword ptr [ebp + 8]
004531fb  8b45fc                           mov      eax, dword ptr [ebp - 4]
004531fe  f30f114008                       movss    dword ptr [eax + 8], xmm0
00453203  8b4dfc                           mov      ecx, dword ptr [ebp - 4]
00453206  f30f2c5108                       cvttss2si edx, dword ptr [ecx + 8]
0045320b  8b45fc                           mov      eax, dword ptr [ebp - 4]
0045320e  895004                           mov      dword ptr [eax + 4], edx
00453211  8be5                             mov      esp, ebp
00453213  5d                               pop      ebp
00453214  c20400                           ret      4
