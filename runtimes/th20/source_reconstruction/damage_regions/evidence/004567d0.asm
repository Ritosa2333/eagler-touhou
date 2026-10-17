004567d0 55                       push       ebp
004567d1 8bec                     mov        ebp, esp
004567d3 83ec1c                   sub        esp, 0x1c
004567d6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004567db 33c5                     xor        eax, ebp
004567dd 8945fc                   mov        dword ptr [ebp - 4], eax
004567e0 51                       push       ecx
004567e1 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004567e6 f30f110424               movss      dword ptr [esp], xmm0
004567eb 51                       push       ecx
004567ec f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004567f1 f30f110424               movss      dword ptr [esp], xmm0
004567f6 51                       push       ecx
004567f7 f30f104508               movss      xmm0, dword ptr [ebp + 8]
004567fc f30f110424               movss      dword ptr [esp], xmm0
00456801 51                       push       ecx
00456802 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00456807 f30f110424               movss      dword ptr [esp], xmm0
0045680c 51                       push       ecx
0045680d f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00456812 f30f110424               movss      dword ptr [esp], xmm0
00456817 e8c4070000               call       0x456fe0
0045681c 83c414                   add        esp, 0x14
0045681f 0fb6c0                   movzx      eax, al
00456822 85c0                     test       eax, eax
00456824 7407                     je         0x45682d
00456826 b001                     mov        al, 1
00456828 e9d9000000               jmp        0x456906
0045682d f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00456832 f30f5c4508               subss      xmm0, dword ptr [ebp + 8]
00456837 f30f104d18               movss      xmm1, dword ptr [ebp + 0x18]
0045683c f30f5c4d0c               subss      xmm1, dword ptr [ebp + 0xc]
00456841 51                       push       ecx
00456842 0f57d2                   xorps      xmm2, xmm2
00456845 f30f111424               movss      dword ptr [esp], xmm2
0045684a 51                       push       ecx
0045684b f30f110c24               movss      dword ptr [esp], xmm1
00456850 51                       push       ecx
00456851 f30f110424               movss      dword ptr [esp], xmm0
00456856 8d4df0                   lea        ecx, [ebp - 0x10]
00456859 e872c5fcff               call       0x422dd0
0045685e 51                       push       ecx
0045685f f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00456864 f30f110424               movss      dword ptr [esp], xmm0
00456869 8d4df0                   lea        ecx, [ebp - 0x10]
0045686c 51                       push       ecx
0045686d 8d55f0                   lea        edx, [ebp - 0x10]
00456870 52                       push       edx
00456871 e84acaffff               call       0x4532c0
00456876 83c40c                   add        esp, 0xc
00456879 51                       push       ecx
0045687a 0f57c0                   xorps      xmm0, xmm0
0045687d f30f110424               movss      dword ptr [esp], xmm0
00456882 51                       push       ecx
00456883 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00456888 f30f110424               movss      dword ptr [esp], xmm0
0045688d 51                       push       ecx
0045688e f30f104508               movss      xmm0, dword ptr [ebp + 8]
00456893 f30f110424               movss      dword ptr [esp], xmm0
00456898 8d4de4                   lea        ecx, [ebp - 0x1c]
0045689b e830c5fcff               call       0x422dd0
004568a0 8d45e4                   lea        eax, [ebp - 0x1c]
004568a3 50                       push       eax
004568a4 8d4df0                   lea        ecx, [ebp - 0x10]
004568a7 e8342efdff               call       0x4296e0
004568ac 90                       nop        
004568ad 8b4d28                   mov        ecx, dword ptr [ebp + 0x28]
004568b0 51                       push       ecx
004568b1 51                       push       ecx
004568b2 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
004568b7 f30f110424               movss      dword ptr [esp], xmm0
004568bc 51                       push       ecx
004568bd f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
004568c2 f30f110424               movss      dword ptr [esp], xmm0
004568c7 51                       push       ecx
004568c8 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
004568cd f30f110424               movss      dword ptr [esp], xmm0
004568d2 51                       push       ecx
004568d3 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
004568d8 f30f110424               movss      dword ptr [esp], xmm0
004568dd 51                       push       ecx
004568de f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004568e3 f30f110424               movss      dword ptr [esp], xmm0
004568e8 51                       push       ecx
004568e9 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
004568ee f30f110424               movss      dword ptr [esp], xmm0
004568f3 51                       push       ecx
004568f4 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004568f9 f30f110424               movss      dword ptr [esp], xmm0
004568fe e87d0a0000               call       0x457380
00456903 83c420                   add        esp, 0x20
00456906 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00456909 33cd                     xor        ecx, ebp
0045690b e8dec00e00               call       0x5429ee
00456910 8be5                     mov        esp, ebp
00456912 5d                       pop        ebp
00456913 c3                       ret        
00456914 cc                       int3       
00456915 cc                       int3       
00456916 cc                       int3       
00456917 cc                       int3       
00456918 cc                       int3       
00456919 cc                       int3       
0045691a cc                       int3       
0045691b cc                       int3       
0045691c cc                       int3       
0045691d cc                       int3       
0045691e cc                       int3       
0045691f cc                       int3       
