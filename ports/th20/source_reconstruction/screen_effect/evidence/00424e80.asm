00424e80 55                       push       ebp
00424e81 8bec                     mov        ebp, esp
00424e83 83ec18                   sub        esp, 0x18
00424e86 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00424e8b 33c5                     xor        eax, ebp
00424e8d 8945fc                   mov        dword ptr [ebp - 4], eax
00424e90 8b4508                   mov        eax, dword ptr [ebp + 8]
00424e93 8b4840                   mov        ecx, dword ptr [eax + 0x40]
00424e96 894de8                   mov        dword ptr [ebp - 0x18], ecx
00424e99 f30f1005accd5600         movss      xmm0, dword ptr [0x56cdac]
00424ea1 f30f5c05a4cd5600         subss      xmm0, dword ptr [0x56cda4]
00424ea9 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00424eb1 f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
00424eb6 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
00424ebe f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00424ec3 f30f1005accd5600         movss      xmm0, dword ptr [0x56cdac]
00424ecb f30f5c05a4cd5600         subss      xmm0, dword ptr [0x56cda4]
00424ed3 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00424edb f30f5805a4cd5600         addss      xmm0, dword ptr [0x56cda4]
00424ee3 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00424ee8 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
00424ef0 f30f5805c0d75600         addss      xmm0, dword ptr [0x56d7c0]
00424ef8 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00424efd 8b5508                   mov        edx, dword ptr [ebp + 8]
00424f00 8b4218                   mov        eax, dword ptr [edx + 0x18]
00424f03 c1e018                   shl        eax, 0x18
00424f06 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424f09 0b4120                   or         eax, dword ptr [ecx + 0x20]
00424f0c 50                       push       eax
00424f0d 8d55ec                   lea        edx, [ebp - 0x14]
00424f10 52                       push       edx
00424f11 e83ae7ffff               call       0x423650
00424f16 83c408                   add        esp, 8
00424f19 b801000000               mov        eax, 1
00424f1e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00424f21 33cd                     xor        ecx, ebp
00424f23 e8c6da1100               call       0x5429ee
00424f28 8be5                     mov        esp, ebp
00424f2a 5d                       pop        ebp
00424f2b c3                       ret        
00424f2c cc                       int3       
00424f2d cc                       int3       
00424f2e cc                       int3       
00424f2f cc                       int3       
