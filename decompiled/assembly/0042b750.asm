
// block 0042b750
0042b750  push       ebp
0042b751  mov        ebp, esp
0042b753  and        esp, 0xfffffff8
0042b756  sub        esp, 0x34c
0042b75c  mov        eax, dword ptr [0x4ad138]
0042b761  xor        eax, esp
0042b763  mov        dword ptr [esp + 0x348], eax
0042b76a  push       ebx
0042b76b  push       esi
0042b76c  push       edi
0042b76d  xor        edi, edi
0042b76f  mov        ebx, ecx
0042b771  mov        dword ptr [esp + 0x24], ebx
0042b775  cmp        dword ptr [ebp + 0x14], edi
0042b778  je         0x42b79b

// block 0042b77a
0042b77a  cmp        dword ptr [ebx + 0x44c], edi
0042b780  je         0x42b79b

// block 0042b782
0042b782  xor        eax, eax
0042b784  pop        edi
0042b785  pop        esi
0042b786  pop        ebx
0042b787  mov        ecx, dword ptr [esp + 0x348]
0042b78e  xor        ecx, esp
0042b790  call       0x46c932

// block 0042b795
0042b795  mov        esp, ebp
0042b797  pop        ebp
0042b798  ret        0x10

// block 0042b79b
0042b79b  mov        eax, dword ptr [ebx + 0x50]
0042b79e  fld        dword ptr [0x4a3e54]
0042b7a4  mov        ecx, dword ptr [ebx + 0x54]
0042b7a7  fstp       dword ptr [esp + 0xc]
0042b7ab  mov        edx, dword ptr [ebx + 0x58]
0042b7ae  push       0x100
0042b7b3  mov        dword ptr [esp + 0x5c], eax
0042b7b7  lea        eax, [esp + 0x254]
0042b7be  push       edi
0042b7bf  push       eax
0042b7c0  mov        dword ptr [esp + 0x2c], edi
0042b7c4  mov        dword ptr [esp + 0x68], ecx
0042b7c8  mov        dword ptr [esp + 0x6c], edx
0042b7cc  call       0x477420

// block 0042b7d1
0042b7d1  fldz       
0042b7d3  fstp       dword ptr [esp + 0x40]
0042b7d7  add        esp, 4
0042b7da  fld        dword ptr [0x4a3e54]
0042b7e0  lea        ecx, [esp + 0x34]
0042b7e4  fstp       dword ptr [esp + 4]
0042b7e8  fld        dword ptr [ebx + 0x68]
0042b7eb  fstp       dword ptr [esp]
0042b7ee  call       0x42e880

// block 0042b7f3
0042b7f3  fld        dword ptr [ebx + 0x50]
0042b7f6  fld        dword ptr [esp + 0x2c]
0042b7fa  fld        st(0)
0042b7fc  faddp      st(2)
0042b7fe  fxch       st(1)
0042b800  fstp       dword ptr [esp + 0x38]
0042b804  mov        ecx, dword ptr [esp + 0x38]
0042b808  fld        dword ptr [ebx + 0x54]
0042b80b  mov        dword ptr [esp + 0x14], ecx
0042b80f  fld        dword ptr [esp + 0x30]
0042b813  fld        st(0)
0042b815  faddp      st(2)
0042b817  fxch       st(1)
0042b819  fstp       dword ptr [esp + 0x3c]
0042b81d  mov        edx, dword ptr [esp + 0x3c]
0042b821  fld        dword ptr [ebx + 0x58]
0042b824  mov        dword ptr [esp + 0x18], edx
0042b828  fld        dword ptr [esp + 0x34]
0042b82c  fld        st(0)
0042b82e  faddp      st(2)
0042b830  fxch       st(1)
0042b832  fstp       dword ptr [esp + 0x40]
0042b836  mov        eax, dword ptr [esp + 0x40]
0042b83a  fldz       
0042b83c  mov        dword ptr [esp + 0x1c], eax
0042b840  fstp       dword ptr [esp + 0x1c]
0042b844  fld        st(2)
0042b846  faddp      st(3)
0042b848  fxch       st(2)
0042b84a  fstp       dword ptr [esp + 0x2c]
0042b84e  fadd       st(0), st(0)
0042b850  fstp       dword ptr [esp + 0x30]
0042b854  fadd       st(0), st(0)
0042b856  fstp       dword ptr [esp + 0x34]
0042b85a  fld        dword ptr [ebp + 0xc]
0042b85d  fmul       st(0), st(0)
0042b85f  fstp       dword ptr [ebp + 0xc]
0042b862  fld        dword ptr [ebx + 0x6c]
0042b865  fcomp      qword ptr [0x4a3d68]
0042b86b  fnstsw     ax
0042b86d  test       ah, 0x41
0042b870  jne        0x42bb48

