
// block 0046deef
0046deef  push       dword ptr [0x4b41dc]
0046def5  call       0x4751de

// block 0046defa
0046defa  pop        ecx
0046defb  test       eax, eax
0046defd  je         0x46df01

// block 0046deff
0046deff  call       eax

// block 0046df01
0046df01  push       0x19
0046df03  call       0x472f8d

// block 0046df08
0046df08  push       1
0046df0a  push       0
0046df0c  call       0x47a10a

// block 0046df11
0046df11  add        esp, 0xc
0046df14  jmp        0x479ff3
