
// block 004111b0
004111b0  push       -1
004111b2  push       0x4974de
004111b7  mov        eax, dword ptr fs:[0]
004111bd  push       eax
004111be  sub        esp, 8
004111c1  push       ebx
004111c2  push       ebp
004111c3  push       esi
004111c4  push       edi
004111c5  mov        eax, dword ptr [0x4ad138]
004111ca  xor        eax, esp
004111cc  push       eax
004111cd  lea        eax, [esp + 0x1c]
004111d1  mov        dword ptr fs:[0], eax
004111d7  mov        esi, dword ptr [esp + 0x2c]
004111db  mov        eax, 0xfffffffe
004111e0  and        dword ptr [esi + 0x14], eax
004111e3  and        dword ptr [esi + 0x28], eax
004111e6  and        dword ptr [esi + 0x3c], eax
004111e9  xor        ebp, ebp
004111eb  mov        dword ptr [esi + 0xd0], 0x4a3738
004111f5  mov        dword ptr [esi + 0xd4], ebp
004111fb  mov        dword ptr [esi + 0xd8], ebp
00411201  mov        dword ptr [esi + 0xdc], ebp
00411207  mov        dword ptr [esi + 0xe0], ebp
0041120d  push       0xf0
00411212  push       ebp
00411213  push       esi
00411214  mov        dword ptr [esp + 0x30], ebp
00411218  call       0x477420

// block 0041121d
0041121d  add        esp, 0xc
00411220  xor        eax, eax
00411222  mov        dword ptr [esp + 0x14], eax
00411226  lea        edi, [esi + 0x40]
00411229  lea        esp, [esp]

// block 00411230
00411230  mov        ecx, dword ptr [0x4cee70]
00411236  push       ebp
00411237  add        eax, 0x44
0041123a  push       eax
0041123b  lea        eax, [esp + 0x20]
0041123f  push       eax
00411240  push       ecx
00411241  mov        eax, 0x17
00411246  xor        ecx, ecx
00411248  call       0x4615a0

// block 0041124d
0041124d  mov        edx, dword ptr [esp + 0x18]
00411251  mov        ecx, edx
00411253  mov        dword ptr [edi], edx
00411255  mov        edx, dword ptr [0x4ce8cc]
0041125b  cmp        ecx, ebp
0041125d  jne        0x411263

// block 0041125f
0041125f  xor        eax, eax
00411261  jmp        0x41129e

// block 00411263
00411263  mov        eax, dword ptr [edx + 0x8856b8]
00411269  cmp        eax, ebp
0041126b  je         0x41127d

// block 0041126d
0041126d  lea        ecx, [ecx]

// block 00411270
00411270  mov        ebx, dword ptr [eax]
00411272  cmp        dword ptr [ebx], ecx
00411274  je         0x411298

// block 00411276
00411276  mov        eax, dword ptr [eax + 4]
00411279  cmp        eax, ebp
0041127b  jne        0x411270

// block 0041127d
0041127d  mov        eax, dword ptr [edx + 0x8856c0]
00411283  cmp        eax, ebp
00411285  je         0x41125f

// block 00411287
00411287  mov        ebx, dword ptr [eax]
00411289  cmp        dword ptr [ebx], ecx
0041128b  je         0x411298

// block 0041128d
0041128d  mov        eax, dword ptr [eax + 4]
00411290  cmp        eax, ebp
00411292  jne        0x411287

// block 00411294
00411294  xor        eax, eax
00411296  jmp        0x41129e

// block 00411298
00411298  mov        eax, ebx
0041129a  cmp        eax, ebp
0041129c  jne        0x4112a0

// block 0041129e
0041129e  mov        dword ptr [edi], ebp

// block 004112a0
004112a0  mov        byte ptr [eax + 0x49c], 0x10
004112a7  mov        ecx, dword ptr [edi]
004112a9  cmp        ecx, ebp
004112ab  jne        0x4112b1

// block 004112ad
004112ad  xor        eax, eax
004112af  jmp        0x4112ee

// block 004112b1
004112b1  mov        eax, dword ptr [edx + 0x8856b8]
004112b7  cmp        eax, ebp
004112b9  je         0x4112cd

// block 004112bb
004112bb  jmp        0x4112c0

// block 004112c0
004112c0  mov        ebx, dword ptr [eax]
004112c2  cmp        dword ptr [ebx], ecx
004112c4  je         0x4112e8

// block 004112c6
004112c6  mov        eax, dword ptr [eax + 4]
004112c9  cmp        eax, ebp
004112cb  jne        0x4112c0

// block 004112cd
004112cd  mov        eax, dword ptr [edx + 0x8856c0]
004112d3  cmp        eax, ebp
004112d5  je         0x4112ad

// block 004112d7
004112d7  mov        ebx, dword ptr [eax]
004112d9  cmp        dword ptr [ebx], ecx
004112db  je         0x4112e8

// block 004112dd
004112dd  mov        eax, dword ptr [eax + 4]
004112e0  cmp        eax, ebp
004112e2  jne        0x4112d7

// block 004112e4
004112e4  xor        eax, eax
004112e6  jmp        0x4112ee

