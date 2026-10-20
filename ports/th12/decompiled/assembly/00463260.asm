
// block 00463260
00463260  sub        esp, 0x104
00463266  mov        eax, dword ptr [0x4ad138]
0046326b  xor        eax, esp
0046326d  mov        dword ptr [esp + 0x100], eax
00463274  mov        eax, dword ptr [esp + 0x108]
0046327b  imul       eax, eax, 0x148
00463281  push       esi
00463282  xor        esi, esi
00463284  add        eax, 0x4d48b8
00463289  push       edi
0046328a  mov        edi, eax
0046328c  cmp        dword ptr [0x4cf3fc], esi
00463292  je         0x463591

// block 00463298
00463298  test       dword ptr [0x4cee78], 0x400
004632a2  jne        0x463402

// block 004632a8
004632a8  lea        eax, [esp + 8]
004632ac  push       eax
004632ad  call       dword ptr [0x498244]

// block 004632b3
004632b3  movzx      esi, byte ptr [esp + 0x5a]
004632b8  movzx      ecx, byte ptr [esp + 0x4c]
004632bd  movzx      edx, byte ptr [esp + 0x15]
004632c2  movzx      eax, byte ptr [esp + 0x58]
004632c7  and        esi, 0x80
004632cd  and        ecx, 0x80
004632d3  add        esi, esi
004632d5  or         esi, ecx
004632d7  movzx      ecx, byte ptr [esp + 0x2c]
004632dc  or         eax, ecx
004632de  movzx      ecx, byte ptr [esp + 0x19]
004632e3  and        eax, 0x80
004632e8  add        esi, esi
004632ea  and        edx, 0x80
004632f0  or         esi, edx
004632f2  movzx      edx, byte ptr [esp + 0x5b]
004632f7  add        esi, esi
004632f9  or         esi, eax
004632fb  movzx      eax, byte ptr [esp + 0x59]
00463300  and        eax, 0x80
00463305  add        esi, esi
00463307  and        edx, 0x80
0046330d  or         esi, edx
0046330f  movzx      edx, byte ptr [esp + 0x23]
00463314  and        ecx, 0x80
0046331a  add        esi, esi
0046331c  or         esi, eax
0046331e  movzx      eax, byte ptr [esp + 0x60]
00463323  and        eax, 0x80
00463328  shl        esi, 7
0046332b  or         esi, ecx
0046332d  movzx      ecx, byte ptr [esp + 0x62]
00463332  shr        ecx, 1
00463334  or         eax, ecx
00463336  movzx      ecx, byte ptr [esp + 0x70]
0046333b  and        edx, 0x80
00463341  shr        eax, 2
00463344  add        esi, esi
00463346  or         esi, edx
00463348  movzx      edx, byte ptr [esp + 0x18]
0046334d  and        edx, 0x80
00463353  or         eax, edx
00463355  movzx      edx, byte ptr [esp + 0x2e]
0046335a  or         ecx, edx
0046335c  movzx      edx, byte ptr [esp + 0x30]
00463361  shr        eax, 1
00463363  and        ecx, 0x80
00463369  or         eax, ecx
0046336b  movzx      ecx, byte ptr [esp + 0x6a]
00463370  or         ecx, edx
00463372  movzx      edx, byte ptr [esp + 0x2d]
00463377  shr        eax, 1
00463379  and        ecx, 0x80
0046337f  or         eax, ecx
00463381  movzx      ecx, byte ptr [esp + 0x6c]
00463386  or         ecx, edx
00463388  movzx      edx, byte ptr [esp + 0x6b]
0046338d  shr        eax, 1
0046338f  and        ecx, 0x80
00463395  or         eax, ecx
00463397  movzx      ecx, byte ptr [esp + 0x2f]
0046339c  shr        eax, 1
0046339e  add        esi, esi
004633a0  or         esi, eax
004633a2  movzx      eax, byte ptr [esp + 0x6e]
004633a7  or         eax, ecx
004633a9  mov        cl, byte ptr [esp + 0x71]
004633ad  and        eax, 0x80
004633b2  or         esi, eax
004633b4  and        dl, 0x80
004633b7  movzx      eax, dl
004633ba  neg        eax
004633bc  sbb        eax, eax
004633be  and        cl, 0x80
004633c1  and        eax, 0xa0
004633c6  movzx      edx, cl
004633c9  or         esi, eax
004633cb  neg        edx
004633cd  mov        al, byte ptr [esp + 0x69]
004633d1  sbb        edx, edx
004633d3  and        al, 0x80
004633d5  and        edx, 0x90
004633db  movzx      ecx, al
004633de  or         esi, edx
004633e0  movzx      edx, byte ptr [esp + 0x6f]
004633e5  neg        ecx
004633e7  sbb        ecx, ecx
004633e9  and        dl, 0x80
004633ec  and        ecx, 0x60
004633ef  movzx      eax, dl
004633f2  or         esi, ecx
004633f4  neg        eax
004633f6  sbb        eax, eax
004633f8  and        eax, 0x50
004633fb  or         esi, eax
004633fd  jmp        0x463591

// block 00463402
00463402  mov        eax, dword ptr [0x4ce908]
00463407  mov        ecx, dword ptr [eax]
00463409  lea        edx, [esp + 8]
0046340d  push       edx
0046340e  push       0x100
00463413  push       eax
00463414  mov        eax, dword ptr [ecx + 0x24]
00463417  call       eax

