00456690 55                       push       ebp
00456691 8bec                     mov        ebp, esp
00456693 83ec1c                   sub        esp, 0x1c
00456696 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0045669b 33c5                     xor        eax, ebp
0045669d 8945fc                   mov        dword ptr [ebp - 4], eax
004566a0 51                       push       ecx
004566a1 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004566a6 f30f110424               movss      dword ptr [esp], xmm0
004566ab 51                       push       ecx
004566ac f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004566b1 f30f110424               movss      dword ptr [esp], xmm0
004566b6 51                       push       ecx
004566b7 f30f104508               movss      xmm0, dword ptr [ebp + 8]
004566bc f30f110424               movss      dword ptr [esp], xmm0
004566c1 51                       push       ecx
004566c2 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
004566c7 f30f110424               movss      dword ptr [esp], xmm0
004566cc 51                       push       ecx
004566cd f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004566d2 f30f110424               movss      dword ptr [esp], xmm0
004566d7 e804090000               call       0x456fe0
004566dc 83c414                   add        esp, 0x14
004566df 0fb6c0                   movzx      eax, al
004566e2 85c0                     test       eax, eax
004566e4 7407                     je         0x4566ed
004566e6 b001                     mov        al, 1
004566e8 e9ce000000               jmp        0x4567bb
004566ed f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004566f2 f30f5c4508               subss      xmm0, dword ptr [ebp + 8]
004566f7 f30f104d18               movss      xmm1, dword ptr [ebp + 0x18]
004566fc f30f5c4d0c               subss      xmm1, dword ptr [ebp + 0xc]
00456701 51                       push       ecx
00456702 0f57d2                   xorps      xmm2, xmm2
00456705 f30f111424               movss      dword ptr [esp], xmm2
0045670a 51                       push       ecx
0045670b f30f110c24               movss      dword ptr [esp], xmm1
00456710 51                       push       ecx
00456711 f30f110424               movss      dword ptr [esp], xmm0
00456716 8d4df0                   lea        ecx, [ebp - 0x10]
00456719 e8b2c6fcff               call       0x422dd0
0045671e 51                       push       ecx
0045671f f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00456724 f30f110424               movss      dword ptr [esp], xmm0
00456729 8d4df0                   lea        ecx, [ebp - 0x10]
0045672c 51                       push       ecx
0045672d 8d55f0                   lea        edx, [ebp - 0x10]
00456730 52                       push       edx
00456731 e88acbffff               call       0x4532c0
00456736 83c40c                   add        esp, 0xc
00456739 51                       push       ecx
0045673a 0f57c0                   xorps      xmm0, xmm0
0045673d f30f110424               movss      dword ptr [esp], xmm0
00456742 51                       push       ecx
00456743 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00456748 f30f110424               movss      dword ptr [esp], xmm0
0045674d 51                       push       ecx
0045674e f30f104508               movss      xmm0, dword ptr [ebp + 8]
00456753 f30f110424               movss      dword ptr [esp], xmm0
00456758 8d4de4                   lea        ecx, [ebp - 0x1c]
0045675b e870c6fcff               call       0x422dd0
00456760 8d45e4                   lea        eax, [ebp - 0x1c]
00456763 50                       push       eax
00456764 8d4df0                   lea        ecx, [ebp - 0x10]
00456767 e8742ffdff               call       0x4296e0
0045676c 90                       nop        
0045676d 8b4d24                   mov        ecx, dword ptr [ebp + 0x24]
00456770 51                       push       ecx
00456771 51                       push       ecx
00456772 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00456777 f30f110424               movss      dword ptr [esp], xmm0
0045677c 51                       push       ecx
0045677d f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00456782 f30f110424               movss      dword ptr [esp], xmm0
00456787 51                       push       ecx
00456788 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
0045678d f30f110424               movss      dword ptr [esp], xmm0
00456792 51                       push       ecx
00456793 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00456798 f30f110424               movss      dword ptr [esp], xmm0
0045679d 51                       push       ecx
0045679e f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
004567a3 f30f110424               movss      dword ptr [esp], xmm0
004567a8 51                       push       ecx
004567a9 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004567ae f30f110424               movss      dword ptr [esp], xmm0
004567b3 e838090000               call       0x4570f0
004567b8 83c41c                   add        esp, 0x1c
004567bb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004567be 33cd                     xor        ecx, ebp
004567c0 e829c20e00               call       0x5429ee
004567c5 8be5                     mov        esp, ebp
004567c7 5d                       pop        ebp
004567c8 c3                       ret        
004567c9 cc                       int3       
004567ca cc                       int3       
004567cb cc                       int3       
004567cc cc                       int3       
004567cd cc                       int3       
004567ce cc                       int3       
004567cf cc                       int3       
