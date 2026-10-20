
// block 0047d5e8
0047d5e8  mov        ecx, dword ptr [0x4b4318]
0047d5ee  mov        dl, byte ptr [ecx]
0047d5f0  test       dl, dl
0047d5f2  jne        0x47d5f7

// block 0047d5f4
0047d5f4  xor        eax, eax
0047d5f6  ret        

// block 0047d5f7
0047d5f7  cmp        dl, 0x30
0047d5fa  jl         0x47d60f

// block 0047d5fc
0047d5fc  cmp        dl, 0x39
0047d5ff  jg         0x47d60f

// block 0047d601
0047d601  movsx      eax, dl
0047d604  sub        eax, 0x2f
0047d607  inc        ecx
0047d608  mov        dword ptr [0x4b4318], ecx
0047d60e  ret        

// block 0047d60f
0047d60f  xor        eax, eax
0047d611  jmp        0x47d634

// block 0047d613
0047d613  test       dl, dl
0047d615  je         0x47d5f4

// block 0047d617
0047d617  cmp        dl, 0x41
0047d61a  jl         0x47d647

// block 0047d61c
0047d61c  cmp        dl, 0x50
0047d61f  jg         0x47d647

// block 0047d621
0047d621  movsx      edx, dl
0047d624  shl        eax, 4
0047d627  inc        ecx
0047d628  lea        eax, [eax + edx - 0x41]
0047d62c  mov        dword ptr [0x4b4318], ecx
0047d632  mov        dl, byte ptr [ecx]

// block 0047d634
0047d634  cmp        dl, 0x40
0047d637  jne        0x47d613

// block 0047d639
0047d639  mov        dl, byte ptr [ecx]
0047d63b  inc        ecx
0047d63c  mov        dword ptr [0x4b4318], ecx
0047d642  cmp        dl, 0x40
0047d645  je         0x47d64a

// block 0047d647
0047d647  or         eax, 0xffffffff

// block 0047d64a
0047d64a  ret        
