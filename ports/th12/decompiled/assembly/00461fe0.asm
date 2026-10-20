
// block 00461fe0
00461fe0  add        ecx, 0x10
00461fe3  push       esi
00461fe4  je         0x462009

// block 00461fe6
00461fe6  jmp        0x461ff0

// block 00461ff0
00461ff0  mov        esi, dword ptr [ecx]
00461ff2  movsx      esi, word ptr [esi + 0x3ea]
00461ff9  cmp        esi, edx
00461ffb  je         0x462011

// block 00461ffd
00461ffd  cmp        edx, -1
00462000  je         0x462011

// block 00462002
00462002  mov        ecx, dword ptr [ecx + 4]
00462005  test       ecx, ecx
00462007  jne        0x461ff0

// block 00462009
00462009  mov        dword ptr [eax], 0
0046200f  pop        esi
00462010  ret        

// block 00462011
00462011  mov        ecx, dword ptr [ecx]
00462013  mov        edx, dword ptr [ecx]
00462015  mov        dword ptr [eax], edx
00462017  pop        esi
00462018  ret        
