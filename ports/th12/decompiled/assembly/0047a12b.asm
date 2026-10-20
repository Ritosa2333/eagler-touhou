
// block 0047a12b
0047a12b  mov        edi, edi
0047a12d  push       ebp
0047a12e  mov        ebp, esp
0047a130  sub        esp, 0x94
0047a136  mov        eax, dword ptr [0x4ad138]
0047a13b  xor        eax, ebp
0047a13d  mov        dword ptr [ebp - 4], eax
0047a140  mov        eax, dword ptr [ebp + 8]
0047a143  push       ebx
0047a144  push       esi
0047a145  mov        esi, dword ptr [ebp + 0x18]
0047a148  xor        ebx, ebx
0047a14a  cmp        dword ptr [ebp + 0xc], 1
0047a14e  push       edi
0047a14f  mov        dword ptr [ebp - 0x8c], eax
0047a155  mov        dword ptr [ebp - 0x94], esi
0047a15b  jne        0x47a25f

// block 0047a161
0047a161  push       ebx
0047a162  push       0x80
0047a167  lea        edi, [ebp - 0x84]
0047a16d  mov        ecx, edi
0047a16f  push       ecx
0047a170  push       dword ptr [ebp + 0x14]
0047a173  mov        dword ptr [ebp - 0x88], ebx
0047a179  push       dword ptr [ebp + 0x10]
0047a17c  push       eax
0047a17d  call       0x48c424

// block 0047a182
0047a182  mov        esi, eax
0047a184  add        esp, 0x18
0047a187  cmp        esi, ebx
0047a189  jne        0x47a1f5

// block 0047a18b
0047a18b  call       dword ptr [0x4980e4]

// block 0047a191
0047a191  cmp        eax, 0x7a
0047a194  jne        0x47a21a

// block 0047a19a
0047a19a  push       ebx
0047a19b  push       ebx
0047a19c  push       ebx
0047a19d  push       dword ptr [ebp + 0x14]
0047a1a0  push       dword ptr [ebp + 0x10]
0047a1a3  push       dword ptr [ebp - 0x8c]
0047a1a9  call       0x48c424

// block 0047a1ae
0047a1ae  add        esp, 0x18
0047a1b1  mov        dword ptr [ebp - 0x90], eax
0047a1b7  cmp        eax, ebx
0047a1b9  je         0x47a21a

// block 0047a1bb
0047a1bb  xor        esi, esi
0047a1bd  inc        esi
0047a1be  push       esi
0047a1bf  push       eax
0047a1c0  call       0x477813

// block 0047a1c5
0047a1c5  mov        edi, eax
0047a1c7  pop        ecx
0047a1c8  pop        ecx
0047a1c9  cmp        edi, ebx
0047a1cb  je         0x47a21a

// block 0047a1cd
0047a1cd  push       ebx
0047a1ce  push       dword ptr [ebp - 0x90]
0047a1d4  mov        dword ptr [ebp - 0x88], esi
0047a1da  push       edi
0047a1db  push       dword ptr [ebp + 0x14]
0047a1de  push       dword ptr [ebp + 0x10]
0047a1e1  push       dword ptr [ebp - 0x8c]
0047a1e7  call       0x48c424

// block 0047a1ec
0047a1ec  mov        esi, eax
0047a1ee  add        esp, 0x18
0047a1f1  cmp        esi, ebx
0047a1f3  je         0x47a213

// block 0047a1f5
0047a1f5  push       1
0047a1f7  push       esi
0047a1f8  call       0x477813

// block 0047a1fd
0047a1fd  pop        ecx
0047a1fe  pop        ecx
0047a1ff  mov        ecx, dword ptr [ebp - 0x94]
0047a205  mov        dword ptr [ecx], eax
0047a207  cmp        eax, ebx
0047a209  jne        0x47a22c

// block 0047a20b
0047a20b  cmp        dword ptr [ebp - 0x88], ebx
0047a211  je         0x47a21a

// block 0047a213
0047a213  push       edi
0047a214  call       0x46c941

// block 0047a219
0047a219  pop        ecx

// block 0047a21a
0047a21a  or         eax, 0xffffffff

// block 0047a21d
0047a21d  mov        ecx, dword ptr [ebp - 4]
0047a220  pop        edi
0047a221  pop        esi
0047a222  xor        ecx, ebp
0047a224  pop        ebx
0047a225  call       0x46c932

// block 0047a22a
0047a22a  leave      
0047a22b  ret        

// block 0047a22c
0047a22c  lea        ecx, [esi - 1]
0047a22f  push       ecx
0047a230  push       edi
0047a231  push       esi
0047a232  push       eax
0047a233  call       0x4830fd

// block 0047a238
0047a238  add        esp, 0x10
0047a23b  test       eax, eax
0047a23d  je         0x47a24c

// block 0047a23f
0047a23f  push       ebx
0047a240  push       ebx
0047a241  push       ebx
0047a242  push       ebx
0047a243  push       ebx
0047a244  call       0x470dc6

// block 0047a249
0047a249  add        esp, 0x14

// block 0047a24c
0047a24c  cmp        dword ptr [ebp - 0x88], ebx
0047a252  je         0x47a25b

// block 0047a254
0047a254  push       edi
0047a255  call       0x46c941

// block 0047a25a
0047a25a  pop        ecx

// block 0047a25b
0047a25b  xor        eax, eax
0047a25d  jmp        0x47a21d

// block 0047a25f
0047a25f  cmp        dword ptr [ebp + 0xc], ebx
0047a262  jne        0x47a21a

// block 0047a264
0047a264  push       ebx
0047a265  push       4
0047a267  mov        edi, 0x4b41d4
0047a26c  push       edi
0047a26d  push       dword ptr [ebp + 0x14]
0047a270  push       dword ptr [ebp + 0x10]
0047a273  push       eax
0047a274  call       0x48c2b1

// block 0047a279
0047a279  add        esp, 0x18
0047a27c  test       eax, eax
0047a27e  je         0x47a21a

// block 0047a280
0047a280  mov        byte ptr [esi], bl

// block 0047a282
0047a282  mov        bl, byte ptr [edi]
0047a284  movzx      eax, bl
0047a287  push       eax
0047a288  call       0x48ba71

// block 0047a28d
0047a28d  pop        ecx
0047a28e  test       eax, eax
0047a290  je         0x47a25b

// block 0047a292
0047a292  mov        al, byte ptr [esi]
0047a294  mov        cl, 0xa
0047a296  imul       cl
0047a298  add        al, bl
0047a29a  sub        al, 0x30
0047a29c  inc        edi
0047a29d  inc        edi
0047a29e  cmp        edi, 0x4b41dc
0047a2a4  mov        byte ptr [esi], al
0047a2a6  jl         0x47a282

// block 0047a2a8
0047a2a8  jmp        0x47a25b
