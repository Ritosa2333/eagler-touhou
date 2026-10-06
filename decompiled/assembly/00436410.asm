
// block 00436410
00436410  push       -1
00436412  push       0x49773b
00436417  mov        eax, dword ptr fs:[0]
0043641d  push       eax
0043641e  sub        esp, 8
00436421  push       edi
00436422  mov        eax, dword ptr [0x4ad138]
00436427  xor        eax, esp
00436429  push       eax
0043642a  lea        eax, [esp + 0x10]
0043642e  mov        dword ptr fs:[0], eax
00436434  push       0xc59c
00436439  call       0x46c9ea

// block 0043643e
0043643e  add        esp, 4
00436441  mov        dword ptr [esp + 0xc], eax
00436445  mov        dword ptr [esp + 0x18], 0
0043644d  test       eax, eax
0043644f  je         0x43645b

// block 00436451
00436451  push       eax
00436452  call       0x4359a0

// block 00436457
00436457  mov        edi, eax
00436459  jmp        0x43645d

// block 0043645b
0043645b  xor        edi, edi

// block 0043645d
0043645d  mov        dword ptr [esp + 0x18], 0xffffffff
00436465  call       0x435ae0

// block 0043646a
0043646a  test       eax, eax
0043646c  je         0x436494

// block 0043646e
0043646e  test       edi, edi
00436470  je         0x436481

// block 00436472
00436472  push       edi
00436473  call       0x436270

// block 00436478
00436478  push       edi
00436479  call       0x46ca4f

// block 0043647e
0043647e  add        esp, 4

// block 00436481
00436481  xor        eax, eax
00436483  mov        ecx, dword ptr [esp + 0x10]
00436487  mov        dword ptr fs:[0], ecx
0043648e  pop        ecx
0043648f  pop        edi
00436490  add        esp, 0x14
00436493  ret        

// block 00436494
00436494  mov        eax, edi
00436496  mov        ecx, dword ptr [esp + 0x10]
0043649a  mov        dword ptr fs:[0], ecx
004364a1  pop        ecx
004364a2  pop        edi
004364a3  add        esp, 0x14
004364a6  ret        
