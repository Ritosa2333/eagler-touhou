
// block 004728b7
004728b7  push       0xc
004728b9  push       0x4aab80
004728be  call       0x46fcec

// block 004728c3
004728c3  push       0xe
004728c5  call       0x46ec9a

// block 004728ca
004728ca  pop        ecx
004728cb  and        dword ptr [ebp - 4], 0
004728cf  mov        esi, dword ptr [ebp + 8]
004728d2  mov        ecx, dword ptr [esi + 4]
004728d5  test       ecx, ecx
004728d7  je         0x472908

// block 004728d9
004728d9  mov        eax, dword ptr [0x4b3d68]
004728de  mov        edx, 0x4b3d64

// block 004728e3
004728e3  mov        dword ptr [ebp - 0x1c], eax
004728e6  test       eax, eax
004728e8  je         0x4728fb

// block 004728ea
004728ea  cmp        dword ptr [eax], ecx
004728ec  jne        0x47291a

// block 004728ee
004728ee  mov        ecx, dword ptr [eax + 4]
004728f1  mov        dword ptr [edx + 4], ecx
004728f4  push       eax
004728f5  call       0x46c941

// block 004728fa
004728fa  pop        ecx

// block 004728fb
004728fb  push       dword ptr [esi + 4]
004728fe  call       0x46c941

// block 00472903
00472903  pop        ecx
00472904  and        dword ptr [esi + 4], 0

// block 00472908
00472908  mov        dword ptr [ebp - 4], 0xfffffffe
0047290f  call       0x47291e

// block 00472914
00472914  call       0x46fd31

// block 00472919
00472919  ret        

// block 0047291a
0047291a  mov        edx, eax
0047291c  jmp        0x4728e3
