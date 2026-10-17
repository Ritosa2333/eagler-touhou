0047cf60 55                       push       ebp
0047cf61 8bec                     mov        ebp, esp
0047cf63 6aff                     push       -1
0047cf65 68fd855600               push       0x5685fd
0047cf6a 64a100000000             mov        eax, dword ptr fs:[0]
0047cf70 50                       push       eax
0047cf71 83ec28                   sub        esp, 0x28
0047cf74 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0047cf79 33c5                     xor        eax, ebp
0047cf7b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0047cf7e 50                       push       eax
0047cf7f 8d45f4                   lea        eax, [ebp - 0xc]
0047cf82 64a300000000             mov        dword ptr fs:[0], eax
0047cf88 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0047cf8b c745e000000000           mov        dword ptr [ebp - 0x20], 0
0047cf92 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0047cf95 e8d6890000               call       0x485970
0047cf9a 8945d8                   mov        dword ptr [ebp - 0x28], eax
0047cf9d 6a08                     push       8
0047cf9f 8d4de8                   lea        ecx, [ebp - 0x18]
0047cfa2 e8d9f0f8ff               call       0x40c080
0047cfa7 8d45e8                   lea        eax, [ebp - 0x18]
0047cfaa 50                       push       eax
0047cfab 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0047cfae e8cd52f9ff               call       0x412280
0047cfb3 c745fc00000000           mov        dword ptr [ebp - 4], 0
0047cfba 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0047cfbd e87e55f9ff               call       0x412540
0047cfc2 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0047cfc5 eb09                     jmp        0x47cfd0
0047cfc7 8d4de8                   lea        ecx, [ebp - 0x18]
0047cfca e8614cf9ff               call       0x411c30
0047cfcf 90                       nop        
0047cfd0 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0047cfd3 51                       push       ecx
0047cfd4 8d4de8                   lea        ecx, [ebp - 0x18]
0047cfd7 e8d44bf9ff               call       0x411bb0
0047cfdc 0fb6d0                   movzx      edx, al
0047cfdf 85d2                     test       edx, edx
0047cfe1 0f84dc000000             je         0x47d0c3
0047cfe7 8d4de8                   lea        ecx, [ebp - 0x18]
0047cfea e811f3f8ff               call       0x40c300
0047cfef 8945d0                   mov        dword ptr [ebp - 0x30], eax
0047cff2 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0047cff5 e806f3f8ff               call       0x40c300
0047cffa 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0047cffd 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0047d000 83785002                 cmp        dword ptr [eax + 0x50], 2
0047d004 740b                     je         0x47d011
0047d006 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0047d009 83795001                 cmp        dword ptr [ecx + 0x50], 1
0047d00d 7402                     je         0x47d011
0047d00f ebb6                     jmp        0x47cfc7
0047d011 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0047d014 837a1800                 cmp        dword ptr [edx + 0x18], 0
0047d018 7402                     je         0x47d01c
0047d01a ebab                     jmp        0x47cfc7
0047d01c 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0047d01f f30f10407c               movss      xmm0, dword ptr [eax + 0x7c]
0047d024 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0047d02c f30f58450c               addss      xmm0, dword ptr [ebp + 0xc]
0047d031 51                       push       ecx
0047d032 f30f110424               movss      dword ptr [esp], xmm0
0047d037 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047d03a 51                       push       ecx
0047d03b 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0047d03e 83c264                   add        edx, 0x64
0047d041 52                       push       edx
0047d042 e8198a0000               call       0x485a60
0047d047 83c40c                   add        esp, 0xc
0047d04a 85c0                     test       eax, eax
0047d04c 7470                     je         0x47d0be
0047d04e 8b4518                   mov        eax, dword ptr [ebp + 0x18]
0047d051 83e003                   and        eax, 3
0047d054 c1e00b                   shl        eax, 0xb
0047d057 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0047d05a 8b5114                   mov        edx, dword ptr [ecx + 0x14]
0047d05d 81e2ffe7ffff             and        edx, 0xffffe7ff
0047d063 0bd0                     or         edx, eax
0047d065 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0047d068 895014                   mov        dword ptr [eax + 0x14], edx
0047d06b 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
0047d06e 51                       push       ecx
0047d06f 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0047d072 e879f8ffff               call       0x47c8f0
0047d077 8b5514                   mov        edx, dword ptr [ebp + 0x14]
0047d07a 83ea01                   sub        edx, 1
0047d07d 895514                   mov        dword ptr [ebp + 0x14], edx
0047d080 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0047d083 8b88886d2800             mov        ecx, dword ptr [eax + 0x286d88]
0047d089 83c101                   add        ecx, 1
0047d08c 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0047d08f 898a886d2800             mov        dword ptr [edx + 0x286d88], ecx
0047d095 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0047d098 83c001                   add        eax, 1
0047d09b 8945e0                   mov        dword ptr [ebp - 0x20], eax
0047d09e 837d1400                 cmp        dword ptr [ebp + 0x14], 0
0047d0a2 7f1a                     jg         0x47d0be
0047d0a4 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d0a7 894dcc                   mov        dword ptr [ebp - 0x34], ecx
0047d0aa c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0047d0b1 8d4de8                   lea        ecx, [ebp - 0x18]
0047d0b4 e8474af9ff               call       0x411b00
0047d0b9 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
0047d0bc eb18                     jmp        0x47d0d6
0047d0be e904ffffff               jmp        0x47cfc7
0047d0c3 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0047d0ca 8d4de8                   lea        ecx, [ebp - 0x18]
0047d0cd e82e4af9ff               call       0x411b00
0047d0d2 90                       nop        
0047d0d3 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0047d0d6 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0047d0d9 64890d00000000           mov        dword ptr fs:[0], ecx
0047d0e0 59                       pop        ecx
0047d0e1 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047d0e4 33cd                     xor        ecx, ebp
0047d0e6 e803590c00               call       0x5429ee
0047d0eb 8be5                     mov        esp, ebp
0047d0ed 5d                       pop        ebp
0047d0ee c21400                   ret        0x14
0047d0f1 cc                       int3       
0047d0f2 cc                       int3       
0047d0f3 cc                       int3       
0047d0f4 cc                       int3       
0047d0f5 cc                       int3       
0047d0f6 cc                       int3       
0047d0f7 cc                       int3       
0047d0f8 cc                       int3       
0047d0f9 cc                       int3       
0047d0fa cc                       int3       
0047d0fb cc                       int3       
0047d0fc cc                       int3       
0047d0fd cc                       int3       
0047d0fe cc                       int3       
0047d0ff cc                       int3       
