
// block 004726ff
004726ff  mov        edi, edi
00472701  push       ebp
00472702  mov        ebp, esp
00472704  push       dword ptr [ebp + 0x14]
00472707  push       0
00472709  push       dword ptr [ebp + 0x10]
0047270c  push       dword ptr [ebp + 0xc]
0047270f  push       dword ptr [ebp + 8]
00472712  push       0x471154
00472717  call       0x472414

// block 0047271c
0047271c  add        esp, 0x18
0047271f  test       eax, eax
00472721  jge        0x472726

// block 00472723
00472723  or         eax, 0xffffffff

// block 00472726
00472726  pop        ebp
00472727  ret        
