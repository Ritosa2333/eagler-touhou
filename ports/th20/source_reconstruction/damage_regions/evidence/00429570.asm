00429570 55                       push       ebp
00429571 8bec                     mov        ebp, esp
00429573 51                       push       ecx
00429574 894dfc                   mov        dword ptr [ebp - 4], ecx
00429577 8b45fc                   mov        eax, dword ptr [ebp - 4]
0042957a 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0042957d f30f104008               movss      xmm0, dword ptr [eax + 8]
00429582 f30f584108               addss      xmm0, dword ptr [ecx + 8]
00429587 51                       push       ecx
00429588 f30f110424               movss      dword ptr [esp], xmm0
0042958d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00429590 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00429593 f30f104204               movss      xmm0, dword ptr [edx + 4]
00429598 f30f584004               addss      xmm0, dword ptr [eax + 4]
0042959d 51                       push       ecx
0042959e f30f110424               movss      dword ptr [esp], xmm0
004295a3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004295a6 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004295a9 f30f1001                 movss      xmm0, dword ptr [ecx]
004295ad f30f5802                 addss      xmm0, dword ptr [edx]
004295b1 51                       push       ecx
004295b2 f30f110424               movss      dword ptr [esp], xmm0
004295b7 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004295ba e81198ffff               call       0x422dd0
004295bf 8b4508                   mov        eax, dword ptr [ebp + 8]
004295c2 8be5                     mov        esp, ebp
004295c4 5d                       pop        ebp
004295c5 c20800                   ret        8
004295c8 cc                       int3       
004295c9 cc                       int3       
004295ca cc                       int3       
004295cb cc                       int3       
004295cc cc                       int3       
004295cd cc                       int3       
004295ce cc                       int3       
004295cf cc                       int3       
