004292f0 55                       push       ebp
004292f1 8bec                     mov        ebp, esp
004292f3 51                       push       ecx
004292f4 894dfc                   mov        dword ptr [ebp - 4], ecx
004292f7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004292fa f30f104008               movss      xmm0, dword ptr [eax + 8]
004292ff f30f59450c               mulss      xmm0, dword ptr [ebp + 0xc]
00429304 51                       push       ecx
00429305 f30f110424               movss      dword ptr [esp], xmm0
0042930a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0042930d f30f104104               movss      xmm0, dword ptr [ecx + 4]
00429312 f30f59450c               mulss      xmm0, dword ptr [ebp + 0xc]
00429317 51                       push       ecx
00429318 f30f110424               movss      dword ptr [esp], xmm0
0042931d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00429320 f30f1002                 movss      xmm0, dword ptr [edx]
00429324 f30f59450c               mulss      xmm0, dword ptr [ebp + 0xc]
00429329 51                       push       ecx
0042932a f30f110424               movss      dword ptr [esp], xmm0
0042932f 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00429332 e8999affff               call       0x422dd0
00429337 8b4508                   mov        eax, dword ptr [ebp + 8]
0042933a 8be5                     mov        esp, ebp
0042933c 5d                       pop        ebp
0042933d c20800                   ret        8
