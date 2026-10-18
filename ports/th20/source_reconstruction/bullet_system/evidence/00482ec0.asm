00482ec0 55                       push       ebp
00482ec1 8bec                     mov        ebp, esp
00482ec3 83ec18                   sub        esp, 0x18
00482ec6 894dfc                   mov        dword ptr [ebp - 4], ecx
00482ec9 8b45fc                   mov        eax, dword ptr [ebp - 4]
00482ecc 8b8820050000             mov        ecx, dword ptr [eax + 0x520]
00482ed2 e82994f8ff               call       0x40c300
00482ed7 8bc8                     mov        ecx, eax
00482ed9 e8b22a0000               call       0x485990
00482ede 0f57c0                   xorps      xmm0, xmm0
00482ee1 0f2f00                   comiss     xmm0, dword ptr [eax]
00482ee4 7222                     jb         0x482f08
00482ee6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482ee9 81c1a0000000             add        ecx, 0xa0
00482eef 894df0                   mov        dword ptr [ebp - 0x10], ecx
00482ef2 6a04                     push       4
00482ef4 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00482ef7 e88497ffff               call       0x47c680
00482efc f30f104018               movss      xmm0, dword ptr [eax + 0x18]
00482f01 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00482f06 eb1e                     jmp        0x482f26
00482f08 8b55fc                   mov        edx, dword ptr [ebp - 4]
00482f0b 8b8a20050000             mov        ecx, dword ptr [edx + 0x520]
00482f11 e8ea93f8ff               call       0x40c300
00482f16 8bc8                     mov        ecx, eax
00482f18 e8732a0000               call       0x485990
00482f1d f30f1000                 movss      xmm0, dword ptr [eax]
00482f21 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00482f26 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00482f2b f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00482f30 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00482f35 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00482f3c f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00482f44 8b45fc                   mov        eax, dword ptr [ebp - 4]
00482f47 0f2f4064                 comiss     xmm0, dword ptr [eax + 0x64]
00482f4b 0f86b2000000             jbe        0x483003
00482f51 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482f54 81c1a0000000             add        ecx, 0xa0
00482f5a 894dec                   mov        dword ptr [ebp - 0x14], ecx
00482f5d 6a04                     push       4
00482f5f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00482f62 e81997ffff               call       0x47c680
00482f67 8b503c                   mov        edx, dword ptr [eax + 0x3c]
00482f6a 83e210                   and        edx, 0x10
00482f6d 0f8589000000             jne        0x482ffc
00482f73 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482f76 81c184000000             add        ecx, 0x84
00482f7c e85f63faff               call       0x4292e0
00482f81 d95de8                   fstp       dword ptr [ebp - 0x18]
00482f84 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00482f89 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00482f90 f30f5c05f0e05600         subss      xmm0, dword ptr [0x56e0f0]
00482f98 51                       push       ecx
00482f99 f30f110424               movss      dword ptr [esp], xmm0
00482f9e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482fa1 81c184000000             add        ecx, 0x84
00482fa7 e874fffcff               call       0x452f20
00482fac 51                       push       ecx
00482fad 0f57c0                   xorps      xmm0, xmm0
00482fb0 f30f110424               movss      dword ptr [esp], xmm0
00482fb5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482fb8 81c184000000             add        ecx, 0x84
00482fbe e81d63faff               call       0x4292e0
00482fc3 51                       push       ecx
00482fc4 d91c24                   fstp       dword ptr [esp]
00482fc7 e83456fbff               call       0x438600
00482fcc 83c404                   add        esp, 4
00482fcf d91c24                   fstp       dword ptr [esp]
00482fd2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482fd5 81c184000000             add        ecx, 0x84
00482fdb e840fffcff               call       0x452f20
00482fe0 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00482fe5 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00482fec 8b45fc                   mov        eax, dword ptr [ebp - 4]
00482fef f30f5c4064               subss      xmm0, dword ptr [eax + 0x64]
00482ff4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482ff7 f30f114164               movss      dword ptr [ecx + 0x64], xmm0
00482ffc b801000000               mov        eax, 1
00483001 eb02                     jmp        0x483005
00483003 33c0                     xor        eax, eax
00483005 8be5                     mov        esp, ebp
00483007 5d                       pop        ebp
00483008 c3                       ret        
00483009 cc                       int3       
0048300a cc                       int3       
0048300b cc                       int3       
0048300c cc                       int3       
0048300d cc                       int3       
0048300e cc                       int3       
0048300f cc                       int3       
