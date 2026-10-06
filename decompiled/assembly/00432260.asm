
// block 00432260
00432260  sub        esp, 0x10
00432263  push       ebx
00432264  push       ebp
00432265  mov        ebp, dword ptr [esp + 0x1c]
00432269  push       esi
0043226a  mov        esi, dword ptr [0x4b43b8]
00432270  push       edi
00432271  mov        edi, dword ptr [0x4ce8cc]
00432277  mov        dword ptr [esi + 0x18f94], 1
00432281  mov        eax, dword ptr [ebp + 0x1ec]
00432287  push       eax
00432288  mov        edx, edi
0043228a  call       0x461920

// block 0043228f
0043228f  xor        ebx, ebx
00432291  cmp        eax, ebx
00432293  jne        0x43229d

// block 00432295
00432295  mov        dword ptr [ebp + 0x1ec], ebx
0043229b  jmp        0x4322f2

// block 0043229d
0043229d  mov        ecx, dword ptr [ebp + 0x1ec]
004322a3  push       ecx
004322a4  mov        edx, edi
004322a6  call       0x461920

// block 004322ab
004322ab  cmp        eax, ebx
004322ad  jne        0x4322b5

// block 004322af
004322af  mov        dword ptr [ebp + 0x1ec], ebx

// block 004322b5
004322b5  add        eax, 0x10
004322b8  cmp        eax, ebx
004322ba  je         0x4322f2

// block 004322bc
004322bc  mov        ecx, 0x4f

// block 004322c1
004322c1  mov        edx, dword ptr [eax]
004322c3  cmp        word ptr [edx + 0x3ea], cx
004322ca  je         0x4322d5

// block 004322cc
004322cc  mov        eax, dword ptr [eax + 4]
004322cf  cmp        eax, ebx
004322d1  jne        0x4322c1

// block 004322d3
004322d3  jmp        0x4322f2

// block 004322d5
004322d5  mov        eax, edx
004322d7  cmp        eax, ebx
004322d9  je         0x4322f2

// block 004322db
004322db  mov        eax, dword ptr [eax + 0x3bc]
004322e1  mov        ecx, dword ptr [0x4ceaac]
004322e7  or         eax, 0xff000000
004322ec  mov        dword ptr [ecx + 0x3bc], eax

// block 004322f2
004322f2  mov        eax, dword ptr [ebp + 4]
004322f5  add        eax, -9
004322f8  cmp        eax, 0x10
004322fb  ja         0x4326b7

// block 00432301
00432301  movzx      edx, byte ptr [eax + 0x4326e0]
00432308  jmp        dword ptr [edx*4 + 0x4326d0]

// block 0043230f
0043230f  fld        dword ptr [0x4a3f50]
00432315  lea        eax, [ebp + 0x204]
0043231b  fstp       dword ptr [esp + 0x14]
0043231f  xor        edi, edi
00432321  fld        dword ptr [0x4a3e04]
00432327  mov        dword ptr [esp + 0x24], eax
0043232b  fstp       dword ptr [esp + 0x18]
0043232f  fldz       
00432331  fstp       dword ptr [esp + 0x1c]

// block 00432335
00432335  mov        ecx, dword ptr [ebp + 0x38]
00432338  mov        edx, dword ptr [esp + 0x24]
0043233c  sub        ecx, edi
0043233e  neg        ecx
00432340  sbb        ecx, ecx
00432342  and        ecx, 0xff808180
00432348  add        ecx, 0xffffff00
0043234e  mov        dword ptr [esi + 0x18f80], ecx
00432354  mov        eax, dword ptr [edx]
00432356  test       eax, eax
00432358  je         0x4323be

// block 0043235a
0043235a  mov        esi, dword ptr [eax + 0x1c]
0043235d  lea        eax, [esi + 0xc]
00432360  push       eax
00432361  call       0x46d8e4

