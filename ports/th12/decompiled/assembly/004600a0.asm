
// block 004600a0
004600a0  sub        esp, 0x70
004600a3  push       ebx
004600a4  push       ebp
004600a5  mov        ebp, dword ptr [esp + 0x8c]
004600ac  push       esi
004600ad  push       edi
004600ae  test       ebp, ebp
004600b0  jne        0x4600d1

// block 004600b2
004600b2  push       0x4a33b8

// block 004600b7
004600b7  mov        edi, 0x4b0ec8
004600bc  call       0x464300

// block 004600c1
004600c1  add        esp, 4
004600c4  or         eax, 0xffffffff
004600c7  pop        edi
004600c8  pop        esi
004600c9  pop        ebp
004600ca  pop        ebx
004600cb  add        esp, 0x70
004600ce  ret        0x14

// block 004600d1
004600d1  cmp        dword ptr [ebp], 7
004600d5  je         0x4600de

// block 004600d7
004600d7  push       0x4a33f4
004600dc  jmp        0x4600b7

// block 004600de
004600de  cmp        byte ptr [ebp + 0x20], 0
004600e2  jne        0x4601e5

// block 004600e8
004600e8  mov        eax, dword ptr [ebp + 0x10]
004600eb  add        eax, ebp
004600ed  cmp        byte ptr [eax], 0x40
004600f0  mov        dword ptr [esp + 0x14], eax
004600f4  jne        0x460174

// block 004600f6
004600f6  cmp        byte ptr [eax + 1], 0x52
004600fa  jne        0x46012a

// block 004600fc
004600fc  mov        ebx, dword ptr [esp + 0x88]
00460103  mov        edi, dword ptr [esp + 0x84]
0046010a  mov        esi, dword ptr [edi + 0x120]
00460110  lea        ebx, [ebx + ebx*4]
00460113  add        ebx, ebx
00460115  add        ebx, ebx
00460117  add        esi, ebx
00460119  mov        dword ptr [esp + 0x94], ebx
00460120  call       0x45fbf0

// block 00460125
00460125  jmp        0x46023e

// block 0046012a
0046012a  mov        eax, dword ptr [esp + 0x88]
00460131  movzx      ecx, word ptr [ebp + 0xa]
00460135  mov        edx, dword ptr [esp + 0x84]
0046013c  mov        esi, dword ptr [edx + 0x120]
00460142  movzx      edi, word ptr [ebp + 0xe]
00460146  movzx      ebx, word ptr [ebp + 0xc]
0046014a  lea        eax, [eax + eax*4]
0046014d  add        eax, eax
0046014f  add        eax, eax
00460151  push       ecx
00460152  add        esi, eax
00460154  mov        dword ptr [esp + 0x98], eax
0046015b  call       0x45fba0

// block 00460160
00460160  mov        ecx, dword ptr [esp + 0x84]
00460167  add        dword ptr [ecx + 0x12c], eax
0046016d  mov        edi, ecx
0046016f  jmp        0x460237

// block 00460174
00460174  mov        eax, dword ptr [esp + 0x88]
0046017b  mov        ebx, dword ptr [esp + 0x84]
00460182  mov        esi, dword ptr [ebx + 0x120]
00460188  movsx      ecx, word ptr [ebp + 0x14]
0046018c  movzx      edi, word ptr [ebp + 0xa]
00460190  lea        edx, [eax + eax*4]
00460193  movsx      eax, word ptr [ebp + 0x16]
00460197  push       eax
00460198  movzx      eax, word ptr [ebp + 0xc]
0046019c  add        edx, edx
0046019e  add        edx, edx
004601a0  push       eax
004601a1  movzx      eax, word ptr [ebp + 0xe]
004601a5  add        esi, edx
004601a7  mov        dword ptr [esp + 0x9c], edx
004601ae  call       0x45f880

// block 004601b3
004601b3  test       eax, eax
004601b5  jge        0x4601db

// block 004601b7
004601b7  mov        ecx, dword ptr [esp + 0x14]
004601bb  push       ecx
004601bc  push       0x4a3478
004601c1  mov        edi, 0x4b0ec8
004601c6  call       0x464300

// block 004601cb
004601cb  add        esp, 8
004601ce  or         eax, 0xffffffff
004601d1  pop        edi
004601d2  pop        esi
004601d3  pop        ebp
004601d4  pop        ebx
004601d5  add        esp, 0x70
004601d8  ret        0x14

// block 004601db
004601db  add        dword ptr [ebx + 0x12c], eax
004601e1  mov        edi, ebx
004601e3  jmp        0x460237

// block 004601e5
004601e5  movzx      eax, word ptr [ebp + 0xc]
004601e9  movzx      ecx, word ptr [ebp + 0xa]
004601ed  mov        edx, dword ptr [esp + 0x88]
004601f4  mov        esi, dword ptr [esp + 0x84]
004601fb  mov        edi, dword ptr [esi + 0x120]
00460201  lea        edx, [edx + edx*4]
00460204  push       eax
00460205  movzx      eax, word ptr [ebp + 0xe]
00460209  add        edx, edx
0046020b  push       ecx
0046020c  mov        ecx, dword ptr [ebp + 0x1c]
0046020f  add        edx, edx
00460211  add        ecx, ebp
00460213  add        edi, edx
00460215  mov        dword ptr [esp + 0x9c], edx
0046021c  call       0x45fa80

// block 00460221
00460221  test       eax, eax
00460223  jge        0x46022f

// block 00460225
00460225  push       0x4a34bc
0046022a  jmp        0x4600b7

// block 0046022f
0046022f  add        dword ptr [esi + 0x12c], eax
00460235  mov        edi, esi

// block 00460237
00460237  mov        ebx, dword ptr [esp + 0x94]

