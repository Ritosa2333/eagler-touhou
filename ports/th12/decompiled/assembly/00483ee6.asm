
// block 00483ee6
00483ee6  xor        dl, dl
00483ee8  cmp        byte ptr [ecx], dl
00483eea  je         0x483f03

// block 00483eec
00483eec  push       ebx
00483eed  push       esi

// block 00483eee
00483eee  mov        al, byte ptr [ecx]
00483ef0  cmp        al, 0x30
00483ef2  jl         0x483f04

// block 00483ef4
00483ef4  cmp        al, 0x39
00483ef6  jg         0x483f04

// block 00483ef8
00483ef8  sub        al, 0x30
00483efa  mov        byte ptr [ecx], al

// block 00483efc
00483efc  inc        ecx

// block 00483efd
00483efd  cmp        byte ptr [ecx], dl
00483eff  jne        0x483eee

// block 00483f01
00483f01  pop        esi
00483f02  pop        ebx

// block 00483f03
00483f03  ret        

// block 00483f04
00483f04  cmp        al, 0x3b
00483f06  jne        0x483efc

// block 00483f08
00483f08  mov        eax, ecx

// block 00483f0a
00483f0a  lea        esi, [eax + 1]
00483f0d  mov        bl, byte ptr [esi]
00483f0f  mov        byte ptr [eax], bl
00483f11  mov        eax, esi
00483f13  cmp        byte ptr [eax], dl
00483f15  jne        0x483f0a

// block 00483f17
00483f17  jmp        0x483efd
