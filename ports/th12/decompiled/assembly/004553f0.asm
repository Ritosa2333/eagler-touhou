
// block 004553f0
004553f0  lea        edx, [eax - 0x2710]
004553f6  cmp        edx, 0x16
004553f9  ja         0x455479

// block 004553fb
004553fb  movzx      edx, byte ptr [edx + 0x4554ac]
00455402  jmp        dword ptr [edx*4 + 0x45547c]

// block 00455409
00455409  mov        eax, dword ptr [ecx + 0x3fc]
0045540f  ret        

// block 00455410
00455410  mov        eax, dword ptr [ecx + 0x400]
00455416  ret        

// block 00455417
00455417  mov        eax, dword ptr [ecx + 0x404]
0045541d  ret        

// block 0045541e
0045541e  mov        eax, dword ptr [ecx + 0x408]
00455424  ret        

// block 00455425
00455425  fld        dword ptr [ecx + 0x40c]
0045542b  jmp        0x4931e0

// block 00455430
00455430  fld        dword ptr [ecx + 0x410]
00455436  jmp        0x4931e0

// block 0045543b
0045543b  fld        dword ptr [ecx + 0x414]
00455441  jmp        0x4931e0

// block 00455446
00455446  fld        dword ptr [ecx + 0x418]
0045544c  jmp        0x4931e0

// block 00455451
00455451  mov        eax, dword ptr [ecx + 0x41c]
00455457  ret        

// block 00455458
00455458  mov        eax, dword ptr [ecx + 0x420]
0045545e  ret        

// block 0045545f
0045545f  test       byte ptr [ecx + 0x480], 1
00455466  push       esi
00455467  mov        esi, 0x4ce560
0045546c  jne        0x455473

// block 0045546e
0045546e  mov        esi, 0x4ce568

// block 00455473
00455473  call       0x464440

// block 00455478
00455478  pop        esi

// block 00455479
00455479  ret        
