
// block 004d9943
004d9943  test       al, 0x1c
004d9946  adc        dword ptr [ecx - 0x4b], edi

// block 004d9949
004d9949  pushal     
004d994b  loope      0x4d9945

// block 004d994d
004d994d  inc        edi
