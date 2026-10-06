
// block 004da197
004da197  jecxz      0x4da15c

// block 004da199
004da199  fsubr      st(6)
004da19b  and        eax, 0x7f98cfde
004da1a0  movsd      dword ptr es:[edi], dword ptr [esi]
004da1a1  shr        dword ptr [ecx + 0x70], cl

// block 004da1a4
004da1a4  stosd      dword ptr es:[edi], eax

// block 004da1a5
004da1a5  xor        al, 0x67
004da1a7  dec        esp
004da1a8  mov        ah, 0x12
004da1aa  test       dword ptr [0x32709460], edx
004da1b0  jns        0x4da197

// block 004da1b2
004da1b2  nop        
004da1b3  inc        ebx
004da1b4  sub        cl, ah
004da1b6  out        0xe7, eax
004da1b8  mov        eax, 0x33b80e32
004da1bd  cmp        dword ptr [eax], 0x66
004da1c0  or         dword ptr [edi], eax
004da1c2  push       edi
004da1c3  xor        dword ptr [ebx], edi
004da1c5  shl        byte ptr [eax + 0x788bc24f], 1
004da1cb  push       eax
004da1cc  cli        

// block 004da1cd
004da1cd  salc       
004da1ce  aas        
004da1cf  sub        al, 0x71
004da1d1  push       edi
004da1d2  std        
004da1d3  dec        edx

// block 004da1d4
004da1d4  wait       
004da1d5  sub        edx, dword ptr [eax + 0x162884ab]
004da1db  xor        eax, 0x626f1643

// block 004da1e0
004da1e0  mov        ch, 0x5c
004da1e2  or         al, 0x83
004da1e4  sub        ch, byte ptr [edi - 0x5c]
004da1e7  and        eax, 0x9d0d0e9f
004da1ec  pop        ecx
004da1ee  cmpsd      dword ptr [esi], dword ptr es:[edi]
004da1ef  popal      
004da1f0  xor        ch, byte ptr [esi + 0x5a]
004da1f3  xchg       ebx, eax
004da1f4  adc        byte ptr [esi - 0x2a], al
004da1f7  jne        0x4da1e0

// block 004da1f9
004da1f9  jecxz      0x4da1d4

// block 004da1fb
004da1fb  adc        eax, 0x2ce38dac
004da200  sub        ebp, dword ptr [eax]
004da202  fmul       st(0)
004da204  jno        0x4da1a4

// block 004da206
004da206  xor        ebp, dword ptr [ecx]
004da208  jno        0x4da1a3

// block 004da20a
004da20a  xchg       ebp, eax
004da20b  in         al, dx
004da20c  pop        ebx
004da20d  or         dh, byte ptr [ebx + 0x6ec63c87]
004da213  pop        es
004da214  stosb      byte ptr es:[edi], al
004da215  mov        byte ptr [0xea1267c9], al
004da21a  adc        byte ptr [esi + 0x53], ah
004da21d  std        
004da21e  btc        dword ptr [esi - 0x143684a4], ebx
004da225  mov        dl, 0x7b
004da227  sbb        esp, eax

// block 004da22a
004da22a  mov        edi, 0xb6b22ad1
004da230  popfd      
004da231  sbb        al, bl

// block 004da233
004da233  dec        edi
004da234  lea        ecx, [ebx + 0x5a]
004da237  hlt        
