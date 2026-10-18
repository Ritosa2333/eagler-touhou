0047bb90 55                       push       ebp
0047bb91 8bec                     mov        ebp, esp
0047bb93 6aff                     push       -1
0047bb95 68a0795600               push       0x5679a0
0047bb9a 64a100000000             mov        eax, dword ptr fs:[0]
0047bba0 50                       push       eax
0047bba1 83ec08                   sub        esp, 8
0047bba4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0047bba9 33c5                     xor        eax, ebp
0047bbab 50                       push       eax
0047bbac 8d45f4                   lea        eax, [ebp - 0xc]
0047bbaf 64a300000000             mov        dword ptr fs:[0], eax
0047bbb5 894df0                   mov        dword ptr [ebp - 0x10], ecx
0047bbb8 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047bbbb 0f57c0                   xorps      xmm0, xmm0
0047bbbe f30f1100                 movss      dword ptr [eax], xmm0
0047bbc2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047bbc5 83c104                   add        ecx, 4
0047bbc8 e8f385f9ff               call       0x4141c0
0047bbcd 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047bbd0 0f57c0                   xorps      xmm0, xmm0
0047bbd3 f30f114114               movss      dword ptr [ecx + 0x14], xmm0
0047bbd8 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0047bbdb 0f57c0                   xorps      xmm0, xmm0
0047bbde f30f114218               movss      dword ptr [edx + 0x18], xmm0
0047bbe3 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047bbe6 0f57c0                   xorps      xmm0, xmm0
0047bbe9 f30f11401c               movss      dword ptr [eax + 0x1c], xmm0
0047bbee 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047bbf1 0f57c0                   xorps      xmm0, xmm0
0047bbf4 f30f114120               movss      dword ptr [ecx + 0x20], xmm0
0047bbf9 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0047bbfc c7422400000000           mov        dword ptr [edx + 0x24], 0
0047bc03 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047bc06 c7402800000000           mov        dword ptr [eax + 0x28], 0
0047bc0d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047bc10 c7412c00000000           mov        dword ptr [ecx + 0x2c], 0
0047bc17 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0047bc1a c7423000000000           mov        dword ptr [edx + 0x30], 0
0047bc21 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047bc24 c7403400000000           mov        dword ptr [eax + 0x34], 0
0047bc2b 33c9                     xor        ecx, ecx
0047bc2d 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0047bc30 66894a38                 mov        word ptr [edx + 0x38], cx
0047bc34 33c0                     xor        eax, eax
0047bc36 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047bc39 6689413a                 mov        word ptr [ecx + 0x3a], ax
0047bc3d 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0047bc40 c7423c15000000           mov        dword ptr [edx + 0x3c], 0x15
0047bc47 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047bc4a c7404026000000           mov        dword ptr [eax + 0x40], 0x26
0047bc51 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047bc54 c7414400000000           mov        dword ptr [ecx + 0x44], 0
0047bc5b 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0047bc5e c6424800                 mov        byte ptr [edx + 0x48], 0
0047bc62 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047bc65 c6404900                 mov        byte ptr [eax + 0x49], 0
0047bc69 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047bc6c 83c104                   add        ecx, 4
0047bc6f 894dec                   mov        dword ptr [ebp - 0x14], ecx
0047bc72 6a02                     push       2
0047bc74 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047bc77 e874a60000               call       0x4862f0
0047bc7c 90                       nop        
0047bc7d 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047bc80 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0047bc83 64890d00000000           mov        dword ptr fs:[0], ecx
0047bc8a 59                       pop        ecx
0047bc8b 8be5                     mov        esp, ebp
0047bc8d 5d                       pop        ebp
0047bc8e c3                       ret        
0047bc8f cc                       int3       
0047bc90 cc                       int3       
0047bc91 cc                       int3       
0047bc92 cc                       int3       
0047bc93 cc                       int3       
0047bc94 cc                       int3       
0047bc95 cc                       int3       
0047bc96 cc                       int3       
0047bc97 cc                       int3       
0047bc98 cc                       int3       
0047bc99 cc                       int3       
0047bc9a cc                       int3       
0047bc9b cc                       int3       
0047bc9c cc                       int3       
0047bc9d cc                       int3       
0047bc9e cc                       int3       
0047bc9f cc                       int3       
