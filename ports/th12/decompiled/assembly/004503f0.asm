
// block 004503f0
004503f0  push       ebx
004503f1  push       ebp
004503f2  mov        ebp, dword ptr [esp + 0xc]
004503f6  push       esi
004503f7  push       edi
004503f8  call       0x4508b0

// block 004503fd
004503fd  fst        qword ptr [ebp + 0x40]
00450400  fcom       qword ptr [ebp + 0x48]
00450403  fnstsw     ax
00450405  test       ah, 5
00450408  jp         0x45040d

// block 0045040a
0045040a  fst        qword ptr [ebp + 0x50]

// block 0045040d
0045040d  fst        qword ptr [ebp + 0x48]
00450410  fsubr      qword ptr [ebp + 0x50]
00450413  fmul       qword ptr [0x4a3d30]
00450419  fcomp      qword ptr [0x4a4220]
0045041f  fnstsw     ax
00450421  test       ah, 1
00450424  jne        0x45042e

// block 00450426
00450426  push       1
00450428  call       dword ptr [0x498084]

// block 0045042e
0045042e  fld        qword ptr [ebp + 0x40]
00450431  fcomp      qword ptr [ebp + 0x50]
00450434  fnstsw     ax
00450436  test       ah, 0x41
00450439  jne        0x450574

// block 0045043f
0045043f  fld        qword ptr [ebp + 0x40]
00450442  fcomp      qword ptr [ebp + 0x50]
00450445  fnstsw     ax
00450447  test       ah, 0x41
0045044a  jne        0x450469

// block 0045044c
0045044c  fld        qword ptr [0x4a3de8]

// block 00450452
00450452  fld        qword ptr [ebp + 0x50]
00450455  fadd       st(1)
00450457  fstp       qword ptr [ebp + 0x50]
0045045a  fld        qword ptr [ebp + 0x40]
0045045d  fcomp      qword ptr [ebp + 0x50]
00450460  fnstsw     ax
00450462  test       ah, 0x41
00450465  je         0x450452

// block 00450467
00450467  fstp       st(0)

// block 00450469
00450469  mov        esi, dword ptr [0x4ce8cc]
0045046f  call       0x45a3c0

// block 00450474
00450474  mov        edi, 0x4cec04
00450479  mov        dword ptr [0x4cee34], edi
0045047f  call       0x430910

// block 00450484
00450484  mov        edx, dword ptr [0x4cee34]
0045048a  mov        eax, dword ptr [0x4ce8f0]
0045048f  mov        ecx, dword ptr [eax]
00450491  add        edx, 0xcc
00450497  push       edx
00450498  push       eax
00450499  mov        eax, dword ptr [ecx + 0xbc]
0045049f  call       eax

// block 004504a1
004504a1  mov        ebx, dword ptr [0x4ce89c]
004504a7  mov        dword ptr [0x4cee38], 1
004504b1  call       0x4624c0

// block 004504b6
004504b6  test       eax, eax
004504b8  jne        0x4504d0

// block 004504ba
004504ba  mov        esi, 0x4cf0d8
004504bf  call       0x464c40

// block 004504c4
004504c4  mov        eax, 1
004504c9  pop        edi
004504ca  pop        esi
004504cb  pop        ebp
004504cc  pop        ebx
004504cd  ret        4

// block 004504d0
004504d0  cmp        eax, -1
004504d3  jne        0x4504eb

// block 004504d5
004504d5  mov        esi, 0x4cf0d8
004504da  call       0x464c40

// block 004504df
004504df  mov        eax, 2
004504e4  pop        edi
004504e5  pop        esi
004504e6  pop        ebp
004504e7  pop        ebx
004504e8  ret        4

// block 004504eb
004504eb  inc        byte ptr [ebp + 0x14]
004504ee  mov        al, byte ptr [ebp + 0x14]
004504f1  movzx      ecx, byte ptr [0x4ceace]
004504f8  movsx      edx, al
004504fb  inc        ecx
004504fc  cmp        ecx, edx
004504fe  jg         0x450566

// block 00450500
00450500  mov        eax, dword ptr [0x4ce8f0]
00450505  mov        ecx, dword ptr [eax]
00450507  mov        edx, dword ptr [ecx + 0xa4]
0045050d  push       eax
0045050e  call       edx

// block 00450510
00450510  call       0x45a380

// block 00450515
00450515  mov        edi, 0x4ce8e8
0045051a  mov        dword ptr [0x4cf278], 0xff
00450524  call       0x430300

// block 00450529
00450529  call       0x462620

// block 0045052e
0045052e  mov        esi, dword ptr [0x4ce8cc]
00450534  call       0x45a3c0

// block 00450539
00450539  mov        eax, dword ptr [0x4ce8f0]
0045053e  mov        ecx, dword ptr [eax]
00450540  mov        edx, dword ptr [ecx + 0x104]
00450546  push       0
00450548  push       0
0045054a  push       eax
0045054b  call       edx

// block 0045054d
0045054d  mov        eax, dword ptr [0x4ce8f0]
00450552  mov        ecx, dword ptr [eax]
00450554  mov        edx, dword ptr [ecx + 0xa8]
0045055a  push       eax
0045055b  call       edx

// block 0045055d
0045055d  mov        byte ptr [ebp + 0x14], 0
00450561  call       0x450580

// block 00450566
00450566  call       0x4508b0

// block 0045056b
0045056b  fsub       qword ptr [ebp + 0x40]
0045056e  fstp       qword ptr [0x4cf2a0]

// block 00450574
00450574  pop        edi
00450575  pop        esi
00450576  pop        ebp
00450577  xor        eax, eax
00450579  pop        ebx
0045057a  ret        4
