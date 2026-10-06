
// block 00421cb0
00421cb0  and        dword ptr [edi + 0x20], 0xfffffffe
00421cb4  push       esi
00421cb5  lea        esi, [edi + 0x24]
00421cb8  call       0x423250

// block 00421cbd
00421cbd  push       0x78
00421cbf  push       0
00421cc1  push       edi
00421cc2  call       0x477420

// block 00421cc7
00421cc7  add        esp, 0xc
00421cca  mov        eax, edi
00421ccc  pop        esi
00421ccd  ret        
