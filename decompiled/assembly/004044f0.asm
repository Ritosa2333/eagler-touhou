
// block 004044f0
004044f0  push       ebp
004044f1  mov        ebp, dword ptr [esp + 8]
004044f5  cmp        dword ptr [ebp + 0x3604], 0
004044fc  jne        0x404555

// block 004044fe
004044fe  mov        byte ptr [0x4d4f38], 0
00404505  mov        ecx, eax

// block 00404507
00404507  mov        dl, byte ptr [eax]
00404509  inc        eax
0040450a  test       dl, dl
0040450c  jne        0x404507

// block 0040450e
0040450e  push       esi
0040450f  push       edi
00404510  mov        edi, 0x4d4f38
00404515  sub        eax, ecx
00404517  mov        esi, ecx
00404519  dec        edi
0040451a  lea        ebx, [ebx]

// block 00404520
00404520  mov        cl, byte ptr [edi + 1]
00404523  inc        edi
00404524  test       cl, cl
00404526  jne        0x404520

// block 00404528
00404528  mov        ecx, eax
0040452a  shr        ecx, 2

// block 0040452d
0040452d  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0040452f
0040452f  mov        ecx, eax
00404531  push       0
00404533  lea        eax, [ebp + 0x3608]
00404539  and        ecx, 3
0040453c  push       eax
0040453d  mov        eax, 0x4d4f38

// block 00404542
00404542  rep movsb  byte ptr es:[edi], byte ptr [esi]

// block 00404544
00404544  call       0x463c10

// block 00404549
00404549  pop        edi
0040454a  mov        dword ptr [ebp + 0x3604], eax
00404550  pop        esi
00404551  test       eax, eax
00404553  je         0x4045b0

// block 00404555
00404555  mov        eax, dword ptr [ebp + 0x3608]
0040455b  push       ebx
0040455c  push       eax
0040455d  call       0x46d04a

// block 00404562
00404562  mov        ecx, dword ptr [ebp + 0x3608]
00404568  mov        edx, dword ptr [ebp + 0x3604]
0040456e  push       ecx
0040456f  push       edx
00404570  push       eax
00404571  mov        dword ptr [ebp + 0x10], eax
00404574  call       0x47a330

// block 00404579
00404579  mov        ecx, dword ptr [ebp + 0x35d4]
0040457f  mov        ebx, dword ptr [ebp + 0x10]
00404582  and        ecx, 1
00404585  add        esp, 0x10
00404588  add        ebx, 0x10
0040458b  add        ecx, 3
0040458e  call       0x45fe60

// block 00404593
00404593  mov        dword ptr [ebp + 0x1c4], eax
00404599  pop        ebx
0040459a  test       eax, eax
0040459c  jne        0x4045b7

// block 0040459e
0040459e  push       0x49f588
004045a3  mov        ecx, 0x4b0ec8
004045a8  call       0x464220

// block 004045ad
004045ad  add        esp, 4

// block 004045b0
004045b0  or         eax, 0xffffffff
004045b3  pop        ebp
004045b4  ret        4

// block 004045b7
004045b7  mov        ecx, dword ptr [ebp + 0x10]
004045ba  lea        eax, [ecx + 0x90]
004045c0  mov        dword ptr [ebp + 0x14], eax
004045c3  mov        edx, dword ptr [ecx + 4]
004045c6  add        edx, ecx
004045c8  mov        dword ptr [ebp + 0x18], edx
004045cb  mov        eax, dword ptr [ecx + 8]
004045ce  add        eax, ecx
004045d0  mov        dword ptr [ebp + 0x1c], eax
004045d3  xor        edx, edx
004045d5  xor        eax, eax
004045d7  cmp        dx, word ptr [ecx]
004045da  jge        0x4045f7

// block 004045dc
004045dc  lea        esp, [esp]

// block 004045e0
004045e0  mov        ecx, dword ptr [ebp + 0x14]
004045e3  mov        edx, dword ptr [ebp + 0x10]
004045e6  add        dword ptr [ecx + eax*4], edx
004045e9  lea        ecx, [ecx + eax*4]
004045ec  mov        ecx, dword ptr [ebp + 0x10]
004045ef  movsx      edx, word ptr [ecx]
004045f2  inc        eax
004045f3  cmp        eax, edx
004045f5  jl         0x4045e0

// block 004045f7
004045f7  mov        eax, dword ptr [ebp + 0x10]
004045fa  movsx      eax, word ptr [eax + 2]
004045fe  imul       eax, eax, 0x4b4
00404604  push       eax
00404605  call       0x46d04a

// block 0040460a
0040460a  add        esp, 4
0040460d  mov        dword ptr [ebp + 0x1c8], eax
00404613  xor        eax, eax
00404615  pop        ebp
00404616  ret        4