// block 0042b876
0042b876  fld        dword ptr [ebp + 0xc]
0042b879  fld        qword ptr [0x4a3d70]
0042b87f  jmp        0x42b888

// block 0042b881
0042b881  fstp       st(0)
0042b883  fld        dword ptr [ebp + 0xc]
0042b886  fxch       st(1)

// block 0042b888
0042b888  mov        ecx, dword ptr [ebp + 8]
0042b88b  fld        dword ptr [ecx + 4]
0042b88e  fld        dword ptr [esp + 0x18]
0042b892  fld        st(0)
0042b894  fsubp      st(2)
0042b896  fld        dword ptr [ecx]
0042b898  fld        dword ptr [esp + 0x14]
0042b89c  fld        st(0)
0042b89e  fsubp      st(2)
0042b8a0  fld        st(1)
0042b8a2  fmulp      st(2)
0042b8a4  fld        st(3)
0042b8a6  fmulp      st(4)
0042b8a8  fxch       st(3)
0042b8aa  fadd       st(1)
0042b8ac  fstp       dword ptr [esp + 0x10]
0042b8b0  fld        dword ptr [esp + 0x10]
0042b8b4  fcomp      st(5)
0042b8b6  fnstsw     ax
0042b8b8  test       ah, 0x41
0042b8bb  je         0x42bb25

// block 0042b8c1
0042b8c1  inc        dword ptr [esp + 0x20]
0042b8c5  test       byte ptr [ebp + 0x10], 1
0042b8c9  mov        byte ptr [esp + edi + 0x250], 1
0042b8d1  je         0x42bb1c

// block 0042b8d7
0042b8d7  fld        st(2)
0042b8d9  fadd       st(4)
0042b8db  fcomp      qword ptr [0x4a3d60]
0042b8e1  fnstsw     ax
0042b8e3  test       ah, 0x41
0042b8e6  jnp        0x42bb1c

// block 0042b8ec
0042b8ec  fld        st(2)
0042b8ee  fsub       st(4)
0042b8f0  fcomp      qword ptr [0x4a3d28]
0042b8f6  fnstsw     ax
0042b8f8  test       ah, 1
0042b8fb  je         0x42bb1c

// block 0042b901
0042b901  fld        st(1)
0042b903  fadd       st(4)
0042b905  fcomp      qword ptr [0x4a3d58]
0042b90b  fnstsw     ax
0042b90d  test       ah, 0x41
0042b910  jnp        0x42bb1c

// block 0042b916
0042b916  fld        st(1)
0042b918  fsub       st(4)
0042b91a  fcomp      qword ptr [0x4a3d50]
0042b920  fnstsw     ax
0042b922  test       ah, 1
0042b925  je         0x42bb1c

// block 0042b92b
0042b92b  fld        dword ptr [ecx + 4]
0042b92e  fsub       st(2)
0042b930  fmul       st(0), st(0)
0042b932  faddp      st(1)
0042b934  fstp       dword ptr [esp + 0x10]
0042b938  fld        dword ptr [esp + 0x10]
0042b93c  fcomp      st(4)
0042b93e  fnstsw     ax
0042b940  fstp       st(3)
0042b942  test       ah, 0x41
0042b945  jp         0x42b981