// block 00432366
00432366  mov        ecx, dword ptr [esi + 0x68]
00432369  mov        edx, dword ptr [ecx*4 + 0x4aee88]
00432370  mov        ecx, dword ptr [esi + 0x64]
00432373  push       edx
00432374  mov        edx, dword ptr [ecx*4 + 0x4aee4c]
0043237b  mov        ecx, dword ptr [esi + 0x5c]
0043237e  push       edx
0043237f  mov        edx, dword ptr [esi + 0x60]
00432382  lea        ecx, [edx + ecx*2]
00432385  mov        edx, dword ptr [ecx*4 + 0x4aee20]
0043238c  mov        ecx, dword ptr [eax + 0xc]
0043238f  push       edx
00432390  mov        edx, dword ptr [eax + 0x10]
00432393  mov        eax, dword ptr [eax + 0x14]
00432396  push       ecx
00432397  inc        edx
00432398  push       edx
00432399  cdq        
0043239a  mov        ecx, 0x64
0043239f  idiv       ecx
004323a1  inc        edi
004323a2  lea        ebx, [esp + 0x2c]
004323a6  push       edx
004323a7  push       esi
004323a8  mov        esi, dword ptr [0x4b43b8]
004323ae  push       edi
004323af  push       0x4a0f4c
004323b4  call       0x4015c0

// block 004323b9
004323b9  add        esp, 0x28
004323bc  jmp        0x4323d1

// block 004323be
004323be  inc        edi
004323bf  push       edi
004323c0  push       0x4a0f70
004323c5  lea        ebx, [esp + 0x1c]
004323c9  call       0x4015c0

// block 004323ce
004323ce  add        esp, 8

// block 004323d1
004323d1  fld        dword ptr [esp + 0x18]
004323d5  add        dword ptr [esp + 0x24], 4
004323da  cmp        edi, 0x19
004323dd  fadd       qword ptr [0x4a3ef8]
004323e3  mov        esi, dword ptr [0x4b43b8]
004323e9  fstp       dword ptr [esp + 0x18]
004323ed  jl         0x432335

// block 004323f3
004323f3  jmp        0x4326ad

// block 004323f8
004323f8  mov        eax, dword ptr [ebp + 0x14]
004323fb  fldz       
004323fd  cmp        eax, 0xa
00432400  fstp       dword ptr [esp + 0x1c]
00432404  mov        edi, dword ptr [ebp + 0x38]
00432407  mov        dword ptr [esp + 0x24], edi
0043240b  mov        dword ptr [esp + 0x10], eax
0043240f  jge        0x432437

// block 00432411
00432411  fild       dword ptr [esp + 0x24]
00432415  fmul       qword ptr [0x4a3ef8]
0043241b  fadd       qword ptr [0x4a3e88]
00432421  fld        qword ptr [0x4a40b0]
00432427  fsub       st(1)
00432429  fimul      dword ptr [esp + 0x10]
0043242d  fdiv       qword ptr [0x4a3e40]
00432433  faddp      st(1)
00432435  jmp        0x43243d

// block 00432437
00432437  fld        dword ptr [0x4a40d4]

// block 0043243d
0043243d  fstp       dword ptr [esp + 0x18]
00432441  mov        ecx, dword ptr [esp + 0x18]
00432445  fld        dword ptr [0x4a40d0]
0043244b  sub        esp, 0xc
0043244e  mov        eax, esp
00432450  fstp       dword ptr [esp + 0x20]
00432454  mov        edx, dword ptr [esp + 0x20]
00432458  mov        dword ptr [eax], edx
0043245a  mov        edx, dword ptr [esp + 0x28]
0043245e  mov        dword ptr [eax + 4], ecx
00432461  push       ebp
00432462  mov        dword ptr [eax + 8], edx
00432465  call       0x4320f0

// block 0043246a
0043246a  fld        dword ptr [0x4a3f50]
00432470  mov        eax, dword ptr [0x4b4518]
00432475  fstp       dword ptr [esp + 0x14]
00432479  mov        esi, dword ptr [eax + 0x1c]
0043247c  lea        ecx, [esi + 0xc]
0043247f  push       ecx
00432480  call       0x46d8e4

