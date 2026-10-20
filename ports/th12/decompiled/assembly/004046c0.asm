
// block 004046c0
004046c0  push       ebp
004046c1  mov        ebp, esp
004046c3  and        esp, 0xfffffff8
004046c6  sub        esp, 0x14
004046c9  mov        eax, dword ptr [ebp + 8]
004046cc  push       ebx
004046cd  push       esi
004046ce  mov        esi, dword ptr [eax + 0x18]
004046d1  push       edi
004046d2  mov        edi, 0x4ced1c
004046d7  mov        dword ptr [esp + 0xc], esi
004046db  mov        dword ptr [esp + 0x10], 1
004046e3  mov        dword ptr [0x4cee34], edi
004046e9  call       0x430a70

// block 004046ee
004046ee  mov        edx, dword ptr [0x4cee34]
004046f4  mov        eax, dword ptr [0x4ce8f0]
004046f9  mov        ecx, dword ptr [eax]
004046fb  add        edx, 0xcc
00404701  push       edx
00404702  push       eax
00404703  mov        eax, dword ptr [ecx + 0xbc]
00404709  call       eax

// block 0040470b
0040470b  mov        ecx, dword ptr [0x4ce8cc]
00404711  mov        dword ptr [0x4cee38], 2
0040471b  mov        byte ptr [ecx + 0x4b5644], 1
00404722  cmp        word ptr [esi], 0
00404726  jl         0x40498d

// block 0040472c
0040472c  movsx      edx, word ptr [esi]
0040472f  mov        eax, dword ptr [ebp + 8]
00404732  mov        ecx, dword ptr [eax + 0x14]
00404735  mov        ebx, dword ptr [ecx + edx*4]
00404738  movsx      edx, byte ptr [ebx + 2]
0040473c  cmp        edx, dword ptr [ebp + 0xc]
0040473f  jne        0x404956

// block 00404745
00404745  fld        dword ptr [esi + 4]
00404748  mov        edi, eax
0040474a  fstp       dword ptr [esp + 0x14]
0040474e  push       ecx
0040474f  fld        dword ptr [esi + 8]
00404752  mov        eax, 0x4ced1c
00404757  fstp       dword ptr [esp + 0x1c]
0040475b  lea        edx, [esp + 0x18]
0040475f  fld        dword ptr [esi + 0xc]
00404762  mov        ecx, ebx
00404764  fstp       dword ptr [esp + 0x20]
00404768  fld        dword ptr [edi + 0x276c]
0040476e  fstp       dword ptr [esp]
00404771  call       0x403f10

// block 00404776
00404776  test       eax, eax
00404778  je         0x40478e

// block 0040477a
0040477a  inc        dword ptr [edi + 0x35b4]
00404780  mov        eax, 0xfffe
00404785  and        word ptr [esi + 2], ax
00404789  jmp        0x404956

// block 0040478e
0040478e  or         byte ptr [ebx + 3], 2
00404792  movzx      eax, word ptr [ebx + 0x1c]
00404796  add        ebx, 0x1c
00404799  test       ax, ax
0040479c  jl         0x404947

// block 004047a2
004047a2  movsx      edi, word ptr [ebx + 6]
004047a6  mov        ecx, dword ptr [ebp + 8]
004047a9  imul       edi, edi, 0x4b4
004047af  add        edi, dword ptr [ecx + 0x1c8]
004047b5  test       ax, ax
004047b8  jne        0x404931

// block 004047be
004047be  mov        ecx, dword ptr [edi + 0x47c]
004047c4  mov        edx, ecx
004047c6  and        edx, 0xf800000
004047cc  cmp        edx, 0x2000000
004047d2  jb         0x40483c

// block 004047d4
004047d4  fld        dword ptr [ebx + 8]
004047d7  fadd       dword ptr [esi + 4]
004047da  fstp       dword ptr [edi + 0x430]
004047e0  fld        dword ptr [ebx + 0xc]
004047e3  fadd       dword ptr [esi + 8]
004047e6  fstp       dword ptr [edi + 0x434]
004047ec  fld        dword ptr [ebx + 0x10]
004047ef  fadd       dword ptr [esi + 0xc]
004047f2  fstp       dword ptr [edi + 0x438]
004047f8  fldz       
004047fa  fcom       dword ptr [ebx + 0x14]
004047fd  fnstsw     ax
004047ff  test       ah, 0x44
00404802  jnp        0x40481c

// block 00404804
00404804  fld        dword ptr [ebx + 0x14]
00404807  mov        eax, dword ptr [edi + 0x3f4]
0040480d  fdiv       dword ptr [eax + 0x38]
00404810  or         ecx, 8
00404813  mov        dword ptr [edi + 0x47c], ecx
00404819  fstp       dword ptr [edi + 0x40]

// block 0040481c
0040481c  fcomp      dword ptr [ebx + 0x18]
0040481f  fnstsw     ax
00404821  test       ah, 0x44
00404824  jnp        0x40483c

