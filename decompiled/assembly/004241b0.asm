
// block 004241b0
004241b0  push       ebp
004241b1  mov        ebp, dword ptr [esp + 8]
004241b5  push       esi
004241b6  xor        esi, esi
004241b8  test       ebp, ebp
004241ba  jle        0x4241f3

// block 004241bc
004241bc  lea        esp, [esp]

// block 004241c0
004241c0  mov        ecx, dword ptr [edi + esi*8]
004241c3  mov        eax, ebx

// block 004241c5
004241c5  mov        dl, byte ptr [eax]
004241c7  cmp        dl, byte ptr [ecx]
004241c9  jne        0x4241e5

// block 004241cb
004241cb  test       dl, dl
004241cd  je         0x4241e1

// block 004241cf
004241cf  mov        dl, byte ptr [eax + 1]
004241d2  cmp        dl, byte ptr [ecx + 1]
004241d5  jne        0x4241e5

// block 004241d7
004241d7  add        eax, 2
004241da  add        ecx, 2
004241dd  test       dl, dl
004241df  jne        0x4241c5

// block 004241e1
004241e1  xor        eax, eax
004241e3  jmp        0x4241ea

// block 004241e5
004241e5  sbb        eax, eax
004241e7  sbb        eax, -1

// block 004241ea
004241ea  test       eax, eax
004241ec  je         0x4241fa

// block 004241ee
004241ee  inc        esi
004241ef  cmp        esi, ebp
004241f1  jl         0x4241c0

// block 004241f3
004241f3  pop        esi
004241f4  xor        eax, eax
004241f6  pop        ebp
004241f7  ret        4

// block 004241fa
004241fa  mov        eax, dword ptr [edi + esi*8 + 4]
004241fe  pop        esi
004241ff  pop        ebp
00424200  ret        4