// block 00432485
00432485  mov        edx, dword ptr [0x4b0cb0]
0043248b  mov        ecx, dword ptr [edx*4 + 0x4aee88]
00432492  mov        edx, dword ptr [esi + 0x64]
00432495  push       ecx
00432496  mov        ecx, dword ptr [edx*4 + 0x4aee4c]
0043249d  mov        edx, dword ptr [esi + 0x5c]
004324a0  push       ecx
004324a1  mov        ecx, dword ptr [esi + 0x60]
004324a4  lea        edx, [ecx + edx*2]
004324a7  mov        ecx, dword ptr [edx*4 + 0x4aee20]
004324ae  mov        edx, dword ptr [eax + 0xc]
004324b1  push       ecx
004324b2  mov        ecx, dword ptr [eax + 0x10]
004324b5  mov        eax, dword ptr [eax + 0x14]
004324b8  push       edx
004324b9  inc        ecx
004324ba  push       ecx
004324bb  cdq        
004324bc  mov        ecx, 0x64
004324c1  idiv       ecx
004324c3  mov        esi, dword ptr [0x4b43b8]
004324c9  inc        edi
004324ca  lea        ebx, [esp + 0x2c]
004324ce  push       edx
004324cf  push       edi
004324d0  push       0x4a0f98
004324d5  call       0x4015c0

// block 004324da
004324da  mov        esi, dword ptr [0x4b43b8]
004324e0  add        esp, 0x24
004324e3  jmp        0x4326b7

// block 004324e8
004324e8  fld        dword ptr [0x4a3f50]
004324ee  mov        edx, dword ptr [ebp + 0x38]
004324f1  fstp       dword ptr [esp + 0x14]
004324f5  push       0x4a0fc4
004324fa  fldz       
004324fc  lea        ebx, [esp + 0x18]
00432500  fstp       dword ptr [esp + 0x20]
00432504  mov        dword ptr [esp + 0x28], edx
00432508  fld        dword ptr [0x4a3e04]
0043250e  fstp       dword ptr [esp + 0x1c]
00432512  call       0x4015c0

// block 00432517
00432517  fild       dword ptr [esp + 0x28]
0043251b  add        esp, 4
0043251e  cmp        dword ptr [ebp + 0x1f8], 0
00432525  fmul       qword ptr [0x4a3f08]
0043252b  fadd       qword ptr [0x4a40c8]
00432531  fstp       dword ptr [esp + 0x18]
00432535  fld        dword ptr [0x4a40c4]
0043253b  fstp       dword ptr [esp + 0x14]
0043253f  jne        0x432562

// block 00432541
00432541  mov        ecx, dword ptr [esp + 0x14]
00432545  mov        edx, dword ptr [esp + 0x18]
00432549  sub        esp, 0xc
0043254c  mov        eax, esp
0043254e  mov        dword ptr [eax], ecx
00432550  mov        ecx, dword ptr [esp + 0x28]
00432554  mov        dword ptr [eax + 4], edx
00432557  push       ebp
00432558  mov        dword ptr [eax + 8], ecx
0043255b  call       0x4320f0

// block 00432560
00432560  jmp        0x43256a

// block 00432562
00432562  mov        dword ptr [esp + 0x24], 0xffffffff

// block 0043256a
0043256a  fld        dword ptr [0x4a3f50]
00432570  xor        edi, edi
00432572  fstp       dword ptr [esp + 0x14]
00432576  fld        dword ptr [0x4a40c0]
0043257c  fstp       dword ptr [esp + 0x18]

