
// block 004694f0
004694f0  push       ebx
004694f1  mov        ebx, dword ptr [eax + 8]
004694f4  push       ebp
004694f5  push       esi
004694f6  push       edi
004694f7  xor        edi, edi
004694f9  sub        ebx, 1
004694fc  js         0x469557

// block 004694fe
004694fe  mov        ebp, dword ptr [eax + 0x8c]

// block 00469504
00469504  mov        ecx, dword ptr [esp + 0x14]
00469508  mov        eax, ebx
0046950a  sub        eax, edi
0046950c  cdq        
0046950d  sub        eax, edx
0046950f  mov        esi, eax
00469511  sar        esi, 1
00469513  add        esi, edi
00469515  mov        edx, dword ptr [ebp + esi*8]
00469519  lea        esp, [esp]

// block 00469520
00469520  mov        al, byte ptr [ecx]
00469522  cmp        al, byte ptr [edx]
00469524  jne        0x469540

// block 00469526
00469526  test       al, al
00469528  je         0x46953c

// block 0046952a
0046952a  mov        al, byte ptr [ecx + 1]
0046952d  cmp        al, byte ptr [edx + 1]
00469530  jne        0x469540

// block 00469532
00469532  add        ecx, 2
00469535  add        edx, 2
00469538  test       al, al
0046953a  jne        0x469520

// block 0046953c
0046953c  xor        eax, eax
0046953e  jmp        0x469545

// block 00469540
00469540  sbb        eax, eax
00469542  sbb        eax, -1

// block 00469545
00469545  test       eax, eax
00469547  je         0x469560

// block 00469549
00469549  jge        0x469550

// block 0046954b
0046954b  lea        ebx, [esi - 1]
0046954e  jmp        0x469553

// block 00469550
00469550  lea        edi, [esi + 1]

// block 00469553
00469553  cmp        edi, ebx
00469555  jle        0x469504

// block 00469557
00469557  pop        edi
00469558  pop        esi
00469559  pop        ebp
0046955a  xor        eax, eax
0046955c  pop        ebx
0046955d  ret        4

// block 00469560
00469560  mov        eax, dword ptr [ebp + esi*8 + 4]
00469564  pop        edi
00469565  pop        esi
00469566  pop        ebp
00469567  add        eax, 0x10
0046956a  pop        ebx
0046956b  ret        4
