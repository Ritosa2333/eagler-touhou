
// block 00431580
00431580  mov        eax, dword ptr [0x4cee70]
00431585  push       esi
00431586  push       0
00431588  push       0x52
0043158a  push       ebx
0043158b  push       eax
0043158c  mov        eax, 0x1d
00431591  xor        ecx, ecx
00431593  call       0x4615a0

// block 00431598
00431598  mov        ecx, dword ptr [ebx]
0043159a  mov        edx, dword ptr [0x4ce8cc]
004315a0  push       ecx
004315a1  call       0x461920

// block 004315a6
004315a6  mov        esi, eax
004315a8  test       esi, esi
004315aa  jne        0x4315ae

// block 004315ac
004315ac  mov        dword ptr [ebx], eax

// block 004315ae
004315ae  lea        edx, [edi*8]
004315b5  sub        edx, edi
004315b7  add        edx, edx
004315b9  add        edx, edx
004315bb  add        edx, edx
004315bd  push       edx
004315be  call       0x46d04a

// block 004315c3
004315c3  mov        ecx, dword ptr [esi + 0x47c]
004315c9  and        ecx, 0xf67fffff
004315cf  or         ecx, 0x6000000
004315d5  add        esp, 4
004315d8  cmp        edi, 2
004315db  mov        dword ptr [esi + 0x478], eax
004315e1  mov        dword ptr [esi + 0x47c], ecx
004315e7  mov        dword ptr [esi + 0x3fc], edi
004315ed  jle        0x43161d

// block 004315ef
004315ef  lea        ecx, [edi + edi]
004315f2  test       ecx, ecx
004315f4  jle        0x431629

// block 004315f6
004315f6  fldz       
004315f8  add        eax, 0xc
004315fb  fld1       
004315fd  or         edx, 0xffffffff

// block 00431600
00431600  fxch       st(1)
00431602  mov        dword ptr [eax + 4], edx
00431605  fst        dword ptr [eax - 4]
00431608  add        eax, 0x1c
0043160b  sub        ecx, 1
0043160e  fxch       st(1)
00431610  fst        dword ptr [eax - 0x1c]
00431613  jne        0x431600

// block 00431615
00431615  fstp       st(1)
00431617  mov        eax, ebx
00431619  fstp       st(0)
0043161b  pop        esi
0043161c  ret        

// block 0043161d
0043161d  and        ecx, 0xf07fffff
00431623  mov        dword ptr [esi + 0x47c], ecx

// block 00431629
00431629  mov        eax, ebx
0043162b  pop        esi
0043162c  ret        
