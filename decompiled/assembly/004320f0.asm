
// block 004320f0
004320f0  sub        esp, 0xc
004320f3  push       ebx
004320f4  push       ebp
004320f5  mov        ebp, dword ptr [esp + 0x18]
004320f9  push       esi
004320fa  mov        esi, dword ptr [0x4b43b8]
00432100  push       edi
00432101  lea        eax, [ebp + 0x2cc]
00432107  push       eax
00432108  push       0x4a0f40
0043210d  lea        ebx, [esp + 0x2c]
00432111  call       0x4015c0

// block 00432116
00432116  mov        eax, dword ptr [ebp + 0x1f0]
0043211c  lea        ecx, [eax + eax*8]
0043211f  mov        dword ptr [esp + 0x28], ecx
00432123  fild       dword ptr [esp + 0x28]
00432127  add        esp, 8
0043212a  fadd       dword ptr [esp + 0x24]
0043212e  fstp       dword ptr [esp + 0x24]
00432132  cmp        eax, 8
00432135  jne        0x432145

// block 00432137
00432137  fld        dword ptr [esp + 0x24]
0043213b  fsub       qword ptr [0x4a3f18]
00432141  fstp       dword ptr [esp + 0x24]

// block 00432145
00432145  mov        esi, dword ptr [0x4b43b8]
0043214b  push       0x4a0f44
00432150  lea        ebx, [esp + 0x28]
00432154  mov        dword ptr [esi + 0x18f80], 0xffffff00
0043215e  call       0x4015c0

// block 00432163
00432163  fld        dword ptr [0x4a3f50]
00432169  mov        esi, dword ptr [0x4b43b8]
0043216f  mov        dword ptr [esi + 0x18f80], 0xffffffff
00432179  fstp       dword ptr [esp + 0x28]
0043217d  fld        dword ptr [0x4a3f28]
00432183  add        esp, 4
00432186  fstp       dword ptr [esp + 0x10]
0043218a  xor        edi, edi
0043218c  fld        dword ptr [0x4a3ef0]
00432192  fstp       dword ptr [esp + 0x14]
00432196  fldz       
00432198  fstp       dword ptr [esp + 0x18]
0043219c  jmp        0x4321a6

// block 004321a0
004321a0  mov        esi, dword ptr [0x4b43b8]

// block 004321a6
004321a6  mov        edx, dword ptr [ebp + 0x110]
004321ac  sub        edx, edi
004321ae  neg        edx
004321b0  sbb        edx, edx
004321b2  and        edx, 0xff808180
004321b8  add        edx, 0xffffff00
004321be  cmp        edi, 0x58
004321c1  mov        dword ptr [esi + 0x18f80], edx
004321c7  jge        0x4321d2

// block 004321c9
004321c9  movsx      eax, byte ptr [edi + 0x4a0e88]
004321d0  jmp        0x4321e6

// block 004321d2
004321d2  jne        0x4321db

// block 004321d4
004321d4  mov        eax, 0x81
004321d9  jmp        0x4321e6

// block 004321db
004321db  xor        eax, eax
004321dd  cmp        edi, 0x59
004321e0  setne      al
004321e3  add        eax, 0x7f

// block 004321e6
004321e6  push       eax
004321e7  push       0x4a0f48
004321ec  lea        ebx, [esp + 0x18]
004321f0  call       0x4015c0

// block 004321f5
004321f5  mov        eax, 0x4ec4ec4f
004321fa  mul        edi
004321fc  shr        edx, 2
004321ff  imul       edx, edx, 0xd
00432202  mov        ecx, edi
00432204  sub        ecx, edx
00432206  add        esp, 8
00432209  cmp        ecx, 0xc
0043220c  jne        0x432228

// block 0043220e
0043220e  fld        dword ptr [0x4a3f28]
00432214  fstp       dword ptr [esp + 0x10]
00432218  fld        dword ptr [esp + 0x14]
0043221c  fadd       qword ptr [0x4a3d68]
00432222  fstp       dword ptr [esp + 0x14]
00432226  jmp        0x432236

// block 00432228
00432228  fld        dword ptr [esp + 0x10]
0043222c  fadd       qword ptr [0x4a3f08]
00432232  fstp       dword ptr [esp + 0x10]

// block 00432236
00432236  inc        edi
00432237  cmp        edi, 0x5b
0043223a  jl         0x4321a0

// block 00432240
00432240  mov        edx, dword ptr [0x4b43b8]
00432246  pop        edi
00432247  pop        esi
00432248  pop        ebp
00432249  mov        dword ptr [edx + 0x18f80], 0xffffffff
00432253  pop        ebx
00432254  add        esp, 0xc
00432257  ret        0x10
