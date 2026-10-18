004578b0 55                       push       ebp
004578b1 8bec                     mov        ebp, esp
004578b3 83ec4c                   sub        esp, 0x4c
004578b6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004578bb 33c5                     xor        eax, ebp
004578bd 8945fc                   mov        dword ptr [ebp - 4], eax
004578c0 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
004578c5 0f2f4528                 comiss     xmm0, dword ptr [ebp + 0x28]
004578c9 760c                     jbe        0x4578d7
004578cb f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
004578d0 f30f1145c0               movss      dword ptr [ebp - 0x40], xmm0
004578d5 eb0a                     jmp        0x4578e1
004578d7 f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
004578dc f30f1145c0               movss      dword ptr [ebp - 0x40], xmm0
004578e1 f30f1045c0               movss      xmm0, dword ptr [ebp - 0x40]
004578e6 f30f1145b4               movss      dword ptr [ebp - 0x4c], xmm0
004578eb f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004578f0 0f2f4514                 comiss     xmm0, dword ptr [ebp + 0x14]
004578f4 760c                     jbe        0x457902
004578f6 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004578fb f30f1145bc               movss      dword ptr [ebp - 0x44], xmm0
00457900 eb0a                     jmp        0x45790c
00457902 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00457907 f30f1145bc               movss      dword ptr [ebp - 0x44], xmm0
0045790c f30f1045bc               movss      xmm0, dword ptr [ebp - 0x44]
00457911 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
00457916 8d4de4                   lea        ecx, [ebp - 0x1c]
00457919 e8f2b4fcff               call       0x422e10
0045791e 8d4df0                   lea        ecx, [ebp - 0x10]
00457921 e8eab4fcff               call       0x422e10
00457926 f30f104508               movss      xmm0, dword ptr [ebp + 8]
0045792b f30f5c451c               subss      xmm0, dword ptr [ebp + 0x1c]
00457930 f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00457935 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
0045793a f30f5c4520               subss      xmm0, dword ptr [ebp + 0x20]
0045793f f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
00457944 f30f10452c               movss      xmm0, dword ptr [ebp + 0x2c]
00457949 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00457950 51                       push       ecx
00457951 f30f110424               movss      dword ptr [esp], xmm0
00457956 8d45e4                   lea        eax, [ebp - 0x1c]
00457959 50                       push       eax
0045795a 8d4de4                   lea        ecx, [ebp - 0x1c]
0045795d 51                       push       ecx
0045795e e83d160000               call       0x458fa0
00457963 83c408                   add        esp, 8
00457966 0f57c0                   xorps      xmm0, xmm0
00457969 f30f110424               movss      dword ptr [esp], xmm0
0045796e 51                       push       ecx
0045796f f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00457974 f30f110424               movss      dword ptr [esp], xmm0
00457979 51                       push       ecx
0045797a f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
0045797f f30f110424               movss      dword ptr [esp], xmm0
00457984 8d4dd8                   lea        ecx, [ebp - 0x28]
00457987 e844b4fcff               call       0x422dd0
0045798c f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00457991 f30f5c452c               subss      xmm0, dword ptr [ebp + 0x2c]
00457996 51                       push       ecx
00457997 f30f110424               movss      dword ptr [esp], xmm0
0045799c e89f0bfeff               call       0x438540
004579a1 d91c24                   fstp       dword ptr [esp]
004579a4 8d55d8                   lea        edx, [ebp - 0x28]
004579a7 52                       push       edx
004579a8 8d45d8                   lea        eax, [ebp - 0x28]
004579ab 50                       push       eax
004579ac e8ef150000               call       0x458fa0
004579b1 83c40c                   add        esp, 0xc
004579b4 f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
004579b9 0f2f45b4                 comiss     xmm0, dword ptr [ebp - 0x4c]
004579bd 0f86f7000000             jbe        0x457aba
004579c3 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
004579c8 f30f584528               addss      xmm0, dword ptr [ebp + 0x28]
004579cd f30f2cc0                 cvttss2si  eax, xmm0
004579d1 99                       cdq        
004579d2 83e203                   and        edx, 3
004579d5 03c2                     add        eax, edx
004579d7 c1f802                   sar        eax, 2
004579da 8945d0                   mov        dword ptr [ebp - 0x30], eax
004579dd 837dd008                 cmp        dword ptr [ebp - 0x30], 8
004579e1 7d07                     jge        0x4579ea
004579e3 c745d008000000           mov        dword ptr [ebp - 0x30], 8
004579ea c745cc00000000           mov        dword ptr [ebp - 0x34], 0
004579f1 eb09                     jmp        0x4579fc
004579f3 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004579f6 83c101                   add        ecx, 1
004579f9 894dcc                   mov        dword ptr [ebp - 0x34], ecx
004579fc 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
004579ff 3b55d0                   cmp        edx, dword ptr [ebp - 0x30]
00457a02 0f8dad000000             jge        0x457ab5
00457a08 51                       push       ecx
00457a09 f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
00457a0e f30f110424               movss      dword ptr [esp], xmm0
00457a13 51                       push       ecx
00457a14 f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00457a19 f30f110424               movss      dword ptr [esp], xmm0
00457a1e 51                       push       ecx
00457a1f f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00457a24 f30f110424               movss      dword ptr [esp], xmm0
00457a29 8d45f0                   lea        eax, [ebp - 0x10]
00457a2c 50                       push       eax
00457a2d e85e170000               call       0x459190
00457a32 83c410                   add        esp, 0x10
00457a35 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00457a3d f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
00457a45 f30f2a4dd0               cvtsi2ss   xmm1, dword ptr [ebp - 0x30]
00457a4a f30f5ec1                 divss      xmm0, xmm1
00457a4e f30f584518               addss      xmm0, dword ptr [ebp + 0x18]
00457a53 f30f114518               movss      dword ptr [ebp + 0x18], xmm0
00457a58 51                       push       ecx
00457a59 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00457a5e f30f110424               movss      dword ptr [esp], xmm0
00457a63 51                       push       ecx
00457a64 f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
00457a69 f30f110424               movss      dword ptr [esp], xmm0
00457a6e 51                       push       ecx
00457a6f f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00457a74 f30f110424               movss      dword ptr [esp], xmm0
00457a79 51                       push       ecx
00457a7a f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00457a7f f30f110424               movss      dword ptr [esp], xmm0
00457a84 51                       push       ecx
00457a85 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00457a8a f30f110424               movss      dword ptr [esp], xmm0
00457a8f 51                       push       ecx
00457a90 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00457a95 f30f110424               movss      dword ptr [esp], xmm0
00457a9a e861f8ffff               call       0x457300
00457a9f 83c418                   add        esp, 0x18
00457aa2 0fb6c8                   movzx      ecx, al
00457aa5 85c9                     test       ecx, ecx
00457aa7 7407                     je         0x457ab0
00457aa9 b001                     mov        al, 1
00457aab e943010000               jmp        0x457bf3
00457ab0 e93effffff               jmp        0x4579f3
00457ab5 e937010000               jmp        0x457bf1
00457aba f30f1045b8               movss      xmm0, dword ptr [ebp - 0x48]
00457abf f30f5e05bcd75600         divss      xmm0, dword ptr [0x56d7bc]
00457ac7 f30f2cd0                 cvttss2si  edx, xmm0
00457acb 8955d4                   mov        dword ptr [ebp - 0x2c], edx
00457ace 837dd403                 cmp        dword ptr [ebp - 0x2c], 3
00457ad2 7d07                     jge        0x457adb
00457ad4 c745d403000000           mov        dword ptr [ebp - 0x2c], 3
00457adb f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00457ae0 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00457ae7 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00457aef f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00457af4 c745c800000000           mov        dword ptr [ebp - 0x38], 0
00457afb eb09                     jmp        0x457b06
00457afd 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
00457b00 83c001                   add        eax, 1
00457b03 8945c8                   mov        dword ptr [ebp - 0x38], eax
00457b06 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00457b09 3b4dd4                   cmp        ecx, dword ptr [ebp - 0x2c]
00457b0c 0f8ddf000000             jge        0x457bf1
00457b12 f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
00457b17 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00457b1e f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00457b26 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00457b2b c745c400000000           mov        dword ptr [ebp - 0x3c], 0
00457b32 eb09                     jmp        0x457b3d
00457b34 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
00457b37 83c201                   add        edx, 1
00457b3a 8955c4                   mov        dword ptr [ebp - 0x3c], edx
00457b3d 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
00457b40 3b45d4                   cmp        eax, dword ptr [ebp - 0x2c]
00457b43 0f8d86000000             jge        0x457bcf
00457b49 51                       push       ecx
00457b4a 0f57c0                   xorps      xmm0, xmm0
00457b4d f30f110424               movss      dword ptr [esp], xmm0
00457b52 51                       push       ecx
00457b53 f30f104528               movss      xmm0, dword ptr [ebp + 0x28]
00457b58 f30f110424               movss      dword ptr [esp], xmm0
00457b5d 51                       push       ecx
00457b5e f30f104524               movss      xmm0, dword ptr [ebp + 0x24]
00457b63 f30f110424               movss      dword ptr [esp], xmm0
00457b68 51                       push       ecx
00457b69 0f57c0                   xorps      xmm0, xmm0
00457b6c f30f110424               movss      dword ptr [esp], xmm0
00457b71 51                       push       ecx
00457b72 0f57c0                   xorps      xmm0, xmm0
00457b75 f30f110424               movss      dword ptr [esp], xmm0
00457b7a f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00457b7f f30f5845e8               addss      xmm0, dword ptr [ebp - 0x18]
00457b84 51                       push       ecx
00457b85 f30f110424               movss      dword ptr [esp], xmm0
00457b8a f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00457b8f f30f5845e4               addss      xmm0, dword ptr [ebp - 0x1c]
00457b94 51                       push       ecx
00457b95 f30f110424               movss      dword ptr [esp], xmm0
00457b9a e8a1f4ffff               call       0x457040
00457b9f 83c41c                   add        esp, 0x1c
00457ba2 0fb6c8                   movzx      ecx, al
00457ba5 85c9                     test       ecx, ecx
00457ba7 7404                     je         0x457bad
00457ba9 b001                     mov        al, 1
00457bab eb46                     jmp        0x457bf3
00457bad 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00457bb0 83ea01                   sub        edx, 1
00457bb3 f30f2ac2                 cvtsi2ss   xmm0, edx
00457bb7 f30f104dd8               movss      xmm1, dword ptr [ebp - 0x28]
00457bbc f30f5ec8                 divss      xmm1, xmm0
00457bc0 f30f584df0               addss      xmm1, dword ptr [ebp - 0x10]
00457bc5 f30f114df0               movss      dword ptr [ebp - 0x10], xmm1
00457bca e965ffffff               jmp        0x457b34
00457bcf 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00457bd2 83e801                   sub        eax, 1
00457bd5 f30f2ac0                 cvtsi2ss   xmm0, eax
00457bd9 f30f104ddc               movss      xmm1, dword ptr [ebp - 0x24]
00457bde f30f5ec8                 divss      xmm1, xmm0
00457be2 f30f584df4               addss      xmm1, dword ptr [ebp - 0xc]
00457be7 f30f114df4               movss      dword ptr [ebp - 0xc], xmm1
00457bec e90cffffff               jmp        0x457afd
00457bf1 32c0                     xor        al, al
00457bf3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00457bf6 33cd                     xor        ecx, ebp
00457bf8 e8f1ad0e00               call       0x5429ee
00457bfd 8be5                     mov        esp, ebp
00457bff 5d                       pop        ebp
00457c00 c3                       ret        
00457c01 cc                       int3       
00457c02 cc                       int3       
00457c03 cc                       int3       
00457c04 cc                       int3       
00457c05 cc                       int3       
00457c06 cc                       int3       
00457c07 cc                       int3       
00457c08 cc                       int3       
00457c09 cc                       int3       
00457c0a cc                       int3       
00457c0b cc                       int3       
00457c0c cc                       int3       
00457c0d cc                       int3       
00457c0e cc                       int3       
00457c0f cc                       int3       