// block 0042b947
0042b947  fstp       st(1)
0042b949  sub        esp, 8
0042b94c  fstp       st(1)
0042b94e  lea        ecx, [esp + 0x1c]
0042b952  fstp       st(0)
0042b954  fld        dword ptr [0x4a41f4]
0042b95a  fstp       dword ptr [esp + 4]
0042b95e  fld        dword ptr [0x4a4060]
0042b964  fstp       dword ptr [esp]
0042b967  push       ecx
0042b968  push       9
0042b96a  call       0x4273f0

// block 0042b96f
0042b96f  fld        qword ptr [0x4a3d70]
0042b975  fld        dword ptr [esp + 0x14]
0042b979  fld        dword ptr [esp + 0x18]
0042b97d  fxch       st(2)
0042b97f  fxch       st(1)

// block 0042b981
0042b981  fld        st(0)
0042b983  fadd       st(2)
0042b985  fst        qword ptr [esp + 0x38]
0042b989  fcomp      qword ptr [0x4a3d60]
0042b98f  fnstsw     ax
0042b991  test       ah, 0x41
0042b994  jnp        0x42baa5

// block 0042b99a
0042b99a  fld        st(0)
0042b99c  fsub       st(2)
0042b99e  fcomp      qword ptr [0x4a3d28]
0042b9a4  fnstsw     ax
0042b9a6  test       ah, 1
0042b9a9  je         0x42baa5

// block 0042b9af
0042b9af  fld        st(2)
0042b9b1  fadd       st(2)
0042b9b3  fcomp      qword ptr [0x4a3d58]
0042b9b9  fnstsw     ax
0042b9bb  test       ah, 0x41
0042b9be  jnp        0x42baa5

// block 0042b9c4
0042b9c4  fld        st(2)
0042b9c6  fsub       st(2)
0042b9c8  fcomp      qword ptr [0x4a3d50]
0042b9ce  fnstsw     ax
0042b9d0  test       ah, 1
0042b9d3  je         0x42baa5

// block 0042b9d9
0042b9d9  test       dword ptr [0x4cee78], 0x8000
0042b9e3  fstp       st(1)
0042b9e5  movsx      edx, word ptr [ebx + 0x4a6]
0042b9ec  fstp       st(1)
0042b9ee  mov        ecx, dword ptr [0x4b44f4]
0042b9f4  fstp       st(0)
0042b9f6  mov        esi, dword ptr [ecx + 0x488]
0042b9fc  lea        eax, [edx + edx + 4]
0042ba00  mov        dword ptr [esp + 0x10], eax
0042ba04  je         0x42ba17

// block 0042ba06
0042ba06  push       0x4cf1d0
0042ba0b  call       dword ptr [0x498088]

// block 0042ba11
0042ba11  inc        byte ptr [0x4cf221]

// block 0042ba17
0042ba17  inc        dword ptr [esi + 0x130]
0042ba1d  call       0x4621c0

// block 0042ba22
0042ba22  fld        qword ptr [esp + 0x38]
0042ba26  fadd       qword ptr [0x4a3d28]
0042ba2c  mov        edx, dword ptr [esp + 0x10]
0042ba30  mov        ebx, eax
0042ba32  or         dword ptr [ebx + 0x480], 1
0042ba39  mov        dword ptr [ebx + 0x20], 0x17
0042ba40  fstp       dword ptr [ebx + 0x430]
0042ba46  fld        dword ptr [esp + 0x18]
0042ba4a  push       edx
0042ba4b  fadd       qword ptr [0x4a3d68]
0042ba51  push       ebx
0042ba52  mov        ecx, esi
0042ba54  fstp       dword ptr [ebx + 0x434]
0042ba5a  fld        dword ptr [esp + 0x24]
0042ba5e  fstp       dword ptr [ebx + 0x438]
0042ba64  call       0x454d10

// block 0042ba69
0042ba69  lea        eax, [esp + 0x64]
0042ba6d  call       0x461250

// block 0042ba72
0042ba72  test       dword ptr [0x4cee78], 0x8000
0042ba7c  je         0x42ba8f

// block 0042ba7e
0042ba7e  push       0x4cf1d0
0042ba83  call       dword ptr [0x49808c]

