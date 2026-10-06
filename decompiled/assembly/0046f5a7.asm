
// block 0046f5a7
0046f5a7  mov        eax, dword ptr [0x4b3d58]
0046f5ac  test       eax, eax
0046f5ae  je         0x46f673

// block 0046f5b4
0046f5b4  mov        ecx, dword ptr [0x4d6454]
0046f5ba  push       0x4000
0046f5bf  shl        ecx, 0xf
0046f5c2  add        ecx, dword ptr [eax + 0xc]
0046f5c5  push       0x8000
0046f5ca  push       ecx
0046f5cb  call       dword ptr [0x49818c]

// block 0046f5d1
0046f5d1  mov        ecx, dword ptr [0x4d6454]
0046f5d7  mov        eax, dword ptr [0x4b3d58]
0046f5dc  mov        edx, 0x80000000
0046f5e1  shr        edx, cl
0046f5e3  or         dword ptr [eax + 8], edx
0046f5e6  mov        eax, dword ptr [0x4b3d58]
0046f5eb  mov        eax, dword ptr [eax + 0x10]
0046f5ee  mov        ecx, dword ptr [0x4d6454]
0046f5f4  and        dword ptr [eax + ecx*4 + 0xc4], 0
0046f5fc  mov        eax, dword ptr [0x4b3d58]
0046f601  mov        eax, dword ptr [eax + 0x10]
0046f604  dec        byte ptr [eax + 0x43]
0046f607  mov        eax, dword ptr [0x4b3d58]
0046f60c  mov        ecx, dword ptr [eax + 0x10]
0046f60f  cmp        byte ptr [ecx + 0x43], 0
0046f613  jne        0x46f61e

// block 0046f615
0046f615  and        dword ptr [eax + 4], 0xfffffffe
0046f619  mov        eax, dword ptr [0x4b3d58]

// block 0046f61e
0046f61e  cmp        dword ptr [eax + 8], -1
0046f622  jne        0x46f66c

// block 0046f624
0046f624  cmp        dword ptr [0x4d6440], 1
0046f62b  jle        0x46f66c

// block 0046f62d
0046f62d  push       dword ptr [eax + 0x10]
0046f630  push       0
0046f632  push       dword ptr [0x4b3c04]
0046f638  call       dword ptr [0x4981d0]

// block 0046f63e
0046f63e  mov        ecx, dword ptr [0x4d6440]
0046f644  mov        eax, dword ptr [0x4b3d58]
0046f649  imul       ecx, ecx, 0x14
0046f64c  mov        edx, dword ptr [0x4d6444]
0046f652  sub        ecx, eax
0046f654  lea        ecx, [ecx + edx - 0x14]
0046f658  push       ecx
0046f659  lea        ecx, [eax + 0x14]
0046f65c  push       ecx
0046f65d  push       eax
0046f65e  call       0x47a6a0

// block 0046f663
0046f663  add        esp, 0xc
0046f666  dec        dword ptr [0x4d6440]

// block 0046f66c
0046f66c  and        dword ptr [0x4b3d58], 0

// block 0046f673
0046f673  ret        
