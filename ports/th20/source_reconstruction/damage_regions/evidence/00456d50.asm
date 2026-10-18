00456d50 55                       push       ebp
00456d51 8bec                     mov        ebp, esp
00456d53 83ec60                   sub        esp, 0x60
00456d56 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00456d5b 33c5                     xor        eax, ebp
00456d5d 8945fc                   mov        dword ptr [ebp - 4], eax
00456d60 68102e4200               push       0x422e10
00456d65 6a02                     push       2
00456d67 6a0c                     push       0xc
00456d69 8d45e4                   lea        eax, [ebp - 0x1c]
00456d6c 50                       push       eax
00456d6d e8ae4efbff               call       0x40bc20
00456d72 51                       push       ecx
00456d73 0f57c0                   xorps      xmm0, xmm0
00456d76 f30f110424               movss      dword ptr [esp], xmm0
00456d7b 51                       push       ecx
00456d7c 0f57c0                   xorps      xmm0, xmm0
00456d7f f30f110424               movss      dword ptr [esp], xmm0
00456d84 51                       push       ecx
00456d85 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
00456d8a f30f110424               movss      dword ptr [esp], xmm0
00456d8f 8d4dd8                   lea        ecx, [ebp - 0x28]
00456d92 e839c0fcff               call       0x422dd0
00456d97 51                       push       ecx
00456d98 0f57c0                   xorps      xmm0, xmm0
00456d9b f30f110424               movss      dword ptr [esp], xmm0
00456da0 51                       push       ecx
00456da1 0f57c0                   xorps      xmm0, xmm0
00456da4 f30f110424               movss      dword ptr [esp], xmm0
00456da9 51                       push       ecx
00456daa f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00456daf f30f110424               movss      dword ptr [esp], xmm0
00456db4 8d4dcc                   lea        ecx, [ebp - 0x34]
00456db7 e814c0fcff               call       0x422dd0
00456dbc 90                       nop        
00456dbd c745c800000000           mov        dword ptr [ebp - 0x38], 0
00456dc4 eb09                     jmp        0x456dcf
00456dc6 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00456dc9 83c101                   add        ecx, 1
00456dcc 894dc8                   mov        dword ptr [ebp - 0x38], ecx
00456dcf 8b552c                   mov        edx, dword ptr [ebp + 0x2c]
00456dd2 d1e2                     shl        edx, 1
00456dd4 3955c8                   cmp        dword ptr [ebp - 0x38], edx
00456dd7 0f8dee010000             jge        0x456fcb
00456ddd f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
00456de2 f30f1145bc               movss      dword ptr [ebp - 0x44], xmm0
00456de7 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
00456dea 2501000080               and        eax, 0x80000001
00456def 7905                     jns        0x456df6
00456df1 48                       dec        eax
00456df2 83c8fe                   or         eax, 0xfffffffe
00456df5 40                       inc        eax
00456df6 85c0                     test       eax, eax
00456df8 7508                     jne        0x456e02
00456dfa 8d4dd8                   lea        ecx, [ebp - 0x28]
00456dfd 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
00456e00 eb06                     jmp        0x456e08
00456e02 8d55cc                   lea        edx, [ebp - 0x34]
00456e05 8955c4                   mov        dword ptr [ebp - 0x3c], edx
00456e08 b80c000000               mov        eax, 0xc
00456e0d 6bc800                   imul       ecx, eax, 0
00456e10 8d540de4                 lea        edx, [ebp + ecx - 0x1c]
00456e14 51                       push       ecx
00456e15 f30f1045bc               movss      xmm0, dword ptr [ebp - 0x44]
00456e1a f30f110424               movss      dword ptr [esp], xmm0
00456e1f 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
00456e22 50                       push       eax
00456e23 52                       push       edx
00456e24 e877210000               call       0x458fa0
00456e29 83c40c                   add        esp, 0xc
00456e2c f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00456e34 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
00456e3c f30f2a4d2c               cvtsi2ss   xmm1, dword ptr [ebp + 0x2c]
00456e41 f30f5ec1                 divss      xmm0, xmm1
00456e45 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00456e4d f30f584528               addss      xmm0, dword ptr [ebp + 0x28]
00456e52 f30f114528               movss      dword ptr [ebp + 0x28], xmm0
00456e57 51                       push       ecx
00456e58 f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
00456e5d f30f110424               movss      dword ptr [esp], xmm0
00456e62 e8d916feff               call       0x438540
00456e67 83c404                   add        esp, 4
00456e6a d95d28                   fstp       dword ptr [ebp + 0x28]
00456e6d f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
00456e72 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
00456e77 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00456e7a 81e101000080             and        ecx, 0x80000001
00456e80 7905                     jns        0x456e87
00456e82 49                       dec        ecx
00456e83 83c9fe                   or         ecx, 0xfffffffe
00456e86 41                       inc        ecx
00456e87 85c9                     test       ecx, ecx
00456e89 7408                     je         0x456e93
00456e8b 8d55d8                   lea        edx, [ebp - 0x28]
00456e8e 8955c0                   mov        dword ptr [ebp - 0x40], edx
00456e91 eb06                     jmp        0x456e99
00456e93 8d45cc                   lea        eax, [ebp - 0x34]
00456e96 8945c0                   mov        dword ptr [ebp - 0x40], eax
00456e99 b90c000000               mov        ecx, 0xc
00456e9e c1e100                   shl        ecx, 0
00456ea1 8d540de4                 lea        edx, [ebp + ecx - 0x1c]
00456ea5 51                       push       ecx
00456ea6 f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
00456eab f30f110424               movss      dword ptr [esp], xmm0
00456eb0 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
00456eb3 50                       push       eax
00456eb4 52                       push       edx
00456eb5 e8e6200000               call       0x458fa0
00456eba 83c40c                   add        esp, 0xc
00456ebd 51                       push       ecx
00456ebe 0f57c0                   xorps      xmm0, xmm0
00456ec1 f30f110424               movss      dword ptr [esp], xmm0
00456ec6 51                       push       ecx
00456ec7 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00456ecc f30f110424               movss      dword ptr [esp], xmm0
00456ed1 51                       push       ecx
00456ed2 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00456ed7 f30f110424               movss      dword ptr [esp], xmm0
00456edc 8d4dac                   lea        ecx, [ebp - 0x54]
00456edf e8ecbefcff               call       0x422dd0
00456ee4 8d4dac                   lea        ecx, [ebp - 0x54]
00456ee7 51                       push       ecx
00456ee8 ba0c000000               mov        edx, 0xc
00456eed 6bc200                   imul       eax, edx, 0
00456ef0 8d4c05e4                 lea        ecx, [ebp + eax - 0x1c]
00456ef4 e8e727fdff               call       0x4296e0
00456ef9 90                       nop        
00456efa 51                       push       ecx
00456efb 0f57c0                   xorps      xmm0, xmm0
00456efe f30f110424               movss      dword ptr [esp], xmm0
00456f03 51                       push       ecx
00456f04 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00456f09 f30f110424               movss      dword ptr [esp], xmm0
00456f0e 51                       push       ecx
00456f0f f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00456f14 f30f110424               movss      dword ptr [esp], xmm0
00456f19 8d4da0                   lea        ecx, [ebp - 0x60]
00456f1c e8afbefcff               call       0x422dd0
00456f21 8d4da0                   lea        ecx, [ebp - 0x60]
00456f24 51                       push       ecx
00456f25 ba0c000000               mov        edx, 0xc
00456f2a c1e200                   shl        edx, 0
00456f2d 8d4c15e4                 lea        ecx, [ebp + edx - 0x1c]
00456f31 e8aa27fdff               call       0x4296e0
00456f36 90                       nop        
00456f37 51                       push       ecx
00456f38 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00456f3d f30f110424               movss      dword ptr [esp], xmm0
00456f42 51                       push       ecx
00456f43 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00456f48 f30f110424               movss      dword ptr [esp], xmm0
00456f4d 51                       push       ecx
00456f4e f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00456f53 f30f110424               movss      dword ptr [esp], xmm0
00456f58 51                       push       ecx
00456f59 f30f104508               movss      xmm0, dword ptr [ebp + 8]
00456f5e f30f110424               movss      dword ptr [esp], xmm0
00456f63 b80c000000               mov        eax, 0xc
00456f68 c1e000                   shl        eax, 0
00456f6b 51                       push       ecx
00456f6c f30f104405e8             movss      xmm0, dword ptr [ebp + eax - 0x18]
00456f72 f30f110424               movss      dword ptr [esp], xmm0
00456f77 b90c000000               mov        ecx, 0xc
00456f7c c1e100                   shl        ecx, 0
00456f7f 51                       push       ecx
00456f80 f30f10440de4             movss      xmm0, dword ptr [ebp + ecx - 0x1c]
00456f86 f30f110424               movss      dword ptr [esp], xmm0
00456f8b ba0c000000               mov        edx, 0xc
00456f90 6bc200                   imul       eax, edx, 0
00456f93 51                       push       ecx
00456f94 f30f104405e8             movss      xmm0, dword ptr [ebp + eax - 0x18]
00456f9a f30f110424               movss      dword ptr [esp], xmm0
00456f9f b90c000000               mov        ecx, 0xc
00456fa4 6bd100                   imul       edx, ecx, 0
00456fa7 51                       push       ecx
00456fa8 f30f104415e4             movss      xmm0, dword ptr [ebp + edx - 0x1c]
00456fae f30f110424               movss      dword ptr [esp], xmm0
00456fb3 e868f9ffff               call       0x456920
00456fb8 83c420                   add        esp, 0x20
00456fbb 0fb6c0                   movzx      eax, al
00456fbe 85c0                     test       eax, eax
00456fc0 7404                     je         0x456fc6
00456fc2 b001                     mov        al, 1
00456fc4 eb07                     jmp        0x456fcd
00456fc6 e9fbfdffff               jmp        0x456dc6
00456fcb 32c0                     xor        al, al
00456fcd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00456fd0 33cd                     xor        ecx, ebp
00456fd2 e817ba0e00               call       0x5429ee
00456fd7 8be5                     mov        esp, ebp
00456fd9 5d                       pop        ebp
00456fda c3                       ret        
00456fdb cc                       int3       
00456fdc cc                       int3       
00456fdd cc                       int3       
00456fde cc                       int3       
00456fdf cc                       int3       
