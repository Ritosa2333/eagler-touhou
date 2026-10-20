
// block 00427230
00427230  sub        esp, 8
00427233  push       ebx
00427234  push       ebp
00427235  push       esi
00427236  push       edi
00427237  mov        edi, eax
00427239  add        edi, 0x14
0042723c  lea        esi, [edi + 0x428]
00427242  mov        ebp, 0xa58
00427247  mov        ebx, 1
0042724c  lea        esp, [esp]

// block 00427250
00427250  cmp        dword ptr [esi + 0x588], 0
00427257  je         0x427361

// block 0042725d
0042725d  test       byte ptr [esi + 0x54], bl
00427260  je         0x427361

// block 00427266
00427266  fld        dword ptr [esi + 0x544]
0042726c  fadd       qword ptr [0x4a3d70]
00427272  fadd       qword ptr [0x4a3d28]
00427278  fstp       dword ptr [esi - 4]
0042727b  fld        dword ptr [esi + 0x548]
00427281  fadd       qword ptr [0x4a3d68]
00427287  fstp       dword ptr [esi]
00427289  fld        dword ptr [esi + 0x54c]
0042728f  fstp       dword ptr [esi + 4]
00427292  mov        eax, dword ptr [esi - 4]
00427295  fld        dword ptr [0x4a3e54]
0042729b  mov        ecx, dword ptr [esi]
0042729d  fcomp      dword ptr [esi]
0042729f  mov        edx, dword ptr [esi + 4]
004272a2  mov        dword ptr [esi + 0x4b0], eax
004272a8  mov        dword ptr [esi + 0x4b4], ecx
004272ae  mov        dword ptr [esi + 0x4b8], edx
004272b4  fnstsw     ax
004272b6  test       ah, 0x41
004272b9  jne        0x427349

// block 004272bf
004272bf  fld        dword ptr [esi + 0x4b4]
004272c5  fsub       qword ptr [0x4a4078]
004272cb  fstp       dword ptr [esp + 0x10]
004272cf  fld        dword ptr [0x4a3e00]
004272d5  fstp       dword ptr [esi + 0x4b4]
004272db  fld        dword ptr [0x4a3d78]
004272e1  fld        dword ptr [esp + 0x10]
004272e5  fcom       st(1)
004272e7  fnstsw     ax
004272e9  fstp       st(1)
004272eb  test       ah, 1
004272ee  jne        0x4272fb

// block 004272f0
004272f0  fstp       st(0)
004272f2  mov        byte ptr [esi + 0x44b], 0xff
004272f9  jmp        0x42732f

// block 004272fb
004272fb  fmul       qword ptr [0x4a4188]
00427301  fnstcw     word ptr [esp + 0x10]
00427305  movzx      eax, word ptr [esp + 0x10]
0042730a  fmul       qword ptr [0x4a3ed0]
00427310  or         eax, 0xc00
00427315  mov        dword ptr [esp + 0x14], eax
00427319  fldcw      word ptr [esp + 0x14]
0042731d  fistp      dword ptr [esp + 0x14]
00427321  mov        al, byte ptr [esp + 0x14]
00427325  mov        byte ptr [esi + 0x44b], al
0042732b  fldcw      word ptr [esp + 0x10]

// block 0042732f
0042732f  mov        ecx, dword ptr [0x4ce8cc]
00427335  lea        eax, [esi + 0x8c]
0042733b  push       ecx
0042733c  call       0x45c900

// block 00427341
00427341  mov        dword ptr [esi + 0x590], ebx
00427347  jmp        0x427361

// block 00427349
00427349  mov        edx, dword ptr [0x4ce8cc]
0042734f  push       edx
00427350  mov        eax, edi
00427352  call       0x45c900

// block 00427357
00427357  mov        dword ptr [esi + 0x590], 0

// block 00427361
00427361  add        edi, 0x9d8
00427367  add        esi, 0x9d8
0042736d  sub        ebp, ebx
0042736f  jne        0x427250

// block 00427375
00427375  pop        edi
00427376  pop        esi
00427377  pop        ebp
00427378  mov        eax, ebx
0042737a  pop        ebx
0042737b  add        esp, 8
0042737e  ret        
