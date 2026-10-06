
// block 00489006
00489006  mov        edi, edi
00489008  push       ebp
00489009  mov        ebp, esp
0048900b  xor        eax, eax

// block 0048900d
0048900d  mov        ecx, dword ptr [ebp + 8]
00489010  cmp        dword ptr [ecx + eax*4], 0
00489014  jne        0x489021

// block 00489016
00489016  inc        eax
00489017  cmp        eax, 3
0048901a  jl         0x48900d

// block 0048901c
0048901c  xor        eax, eax
0048901e  inc        eax
0048901f  pop        ebp
00489020  ret        

// block 00489021
00489021  xor        eax, eax
00489023  pop        ebp
00489024  ret        
