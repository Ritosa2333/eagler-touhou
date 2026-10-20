
// block 004271c0
004271c0  mov        ecx, dword ptr [0x4b4534]
004271c6  cmp        dword ptr [ecx + 0x14], 0x3c
004271ca  jl         0x427210

// block 004271cc
004271cc  mov        ecx, dword ptr [ecx + 0x30]
004271cf  inc        ecx
004271d0  cmp        ecx, 4
004271d3  ja         0x427210

// block 004271d5
004271d5  jmp        dword ptr [ecx*4 + 0x427214]

// block 004271dc
004271dc  mov        ecx, dword ptr [eax + 0x9b4]
004271e2  cmp        ecx, 1
004271e5  je         0x4271ec

// block 004271e7
004271e7  cmp        ecx, 2
004271ea  jne        0x427210

// block 004271ec
004271ec  fldz       
004271ee  mov        dword ptr [eax + 0x9b0], 7
004271f8  fst        dword ptr [eax + 0x978]
004271fe  fst        dword ptr [eax + 0x97c]
00427204  fstp       dword ptr [eax + 0x9bc]
0042720a  mov        eax, 1
0042720f  ret        

// block 00427210
00427210  xor        eax, eax
00427212  ret        
