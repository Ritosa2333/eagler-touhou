00458e90 55                       push       ebp
00458e91 8bec                     mov        ebp, esp
00458e93 83ec1c                   sub        esp, 0x1c
00458e96 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00458e9b 33c5                     xor        eax, ebp
00458e9d 8945fc                   mov        dword ptr [ebp - 4], eax
00458ea0 8d4df0                   lea        ecx, [ebp - 0x10]
00458ea3 e8689ffcff               call       0x422e10
00458ea8 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00458ead f30f5c4508               subss      xmm0, dword ptr [ebp + 8]
00458eb2 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00458eb7 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00458ebc f30f5c450c               subss      xmm0, dword ptr [ebp + 0xc]
00458ec1 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00458ec6 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00458ecb 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
00458ed2 9f                       lahf       
00458ed3 f6c444                   test       ah, 0x44
00458ed6 7b22                     jnp        0x458efa
00458ed8 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00458edd 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00458ee4 51                       push       ecx
00458ee5 f30f110424               movss      dword ptr [esp], xmm0
00458eea 8d45f0                   lea        eax, [ebp - 0x10]
00458eed 50                       push       eax
00458eee 8d4df0                   lea        ecx, [ebp - 0x10]
00458ef1 51                       push       ecx
00458ef2 e8a9000000               call       0x458fa0
00458ef7 83c40c                   add        esp, 0xc
00458efa 51                       push       ecx
00458efb f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00458f00 f30f110424               movss      dword ptr [esp], xmm0
00458f05 e876c7feff               call       0x445680
00458f0a 83c404                   add        esp, 4
00458f0d d95de8                   fstp       dword ptr [ebp - 0x18]
00458f10 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00458f15 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458f1d 0f2f45e8                 comiss     xmm0, dword ptr [ebp - 0x18]
00458f21 7232                     jb         0x458f55
00458f23 51                       push       ecx
00458f24 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00458f29 f30f110424               movss      dword ptr [esp], xmm0
00458f2e e84dc7feff               call       0x445680
00458f33 83c404                   add        esp, 4
00458f36 d95de4                   fstp       dword ptr [ebp - 0x1c]
00458f39 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00458f3e f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00458f46 0f2f45e4                 comiss     xmm0, dword ptr [ebp - 0x1c]
00458f4a 7209                     jb         0x458f55
00458f4c c745ec01000000           mov        dword ptr [ebp - 0x14], 1
00458f53 eb07                     jmp        0x458f5c
00458f55 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
00458f5c 0fb645ec                 movzx      eax, byte ptr [ebp - 0x14]
00458f60 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00458f63 33cd                     xor        ecx, ebp
00458f65 e8849a0e00               call       0x5429ee
00458f6a 8be5                     mov        esp, ebp
00458f6c 5d                       pop        ebp
00458f6d c3                       ret        
00458f6e cc                       int3       
00458f6f cc                       int3       