// block 00432580
00432580  mov        esi, dword ptr [0x4b43b8]
00432586  mov        ebx, dword ptr [0x4b451c]
0043258c  mov        edx, edi
0043258e  sub        edx, dword ptr [esp + 0x24]
00432592  neg        edx
00432594  sbb        edx, edx
00432596  and        edx, 0xff808180
0043259c  add        edx, 0xffffff00
004325a2  mov        dword ptr [esi + 0x18f80], edx
004325a8  mov        eax, dword ptr [0x4b0c94]
004325ad  mov        ecx, dword ptr [0x4b0c90]
004325b3  lea        ecx, [eax + ecx*2]
004325b6  mov        eax, dword ptr [0x4b0ca8]
004325bb  imul       ecx, ecx, 0x45f4
004325c1  lea        edx, [eax + eax*4]
004325c4  lea        edx, [edi + edx*2]
004325c7  lea        eax, [edx*8]
004325ce  sub        eax, edx
004325d0  add        eax, eax
004325d2  add        eax, eax
004325d4  lea        edx, [eax + ecx]
004325d7  mov        ebp, dword ptr [edx + ebx + 0x28]
004325db  add        edx, ebx
004325dd  or         ebp, dword ptr [edx + 0x2c]
004325e0  je         0x432670

// block 004325e6
004325e6  lea        ecx, [ecx + ebx + 8]
004325ea  lea        eax, [eax + ecx + 0x20]
004325ee  push       eax
004325ef  call       0x46d8e4

// block 004325f4
004325f4  mov        ecx, dword ptr [0x4b0c94]
004325fa  mov        edx, dword ptr [0x4b0c90]
00432600  lea        edx, [ecx + edx*2]
00432603  mov        ecx, dword ptr [0x4b0ca8]
00432609  imul       edx, edx, 0x45f4
0043260f  add        edx, dword ptr [0x4b451c]
00432615  lea        ecx, [ecx + ecx*4]
00432618  lea        ecx, [edi + ecx*2]
0043261b  lea        esi, [ecx*8]
00432622  sub        esi, ecx
00432624  lea        ecx, [edx + esi*4]
00432627  movsx      edx, byte ptr [ecx + 0x1c]
0043262b  mov        edx, dword ptr [edx*4 + 0x4aee60]
00432632  push       edx
00432633  mov        edx, dword ptr [eax + 0xc]
00432636  push       edx
00432637  mov        edx, dword ptr [eax + 0x10]
0043263a  mov        eax, dword ptr [eax + 0x14]
0043263d  inc        edx
0043263e  push       edx
0043263f  cdq        
00432640  mov        esi, 0x64
00432645  idiv       esi
00432647  movsx      eax, byte ptr [ecx + 0x1d]
0043264b  mov        esi, dword ptr [0x4b43b8]
00432651  add        ecx, 0x1e
00432654  inc        edi
00432655  lea        ebx, [esp + 0x24]
00432659  push       edx
0043265a  mov        edx, dword ptr [ecx - 6]
0043265d  push       eax
0043265e  push       edx
0043265f  push       ecx
00432660  push       edi
00432661  push       0x4a0fe0
00432666  call       0x4015c0

// block 0043266b
0043266b  add        esp, 0x28
0043266e  jmp        0x432690

// block 00432670
00432670  movsx      eax, byte ptr [edx + 0x1d]
00432674  mov        ecx, dword ptr [edx + 0x18]
00432677  push       eax
00432678  push       ecx
00432679  add        edx, 0x1e
0043267c  push       edx
0043267d  inc        edi
0043267e  push       edi
0043267f  push       0x4a1004
00432684  lea        ebx, [esp + 0x28]
00432688  call       0x4015c0

// block 0043268d
0043268d  add        esp, 0x14

// block 00432690
00432690  cmp        edi, 0xa
00432693  fld        dword ptr [esp + 0x18]
00432697  fadd       qword ptr [0x4a3f08]
0043269d  fstp       dword ptr [esp + 0x18]
004326a1  jl         0x432580

// block 004326a7
004326a7  mov        esi, dword ptr [0x4b43b8]

// block 004326ad
004326ad  mov        dword ptr [esi + 0x18f80], 0xffffffff

// block 004326b7
004326b7  pop        edi
004326b8  mov        dword ptr [esi + 0x18f94], 0
004326c2  pop        esi
004326c3  pop        ebp
004326c4  mov        eax, 1
004326c9  pop        ebx
004326ca  add        esp, 0x10
004326cd  ret        4
