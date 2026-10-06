
// block 00430410
00430410  push       ecx
00430411  push       ebp
00430412  mov        ebp, dword ptr [esp + 0xc]
00430416  push       esi
00430417  mov        esi, dword ptr [eax + 0x9a4]
0043041d  test       esi, esi
0043041f  jne        0x430429

// block 00430421
00430421  pop        esi
00430422  xor        eax, eax
00430424  pop        ebp
00430425  pop        ecx
00430426  ret        0xc

// block 00430429
00430429  push       ebx
0043042a  mov        ebx, dword ptr [eax + 0x9a0]
00430430  push       5
00430432  push       0x4a0d30
00430437  push       ebp
00430438  call       0x46dc03

// block 0043043d
0043043d  add        esp, 0xc
00430440  test       eax, eax
00430442  je         0x430479

// block 00430444
00430444  mov        ecx, 0x4a0d30
00430449  mov        eax, 0x4a0d38
0043044e  mov        edi, edi

// block 00430450
00430450  mov        dl, byte ptr [eax]
00430452  cmp        dl, byte ptr [ecx]
00430454  jne        0x430470

// block 00430456
00430456  test       dl, dl
00430458  je         0x43046c

// block 0043045a
0043045a  mov        dl, byte ptr [eax + 1]
0043045d  cmp        dl, byte ptr [ecx + 1]
00430460  jne        0x430470

// block 00430462
00430462  add        eax, 2
00430465  add        ecx, 2
00430468  test       dl, dl
0043046a  jne        0x430450

// block 0043046c
0043046c  xor        eax, eax
0043046e  jmp        0x430475

// block 00430470
00430470  sbb        eax, eax
00430472  sbb        eax, -1

// block 00430475
00430475  test       eax, eax
00430477  jne        0x430482

// block 00430479
00430479  pop        ebx
0043047a  pop        esi
0043047b  xor        eax, eax
0043047d  pop        ebp
0043047e  pop        ecx
0043047f  ret        0xc

// block 00430482
00430482  push       edi
00430483  test       ebx, ebx
00430485  jbe        0x4304dc

// block 00430487
00430487  push       5
00430489  push       esi
0043048a  push       ebp
0043048b  call       0x46dc03

// block 00430490
00430490  add        esp, 0xc
00430493  test       eax, eax
00430495  jne        0x4304c6

// block 00430497
00430497  lea        eax, [esp + 0x10]
0043049b  push       eax
0043049c  lea        ecx, [esp + 0x1c]
004304a0  push       ecx
004304a1  add        esi, 6
004304a4  push       0x4a0d40
004304a9  push       esi
004304aa  call       0x46dd2d

// block 004304af
004304af  mov        edx, dword ptr [esp + 0x2c]
004304b3  add        esp, 0x10
004304b6  cmp        dword ptr [esp + 0x18], edx
004304ba  jne        0x4304c6

// block 004304bc
004304bc  mov        eax, dword ptr [esp + 0x20]
004304c0  cmp        dword ptr [esp + 0x10], eax
004304c4  je         0x4304e7

// block 004304c6
004304c6  push       0xa
004304c8  push       esi
004304c9  mov        edi, esi
004304cb  call       0x46d1a0

// block 004304d0
004304d0  inc        eax
004304d1  mov        esi, eax
004304d3  sub        edi, esi
004304d5  add        esp, 8
004304d8  add        ebx, edi
004304da  jne        0x430487

// block 004304dc
004304dc  pop        edi
004304dd  pop        ebx
004304de  pop        esi
004304df  or         eax, 0xffffffff
004304e2  pop        ebp
004304e3  pop        ecx
004304e4  ret        0xc

// block 004304e7
004304e7  pop        edi
004304e8  pop        ebx
004304e9  pop        esi
004304ea  xor        eax, eax
004304ec  pop        ebp
004304ed  pop        ecx
004304ee  ret        0xc