// block 0046023e
0046023e  mov        edx, dword ptr [edi + 0x120]
00460244  mov        ecx, dword ptr [edx + ebx]
00460247  lea        eax, [edx + ebx]
0046024a  mov        edx, dword ptr [ecx]
0046024c  mov        eax, dword ptr [eax]
0046024e  lea        ecx, [esp + 0x18]
00460252  push       ecx
00460253  mov        ecx, dword ptr [edx + 0x44]
00460256  push       0
00460258  push       eax
00460259  call       ecx

// block 0046025b
0046025b  xor        edx, edx
0046025d  lea        esi, [ebp + 0x40]
00460260  mov        dword ptr [esp + 0x84], 0
0046026b  cmp        dx, word ptr [ebp + 4]
0046026f  jae        0x460381

// block 00460275
00460275  jmp        0x460287

// block 00460280
00460280  mov        ebx, dword ptr [esp + 0x94]

// block 00460287
00460287  mov        edx, dword ptr [edi + 0x120]
0046028d  fild       dword ptr [esp + 0x30]
00460291  mov        ecx, dword ptr [edi]
00460293  mov        eax, dword ptr [esi]
00460295  add        edx, ebx
00460297  mov        dword ptr [esp + 0x40], edx
0046029b  mov        edx, dword ptr [esp + 0x30]
0046029f  mov        dword ptr [esp + 0x38], ecx
004602a3  mov        ecx, dword ptr [esp + 0x88]
004602aa  add        eax, ebp
004602ac  mov        dword ptr [esp + 0x3c], ecx
004602b0  test       edx, edx
004602b2  jge        0x4602ba

// block 004602b4
004602b4  fadd       dword ptr [0x4a3e10]

// block 004602ba
004602ba  movzx      ecx, word ptr [ebp + 0xa]
004602be  fstp       dword ptr [esp + 0x14]
004602c2  fld        dword ptr [esp + 0x14]
004602c6  mov        dword ptr [esp + 0x14], ecx
004602ca  fld        st(0)
004602cc  mov        edx, dword ptr [esp + 0x34]
004602d0  fild       dword ptr [esp + 0x14]
004602d4  fdivp      st(1)
004602d6  fstp       dword ptr [esp + 0x74]
004602da  fild       dword ptr [esp + 0x34]
004602de  test       edx, edx
004602e0  jge        0x4602e8

// block 004602e2
004602e2  fadd       dword ptr [0x4a3e10]

// block 004602e8
004602e8  movzx      ecx, word ptr [ebp + 0xc]
004602ec  fstp       dword ptr [esp + 0x14]
004602f0  fld        dword ptr [esp + 0x14]
004602f4  mov        dword ptr [esp + 0x14], ecx
004602f8  fld        st(0)
004602fa  lea        ebx, [esp + 0x38]
004602fe  mov        edx, edi
00460300  fild       dword ptr [esp + 0x14]
00460304  fdivp      st(1)
00460306  fstp       dword ptr [esp + 0x78]
0046030a  fld        dword ptr [eax + 4]
0046030d  fld        dword ptr [esp + 0x74]
00460311  fld        st(0)
00460313  fmulp      st(2)
00460315  fxch       st(1)
00460317  fstp       dword ptr [esp + 0x44]
0046031b  fld        dword ptr [eax + 8]
0046031e  fld        dword ptr [esp + 0x78]
00460322  fld        st(0)
00460324  fmulp      st(2)
00460326  fxch       st(1)
00460328  fstp       dword ptr [esp + 0x48]
0046032c  fld        dword ptr [eax + 0xc]
0046032f  fadd       dword ptr [eax + 4]
00460332  fmulp      st(2)
00460334  fxch       st(1)
00460336  fstp       dword ptr [esp + 0x4c]
0046033a  fld        dword ptr [eax + 0x10]
0046033d  fadd       dword ptr [eax + 8]
00460340  mov        eax, dword ptr [esp + 0x8c]
00460347  fmulp      st(1)
00460349  fstp       dword ptr [esp + 0x50]
0046034d  fxch       st(1)
0046034f  fstp       dword ptr [esp + 0x58]
00460353  fstp       dword ptr [esp + 0x54]
00460357  call       0x460610

// block 0046035c
0046035c  mov        eax, dword ptr [esp + 0x84]
00460363  movzx      edx, word ptr [ebp + 4]
00460367  inc        dword ptr [esp + 0x8c]
0046036e  inc        eax
0046036f  add        esi, 4
00460372  cmp        eax, edx
00460374  mov        dword ptr [esp + 0x84], eax
0046037b  jl         0x460280

// block 00460381
00460381  xor        eax, eax
00460383  xor        edx, edx
00460385  cmp        ax, word ptr [ebp + 6]
00460389  jae        0x4603bc

// block 0046038b
0046038b  mov        eax, dword ptr [esp + 0x90]
00460392  add        eax, eax
00460394  add        eax, eax
00460396  lea        ecx, [esi + 4]
00460399  lea        esp, [esp]

// block 004603a0
004603a0  mov        esi, dword ptr [ecx]
004603a2  mov        ebx, dword ptr [edi + 0x11c]
004603a8  add        esi, ebp
004603aa  mov        dword ptr [eax + ebx], esi
004603ad  movzx      esi, word ptr [ebp + 6]
004603b1  inc        edx
004603b2  add        eax, 4
004603b5  add        ecx, 8
004603b8  cmp        edx, esi
004603ba  jl         0x4603a0

// block 004603bc
004603bc  pop        edi
004603bd  pop        esi
004603be  pop        ebp
004603bf  mov        eax, 1
004603c4  pop        ebx
004603c5  add        esp, 0x70
004603c8  ret        0x14