// block 0042ba89
0042ba89  dec        byte ptr [0x4cf221]

// block 0042ba8f
0042ba8f  fld        qword ptr [0x4a3d70]
0042ba95  mov        ebx, dword ptr [esp + 0x24]
0042ba99  fld        dword ptr [esp + 0x14]
0042ba9d  fld        dword ptr [esp + 0x18]
0042baa1  fxch       st(2)
0042baa3  fxch       st(1)

// block 0042baa5
0042baa5  fadd       dword ptr [esp + 0x2c]
0042baa9  inc        edi
0042baaa  fstp       dword ptr [esp + 0x14]
0042baae  fld        dword ptr [esp + 0x30]
0042bab2  faddp      st(2)
0042bab4  fxch       st(1)
0042bab6  fstp       dword ptr [esp + 0x18]
0042baba  fld        dword ptr [esp + 0x1c]
0042babe  fadd       dword ptr [esp + 0x34]
0042bac2  fstp       dword ptr [esp + 0x1c]
0042bac6  fld        dword ptr [esp + 0xc]
0042baca  fld        qword ptr [0x4a3d68]
0042bad0  fadd       st(1), st(0)
0042bad2  fxch       st(1)
0042bad4  fstp       dword ptr [esp + 0xc]
0042bad8  fld        dword ptr [esp + 0xc]
0042badc  fadd       qword ptr [0x4a4078]
0042bae2  fld        dword ptr [ebx + 0x6c]
0042bae5  fcompp     
0042bae7  fnstsw     ax
0042bae9  test       ah, 0x41
0042baec  je         0x42b881

// block 0042baf2
0042baf2  cmp        dword ptr [esp + 0x20], 0
0042baf7  mov        dword ptr [esp + 0x10], edi
0042bafb  je         0x42bb44

// block 0042bafd
0042bafd  xor        esi, esi
0042baff  test       edi, edi
0042bb01  jle        0x42bb2e

// block 0042bb03
0042bb03  cmp        byte ptr [esp + esi + 0x250], 0
0042bb0b  je         0x42bb12

// block 0042bb0d
0042bb0d  inc        esi
0042bb0e  cmp        esi, edi
0042bb10  jl         0x42bb03

// block 0042bb12
0042bb12  test       esi, esi
0042bb14  je         0x42bb2e

// block 0042bb16
0042bb16  fstp       st(0)
0042bb18  fldz       
0042bb1a  jmp        0x42bb6f

// block 0042bb1c
0042bb1c  fstp       st(0)
0042bb1e  fstp       st(3)
0042bb20  jmp        0x42b981

// block 0042bb25
0042bb25  fstp       st(0)
0042bb27  fstp       st(3)
0042bb29  jmp        0x42baa5

// block 0042bb2e
0042bb2e  xor        eax, eax
0042bb30  test       edi, edi
0042bb32  jle        0x42bb44

// block 0042bb34
0042bb34  cmp        byte ptr [esp + esi + 0x250], 0
0042bb3c  jne        0x42bb63

// block 0042bb3e
0042bb3e  inc        esi
0042bb3f  inc        eax
0042bb40  cmp        esi, edi
0042bb42  jl         0x42bb34

// block 0042bb44
0042bb44  fstp       st(1)

// block 0042bb46
0042bb46  fstp       st(0)

// block 0042bb48
0042bb48  mov        ecx, dword ptr [esp + 0x354]
0042bb4f  mov        eax, dword ptr [esp + 0x20]
0042bb53  pop        edi
0042bb54  pop        esi
0042bb55  pop        ebx
0042bb56  xor        ecx, esp
0042bb58  call       0x46c932

// block 0042bb5d
0042bb5d  mov        esp, ebp
0042bb5f  pop        ebp
0042bb60  ret        0x10

// block 0042bb63
0042bb63  cmp        esi, edi
0042bb65  mov        dword ptr [esp + 0xc], eax
0042bb69  jge        0x42bb44

// block 0042bb6b
0042bb6b  fimul      dword ptr [esp + 0xc]

