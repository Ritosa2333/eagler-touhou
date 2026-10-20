
// block 00450080
00450080  test       byte ptr [0x4cf428], 0x10
00450087  push       ebx
00450088  push       ebp
00450089  mov        ebp, dword ptr [esp + 0xc]
0045008d  push       esi
0045008e  push       edi
0045008f  je         0x4500bb

// block 00450091
00450091  fld        qword ptr [ebp + 0x40]
00450094  fcomp      qword ptr [ebp + 0x50]
00450097  fnstsw     ax
00450099  test       ah, 0x41
0045009c  jne        0x4500bb

// block 0045009e
0045009e  fld        qword ptr [0x4a3de8]

// block 004500a4
004500a4  fld        qword ptr [ebp + 0x50]
004500a7  fadd       st(1)
004500a9  fstp       qword ptr [ebp + 0x50]
004500ac  fld        qword ptr [ebp + 0x40]
004500af  fcomp      qword ptr [ebp + 0x50]
004500b2  fnstsw     ax
004500b4  test       ah, 0x41
004500b7  je         0x4500a4

// block 004500b9
004500b9  fstp       st(0)

// block 004500bb
004500bb  mov        esi, dword ptr [0x4ce8cc]
004500c1  call       0x45a3c0

// block 004500c6
004500c6  mov        edi, 0x4cec04
004500cb  mov        dword ptr [0x4cee34], edi
004500d1  call       0x430910

// block 004500d6
004500d6  mov        edx, dword ptr [0x4cee34]
004500dc  mov        eax, dword ptr [0x4ce8f0]
004500e1  mov        ecx, dword ptr [eax]
004500e3  add        edx, 0xcc
004500e9  push       edx
004500ea  push       eax
004500eb  mov        eax, dword ptr [ecx + 0xbc]
004500f1  call       eax

// block 004500f3
004500f3  mov        ebx, dword ptr [0x4ce89c]
004500f9  mov        dword ptr [0x4cee38], 1
00450103  call       0x4624c0

// block 00450108
00450108  test       eax, eax
0045010a  jne        0x450122

// block 0045010c
0045010c  mov        esi, 0x4cf0d8
00450111  call       0x464c40

// block 00450116
00450116  mov        eax, 1
0045011b  pop        edi
0045011c  pop        esi
0045011d  pop        ebp
0045011e  pop        ebx
0045011f  ret        4

// block 00450122
00450122  cmp        eax, -1
00450125  jne        0x45013d

// block 00450127
00450127  mov        esi, 0x4cf0d8
0045012c  call       0x464c40

// block 00450131
00450131  mov        eax, 2
00450136  pop        edi
00450137  pop        esi
00450138  pop        ebp
00450139  pop        ebx
0045013a  ret        4

// block 0045013d
0045013d  inc        byte ptr [ebp + 0x14]
00450140  mov        al, byte ptr [ebp + 0x14]
00450143  movzx      ecx, byte ptr [0x4ceace]
0045014a  movsx      edx, al
0045014d  inc        ecx
0045014e  cmp        ecx, edx
00450150  jg         0x4501ba

// block 00450152
00450152  mov        eax, dword ptr [0x4ce8f0]
00450157  mov        ecx, dword ptr [eax]
00450159  mov        edx, dword ptr [ecx + 0xa4]
0045015f  push       eax
00450160  call       edx

// block 00450162
00450162  call       0x45a380

// block 00450167
00450167  mov        edi, 0x4ce8e8
0045016c  mov        dword ptr [0x4cf278], 0xff
00450176  call       0x430300

// block 0045017b
0045017b  call       0x462620

// block 00450180
00450180  mov        esi, dword ptr [0x4ce8cc]
00450186  call       0x45a3c0

// block 0045018b
0045018b  mov        eax, dword ptr [0x4ce8f0]
00450190  mov        ecx, dword ptr [eax]
00450192  mov        edx, dword ptr [ecx + 0x104]
00450198  push       0
0045019a  push       0
0045019c  push       eax
0045019d  call       edx

// block 0045019f
0045019f  mov        eax, dword ptr [0x4ce8f0]
004501a4  mov        ecx, dword ptr [eax]
004501a6  mov        edx, dword ptr [ecx + 0xa8]
004501ac  push       eax
004501ad  call       edx

// block 004501af
004501af  mov        esi, ebp
004501b1  mov        byte ptr [ebp + 0x14], 0
004501b5  call       0x4501e0

// block 004501ba
004501ba  call       0x4508b0

// block 004501bf
004501bf  fsub       qword ptr [ebp + 0x40]
004501c2  pop        edi
004501c3  pop        esi
004501c4  pop        ebp
004501c5  fstp       qword ptr [0x4cf2a0]
004501cb  xor        eax, eax
004501cd  pop        ebx
004501ce  ret        4
