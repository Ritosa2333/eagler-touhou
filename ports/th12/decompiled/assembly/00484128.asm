
// block 00484128
00484128  xor        dl, dl
0048412a  cmp        byte ptr [ecx], dl
0048412c  je         0x484145

// block 0048412e
0048412e  push       ebx
0048412f  push       esi

// block 00484130
00484130  mov        al, byte ptr [ecx]
00484132  cmp        al, 0x30
00484134  jl         0x484146

// block 00484136
00484136  cmp        al, 0x39
00484138  jg         0x484146

// block 0048413a
0048413a  sub        al, 0x30
0048413c  mov        byte ptr [ecx], al

// block 0048413e
0048413e  inc        ecx

// block 0048413f
0048413f  cmp        byte ptr [ecx], dl
00484141  jne        0x484130

// block 00484143
00484143  pop        esi
00484144  pop        ebx

// block 00484145
00484145  ret        

// block 00484146
00484146  cmp        al, 0x3b
00484148  jne        0x48413e

// block 0048414a
0048414a  mov        eax, ecx

// block 0048414c
0048414c  lea        esi, [eax + 1]
0048414f  mov        bl, byte ptr [esi]
00484151  mov        byte ptr [eax], bl
00484153  mov        eax, esi
00484155  cmp        byte ptr [eax], dl
00484157  jne        0x48414c

// block 00484159
00484159  jmp        0x48413f
