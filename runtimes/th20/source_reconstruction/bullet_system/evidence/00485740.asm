00485740 55                       push       ebp
00485741 8bec                     mov        ebp, esp
00485743 83ec34                   sub        esp, 0x34
00485746 f30f100580fd5600         movss      xmm0, dword ptr [0x56fd80]
0048574e 0f2f4518                 comiss     xmm0, dword ptr [ebp + 0x18]
00485752 720d                     jb         0x485761
00485754 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
0048575c f30f114518               movss      dword ptr [ebp + 0x18], xmm0
00485761 f30f100580fd5600         movss      xmm0, dword ptr [0x56fd80]
00485769 0f2f4514                 comiss     xmm0, dword ptr [ebp + 0x14]
0048576d 720f                     jb         0x48577e
0048576f f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00485774 f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
00485779 e96d010000               jmp        0x4858eb
0048577e f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00485783 0f2f0554fd5600           comiss     xmm0, dword ptr [0x56fd54]
0048578a 7240                     jb         0x4857cc
0048578c f30f100558fd5600         movss      xmm0, dword ptr [0x56fd58]
00485794 0f2f4514                 comiss     xmm0, dword ptr [ebp + 0x14]
00485798 7632                     jbe        0x4857cc
0048579a 8b4508                   mov        eax, dword ptr [ebp + 8]
0048579d 50                       push       eax
0048579e e88db0fdff               call       0x460830
004857a3 83c404                   add        esp, 4
004857a6 8945e8                   mov        dword ptr [ebp - 0x18], eax
004857a9 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
004857ac 51                       push       ecx
004857ad 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004857b0 e89b9b0700               call       0x4ff350
004857b5 d95de4                   fstp       dword ptr [ebp - 0x1c]
004857b8 f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
004857bd f30f584518               addss      xmm0, dword ptr [ebp + 0x18]
004857c2 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004857c7 e915010000               jmp        0x4858e1
004857cc f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004857d1 0f2f055cfd5600           comiss     xmm0, dword ptr [0x56fd5c]
004857d8 7260                     jb         0x48583a
004857da f30f100560fd5600         movss      xmm0, dword ptr [0x56fd60]
004857e2 0f2f4514                 comiss     xmm0, dword ptr [ebp + 0x14]
004857e6 7652                     jbe        0x48583a
004857e8 8b5508                   mov        edx, dword ptr [ebp + 8]
004857eb 52                       push       edx
004857ec e83fb0fdff               call       0x460830
004857f1 83c404                   add        esp, 4
004857f4 8945e0                   mov        dword ptr [ebp - 0x20], eax
004857f7 8b4510                   mov        eax, dword ptr [ebp + 0x10]
004857fa 50                       push       eax
004857fb 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004857fe e84d9b0700               call       0x4ff350
00485803 d95ddc                   fstp       dword ptr [ebp - 0x24]
00485806 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
0048580b b9a8a45b00               mov        ecx, 0x5ba4a8
00485810 f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
00485815 e8c640faff               call       0x4298e0
0048581a d95dd8                   fstp       dword ptr [ebp - 0x28]
0048581d f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
00485822 f30f594518               mulss      xmm0, dword ptr [ebp + 0x18]
00485827 f30f104dd4               movss      xmm1, dword ptr [ebp - 0x2c]
0048582c f30f58c8                 addss      xmm1, xmm0
00485830 f30f114df4               movss      dword ptr [ebp - 0xc], xmm1
00485835 e99d000000               jmp        0x4858d7
0048583a f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
0048583f 0f2f0560fd5600           comiss     xmm0, dword ptr [0x56fd60]
00485846 7231                     jb         0x485879
00485848 f30f100564fd5600         movss      xmm0, dword ptr [0x56fd64]
00485850 0f2f4514                 comiss     xmm0, dword ptr [ebp + 0x14]
00485854 7623                     jbe        0x485879
00485856 b9a8a45b00               mov        ecx, 0x5ba4a8
0048585b e88040faff               call       0x4298e0
00485860 d95dd0                   fstp       dword ptr [ebp - 0x30]
00485863 f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
00485868 f30f594518               mulss      xmm0, dword ptr [ebp + 0x18]
0048586d f30f58450c               addss      xmm0, dword ptr [ebp + 0xc]
00485872 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00485877 eb54                     jmp        0x4858cd
00485879 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
0048587e 0f2f0564fd5600           comiss     xmm0, dword ptr [0x56fd64]
00485885 7232                     jb         0x4858b9
00485887 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0048588a 51                       push       ecx
0048588b e8d027ffff               call       0x478060
00485890 83c404                   add        esp, 4
00485893 8945cc                   mov        dword ptr [ebp - 0x34], eax
00485896 6a00                     push       0
00485898 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0048589b e8c0fdffff               call       0x485660
004858a0 8bc8                     mov        ecx, eax
004858a2 e8194affff               call       0x47a2c0
004858a7 50                       push       eax
004858a8 8b5510                   mov        edx, dword ptr [ebp + 0x10]
004858ab 52                       push       edx
004858ac e82f4affff               call       0x47a2e0
004858b1 83c408                   add        esp, 8
004858b4 d95dfc                   fstp       dword ptr [ebp - 4]
004858b7 eb0a                     jmp        0x4858c3
004858b9 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004858be f30f1145fc               movss      dword ptr [ebp - 4], xmm0
004858c3 f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
004858c8 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
004858cd f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
004858d2 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004858d7 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
004858dc f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004858e1 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004858e6 f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
004858eb d945ec                   fld        dword ptr [ebp - 0x14]
004858ee 8be5                     mov        esp, ebp
004858f0 5d                       pop        ebp
004858f1 c3                       ret        
004858f2 cc                       int3       
004858f3 cc                       int3       
004858f4 cc                       int3       
004858f5 cc                       int3       
004858f6 cc                       int3       
004858f7 cc                       int3       
004858f8 cc                       int3       
004858f9 cc                       int3       
004858fa cc                       int3       
004858fb cc                       int3       
004858fc cc                       int3       
004858fd cc                       int3       
004858fe cc                       int3       
004858ff cc                       int3       