// block 0042bb6f
0042bb6f  fstp       dword ptr [ebx + 0x6c]

// block 0042bb72
0042bb72  cmp        esi, edi
0042bb74  jge        0x42bb46

// block 0042bb76
0042bb76  cmp        byte ptr [esp + esi + 0x250], 0
0042bb7e  je         0x42bb83

// block 0042bb80
0042bb80  inc        esi
0042bb81  jmp        0x42bb72

// block 0042bb83
0042bb83  cmp        esi, edi
0042bb85  jge        0x42bb46

// block 0042bb87
0042bb87  xor        eax, eax
0042bb89  mov        dword ptr [esp + 0x28], esi

// block 0042bb8d
0042bb8d  cmp        byte ptr [esp + esi + 0x250], 0
0042bb95  jne        0x42bb9d

// block 0042bb97
0042bb97  inc        esi
0042bb98  inc        eax
0042bb99  cmp        esi, edi
0042bb9b  jl         0x42bb8d

// block 0042bb9d
0042bb9d  fild       dword ptr [esp + 0x28]
0042bba1  mov        dword ptr [esp + 0xc], eax
0042bba5  fstp       dword ptr [esp + 0x28]
0042bba9  fld        dword ptr [esp + 0x28]
0042bbad  fstp       dword ptr [esp + 0x38]
0042bbb1  fld        dword ptr [esp + 0x2c]
0042bbb5  fld        dword ptr [esp + 0x38]
0042bbb9  fld        st(0)
0042bbbb  fmulp      st(2)
0042bbbd  fxch       st(1)
0042bbbf  fstp       dword ptr [esp + 0x4c]
0042bbc3  fld        dword ptr [esp + 0x30]
0042bbc7  fmul       st(1)
0042bbc9  fstp       dword ptr [esp + 0x50]
0042bbcd  fmul       dword ptr [esp + 0x34]
0042bbd1  fstp       dword ptr [esp + 0x54]
0042bbd5  fld        dword ptr [esp + 0x4c]
0042bbd9  fadd       dword ptr [esp + 0x58]
0042bbdd  fstp       dword ptr [esp + 0x14]
0042bbe1  fld        dword ptr [esp + 0x5c]
0042bbe5  fadd       dword ptr [esp + 0x50]
0042bbe9  fstp       dword ptr [esp + 0x18]
0042bbed  fld        dword ptr [esp + 0x60]
0042bbf1  fadd       dword ptr [esp + 0x54]
0042bbf5  fstp       dword ptr [esp + 0x1c]
0042bbf9  fld        dword ptr [esp + 0x14]
0042bbfd  fld        st(0)
0042bbff  fadd       st(2)
0042bc01  fcomp      qword ptr [0x4a3d60]
0042bc07  fnstsw     ax
0042bc09  test       ah, 0x41
0042bc0c  jnp        0x42bd7b

// block 0042bc12
0042bc12  fsub       st(1)
0042bc14  fcomp      qword ptr [0x4a3d28]
0042bc1a  fnstsw     ax
0042bc1c  test       ah, 1
0042bc1f  je         0x42bd7d

// block 0042bc25
0042bc25  fld        dword ptr [esp + 0x18]
0042bc29  fld        st(0)
0042bc2b  fadd       st(2)
0042bc2d  fcomp      qword ptr [0x4a3d58]
0042bc33  fnstsw     ax
0042bc35  test       ah, 0x41
0042bc38  jnp        0x42bd7b

// block 0042bc3e
0042bc3e  fsub       st(1)
0042bc40  fcomp      qword ptr [0x4a3d50]
0042bc46  fnstsw     ax
0042bc48  test       ah, 1
0042bc4b  je         0x42bd7d

// block 0042bc51
0042bc51  push       0x1e8
0042bc56  fstp       st(0)
0042bc58  lea        eax, [esp + 0x6c]
0042bc5c  push       0
0042bc5e  push       eax
0042bc5f  call       0x477420

