0047c8f0 55                       push       ebp
0047c8f1 8bec                     mov        ebp, esp
0047c8f3 81ec98000000             sub        esp, 0x98
0047c8f9 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0047c8fe 33c5                     xor        eax, ebp
0047c900 8945fc                   mov        dword ptr [ebp - 4], eax
0047c903 894dc0                   mov        dword ptr [ebp - 0x40], ecx
0047c906 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
0047c909 8b881c050000             mov        ecx, dword ptr [eax + 0x51c]
0047c90f 894d8c                   mov        dword ptr [ebp - 0x74], ecx
0047c912 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
0047c915 8b821c050000             mov        eax, dword ptr [edx + 0x51c]
0047c91b 50                       push       eax
0047c91c 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0047c922 e879adffff               call       0x4776a0
0047c927 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047c92a 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
0047c92d 8955bc                   mov        dword ptr [ebp - 0x44], edx
0047c930 6a01                     push       1
0047c932 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
0047c935 e816abffff               call       0x477450
0047c93a 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047c93d 83c15c                   add        ecx, 0x5c
0047c940 e8fbc6ffff               call       0x479040
0047c945 90                       nop        
0047c946 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
0047c949 8b4814                   mov        ecx, dword ptr [eax + 0x14]
0047c94c c1e909                   shr        ecx, 9
0047c94f 83e101                   and        ecx, 1
0047c952 0f8568010000             jne        0x47cac0
0047c958 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
0047c95b 837a3400                 cmp        dword ptr [edx + 0x34], 0
0047c95f 0f8c2d010000             jl         0x47ca92
0047c965 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
0047c968 8b8820050000             mov        ecx, dword ptr [eax + 0x520]
0047c96e e88df9f8ff               call       0x40c300
0047c973 8945a4                   mov        dword ptr [ebp - 0x5c], eax
0047c976 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047c979 8b8920050000             mov        ecx, dword ptr [ecx + 0x520]
0047c97f e87cf9f8ff               call       0x40c300
0047c984 8bc8                     mov        ecx, eax
0047c986 e825900000               call       0x4859b0
0047c98b 8945b4                   mov        dword ptr [ebp - 0x4c], eax
0047c98e 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
0047c991 8b4234                   mov        eax, dword ptr [edx + 0x34]
0047c994 8945b8                   mov        dword ptr [ebp - 0x48], eax
0047c997 6a00                     push       0
0047c999 6aff                     push       -1
0047c99b 51                       push       ecx
0047c99c 0f57c0                   xorps      xmm0, xmm0
0047c99f f30f110424               movss      dword ptr [esp], xmm0
0047c9a4 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047c9a7 83c164                   add        ecx, 0x64
0047c9aa 51                       push       ecx
0047c9ab 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
0047c9ae 52                       push       edx
0047c9af 6814fd5600               push       0x56fd14
0047c9b4 8d4588                   lea        eax, [ebp - 0x78]
0047c9b7 50                       push       eax
0047c9b8 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
0047c9bb e820c7ffff               call       0x4790e0
0047c9c0 8b08                     mov        ecx, dword ptr [eax]
0047c9c2 894db0                   mov        dword ptr [ebp - 0x50], ecx
0047c9c5 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
0047c9c8 8955ac                   mov        dword ptr [ebp - 0x54], edx
0047c9cb 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
0047c9ce 8b4828                   mov        ecx, dword ptr [eax + 0x28]
0047c9d1 894da8                   mov        dword ptr [ebp - 0x58], ecx
0047c9d4 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
0047c9d7 52                       push       edx
0047c9d8 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0047c9db 50                       push       eax
0047c9dc 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
0047c9df e8ac990000               call       0x486390
0047c9e4 90                       nop        
0047c9e5 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047c9e8 8b5114                   mov        edx, dword ptr [ecx + 0x14]
0047c9eb c1ea0b                   shr        edx, 0xb
0047c9ee 83e203                   and        edx, 3
0047c9f1 83fa01                   cmp        edx, 1
0047c9f4 7532                     jne        0x47ca28
0047c9f6 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
0047c9f9 8b8820050000             mov        ecx, dword ptr [eax + 0x520]
0047c9ff e8fcf8f8ff               call       0x40c300
0047ca04 89459c                   mov        dword ptr [ebp - 0x64], eax
0047ca07 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047ca0a 8b5128                   mov        edx, dword ptr [ecx + 0x28]
0047ca0d 8955a0                   mov        dword ptr [ebp - 0x60], edx
0047ca10 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
0047ca13 50                       push       eax
0047ca14 8d4d84                   lea        ecx, [ebp - 0x7c]
0047ca17 51                       push       ecx
0047ca18 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
0047ca1b e8708c0000               call       0x485690
0047ca20 8bc8                     mov        ecx, eax
0047ca22 e829990000               call       0x486350
0047ca27 90                       nop        
0047ca28 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
0047ca2b 8b4214                   mov        eax, dword ptr [edx + 0x14]
0047ca2e c1e80b                   shr        eax, 0xb
0047ca31 83e003                   and        eax, 3
0047ca34 83f802                   cmp        eax, 2
0047ca37 7559                     jne        0x47ca92
0047ca39 8d4dc4                   lea        ecx, [ebp - 0x3c]
0047ca3c e8efefffff               call       0x47ba30
0047ca41 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047ca44 83c164                   add        ecx, 0x64
0047ca47 8b11                     mov        edx, dword ptr [ecx]
0047ca49 8955c4                   mov        dword ptr [ebp - 0x3c], edx
0047ca4c 8b4104                   mov        eax, dword ptr [ecx + 4]
0047ca4f 8945c8                   mov        dword ptr [ebp - 0x38], eax
0047ca52 8b4908                   mov        ecx, dword ptr [ecx + 8]
0047ca55 894dcc                   mov        dword ptr [ebp - 0x34], ecx
0047ca58 0f57c0                   xorps      xmm0, xmm0
0047ca5b f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
0047ca60 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
0047ca63 8b8288000000             mov        eax, dword ptr [edx + 0x88]
0047ca69 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0047ca6c 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047ca6f 8b8920050000             mov        ecx, dword ptr [ecx + 0x520]
0047ca75 e85600faff               call       0x41cad0
0047ca7a 894598                   mov        dword ptr [ebp - 0x68], eax
0047ca7d 6a00                     push       0
0047ca7f 8d55c4                   lea        edx, [ebp - 0x3c]
0047ca82 52                       push       edx
0047ca83 6a04                     push       4
0047ca85 8d4580                   lea        eax, [ebp - 0x80]
0047ca88 50                       push       eax
0047ca89 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
0047ca8c e8df100200               call       0x49db70
0047ca91 90                       nop        
0047ca92 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047ca95 51                       push       ecx
0047ca96 f30f104164               movss      xmm0, dword ptr [ecx + 0x64]
0047ca9b f30f110424               movss      dword ptr [esp], xmm0
0047caa0 6a47                     push       0x47
0047caa2 b930a85b00               mov        ecx, 0x5ba830
0047caa7 e804a4faff               call       0x426eb0
0047caac 8b5508                   mov        edx, dword ptr [ebp + 8]
0047caaf 52                       push       edx
0047cab0 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
0047cab3 83c064                   add        eax, 0x64
0047cab6 50                       push       eax
0047cab7 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047caba e811090000               call       0x47d3d0
0047cabf 90                       nop        
0047cac0 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047cac3 83c170                   add        ecx, 0x70
0047cac6 894d94                   mov        dword ptr [ebp - 0x6c], ecx
0047cac9 b9e4ef5a00               mov        ecx, 0x5aefe4
0047cace e80dc8faff               call       0x4292e0
0047cad3 51                       push       ecx
0047cad4 d91c24                   fstp       dword ptr [esp]
0047cad7 8d9574ffffff             lea        edx, [ebp - 0x8c]
0047cadd 52                       push       edx
0047cade 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
0047cae1 e80ac8faff               call       0x4292f0
0047cae6 894590                   mov        dword ptr [ebp - 0x70], eax
0047cae9 51                       push       ecx
0047caea f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
0047caf2 f30f110424               movss      dword ptr [esp], xmm0
0047caf7 8d8568ffffff             lea        eax, [ebp - 0x98]
0047cafd 50                       push       eax
0047cafe 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
0047cb01 e8ea64fdff               call       0x452ff0
0047cb06 50                       push       eax
0047cb07 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047cb0a 83c164                   add        ecx, 0x64
0047cb0d e8cecbfaff               call       0x4296e0
0047cb12 90                       nop        
0047cb13 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047cb16 c7415004000000           mov        dword ptr [ecx + 0x50], 4
0047cb1d 51                       push       ecx
0047cb1e f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
0047cb26 f30f110424               movss      dword ptr [esp], xmm0
0047cb2b 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047cb2e 81c1d8040000             add        ecx, 0x4d8
0047cb34 e8b7faffff               call       0x47c5f0
0047cb39 51                       push       ecx
0047cb3a f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
0047cb42 f30f110424               movss      dword ptr [esp], xmm0
0047cb47 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047cb4a 81c1e8040000             add        ecx, 0x4e8
0047cb50 e89bfaffff               call       0x47c5f0
0047cb55 6a00                     push       0
0047cb57 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047cb5a 81c1f8040000             add        ecx, 0x4f8
0047cb60 e8bb69faff               call       0x423520
0047cb65 8b558c                   mov        edx, dword ptr [ebp - 0x74]
0047cb68 52                       push       edx
0047cb69 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0047cb6f e82cabffff               call       0x4776a0
0047cb74 33c0                     xor        eax, eax
0047cb76 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047cb79 33cd                     xor        ecx, ebp
0047cb7b e86e5e0c00               call       0x5429ee
0047cb80 8be5                     mov        esp, ebp
0047cb82 5d                       pop        ebp
0047cb83 c20400                   ret        4
0047cb86 cc                       int3       
0047cb87 cc                       int3       
0047cb88 cc                       int3       
0047cb89 cc                       int3       
0047cb8a cc                       int3       
0047cb8b cc                       int3       
0047cb8c cc                       int3       
0047cb8d cc                       int3       
0047cb8e cc                       int3       
0047cb8f cc                       int3       
