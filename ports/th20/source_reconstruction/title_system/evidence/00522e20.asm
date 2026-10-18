00522e20 55                       push       ebp
00522e21 8bec                     mov        ebp, esp
00522e23 81ecac000000             sub        esp, 0xac
00522e29 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00522e2e 33c5                     xor        eax, ebp
00522e30 8945fc                   mov        dword ptr [ebp - 4], eax
00522e33 56                       push       esi
00522e34 57                       push       edi
00522e35 894dac                   mov        dword ptr [ebp - 0x54], ecx
00522e38 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
00522e3b 8b4820                   mov        ecx, dword ptr [eax + 0x20]
00522e3e 894d90                   mov        dword ptr [ebp - 0x70], ecx
00522e41 837d9002                 cmp        dword ptr [ebp - 0x70], 2
00522e45 7405                     je         0x522e4c
00522e47 e9d6050000               jmp        0x523422
00522e4c 6aff                     push       -1
00522e4e 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00522e54 e80733faff               call       0x4c6160
00522e59 68000000ff               push       0xff000000
00522e5e 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00522e64 e847dcf4ff               call       0x470ab0
00522e69 6a04                     push       4
00522e6b 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00522e71 e80a33faff               call       0x4c6180
00522e76 6a01                     push       1
00522e78 6a00                     push       0
00522e7a 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00522e80 e89b57f9ff               call       0x4b8620
00522e85 90                       nop        
00522e86 51                       push       ecx
00522e87 0f57c0                   xorps      xmm0, xmm0
00522e8a f30f110424               movss      dword ptr [esp], xmm0
00522e8f 51                       push       ecx
00522e90 f30f100530fa5600         movss      xmm0, dword ptr [0x56fa30]
00522e98 f30f110424               movss      dword ptr [esp], xmm0
00522e9d 51                       push       ecx
00522e9e f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
00522ea6 f30f110424               movss      dword ptr [esp], xmm0
00522eab 8d8d60ffffff             lea        ecx, [ebp - 0xa0]
00522eb1 e81affefff               call       0x422dd0
00522eb6 89458c                   mov        dword ptr [ebp - 0x74], eax
00522eb9 83ec08                   sub        esp, 8
00522ebc 8bf4                     mov        esi, esp
00522ebe 6a00                     push       0
00522ec0 e8bb11f4ff               call       0x464080
00522ec5 83c404                   add        esp, 4
00522ec8 8bc8                     mov        ecx, eax
00522eca e861f8eeff               call       0x412730
00522ecf 8b148588ff5a00           mov        edx, dword ptr [eax*4 + 0x5aff88]
00522ed6 52                       push       edx
00522ed7 8bce                     mov        ecx, esi
00522ed9 e8f2aeeeff               call       0x40ddd0
00522ede 8b458c                   mov        eax, dword ptr [ebp - 0x74]
00522ee1 50                       push       eax
00522ee2 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00522ee8 51                       push       ecx
00522ee9 e82293f4ff               call       0x46c210
00522eee 83c410                   add        esp, 0x10
00522ef1 e86a8affff               call       0x51b960
00522ef6 894584                   mov        dword ptr [ebp - 0x7c], eax
00522ef9 6a00                     push       0
00522efb e88011f4ff               call       0x464080
00522f00 83c404                   add        esp, 4
00522f03 8bc8                     mov        ecx, eax
00522f05 e8a69befff               call       0x41cab0
00522f0a 894588                   mov        dword ptr [ebp - 0x78], eax
00522f0d 51                       push       ecx
00522f0e 0f57c0                   xorps      xmm0, xmm0
00522f11 f30f110424               movss      dword ptr [esp], xmm0
00522f16 51                       push       ecx
00522f17 f30f10057cfe5600         movss      xmm0, dword ptr [0x56fe7c]
00522f1f f30f110424               movss      dword ptr [esp], xmm0
00522f24 51                       push       ecx
00522f25 f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
00522f2d f30f110424               movss      dword ptr [esp], xmm0
00522f32 8d8d54ffffff             lea        ecx, [ebp - 0xac]
00522f38 e893feefff               call       0x422dd0
00522f3d 894580                   mov        dword ptr [ebp - 0x80], eax
00522f40 83ec08                   sub        esp, 8
00522f43 8bf4                     mov        esi, esp
00522f45 8b5588                   mov        edx, dword ptr [ebp - 0x78]
00522f48 52                       push       edx
00522f49 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
00522f4c e87fcbfbff               call       0x4dfad0
00522f51 50                       push       eax
00522f52 8bce                     mov        ecx, esi
00522f54 e877aeeeff               call       0x40ddd0
00522f59 8b4580                   mov        eax, dword ptr [ebp - 0x80]
00522f5c 50                       push       eax
00522f5d 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00522f63 51                       push       ecx
00522f64 e8a792f4ff               call       0x46c210
00522f69 83c410                   add        esp, 0x10
00522f6c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00522f72 e86938fcff               call       0x4e67e0
00522f77 90                       nop        
00522f78 51                       push       ecx
00522f79 0f57c0                   xorps      xmm0, xmm0
00522f7c f30f110424               movss      dword ptr [esp], xmm0
00522f81 51                       push       ecx
00522f82 f30f100580fb5600         movss      xmm0, dword ptr [0x56fb80]
00522f8a f30f110424               movss      dword ptr [esp], xmm0
00522f8f 51                       push       ecx
00522f90 f30f100504fc5600         movss      xmm0, dword ptr [0x56fc04]
00522f98 f30f110424               movss      dword ptr [esp], xmm0
00522f9d 8d4dd8                   lea        ecx, [ebp - 0x28]
00522fa0 e82bfeefff               call       0x422dd0
00522fa5 b968a55b00               mov        ecx, 0x5ba568
00522faa e8f110f4ff               call       0x4640a0
00522faf 898578ffffff             mov        dword ptr [ebp - 0x88], eax
00522fb5 6a01                     push       1
00522fb7 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00522fbd e85e3afcff               call       0x4e6a20
00522fc2 90                       nop        
00522fc3 c745a400000000           mov        dword ptr [ebp - 0x5c], 0
00522fca eb09                     jmp        0x522fd5
00522fcc 8b55a4                   mov        edx, dword ptr [ebp - 0x5c]
00522fcf 83c201                   add        edx, 1
00522fd2 8955a4                   mov        dword ptr [ebp - 0x5c], edx
00522fd5 837da40a                 cmp        dword ptr [ebp - 0x5c], 0xa
00522fd9 0f8db8010000             jge        0x523197
00522fdf 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
00522fe2 83b8e456000000           cmp        dword ptr [eax + 0x56e4], 0
00522fe9 742d                     je         0x523018
00522feb 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
00522fee c1e104                   shl        ecx, 4
00522ff1 baff000000               mov        edx, 0xff
00522ff6 2bd1                     sub        edx, ecx
00522ff8 89957cffffff             mov        dword ptr [ebp - 0x84], edx
00522ffe 8b857cffffff             mov        eax, dword ptr [ebp - 0x84]
00523004 0d00ffffff               or         eax, 0xffffff00
00523009 50                       push       eax
0052300a 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00523010 e8bbdaf4ff               call       0x470ad0
00523015 90                       nop        
00523016 eb31                     jmp        0x523049
00523018 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
0052301b 83c124                   add        ecx, 0x24
0052301e e8dd92eeff               call       0x40c300
00523023 3b45a4                   cmp        eax, dword ptr [ebp - 0x5c]
00523026 7413                     je         0x52303b
00523028 68408080ff               push       0xff808040
0052302d 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00523033 e898daf4ff               call       0x470ad0
00523038 90                       nop        
00523039 eb0e                     jmp        0x523049
0052303b 6aff                     push       -1
0052303d 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00523043 e888daf4ff               call       0x470ad0
00523048 90                       nop        
00523049 b968a55b00               mov        ecx, 0x5ba568
0052304e e8ed52f9ff               call       0x4b8340
00523053 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
00523059 b968a55b00               mov        ecx, 0x5ba568
0052305e e80d4ff9ff               call       0x4b7f70
00523063 898570ffffff             mov        dword ptr [ebp - 0x90], eax
00523069 6b75a428                 imul       esi, dword ptr [ebp - 0x5c], 0x28
0052306d 69bd78ffffff90010000     imul       edi, dword ptr [ebp - 0x88], 0x190
00523077 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
0052307d 51                       push       ecx
0052307e 8b9570ffffff             mov        edx, dword ptr [ebp - 0x90]
00523084 52                       push       edx
00523085 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052308b e8d0a3f9ff               call       0x4bd460
00523090 8d443818                 lea        eax, [eax + edi + 0x18]
00523094 03c6                     add        eax, esi
00523096 8945b0                   mov        dword ptr [ebp - 0x50], eax
00523099 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0052309c 894d9c                   mov        dword ptr [ebp - 0x64], ecx
0052309f 8b559c                   mov        edx, dword ptr [ebp - 0x64]
005230a2 8b459c                   mov        eax, dword ptr [ebp - 0x64]
005230a5 8b4a18                   mov        ecx, dword ptr [edx + 0x18]
005230a8 0b481c                   or         ecx, dword ptr [eax + 0x1c]
005230ab 0f8487000000             je         0x523138
005230b1 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
005230b4 83c218                   add        edx, 0x18
005230b7 52                       push       edx
005230b8 8d45b4                   lea        eax, [ebp - 0x4c]
005230bb 50                       push       eax
005230bc e8af91f8ff               call       0x4ac270
005230c1 83c408                   add        esp, 8
005230c4 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
005230c7 f30f5a4120               cvtss2sd   xmm0, dword ptr [ecx + 0x20]
005230cc 83ec08                   sub        esp, 8
005230cf f20f110424               movsd      qword ptr [esp], xmm0
005230d4 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
005230d7 0fbe4208                 movsx      eax, byte ptr [edx + 8]
005230db 8b0c85a8ff5a00           mov        ecx, dword ptr [eax*4 + 0x5affa8]
005230e2 51                       push       ecx
005230e3 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
005230e6 52                       push       edx
005230e7 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
005230ea 50                       push       eax
005230eb 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
005230ee 51                       push       ecx
005230ef 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
005230f2 83c201                   add        edx, 1
005230f5 52                       push       edx
005230f6 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
005230f9 056c070000               add        eax, 0x76c
005230fe 50                       push       eax
005230ff 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00523102 0fbe5109                 movsx      edx, byte ptr [ecx + 9]
00523106 52                       push       edx
00523107 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
0052310a 8b4804                   mov        ecx, dword ptr [eax + 4]
0052310d 51                       push       ecx
0052310e 8b10                     mov        edx, dword ptr [eax]
00523110 52                       push       edx
00523111 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00523114 83c00a                   add        eax, 0xa
00523117 50                       push       eax
00523118 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
0052311b 83c101                   add        ecx, 1
0052311e 51                       push       ecx
0052311f 68c8525700               push       0x5752c8
00523124 8d55d8                   lea        edx, [ebp - 0x28]
00523127 52                       push       edx
00523128 a198065c00               mov        eax, dword ptr [0x5c0698]
0052312d 50                       push       eax
0052312e e85d98f4ff               call       0x46c990
00523133 83c440                   add        esp, 0x40
00523136 eb48                     jmp        0x523180
00523138 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0052313b f30f5a4120               cvtss2sd   xmm0, dword ptr [ecx + 0x20]
00523140 83ec08                   sub        esp, 8
00523143 f20f110424               movsd      qword ptr [esp], xmm0
00523148 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
0052314b 0fbe4209                 movsx      eax, byte ptr [edx + 9]
0052314f 50                       push       eax
00523150 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00523153 8b5104                   mov        edx, dword ptr [ecx + 4]
00523156 52                       push       edx
00523157 8b01                     mov        eax, dword ptr [ecx]
00523159 50                       push       eax
0052315a 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0052315d 83c10a                   add        ecx, 0xa
00523160 51                       push       ecx
00523161 8b55a4                   mov        edx, dword ptr [ebp - 0x5c]
00523164 83c201                   add        edx, 1
00523167 52                       push       edx
00523168 6800535700               push       0x575300
0052316d 8d45d8                   lea        eax, [ebp - 0x28]
00523170 50                       push       eax
00523171 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00523177 51                       push       ecx
00523178 e81398f4ff               call       0x46c990
0052317d 83c428                   add        esp, 0x28
00523180 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00523185 f30f580544265700         addss      xmm0, dword ptr [0x572644]
0052318d f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00523192 e935feffff               jmp        0x522fcc
00523197 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
0052319a 83bae456000000           cmp        dword ptr [edx + 0x56e4], 0
005231a1 0f857b020000             jne        0x523422
005231a7 a17cff5a00               mov        eax, dword ptr [0x5aff7c]
005231ac 50                       push       eax
005231ad e86ecc0200               call       0x54fe20
005231b2 83c404                   add        esp, 4
005231b5 8945a0                   mov        dword ptr [ebp - 0x60], eax
005231b8 51                       push       ecx
005231b9 0f57c0                   xorps      xmm0, xmm0
005231bc f30f110424               movss      dword ptr [esp], xmm0
005231c1 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
005231c4 83c124                   add        ecx, 0x24
005231c7 e83491eeff               call       0x40c300
005231cc f30f2ac0                 cvtsi2ss   xmm0, eax
005231d0 f30f590544265700         mulss      xmm0, dword ptr [0x572644]
005231d8 f30f580580fb5600         addss      xmm0, dword ptr [0x56fb80]
005231e0 51                       push       ecx
005231e1 f30f110424               movss      dword ptr [esp], xmm0
005231e6 51                       push       ecx
005231e7 f30f100560565700         movss      xmm0, dword ptr [0x575660]
005231ef f30f110424               movss      dword ptr [esp], xmm0
005231f4 8d4df0                   lea        ecx, [ebp - 0x10]
005231f7 e8d4fbefff               call       0x422dd0
005231fc 6aff                     push       -1
005231fe 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00523204 e8c7d8f4ff               call       0x470ad0
00523209 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
0052320c 81c1d4560000             add        ecx, 0x56d4
00523212 51                       push       ecx
00523213 68e4e05600               push       0x56e0e4
00523218 8d55f0                   lea        edx, [ebp - 0x10]
0052321b 52                       push       edx
0052321c a198065c00               mov        eax, dword ptr [0x5c0698]
00523221 50                       push       eax
00523222 e86997f4ff               call       0x46c990
00523227 83c410                   add        esp, 0x10
0052322a 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
0052322d 6b91e056000009           imul       edx, dword ptr [ecx + 0x56e0], 9
00523234 f30f2ac2                 cvtsi2ss   xmm0, edx
00523238 f30f5845f0               addss      xmm0, dword ptr [ebp - 0x10]
0052323d f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00523242 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
00523245 83b8e056000008           cmp        dword ptr [eax + 0x56e0], 8
0052324c 7512                     jne        0x523260
0052324e f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00523253 f30f5c05a4f75600         subss      xmm0, dword ptr [0x56f7a4]
0052325b f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00523260 6800ffffff               push       0xffffff00
00523265 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052326b e860d8f4ff               call       0x470ad0
00523270 6824255700               push       0x572524
00523275 8d4df0                   lea        ecx, [ebp - 0x10]
00523278 51                       push       ecx
00523279 8b1598065c00             mov        edx, dword ptr [0x5c0698]
0052327f 52                       push       edx
00523280 e80b97f4ff               call       0x46c990
00523285 83c40c                   add        esp, 0xc
00523288 6aff                     push       -1
0052328a 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00523290 e83bd8f4ff               call       0x470ad0
00523295 f30f10057cfb5600         movss      xmm0, dword ptr [0x56fb7c]
0052329d f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
005232a2 51                       push       ecx
005232a3 0f57c0                   xorps      xmm0, xmm0
005232a6 f30f110424               movss      dword ptr [esp], xmm0
005232ab 51                       push       ecx
005232ac f30f100588fe5600         movss      xmm0, dword ptr [0x56fe88]
005232b4 f30f110424               movss      dword ptr [esp], xmm0
005232b9 51                       push       ecx
005232ba f30f100574565700         movss      xmm0, dword ptr [0x575674]
005232c2 f30f110424               movss      dword ptr [esp], xmm0
005232c7 8d4de4                   lea        ecx, [ebp - 0x1c]
005232ca e801fbefff               call       0x422dd0
005232cf 90                       nop        
005232d0 c745a800000000           mov        dword ptr [ebp - 0x58], 0
005232d7 eb09                     jmp        0x5232e2
005232d9 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
005232dc 83c001                   add        eax, 1
005232df 8945a8                   mov        dword ptr [ebp - 0x58], eax
005232e2 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
005232e5 3b4da0                   cmp        ecx, dword ptr [ebp - 0x60]
005232e8 0f8d19010000             jge        0x523407
005232ee 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
005232f1 81c2e8560000             add        edx, 0x56e8
005232f7 89956cffffff             mov        dword ptr [ebp - 0x94], edx
005232fd 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
00523300 50                       push       eax
00523301 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00523307 e8a47ef4ff               call       0x46b1b0
0052330c 85c0                     test       eax, eax
0052330e 7413                     je         0x523323
00523310 6800ffffff               push       0xffffff00
00523315 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052331b e8b0d7f4ff               call       0x470ad0
00523320 90                       nop        
00523321 eb11                     jmp        0x523334
00523323 68808080ff               push       0xff808080
00523328 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052332e e89dd7f4ff               call       0x470ad0
00523333 90                       nop        
00523334 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00523337 83e903                   sub        ecx, 3
0052333a 394da8                   cmp        dword ptr [ebp - 0x58], ecx
0052333d 7d27                     jge        0x523366
0052333f 8b157cff5a00             mov        edx, dword ptr [0x5aff7c]
00523345 0355a8                   add        edx, dword ptr [ebp - 0x58]
00523348 0fbe02                   movsx      eax, byte ptr [edx]
0052334b 50                       push       eax
0052334c 68a4535700               push       0x5753a4
00523351 8d4de4                   lea        ecx, [ebp - 0x1c]
00523354 51                       push       ecx
00523355 8b1598065c00             mov        edx, dword ptr [0x5c0698]
0052335b 52                       push       edx
0052335c e82f96f4ff               call       0x46c990
00523361 83c410                   add        esp, 0x10
00523364 eb51                     jmp        0x5233b7
00523366 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
00523369 83e803                   sub        eax, 3
0052336c 3945a8                   cmp        dword ptr [ebp - 0x58], eax
0052336f 7509                     jne        0x52337a
00523371 c7459481000000           mov        dword ptr [ebp - 0x6c], 0x81
00523378 eb21                     jmp        0x52339b
0052337a 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
0052337d 83e902                   sub        ecx, 2
00523380 394da8                   cmp        dword ptr [ebp - 0x58], ecx
00523383 7509                     jne        0x52338e
00523385 c745987f000000           mov        dword ptr [ebp - 0x68], 0x7f
0052338c eb07                     jmp        0x523395
0052338e c7459880000000           mov        dword ptr [ebp - 0x68], 0x80
00523395 8b5598                   mov        edx, dword ptr [ebp - 0x68]
00523398 895594                   mov        dword ptr [ebp - 0x6c], edx
0052339b 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
0052339e 50                       push       eax
0052339f 68a4535700               push       0x5753a4
005233a4 8d4de4                   lea        ecx, [ebp - 0x1c]
005233a7 51                       push       ecx
005233a8 8b1598065c00             mov        edx, dword ptr [0x5c0698]
005233ae 52                       push       edx
005233af e8dc95f4ff               call       0x46c990
005233b4 83c410                   add        esp, 0x10
005233b7 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
005233ba 99                       cdq        
005233bb b90d000000               mov        ecx, 0xd
005233c0 f7f9                     idiv       ecx
005233c2 83fa0c                   cmp        edx, 0xc
005233c5 7529                     jne        0x5233f0
005233c7 f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
005233cf f30f5c0558265700         subss      xmm0, dword ptr [0x572658]
005233d7 f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
005233dc f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
005233e1 f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
005233e9 f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
005233ee eb12                     jmp        0x523402
005233f0 f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
005233f5 f30f580544265700         addss      xmm0, dword ptr [0x572644]
005233fd f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00523402 e9d2feffff               jmp        0x5232d9
00523407 6aff                     push       -1
00523409 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052340f e8bcd6f4ff               call       0x470ad0
00523414 6a00                     push       0
00523416 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0052341c e8ff35fcff               call       0x4e6a20
00523421 90                       nop        
00523422 b801000000               mov        eax, 1
00523427 5f                       pop        edi
00523428 5e                       pop        esi
00523429 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052342c 33cd                     xor        ecx, ebp
0052342e e8bbf50100               call       0x5429ee
00523433 8be5                     mov        esp, ebp
00523435 5d                       pop        ebp
00523436 c3                       ret        
00523437 cc                       int3       
00523438 cc                       int3       
00523439 cc                       int3       
0052343a cc                       int3       
0052343b cc                       int3       
0052343c cc                       int3       
0052343d cc                       int3       
0052343e cc                       int3       
0052343f cc                       int3       
