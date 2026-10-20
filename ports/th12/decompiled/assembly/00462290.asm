
// block 00462290
00462290  lea        edx, [ecx + 0xbc]
00462296  cmp        eax, edx
00462298  jb         0x4622aa

// block 0046229a
0046229a  add        ecx, 0x4b40bc
004622a0  cmp        eax, ecx
004622a2  jae        0x4622aa

// block 004622a4
004622a4  mov        eax, 1
004622a9  ret        

// block 004622aa
004622aa  xor        eax, eax
004622ac  ret        
