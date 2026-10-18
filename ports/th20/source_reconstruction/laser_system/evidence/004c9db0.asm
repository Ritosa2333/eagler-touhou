004c9db0 55                       push       ebp
004c9db1 8bec                     mov        ebp, esp
004c9db3 6aff                     push       -1
004c9db5 68fd855600               push       0x5685fd
004c9dba 64a100000000             mov        eax, dword ptr fs:[0]
004c9dc0 50                       push       eax
004c9dc1 83ec28                   sub        esp, 0x28
004c9dc4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c9dc9 33c5                     xor        eax, ebp
004c9dcb 8945f0                   mov        dword ptr [ebp - 0x10], eax
004c9dce 50                       push       eax
004c9dcf 8d45f4                   lea        eax, [ebp - 0xc]
004c9dd2 64a300000000             mov        dword ptr fs:[0], eax
004c9dd8 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004c9ddb c745dc00000000           mov        dword ptr [ebp - 0x24], 0
004c9de2 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004c9de5 83c030                   add        eax, 0x30
004c9de8 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c9deb 8b11                     mov        edx, dword ptr [ecx]
004c9ded 8910                     mov        dword ptr [eax], edx
004c9def 8b5104                   mov        edx, dword ptr [ecx + 4]
004c9df2 895004                   mov        dword ptr [eax + 4], edx
004c9df5 8b4908                   mov        ecx, dword ptr [ecx + 8]
004c9df8 894808                   mov        dword ptr [eax + 8], ecx
004c9dfb 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004c9dfe 83c23c                   add        edx, 0x3c
004c9e01 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004c9e04 8b08                     mov        ecx, dword ptr [eax]
004c9e06 890a                     mov        dword ptr [edx], ecx
004c9e08 8b4804                   mov        ecx, dword ptr [eax + 4]
004c9e0b 894a04                   mov        dword ptr [edx + 4], ecx
004c9e0e 8b4008                   mov        eax, dword ptr [eax + 8]
004c9e11 894208                   mov        dword ptr [edx + 8], eax
004c9e14 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004c9e17 83c110                   add        ecx, 0x10
004c9e1a 894dd8                   mov        dword ptr [ebp - 0x28], ecx
004c9e1d 6a08                     push       8
004c9e1f 8d4de8                   lea        ecx, [ebp - 0x18]
004c9e22 e85922f4ff               call       0x40c080
004c9e27 8d55e8                   lea        edx, [ebp - 0x18]
004c9e2a 52                       push       edx
004c9e2b 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c9e2e e84d84f4ff               call       0x412280
004c9e33 c745fc00000000           mov        dword ptr [ebp - 4], 0
004c9e3a 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c9e3d e8fe86f4ff               call       0x412540
004c9e42 8945d0                   mov        dword ptr [ebp - 0x30], eax
004c9e45 eb09                     jmp        0x4c9e50
004c9e47 8d4de8                   lea        ecx, [ebp - 0x18]
004c9e4a e8e17df4ff               call       0x411c30
004c9e4f 90                       nop        
004c9e50 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004c9e53 50                       push       eax
004c9e54 8d4de8                   lea        ecx, [ebp - 0x18]
004c9e57 e8547df4ff               call       0x411bb0
004c9e5c 0fb6c8                   movzx      ecx, al
004c9e5f 85c9                     test       ecx, ecx
004c9e61 7471                     je         0x4c9ed4
004c9e63 8d4de8                   lea        ecx, [ebp - 0x18]
004c9e66 e89524f4ff               call       0x40c300
004c9e6b 8945e0                   mov        dword ptr [ebp - 0x20], eax
004c9e6e 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c9e71 e8ea52f4ff               call       0x40f160
004c9e76 8b10                     mov        edx, dword ptr [eax]
004c9e78 837a2001                 cmp        dword ptr [edx + 0x20], 1
004c9e7c 7451                     je         0x4c9ecf
004c9e7e 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c9e81 e8da52f4ff               call       0x40f160
004c9e86 8b00                     mov        eax, dword ptr [eax]
004c9e88 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004c9e8b 83e101                   and        ecx, 1
004c9e8e 743f                     je         0x4c9ecf
004c9e90 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c9e93 e8c852f4ff               call       0x40f160
004c9e98 8b10                     mov        edx, dword ptr [eax]
004c9e9a 8955d4                   mov        dword ptr [ebp - 0x2c], edx
004c9e9d 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
004c9ea0 8b08                     mov        ecx, dword ptr [eax]
004c9ea2 8b512c                   mov        edx, dword ptr [ecx + 0x2c]
004c9ea5 8955cc                   mov        dword ptr [ebp - 0x34], edx
004c9ea8 8b4518                   mov        eax, dword ptr [ebp + 0x18]
004c9eab 50                       push       eax
004c9eac 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
004c9eaf 51                       push       ecx
004c9eb0 51                       push       ecx
004c9eb1 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004c9eb6 f30f110424               movss      dword ptr [esp], xmm0
004c9ebb 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004c9ebe 52                       push       edx
004c9ebf 8b4508                   mov        eax, dword ptr [ebp + 8]
004c9ec2 50                       push       eax
004c9ec3 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004c9ec6 ff55cc                   call       dword ptr [ebp - 0x34]
004c9ec9 0345dc                   add        eax, dword ptr [ebp - 0x24]
004c9ecc 8945dc                   mov        dword ptr [ebp - 0x24], eax
004c9ecf e973ffffff               jmp        0x4c9e47
004c9ed4 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004c9edb 8d4de8                   lea        ecx, [ebp - 0x18]
004c9ede e81d7cf4ff               call       0x411b00
004c9ee3 90                       nop        
004c9ee4 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004c9ee7 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c9eea 64890d00000000           mov        dword ptr fs:[0], ecx
004c9ef1 59                       pop        ecx
004c9ef2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c9ef5 33cd                     xor        ecx, ebp
004c9ef7 e8f28a0700               call       0x5429ee
004c9efc 8be5                     mov        esp, ebp
004c9efe 5d                       pop        ebp
004c9eff c21400                   ret        0x14
004c9f02 cc                       int3       
004c9f03 cc                       int3       
004c9f04 cc                       int3       
004c9f05 cc                       int3       
004c9f06 cc                       int3       
004c9f07 cc                       int3       
004c9f08 cc                       int3       
004c9f09 cc                       int3       
004c9f0a cc                       int3       
004c9f0b cc                       int3       
004c9f0c cc                       int3       
004c9f0d cc                       int3       
004c9f0e cc                       int3       
004c9f0f cc                       int3       
