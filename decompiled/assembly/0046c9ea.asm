
// block 0046c9ea
0046c9ea  mov        edi, edi
0046c9ec  push       ebp
0046c9ed  mov        ebp, esp
0046c9ef  sub        esp, 0xc
0046c9f2  jmp        0x46ca01

// block 0046c9f4
0046c9f4  push       dword ptr [ebp + 8]
0046c9f7  call       0x46ff67

// block 0046c9fc
0046c9fc  pop        ecx
0046c9fd  test       eax, eax
0046c9ff  je         0x46ca10

// block 0046ca01
0046ca01  push       dword ptr [ebp + 8]
0046ca04  call       0x46d04a

// block 0046ca09
0046ca09  pop        ecx
0046ca0a  test       eax, eax
0046ca0c  je         0x46c9f4

// block 0046ca0e
0046ca0e  leave      
0046ca0f  ret        

// block 0046ca10
0046ca10  test       byte ptr [0x4b38cc], 1
0046ca17  mov        esi, 0x4b38c0
0046ca1c  jne        0x46ca37

// block 0046ca1e
0046ca1e  or         dword ptr [0x4b38cc], 1
0046ca25  mov        ecx, esi
0046ca27  call       0x46c9cf

// block 0046ca2c
0046ca2c  push       0x497ac1
0046ca31  call       0x46da32

// block 0046ca36
0046ca36  pop        ecx

// block 0046ca37
0046ca37  push       esi
0046ca38  lea        ecx, [ebp - 0xc]
0046ca3b  call       0x44f180

// block 0046ca40
0046ca40  push       0x4ab5d0
0046ca45  lea        eax, [ebp - 0xc]
0046ca48  push       eax
0046ca49  call       0x46ff8f

// block 0046ca4e
0046ca4e  int3       
