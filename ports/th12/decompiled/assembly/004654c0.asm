
// block 004654c0
004654c0  mov        eax, ecx
004654c2  xor        ecx, ecx
004654c4  mov        word ptr [eax + 0x134], cx
004654cb  mov        edx, 1
004654d0  mov        word ptr [eax + 0x136], dx
004654d7  mov        ecx, 2
004654dc  mov        word ptr [eax + 0x138], cx
004654e3  mov        edx, 3
004654e8  mov        word ptr [eax + 0x144], dx
004654ef  mov        ecx, 4
004654f4  or         edx, 0xffffffff
004654f7  mov        word ptr [eax + 0x13a], cx
004654fe  or         ecx, edx
00465500  mov        word ptr [eax + 0x13c], dx
00465507  mov        word ptr [eax + 0x13e], cx
0046550e  or         edx, edx
00465510  or         ecx, 0xffffffff
00465513  mov        word ptr [eax + 0x140], dx
0046551a  mov        word ptr [eax + 0x142], cx
00465521  ret        
