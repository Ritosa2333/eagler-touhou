
// block 00472728
00472728  mov        edi, edi
0047272a  push       ebp
0047272b  mov        ebp, esp
0047272d  push       dword ptr [ebp + 0x18]
00472730  push       dword ptr [ebp + 0x14]
00472733  push       dword ptr [ebp + 0x10]
00472736  push       dword ptr [ebp + 0xc]
00472739  push       dword ptr [ebp + 8]
0047273c  push       0x471154
00472741  call       0x472414

// block 00472746
00472746  add        esp, 0x18
00472749  test       eax, eax
0047274b  jge        0x472750

// block 0047274d
0047274d  or         eax, 0xffffffff

// block 00472750
00472750  pop        ebp
00472751  ret        
