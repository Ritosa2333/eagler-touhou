
// block 004d9702
004d9702  or         dword ptr [ebx], edi
004d9704  cmp        ah, dh
004d9706  test       byte ptr [ecx - 0x40], bh

// block 004d9774
004d9774  jecxz      0x4d9798

// block 004d9776
004d9776  jo         0x4d9702

// block 004d9778
004d9778  add        ah, byte ptr ds:[edi + 0x19]

// block 004d9798
004d9798  add        dword ptr ds:[esi + 0x7b], 0x797a9e41
004d97a0  js         0x4d97dd

// block 004d97a2
004d97a2  pop        es
004d97a3  xchg       ah, cl
004d97a5  pop        ss
004d97a6  add        ah, byte ptr [edx + edx*8 + 0x4603d918]
