
// block 00472509
00472509  mov        edi, edi
0047250b  push       ebp
0047250c  mov        ebp, esp
0047250e  push       dword ptr [ebp + 0x18]
00472511  push       dword ptr [ebp + 0x14]
00472514  push       dword ptr [ebp + 0x10]
00472517  push       dword ptr [ebp + 0xc]
0047251a  push       dword ptr [ebp + 8]
0047251d  push       0x47021f
00472522  call       0x472414

// block 00472527
00472527  add        esp, 0x18
0047252a  test       eax, eax
0047252c  jge        0x472531

// block 0047252e
0047252e  or         eax, 0xffffffff

// block 00472531
00472531  pop        ebp
00472532  ret        
