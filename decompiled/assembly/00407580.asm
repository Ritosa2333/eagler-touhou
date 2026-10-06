
// block 00407580
00407580  sub        esp, 0x1c
00407583  fld        dword ptr [0x4a3d78]
00407589  mov        ecx, dword ptr [ebx + 0x50c]
0040758f  mov        eax, dword ptr [ebx + 0x508]
00407595  fstp       dword ptr [esp + 0x10]
00407599  fld        dword ptr [0x4a3ec0]
0040759f  mov        edx, dword ptr [ebx + 0x510]
004075a5  fstp       dword ptr [esp + 0x14]
004075a9  mov        dword ptr [esp + 8], ecx
004075ad  fldz       
004075af  mov        dword ptr [esp + 4], eax
004075b3  mov        eax, dword ptr [0x4b43cc]
004075b8  fstp       dword ptr [esp + 0x18]
004075bc  fld        dword ptr [esp + 8]
004075c0  mov        ecx, dword ptr [eax + 0x7c]
004075c3  fadd       qword ptr [0x4a3d70]
004075c9  push       esi
004075ca  not        ecx
004075cc  push       edi
004075cd  and        ecx, 1
004075d0  fstp       dword ptr [esp + 8]
004075d4  mov        dword ptr [esp + 0x14], edx
004075d8  fld        dword ptr [esp + 8]
004075dc  push       ecx
004075dd  fsub       qword ptr [0x4a3ee0]
004075e3  lea        edx, [esp + 0x1c]
004075e7  push       edx
004075e8  lea        eax, [esp + 0x14]
004075ec  fstp       dword ptr [esp + 0x18]
004075f0  call       0x40cd00

// block 004075f5
004075f5  mov        eax, dword ptr [0x4b43cc]
004075fa  mov        ecx, dword ptr [eax + 0x7c]
004075fd  not        ecx
004075ff  and        ecx, 1
00407602  push       ecx
00407603  lea        esi, [esp + 0x1c]
00407607  lea        edi, [esp + 0x10]
0040760b  call       0x4285f0

// block 00407610
00407610  fld        dword ptr [0x4a3db8]
00407616  mov        eax, dword ptr [ebx + 0x50c]
0040761c  fstp       dword ptr [esp + 0x18]
00407620  fld        dword ptr [0x4a434c]
00407626  mov        edx, dword ptr [ebx + 0x508]
0040762c  mov        ecx, dword ptr [ebx + 0x510]
00407632  fstp       dword ptr [esp + 0x1c]
00407636  mov        dword ptr [esp + 0x10], eax
0040763a  fld        dword ptr [esp + 0x10]
0040763e  fsub       qword ptr [0x4a40b0]
00407644  mov        dword ptr [esp + 0xc], edx
00407648  mov        edx, dword ptr [0x4b43cc]
0040764e  mov        dword ptr [esp + 0x14], ecx
00407652  fstp       dword ptr [esp + 0x10]
00407656  mov        eax, dword ptr [edx + 0x7c]
00407659  not        eax
0040765b  and        eax, 1
0040765e  push       eax
0040765f  mov        ecx, esi
00407661  push       ecx
00407662  mov        eax, edi
00407664  call       0x40cd00

// block 00407669
00407669  mov        edx, dword ptr [0x4b43cc]
0040766f  mov        eax, dword ptr [edx + 0x7c]
00407672  not        eax
00407674  and        eax, 1
00407677  push       eax
00407678  call       0x4285f0

// block 0040767d
0040767d  fld        dword ptr [0x4a3fc8]
00407683  mov        edx, dword ptr [ebx + 0x50c]
00407689  fstp       dword ptr [esp + 0x18]
0040768d  fld        dword ptr [0x4a3ebc]
00407693  mov        ecx, dword ptr [ebx + 0x508]
00407699  mov        eax, dword ptr [ebx + 0x510]
0040769f  fstp       dword ptr [esp + 0x1c]
004076a3  mov        dword ptr [esp + 0x10], edx
004076a7  fld        dword ptr [esp + 0x10]
004076ab  fsub       qword ptr [0x4a3d70]
004076b1  mov        dword ptr [esp + 0xc], ecx
004076b5  mov        dword ptr [esp + 0x14], eax
004076b9  fstp       dword ptr [esp + 8]
004076bd  fld        dword ptr [esp + 8]
004076c1  fsub       qword ptr [0x4a43a0]
004076c7  fstp       dword ptr [esp + 0x10]
004076cb  mov        ecx, dword ptr [0x4b43cc]
004076d1  mov        edx, dword ptr [ecx + 0x7c]
004076d4  not        edx
004076d6  and        edx, 1
004076d9  push       edx
004076da  mov        eax, esi
004076dc  push       eax
004076dd  mov        eax, edi
004076df  call       0x40cd00

// block 004076e4
004076e4  mov        ecx, dword ptr [0x4b43cc]
004076ea  mov        edx, dword ptr [ecx + 0x7c]
004076ed  not        edx
004076ef  and        edx, 1
004076f2  push       edx
004076f3  call       0x4285f0

// block 004076f8
004076f8  pop        edi
004076f9  xor        eax, eax
004076fb  pop        esi
004076fc  add        esp, 0x1c
004076ff  ret        
