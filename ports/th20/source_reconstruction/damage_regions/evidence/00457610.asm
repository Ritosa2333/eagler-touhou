00457610 55                       push       ebp
00457611 8bec                     mov        ebp, esp
00457613 83ec20                   sub        esp, 0x20
00457616 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0045761b 33c5                     xor        eax, ebp
0045761d 8945fc                   mov        dword ptr [ebp - 4], eax
00457620 8d4df0                   lea        ecx, [ebp - 0x10]
00457623 e8e8b7fcff               call       0x422e10
00457628 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
0045762d f30f5c4508               subss      xmm0, dword ptr [ebp + 8]
00457632 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00457637 f30f104520               movss      xmm0, dword ptr [ebp + 0x20]
0045763c f30f5c450c               subss      xmm0, dword ptr [ebp + 0xc]
00457641 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00457646 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
0045764b 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00457652 51                       push       ecx
00457653 f30f110424               movss      dword ptr [esp], xmm0
00457658 8d45f0                   lea        eax, [ebp - 0x10]
0045765b 50                       push       eax
0045765c 8d4df0                   lea        ecx, [ebp - 0x10]
0045765f 51                       push       ecx
00457660 e83b190000               call       0x458fa0
00457665 83c408                   add        esp, 8
00457668 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
0045766d f30f110424               movss      dword ptr [esp], xmm0
00457672 e809e0feff               call       0x445680
00457677 83c404                   add        esp, 4
0045767a d95dec                   fstp       dword ptr [ebp - 0x14]
0045767d f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00457682 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0045768a f30f584524               addss      xmm0, dword ptr [ebp + 0x24]
0045768f 0f2f45ec                 comiss     xmm0, dword ptr [ebp - 0x14]
00457693 7230                     jb         0x4576c5
00457695 51                       push       ecx
00457696 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
0045769b f30f110424               movss      dword ptr [esp], xmm0
004576a0 e8dbdffeff               call       0x445680
004576a5 83c404                   add        esp, 4
004576a8 d95de8                   fstp       dword ptr [ebp - 0x18]
004576ab f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004576b0 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004576b8 0f2f45e8                 comiss     xmm0, dword ptr [ebp - 0x18]
004576bc 7207                     jb         0x4576c5
004576be b001                     mov        al, 1
004576c0 e9da010000               jmp        0x45789f
004576c5 51                       push       ecx
004576c6 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004576cb f30f110424               movss      dword ptr [esp], xmm0
004576d0 e8abdffeff               call       0x445680
004576d5 83c404                   add        esp, 4
004576d8 d95de4                   fstp       dword ptr [ebp - 0x1c]
004576db f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004576e0 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004576e8 0f2f45e4                 comiss     xmm0, dword ptr [ebp - 0x1c]
004576ec 7235                     jb         0x457723
004576ee 51                       push       ecx
004576ef f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
004576f4 f30f110424               movss      dword ptr [esp], xmm0
004576f9 e882dffeff               call       0x445680
004576fe 83c404                   add        esp, 4
00457701 d95de0                   fstp       dword ptr [ebp - 0x20]
00457704 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00457709 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00457711 f30f584524               addss      xmm0, dword ptr [ebp + 0x24]
00457716 0f2f45e0                 comiss     xmm0, dword ptr [ebp - 0x20]
0045771a 7207                     jb         0x457723
0045771c b001                     mov        al, 1
0045771e e97c010000               jmp        0x45789f
00457723 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00457728 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00457730 f30f104df0               movss      xmm1, dword ptr [ebp - 0x10]
00457735 f30f5cc8                 subss      xmm1, xmm0
00457739 f30f114d08               movss      dword ptr [ebp + 8], xmm1
0045773e f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00457743 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0045774b f30f104df4               movss      xmm1, dword ptr [ebp - 0xc]
00457750 f30f5cc8                 subss      xmm1, xmm0
00457754 f30f114d0c               movss      dword ptr [ebp + 0xc], xmm1
00457759 f30f104508               movss      xmm0, dword ptr [ebp + 8]
0045775e f30f594508               mulss      xmm0, dword ptr [ebp + 8]
00457763 f30f104d0c               movss      xmm1, dword ptr [ebp + 0xc]
00457768 f30f594d0c               mulss      xmm1, dword ptr [ebp + 0xc]
0045776d f30f58c1                 addss      xmm0, xmm1
00457771 f30f104d24               movss      xmm1, dword ptr [ebp + 0x24]
00457776 f30f594d24               mulss      xmm1, dword ptr [ebp + 0x24]
0045777b 0f2fc8                   comiss     xmm1, xmm0
0045777e 7607                     jbe        0x457787
00457780 b001                     mov        al, 1
00457782 e918010000               jmp        0x45789f
00457787 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
0045778c f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00457794 f30f5845f0               addss      xmm0, dword ptr [ebp - 0x10]
00457799 f30f114508               movss      dword ptr [ebp + 8], xmm0
0045779e f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004577a3 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004577ab f30f104df4               movss      xmm1, dword ptr [ebp - 0xc]
004577b0 f30f5cc8                 subss      xmm1, xmm0
004577b4 f30f114d0c               movss      dword ptr [ebp + 0xc], xmm1
004577b9 f30f104508               movss      xmm0, dword ptr [ebp + 8]
004577be f30f594508               mulss      xmm0, dword ptr [ebp + 8]
004577c3 f30f104d0c               movss      xmm1, dword ptr [ebp + 0xc]
004577c8 f30f594d0c               mulss      xmm1, dword ptr [ebp + 0xc]
004577cd f30f58c1                 addss      xmm0, xmm1
004577d1 f30f104d24               movss      xmm1, dword ptr [ebp + 0x24]
004577d6 f30f594d24               mulss      xmm1, dword ptr [ebp + 0x24]
004577db 0f2fc8                   comiss     xmm1, xmm0
004577de 7607                     jbe        0x4577e7
004577e0 b001                     mov        al, 1
004577e2 e9b8000000               jmp        0x45789f
004577e7 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004577ec f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004577f4 f30f104df0               movss      xmm1, dword ptr [ebp - 0x10]
004577f9 f30f5cc8                 subss      xmm1, xmm0
004577fd f30f114d08               movss      dword ptr [ebp + 8], xmm1
00457802 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00457807 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0045780f f30f5845f4               addss      xmm0, dword ptr [ebp - 0xc]
00457814 f30f11450c               movss      dword ptr [ebp + 0xc], xmm0
00457819 f30f104508               movss      xmm0, dword ptr [ebp + 8]
0045781e f30f594508               mulss      xmm0, dword ptr [ebp + 8]
00457823 f30f104d0c               movss      xmm1, dword ptr [ebp + 0xc]
00457828 f30f594d0c               mulss      xmm1, dword ptr [ebp + 0xc]
0045782d f30f58c1                 addss      xmm0, xmm1
00457831 f30f104d24               movss      xmm1, dword ptr [ebp + 0x24]
00457836 f30f594d24               mulss      xmm1, dword ptr [ebp + 0x24]
0045783b 0f2fc8                   comiss     xmm1, xmm0
0045783e 7604                     jbe        0x457844
00457840 b001                     mov        al, 1
00457842 eb5b                     jmp        0x45789f
00457844 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00457849 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00457851 f30f5845f0               addss      xmm0, dword ptr [ebp - 0x10]
00457856 f30f114508               movss      dword ptr [ebp + 8], xmm0
0045785b f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00457860 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00457868 f30f5845f4               addss      xmm0, dword ptr [ebp - 0xc]
0045786d f30f11450c               movss      dword ptr [ebp + 0xc], xmm0
00457872 f30f104508               movss      xmm0, dword ptr [ebp + 8]
00457877 f30f594508               mulss      xmm0, dword ptr [ebp + 8]
0045787c f30f104d0c               movss      xmm1, dword ptr [ebp + 0xc]
00457881 f30f594d0c               mulss      xmm1, dword ptr [ebp + 0xc]
00457886 f30f58c1                 addss      xmm0, xmm1
0045788a f30f104d24               movss      xmm1, dword ptr [ebp + 0x24]
0045788f f30f594d24               mulss      xmm1, dword ptr [ebp + 0x24]
00457894 0f2fc8                   comiss     xmm1, xmm0
00457897 7604                     jbe        0x45789d
00457899 b001                     mov        al, 1
0045789b eb02                     jmp        0x45789f
0045789d 32c0                     xor        al, al
0045789f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004578a2 33cd                     xor        ecx, ebp
004578a4 e845b10e00               call       0x5429ee
004578a9 8be5                     mov        esp, ebp
004578ab 5d                       pop        ebp
004578ac c3                       ret        
004578ad cc                       int3       
004578ae cc                       int3       
004578af cc                       int3       
