00487590 55                       push       ebp
00487591 8bec                     mov        ebp, esp
00487593 83ec70                   sub        esp, 0x70
00487596 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0048759b 33c5                     xor        eax, ebp
0048759d 8945fc                   mov        dword ptr [ebp - 4], eax
004875a0 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
004875a3 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
004875a6 8b4878                   mov        ecx, dword ptr [eax + 0x78]
004875a9 83e101                   and        ecx, 1
004875ac 0f84e8020000             je         0x48789a
004875b2 ba04000000               mov        edx, 4
004875b7 d1e2                     shl        edx, 1
004875b9 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
004875bc 8d4c1014                 lea        ecx, [eax + edx + 0x14]
004875c0 e80b59fcff               call       0x44ced0
004875c5 8945c8                   mov        dword ptr [ebp - 0x38], eax
004875c8 837dc800                 cmp        dword ptr [ebp - 0x38], 0
004875cc 0f84c8020000             je         0x48789a
004875d2 6a02                     push       2
004875d4 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004875da e821150000               call       0x488b00
004875df 6a02                     push       2
004875e1 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004875e7 e814140000               call       0x488a00
004875ec 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004875ef e8ec5afdff               call       0x45d0e0
004875f4 8a4803                   mov        cl, byte ptr [eax + 3]
004875f7 884dd3                   mov        byte ptr [ebp - 0x2d], cl
004875fa 0fb655d3                 movzx      edx, byte ptr [ebp - 0x2d]
004875fe 52                       push       edx
004875ff 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00487605 e836130000               call       0x488940
0048760a 90                       nop        
0048760b 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
0048760e 8b4878                   mov        ecx, dword ptr [eax + 0x78]
00487611 d1e9                     shr        ecx, 1
00487613 83e101                   and        ecx, 1
00487616 744a                     je         0x487662
00487618 51                       push       ecx
00487619 0f57c0                   xorps      xmm0, xmm0
0048761c f30f110424               movss      dword ptr [esp], xmm0
00487621 51                       push       ecx
00487622 f30f100578fe5600         movss      xmm0, dword ptr [0x56fe78]
0048762a f30f110424               movss      dword ptr [esp], xmm0
0048762f 51                       push       ecx
00487630 f30f100580fe5600         movss      xmm0, dword ptr [0x56fe80]
00487638 f30f110424               movss      dword ptr [esp], xmm0
0048763d 8d4de4                   lea        ecx, [ebp - 0x1c]
00487640 e88bb7f9ff               call       0x422dd0
00487645 6a20                     push       0x20
00487647 6a08                     push       8
00487649 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
0048764c 8b427c                   mov        eax, dword ptr [edx + 0x7c]
0048764f 50                       push       eax
00487650 8d4de4                   lea        ecx, [ebp - 0x1c]
00487653 51                       push       ecx
00487654 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0048765a e8e156feff               call       0x46cd40
0048765f 90                       nop        
00487660 eb4d                     jmp        0x4876af
00487662 51                       push       ecx
00487663 0f57c0                   xorps      xmm0, xmm0
00487666 f30f110424               movss      dword ptr [esp], xmm0
0048766b 51                       push       ecx
0048766c f30f100578fe5600         movss      xmm0, dword ptr [0x56fe78]
00487674 f30f110424               movss      dword ptr [esp], xmm0
00487679 51                       push       ecx
0048767a f30f100584fe5600         movss      xmm0, dword ptr [0x56fe84]
00487682 f30f110424               movss      dword ptr [esp], xmm0
00487687 8d4dd8                   lea        ecx, [ebp - 0x28]
0048768a e841b7f9ff               call       0x422dd0
0048768f 90                       nop        
00487690 83ec08                   sub        esp, 8
00487693 8bcc                     mov        ecx, esp
00487695 68f8fd5600               push       0x56fdf8
0048769a e83167f8ff               call       0x40ddd0
0048769f 8d55d8                   lea        edx, [ebp - 0x28]
004876a2 52                       push       edx
004876a3 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004876a9 e8e257feff               call       0x46ce90
004876ae 90                       nop        
004876af 51                       push       ecx
004876b0 0f57c0                   xorps      xmm0, xmm0
004876b3 f30f110424               movss      dword ptr [esp], xmm0
004876b8 51                       push       ecx
004876b9 f30f100578fe5600         movss      xmm0, dword ptr [0x56fe78]
004876c1 f30f110424               movss      dword ptr [esp], xmm0
004876c6 51                       push       ecx
004876c7 f30f100588fe5600         movss      xmm0, dword ptr [0x56fe88]
004876cf f30f110424               movss      dword ptr [esp], xmm0
004876d4 8d4df0                   lea        ecx, [ebp - 0x10]
004876d7 e8f4b6f9ff               call       0x422dd0
004876dc b968a55b00               mov        ecx, 0x5ba568
004876e1 e86a110000               call       0x488850
004876e6 0fb6c0                   movzx      eax, al
004876e9 85c0                     test       eax, eax
004876eb 7409                     je         0x4876f6
004876ed c745c401000000           mov        dword ptr [ebp - 0x3c], 1
004876f4 eb07                     jmp        0x4876fd
004876f6 c745c400000000           mov        dword ptr [ebp - 0x3c], 0
004876fd 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00487700 894dcc                   mov        dword ptr [ebp - 0x34], ecx
00487703 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00487709 e842850800               call       0x50fc50
0048770e 05080b0000               add        eax, 0xb08
00487713 8945bc                   mov        dword ptr [ebp - 0x44], eax
00487716 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
00487719 8b4274                   mov        eax, dword ptr [edx + 0x74]
0048771c 8945c0                   mov        dword ptr [ebp - 0x40], eax
0048771f 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00487722 51                       push       ecx
00487723 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00487726 e8e5f6ffff               call       0x486e10
0048772b 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
0048772e 83bc90c000000064         cmp        dword ptr [eax + edx*4 + 0xc0], 0x64
00487736 7c24                     jl         0x48775c
00487738 83ec08                   sub        esp, 8
0048773b 8bcc                     mov        ecx, esp
0048773d 68fcfd5600               push       0x56fdfc
00487742 e88966f8ff               call       0x40ddd0
00487747 8d45f0                   lea        eax, [ebp - 0x10]
0048774a 50                       push       eax
0048774b 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00487751 e83a57feff               call       0x46ce90
00487756 90                       nop        
00487757 e913010000               jmp        0x48786f
0048775c 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00487762 e8e9840800               call       0x50fc50
00487767 05080b0000               add        eax, 0xb08
0048776c 8945b4                   mov        dword ptr [ebp - 0x4c], eax
0048776f 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00487772 8b5174                   mov        edx, dword ptr [ecx + 0x74]
00487775 8955b8                   mov        dword ptr [ebp - 0x48], edx
00487778 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
0048777b 50                       push       eax
0048777c 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
0048777f e88cf6ffff               call       0x486e10
00487784 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00487787 83bc88c800000064         cmp        dword ptr [eax + ecx*4 + 0xc8], 0x64
0048778f 7c55                     jl         0x4877e6
00487791 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00487797 e8b4840800               call       0x50fc50
0048779c 05080b0000               add        eax, 0xb08
004877a1 8945ac                   mov        dword ptr [ebp - 0x54], eax
004877a4 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
004877a7 8b4274                   mov        eax, dword ptr [edx + 0x74]
004877aa 8945b0                   mov        dword ptr [ebp - 0x50], eax
004877ad 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
004877b0 51                       push       ecx
004877b1 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
004877b4 e857f6ffff               call       0x486e10
004877b9 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
004877bc 8b8490c0000000           mov        eax, dword ptr [eax + edx*4 + 0xc0]
004877c3 8945a8                   mov        dword ptr [ebp - 0x58], eax
004877c6 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004877c9 51                       push       ecx
004877ca 6804fe5600               push       0x56fe04
004877cf 8d55f0                   lea        edx, [ebp - 0x10]
004877d2 52                       push       edx
004877d3 a198065c00               mov        eax, dword ptr [0x5c0698]
004877d8 50                       push       eax
004877d9 e8b251feff               call       0x46c990
004877de 83c410                   add        esp, 0x10
004877e1 e989000000               jmp        0x48786f
004877e6 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004877ec e85f840800               call       0x50fc50
004877f1 05080b0000               add        eax, 0xb08
004877f6 8945a0                   mov        dword ptr [ebp - 0x60], eax
004877f9 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004877fc 8b5174                   mov        edx, dword ptr [ecx + 0x74]
004877ff 8955a4                   mov        dword ptr [ebp - 0x5c], edx
00487802 8b45a4                   mov        eax, dword ptr [ebp - 0x5c]
00487805 50                       push       eax
00487806 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00487809 e802f6ffff               call       0x486e10
0048780e 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00487811 8b9488c8000000           mov        edx, dword ptr [eax + ecx*4 + 0xc8]
00487818 895594                   mov        dword ptr [ebp - 0x6c], edx
0048781b 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00487821 e82a840800               call       0x50fc50
00487826 05080b0000               add        eax, 0xb08
0048782b 894598                   mov        dword ptr [ebp - 0x68], eax
0048782e 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00487831 8b4874                   mov        ecx, dword ptr [eax + 0x74]
00487834 894d9c                   mov        dword ptr [ebp - 0x64], ecx
00487837 8b559c                   mov        edx, dword ptr [ebp - 0x64]
0048783a 52                       push       edx
0048783b 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
0048783e e8cdf5ffff               call       0x486e10
00487843 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00487846 8b9488c0000000           mov        edx, dword ptr [eax + ecx*4 + 0xc0]
0048784d 895590                   mov        dword ptr [ebp - 0x70], edx
00487850 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
00487853 50                       push       eax
00487854 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00487857 51                       push       ecx
00487858 6810fe5600               push       0x56fe10
0048785d 8d55f0                   lea        edx, [ebp - 0x10]
00487860 52                       push       edx
00487861 a198065c00               mov        eax, dword ptr [0x5c0698]
00487866 50                       push       eax
00487867 e82451feff               call       0x46c990
0048786c 83c414                   add        esp, 0x14
0048786f 6a00                     push       0
00487871 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00487877 e884120000               call       0x488b00
0048787c 6a00                     push       0
0048787e 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00487884 e877110000               call       0x488a00
00487889 68ff000000               push       0xff
0048788e 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00487894 e8a7100000               call       0x488940
00487899 90                       nop        
0048789a b801000000               mov        eax, 1
0048789f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004878a2 33cd                     xor        ecx, ebp
004878a4 e845b10b00               call       0x5429ee
004878a9 8be5                     mov        esp, ebp
004878ab 5d                       pop        ebp
004878ac c3                       ret        
004878ad cc                       int3       
004878ae cc                       int3       
004878af cc                       int3       
