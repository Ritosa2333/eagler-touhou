004d46e0 55                       push       ebp
004d46e1 8bec                     mov        ebp, esp
004d46e3 83ec2c                   sub        esp, 0x2c
004d46e6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004d46eb 33c5                     xor        eax, ebp
004d46ed 8945fc                   mov        dword ptr [ebp - 4], eax
004d46f0 894de0                   mov        dword ptr [ebp - 0x20], ecx
004d46f3 8d4de4                   lea        ecx, [ebp - 0x1c]
004d46f6 e815e7f4ff               call       0x422e10
004d46fb 8d4df0                   lea        ecx, [ebp - 0x10]
004d46fe e80de7f4ff               call       0x422e10
004d4703 90                       nop        
004d4704 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004d4707 83c058                   add        eax, 0x58
004d470a 50                       push       eax
004d470b 8d4dd4                   lea        ecx, [ebp - 0x2c]
004d470e 51                       push       ecx
004d470f 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004d4712 e8294df5ff               call       0x429440
004d4717 8b10                     mov        edx, dword ptr [eax]
004d4719 8955e4                   mov        dword ptr [ebp - 0x1c], edx
004d471c 8b4804                   mov        ecx, dword ptr [eax + 4]
004d471f 894de8                   mov        dword ptr [ebp - 0x18], ecx
004d4722 8b5008                   mov        edx, dword ptr [eax + 8]
004d4725 8955ec                   mov        dword ptr [ebp - 0x14], edx
004d4728 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004d472b f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004d4730 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
004d4737 51                       push       ecx
004d4738 f30f110424               movss      dword ptr [esp], xmm0
004d473d 8d4de4                   lea        ecx, [ebp - 0x1c]
004d4740 51                       push       ecx
004d4741 8d55f0                   lea        edx, [ebp - 0x10]
004d4744 52                       push       edx
004d4745 e85648f8ff               call       0x458fa0
004d474a 83c40c                   add        esp, 0xc
004d474d f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004d4752 f30f5c450c               subss      xmm0, dword ptr [ebp + 0xc]
004d4757 f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
004d475c f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
004d4761 f30f5c450c               subss      xmm0, dword ptr [ebp + 0xc]
004d4766 f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
004d476b f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004d4770 f30f58450c               addss      xmm0, dword ptr [ebp + 0xc]
004d4775 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004d477a f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
004d477f f30f58450c               addss      xmm0, dword ptr [ebp + 0xc]
004d4784 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004d4789 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004d478c f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
004d4791 0f2f4074                 comiss     xmm0, dword ptr [eax + 0x74]
004d4795 7740                     ja         0x4d47d7
004d4797 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004d479a f30f104178               movss      xmm0, dword ptr [ecx + 0x78]
004d479f f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004d47a7 f30f104de8               movss      xmm1, dword ptr [ebp - 0x18]
004d47ac 0f2fc8                   comiss     xmm1, xmm0
004d47af 7726                     ja         0x4d47d7
004d47b1 0f57c0                   xorps      xmm0, xmm0
004d47b4 0f2f45f0                 comiss     xmm0, dword ptr [ebp - 0x10]
004d47b8 771d                     ja         0x4d47d7
004d47ba 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004d47bd f30f104278               movss      xmm0, dword ptr [edx + 0x78]
004d47c2 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
004d47c9 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004d47d1 0f2f45f4                 comiss     xmm0, dword ptr [ebp - 0xc]
004d47d5 7604                     jbe        0x4d47db
004d47d7 33c0                     xor        eax, eax
004d47d9 eb05                     jmp        0x4d47e0
004d47db b802000000               mov        eax, 2
004d47e0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d47e3 33cd                     xor        ecx, ebp
004d47e5 e804e20600               call       0x5429ee
004d47ea 8be5                     mov        esp, ebp
004d47ec 5d                       pop        ebp
004d47ed c20800                   ret        8