// block 0042bc64
0042bc64  fild       dword ptr [esp + 0x18]
0042bc68  fld        qword ptr [0x4a3d68]
0042bc6e  mov        ecx, dword ptr [esp + 0x20]
0042bc72  mov        edx, dword ptr [esp + 0x24]
0042bc76  fmul       st(1), st(0)
0042bc78  mov        eax, dword ptr [esp + 0x28]
0042bc7c  fxch       st(1)
0042bc7e  mov        dword ptr [esp + 0x74], ecx
0042bc82  mov        cx, word ptr [ebx + 0x4a4]
0042bc89  fstp       dword ptr [esp + 0x88]
0042bc90  mov        dword ptr [esp + 0x78], edx
0042bc94  fld        dword ptr [esp + 0x88]
0042bc9b  mov        dx, word ptr [ebx + 0x4a6]
0042bca2  fstp       dword ptr [esp + 0x84]
0042bca9  add        esp, 0xc
0042bcac  fld        dword ptr [0x4a3e54]
0042bcb2  mov        dword ptr [esp + 0x70], eax
0042bcb6  fstp       dword ptr [esp + 0x88]
0042bcbd  mov        word ptr [esp + 0x8c], cx
0042bcc5  fld        dword ptr [ebx + 0x68]
0042bcc8  mov        word ptr [esp + 0x8e], dx
0042bcd0  fstp       dword ptr [esp + 0x74]
0042bcd4  fld        dword ptr [ebx + 0x70]
0042bcd7  fstp       dword ptr [esp + 0x84]
0042bcde  fld        dword ptr [esp + 0x28]
0042bce2  fmulp      st(1)
0042bce4  fsubr      dword ptr [ebx + 0x474]
0042bcea  mov        ebx, dword ptr [0x4b44f4]
0042bcf0  lea        edi, [ebx + 0x468]
0042bcf6  mov        dword ptr [esp + 0x38], ebx
0042bcfa  fstp       dword ptr [esp + 0x80]
0042bd01  cmp        dword ptr [edi], 0x100
0042bd07  jge        0x42bd6b

// block 0042bd09
0042bd09  inc        dword ptr [ebx + 0x46c]
0042bd0f  add        ebx, 0x46c
0042bd15  cmp        dword ptr [ebx], 0x10000
0042bd1b  jge        0x42bd23

// block 0042bd1d
0042bd1d  mov        dword ptr [ebx], 0x10000

// block 0042bd23
0042bd23  push       0xfa4
0042bd28  call       0x46c9ea

// block 0042bd2d
0042bd2d  add        esp, 4
0042bd30  test       eax, eax
0042bd32  je         0x42bd3b

// block 0042bd34
0042bd34  call       0x428520

// block 0042bd39
0042bd39  jmp        0x42bd3d

// block 0042bd3b
0042bd3b  xor        eax, eax

// block 0042bd3d
0042bd3d  mov        ecx, dword ptr [ebx]
0042bd3f  mov        edx, dword ptr [esp + 0x38]
0042bd43  mov        dword ptr [eax + 0x80], ecx
0042bd49  mov        ecx, dword ptr [edx + 0x464]
0042bd4f  mov        dword ptr [eax + 4], ecx
0042bd52  mov        dword ptr [ecx + 8], eax
0042bd55  inc        dword ptr [edi]
0042bd57  mov        dword ptr [edx + 0x464], eax
0042bd5d  mov        edx, dword ptr [eax]
0042bd5f  mov        edx, dword ptr [edx + 4]
0042bd62  lea        ecx, [esp + 0x68]
0042bd66  push       ecx
0042bd67  mov        ecx, eax
0042bd69  call       edx

// block 0042bd6b
0042bd6b  fld        qword ptr [0x4a3d70]
0042bd71  mov        edi, dword ptr [esp + 0x10]
0042bd75  mov        ebx, dword ptr [esp + 0x24]
0042bd79  jmp        0x42bd7d

// block 0042bd7b
0042bd7b  fstp       st(0)

// block 0042bd7d
0042bd7d  cmp        esi, edi
0042bd7f  jl         0x42bb76

// block 0042bd85
0042bd85  jmp        0x42bb46
