
// block 004102b0
004102b0  sub        esp, 8
004102b3  push       ebx
004102b4  push       ebp
004102b5  push       esi
004102b6  push       edi
004102b7  mov        esi, ecx
004102b9  push       0xd8
004102be  mov        ebp, edx
004102c0  mov        dword ptr [esp + 0x18], esi
004102c4  call       0x46d04a

// block 004102c9
004102c9  push       0xd8
004102ce  mov        edi, eax
004102d0  xor        ebx, ebx
004102d2  push       ebx
004102d3  push       edi
004102d4  mov        dword ptr [esi + 0x478], edi
004102da  call       0x477420

// block 004102df
004102df  fld        dword ptr [ebp]
004102e2  fstp       dword ptr [edi]
004102e4  add        esp, 0x10
004102e7  fld        dword ptr [ebp + 4]
004102ea  mov        esi, 0x4ce568
004102ef  fstp       dword ptr [edi + 4]
004102f2  call       0x464440

// block 004102f7
004102f7  mov        dword ptr [esp + 0x10], eax
004102fb  fild       dword ptr [esp + 0x10]
004102ff  test       eax, eax
00410301  jge        0x410309

// block 00410303
00410303  fadd       dword ptr [0x4a3e10]

// block 00410309
00410309  fmul       qword ptr [0x4a3ea8]
0041030f  fsub       qword ptr [0x4a3ce0]
00410315  fstp       dword ptr [esp + 0x10]
00410319  fld        dword ptr [esp + 0x10]
0041031d  fmul       qword ptr [0x4a3cd8]
00410323  fstp       dword ptr [edi + 0xc0]
00410329  mov        eax, dword ptr [edi + 0xd4]
0041032f  test       al, 1
00410331  jne        0x41035e

// block 00410333
00410333  fldz       
00410335  or         eax, 1
00410338  fstp       dword ptr [edi + 0xcc]
0041033e  mov        dword ptr [edi + 0xc8], ebx
00410344  mov        dword ptr [edi + 0xc4], 0xfff0bdc1
0041034e  mov        dword ptr [edi + 0xd0], 0x4b2ed0
00410358  mov        dword ptr [edi + 0xd4], eax

// block 0041035e
0041035e  fld1       
00410360  mov        dword ptr [edi + 0xc8], 1
0041036a  fstp       dword ptr [edi + 0xcc]
00410370  mov        dword ptr [edi + 0xc4], ebx
00410376  mov        dword ptr [esp + 0x10], ebx
0041037a  xor        ecx, ecx
0041037c  lea        eax, [edi + 0x82]
00410382  mov        esi, 0xff0040ff

// block 00410387
00410387  or         bl, 0xff
0041038a  cmp        ecx, 8
0041038d  mov        dword ptr [eax - 2], esi
00410390  jge        0x41039d

// block 00410392
00410392  mov        dl, cl
00410394  shl        dl, 5
00410397  sub        bl, dl
00410399  mov        byte ptr [eax], bl
0041039b  jmp        0x4103ad

// block 0041039d
0041039d  mov        dl, byte ptr [esp + 0x10]
004103a1  shl        dl, 4
004103a4  sub        bl, dl
004103a6  inc        dword ptr [esp + 0x10]
004103aa  mov        byte ptr [eax + 1], bl

// block 004103ad
004103ad  inc        ecx
004103ae  add        eax, 4
004103b1  cmp        ecx, 0x10
004103b4  jl         0x410387

// block 004103b6
004103b6  mov        ecx, dword ptr [ebp]
004103b9  mov        eax, dword ptr [esp + 0x14]
004103bd  mov        dword ptr [eax + 0x430], ecx
004103c3  mov        edx, dword ptr [ebp + 4]
004103c6  pop        edi
004103c7  mov        dword ptr [eax + 0x434], edx
004103cd  mov        ecx, dword ptr [ebp + 8]
004103d0  pop        esi
004103d1  pop        ebp
004103d2  mov        dword ptr [eax + 0x438], ecx
004103d8  mov        dword ptr [eax + 0x20], 0xd
004103df  xor        eax, eax
004103e1  pop        ebx
004103e2  add        esp, 8
004103e5  ret        