// block 00463419
00463419  xor        esi, esi
0046341b  cmp        eax, 0x8007001e
00463420  je         0x463426

// block 00463422
00463422  test       eax, eax
00463424  je         0x463438

// block 00463426
00463426  mov        eax, dword ptr [0x4ce908]
0046342b  mov        ecx, dword ptr [eax]
0046342d  mov        edx, dword ptr [ecx + 0x1c]
00463430  push       eax
00463431  call       edx

// block 00463433
00463433  jmp        0x463591

// block 00463438
00463438  movzx      esi, byte ptr [esp + 0x1b]
0046343d  movzx      eax, byte ptr [esp + 0x28]
00463442  movzx      ecx, byte ptr [esp + 0x24]
00463447  movzx      edx, byte ptr [esp + 0x21]
0046344c  and        esi, 0x80
00463452  and        eax, 0x80
00463457  add        esi, esi
00463459  or         esi, eax
0046345b  movzx      eax, byte ptr [esp + 0xcf]
00463463  or         edx, eax
00463465  movzx      eax, byte ptr [esp + 0x25]
0046346a  and        edx, 0x80
00463470  add        esi, esi
00463472  and        ecx, 0x80
00463478  or         esi, ecx
0046347a  movzx      ecx, byte ptr [esp + 0x27]
0046347f  add        esi, esi
00463481  or         esi, edx
00463483  movzx      edx, byte ptr [esp + 0x18]
00463488  and        edx, 0x80
0046348e  and        ecx, 0x80
00463494  add        esi, esi
00463496  or         esi, ecx
00463498  movzx      ecx, byte ptr [esp + 9]
0046349d  and        eax, 0x80
004634a2  add        esi, esi
004634a4  or         esi, edx
004634a6  movzx      edx, byte ptr [esp + 0x35]
004634ab  and        edx, 0x80
004634b1  shl        esi, 7
004634b4  or         esi, eax
004634b6  movzx      eax, byte ptr [esp + 0x34]
004634bb  and        ecx, 0x80
004634c1  shr        eax, 1
004634c3  or         edx, eax
004634c5  movzx      eax, byte ptr [esp + 0x50]
004634ca  shr        edx, 2
004634cd  add        esi, esi
004634cf  or         esi, ecx
004634d1  movzx      ecx, byte ptr [esp + 0x32]
004634d6  and        ecx, 0x80
004634dc  or         edx, ecx
004634de  movzx      ecx, byte ptr [esp + 0xd0]
004634e6  or         eax, ecx
004634e8  movzx      ecx, byte ptr [esp + 0x58]
004634ed  shr        edx, 1
004634ef  and        eax, 0x80
004634f4  or         edx, eax
004634f6  movzx      eax, byte ptr [esp + 0xd8]
004634fe  or         eax, ecx
00463500  movzx      ecx, byte ptr [esp + 0xd3]
00463508  shr        edx, 1
0046350a  and        eax, 0x80
0046350f  or         edx, eax
00463511  movzx      eax, byte ptr [esp + 0x53]
00463516  or         eax, ecx
00463518  movzx      ecx, byte ptr [esp + 0x51]
0046351d  shr        edx, 1
0046351f  and        eax, 0x80
00463524  or         edx, eax
00463526  movzx      eax, byte ptr [esp + 0xd5]
0046352e  shr        edx, 1
00463530  add        esi, esi
00463532  or         esi, edx
00463534  movzx      edx, byte ptr [esp + 0x55]
00463539  or         edx, eax
0046353b  mov        al, byte ptr [esp + 0x57]
0046353f  and        edx, 0x80
00463545  and        cl, 0x80
00463548  or         esi, edx
0046354a  movzx      edx, cl
0046354d  neg        edx
0046354f  sbb        edx, edx
00463551  and        al, 0x80
00463553  and        edx, 0x90
00463559  movzx      ecx, al
0046355c  or         esi, edx
0046355e  neg        ecx
00463560  sbb        ecx, ecx
00463562  and        ecx, 0x60
00463565  or         esi, ecx
00463567  mov        dl, byte ptr [esp + 0x4f]
0046356b  movzx      ecx, byte ptr [esp + 0x59]
00463570  and        dl, 0x80
00463573  movzx      eax, dl
00463576  neg        eax
00463578  sbb        eax, eax
0046357a  and        cl, 0x80
0046357d  and        eax, 0x50
00463580  movzx      edx, cl
00463583  or         esi, eax
00463585  neg        edx
00463587  sbb        edx, edx
00463589  and        edx, 0xa0
0046358f  or         esi, edx

// block 00463591
00463591  mov        ecx, esi
00463593  call       0x462a80

// block 00463598
00463598  mov        esi, eax
0046359a  mov        eax, dword ptr [edi]
0046359c  mov        ecx, edi
0046359e  mov        dword ptr [edi + 4], eax
004635a1  mov        dword ptr [edi], esi
004635a3  call       0x465440

// block 004635a8
004635a8  mov        ecx, dword ptr [esp + 0x108]
004635af  pop        edi
004635b0  mov        eax, esi
004635b2  pop        esi
004635b3  xor        ecx, esp
004635b5  call       0x46c932

// block 004635ba
004635ba  add        esp, 0x104
004635c0  ret        4