// block 00404826
00404826  fld        dword ptr [ebx + 0x18]
00404829  mov        ecx, dword ptr [edi + 0x3f4]
0040482f  fdiv       dword ptr [ecx + 0x34]
00404832  or         dword ptr [edi + 0x47c], 8
00404839  fstp       dword ptr [edi + 0x44]

// block 0040483c
0040483c  mov        edx, dword ptr [edi + 0x47c]
00404842  and        edx, 0xf800000
00404848  cmp        edx, 0x4000000
0040484e  jne        0x404872

// block 00404850
00404850  cmp        dword ptr [0x4cf278], 1
00404857  je         0x4048a8

// block 00404859
00404859  mov        esi, dword ptr [0x4ce8cc]
0040485f  call       0x45a3c0

// block 00404864
00404864  mov        dword ptr [0x4cf278], 1
0040486e  push       1
00404870  jmp        0x404892

// block 00404872
00404872  cmp        dword ptr [0x4cf278], 0
00404879  je         0x4048a8

// block 0040487b
0040487b  mov        esi, dword ptr [0x4ce8cc]
00404881  call       0x45a3c0

// block 00404886
00404886  mov        dword ptr [0x4cf278], 0
00404890  push       0

// block 00404892
00404892  mov        eax, dword ptr [0x4ce8f0]
00404897  mov        ecx, dword ptr [eax]
00404899  mov        edx, dword ptr [ecx + 0xe4]
0040489f  push       0x1c
004048a1  push       eax
004048a2  call       edx

// block 004048a4
004048a4  mov        esi, dword ptr [esp + 0xc]

// block 004048a8
004048a8  mov        eax, dword ptr [edi + 0x47c]
004048ae  shr        eax, 0xc
004048b1  and        eax, 1
004048b4  je         0x4048e6

// block 004048b6
004048b6  cmp        dword ptr [esp + 0x10], 0
004048bb  je         0x4048e6

// block 004048bd
004048bd  mov        esi, dword ptr [0x4ce8cc]
004048c3  call       0x45a3c0

// block 004048c8
004048c8  mov        eax, dword ptr [0x4ce8f0]
004048cd  mov        ecx, dword ptr [eax]
004048cf  mov        edx, dword ptr [ecx + 0xe4]
004048d5  push       0
004048d7  push       0xe
004048d9  push       eax
004048da  call       edx

// block 004048dc
004048dc  mov        dword ptr [esp + 0x10], 0
004048e4  jmp        0x404917

// block 004048e6
004048e6  test       eax, eax
004048e8  jne        0x40491b

// block 004048ea
004048ea  cmp        dword ptr [esp + 0x10], eax
004048ee  jne        0x40491b

// block 004048f0
004048f0  mov        esi, dword ptr [0x4ce8cc]
004048f6  call       0x45a3c0

// block 004048fb
004048fb  mov        eax, dword ptr [0x4ce8f0]
00404900  mov        ecx, dword ptr [eax]
00404902  mov        edx, dword ptr [ecx + 0xe4]
00404908  push       1
0040490a  push       0xe
0040490c  push       eax
0040490d  call       edx

// block 0040490f
0040490f  mov        dword ptr [esp + 0x10], 1

// block 00404917
00404917  mov        esi, dword ptr [esp + 0xc]

// block 0040491b
0040491b  mov        eax, dword ptr [0x4ce8cc]
00404920  push       eax
00404921  mov        eax, edi
00404923  call       0x45c900

// block 00404928
00404928  mov        eax, dword ptr [ebp + 8]
0040492b  inc        dword ptr [eax + 0x35b8]

// block 00404931
00404931  movsx      ecx, word ptr [ebx + 2]
00404935  movzx      eax, word ptr [ebx + ecx]
00404939  add        ebx, ecx
0040493b  test       ax, ax
0040493e  jge        0x4047a2

// block 00404944
00404944  mov        edi, dword ptr [ebp + 8]

// block 00404947
00404947  mov        eax, 1
0040494c  or         word ptr [esi + 2], ax
00404950  add        dword ptr [edi + 0x35b0], eax

// block 00404956
00404956  add        esi, 0x10
00404959  cmp        word ptr [esi], 0
0040495d  mov        dword ptr [esp + 0xc], esi
00404961  jge        0x40472c

// block 00404967
00404967  cmp        dword ptr [esp + 0x10], 0
0040496c  jne        0x40498d

// block 0040496e
0040496e  mov        esi, dword ptr [0x4ce8cc]
00404974  call       0x45a3c0

// block 00404979
00404979  mov        eax, dword ptr [0x4ce8f0]
0040497e  mov        edx, dword ptr [eax]
00404980  push       1
00404982  push       0xe
00404984  push       eax
00404985  mov        eax, dword ptr [edx + 0xe4]
0040498b  call       eax

// block 0040498d
0040498d  pop        edi
0040498e  pop        esi
0040498f  xor        eax, eax
00404991  pop        ebx
00404992  mov        esp, ebp
00404994  pop        ebp
00404995  ret        8
