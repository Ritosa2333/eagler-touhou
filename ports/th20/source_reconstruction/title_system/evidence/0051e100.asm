0051e100 55                       push       ebp
0051e101 8bec                     mov        ebp, esp
0051e103 83ec0c                   sub        esp, 0xc
0051e106 894df4                   mov        dword ptr [ebp - 0xc], ecx
0051e109 f30f2a4508               cvtsi2ss   xmm0, dword ptr [ebp + 8]
0051e10e f30f1145f8               movss      dword ptr [ebp - 8], xmm0
0051e113 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
0051e118 f30f594510               mulss      xmm0, dword ptr [ebp + 0x10]
0051e11d f30f5e05ccc85600         divss      xmm0, dword ptr [0x56c8cc]
0051e125 f30f59450c               mulss      xmm0, dword ptr [ebp + 0xc]
0051e12a f30f104df8               movss      xmm1, dword ptr [ebp - 8]
0051e12f f30f5cc8                 subss      xmm1, xmm0
0051e133 f30f114df8               movss      dword ptr [ebp - 8], xmm1
0051e138 f30f2c45f8               cvttss2si  eax, dword ptr [ebp - 8]
0051e13d 8945fc                   mov        dword ptr [ebp - 4], eax
0051e140 837dfc00                 cmp        dword ptr [ebp - 4], 0
0051e144 7d09                     jge        0x51e14f
0051e146 c745fc00000000           mov        dword ptr [ebp - 4], 0
0051e14d eb10                     jmp        0x51e15f
0051e14f 817dfcff000000           cmp        dword ptr [ebp - 4], 0xff
0051e156 7e07                     jle        0x51e15f
0051e158 c745fcff000000           mov        dword ptr [ebp - 4], 0xff
0051e15f 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
0051e163 8be5                     mov        esp, ebp
0051e165 5d                       pop        ebp
0051e166 c20c00                   ret        0xc
0051e169 cc                       int3       
0051e16a cc                       int3       
0051e16b cc                       int3       
0051e16c cc                       int3       
0051e16d cc                       int3       
0051e16e cc                       int3       
0051e16f cc                       int3       
