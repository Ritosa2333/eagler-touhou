
// block 00474438
00474438  mov        edi, edi
0047443a  push       ebp
0047443b  mov        ebp, esp
0047443d  push       ebx
0047443e  push       edi
0047443f  mov        edi, dword ptr [ebp + 0x10]
00474442  xor        ebx, ebx
00474444  cmp        edi, ebx
00474446  jle        0x474474

// block 00474448
00474448  push       esi
00474449  lea        esi, [ebp + 0x10]

// block 0047444c
0047444c  add        esi, 4
0047444f  push       dword ptr [esi]
00474451  push       dword ptr [ebp + 0xc]
00474454  push       dword ptr [ebp + 8]
00474457  call       0x483089

// block 0047445c
0047445c  add        esp, 0xc
0047445f  test       eax, eax
00474461  je         0x474470

// block 00474463
00474463  push       ebx
00474464  push       ebx
00474465  push       ebx
00474466  push       ebx
00474467  push       ebx
00474468  call       0x470dc6

// block 0047446d
0047446d  add        esp, 0x14

// block 00474470
00474470  dec        edi
00474471  jne        0x47444c

// block 00474473
00474473  pop        esi

// block 00474474
00474474  pop        edi
00474475  pop        ebx
00474476  pop        ebp
00474477  ret        