// block 004112e8
004112e8  mov        eax, ebx
004112ea  cmp        eax, ebp
004112ec  jne        0x4112f0

// block 004112ee
004112ee  mov        dword ptr [edi], ebp

// block 004112f0
004112f0  mov        byte ptr [eax + 0x49d], 0x10
004112f7  mov        ecx, dword ptr [edi]
004112f9  cmp        ecx, ebp
004112fb  jne        0x411301

// block 004112fd
004112fd  xor        eax, eax
004112ff  jmp        0x411342

// block 00411301
00411301  mov        eax, dword ptr [edx + 0x8856b8]
00411307  cmp        eax, ebp
00411309  je         0x41131d

// block 0041130b
0041130b  jmp        0x411310

// block 00411310
00411310  mov        ebx, dword ptr [eax]
00411312  cmp        dword ptr [ebx], ecx
00411314  je         0x411338

// block 00411316
00411316  mov        eax, dword ptr [eax + 4]
00411319  cmp        eax, ebp
0041131b  jne        0x411310

// block 0041131d
0041131d  mov        eax, dword ptr [edx + 0x8856c0]
00411323  cmp        eax, ebp
00411325  je         0x4112fd

// block 00411327
00411327  mov        edx, dword ptr [eax]
00411329  cmp        dword ptr [edx], ecx
0041132b  je         0x41133c

// block 0041132d
0041132d  mov        eax, dword ptr [eax + 4]
00411330  cmp        eax, ebp
00411332  jne        0x411327

// block 00411334
00411334  xor        eax, eax
00411336  jmp        0x411342

// block 00411338
00411338  mov        eax, ebx
0041133a  jmp        0x41133e

// block 0041133c
0041133c  mov        eax, edx

// block 0041133e
0041133e  cmp        eax, ebp
00411340  jne        0x411344

// block 00411342
00411342  mov        dword ptr [edi], ebp

// block 00411344
00411344  or         dword ptr [eax + 0x480], 8
0041134b  mov        eax, dword ptr [esp + 0x14]
0041134f  inc        eax
00411350  add        edi, 4
00411353  mov        dword ptr [esp + 0x14], eax
00411357  cmp        eax, 5
0041135a  jb         0x411230

// block 00411360
00411360  mov        eax, dword ptr [esp + 0x30]
00411364  fldz       
00411366  mov        dword ptr [esi + 0x54], eax
00411369  mov        eax, dword ptr [esi + 0x14]
0041136c  mov        edx, 0xfff0bdc1
00411371  mov        ecx, 0x4b2ed0
00411376  test       al, 1
00411378  jne        0x41138c

// block 0041137a
0041137a  or         eax, 1
0041137d  fst        dword ptr [esi + 0xc]
00411380  mov        dword ptr [esi + 8], ebp
00411383  mov        dword ptr [esi + 4], edx
00411386  mov        dword ptr [esi + 0x10], ecx
00411389  mov        dword ptr [esi + 0x14], eax

// block 0041138c
0041138c  or         edi, 0xffffffff
0041138f  fst        dword ptr [esi + 0xc]
00411392  mov        dword ptr [esi + 8], ebp
00411395  mov        dword ptr [esi + 4], edi
00411398  mov        eax, dword ptr [esi + 0x28]
0041139b  test       al, 1
0041139d  jne        0x4113b1

// block 0041139f
0041139f  or         eax, 1
004113a2  fst        dword ptr [esi + 0x20]
004113a5  mov        dword ptr [esi + 0x1c], ebp
004113a8  mov        dword ptr [esi + 0x18], edx
004113ab  mov        dword ptr [esi + 0x24], ecx
004113ae  mov        dword ptr [esi + 0x28], eax

// block 004113b1
004113b1  fst        dword ptr [esi + 0x20]
004113b4  mov        dword ptr [esi + 0x1c], ebp
004113b7  mov        dword ptr [esi + 0x18], edi
004113ba  mov        eax, dword ptr [esi + 0x3c]
004113bd  test       al, 1
004113bf  jne        0x4113d3

// block 004113c1
004113c1  or         eax, 1
004113c4  fst        dword ptr [esi + 0x34]
004113c7  mov        dword ptr [esi + 0x30], ebp
004113ca  mov        dword ptr [esi + 0x2c], edx
004113cd  mov        dword ptr [esi + 0x38], ecx
004113d0  mov        dword ptr [esi + 0x3c], eax

// block 004113d3
004113d3  fstp       dword ptr [esi + 0x34]
004113d6  mov        dword ptr [esi + 0x30], ebp
004113d9  mov        dword ptr [esi + 0x2c], edi
004113dc  or         dword ptr [esi + 0x74], 1
004113e0  mov        dword ptr [esi + 0x7c], 0xffffff
004113e7  mov        eax, esi
004113e9  mov        ecx, dword ptr [esp + 0x1c]
004113ed  mov        dword ptr fs:[0], ecx
004113f4  pop        ecx
004113f5  pop        edi
004113f6  pop        esi
004113f7  pop        ebp
004113f8  pop        ebx
004113f9  add        esp, 0x14
004113fc  ret        8
