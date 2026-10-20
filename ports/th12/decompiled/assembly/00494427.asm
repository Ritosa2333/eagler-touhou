
// block 00494427
00494427  sub        esp, 0x2c
0049442a  and        eax, 0x3f
0049442d  jmp        dword ptr [eax*4 + 0x4b364e]

// block 00494434
00494434  fdiv       st(0)
00494436  add        esp, 0x2c
00494439  ret        

// block 0049443a
0049443a  add        esp, 0x2c
0049443d  int        6

// block 0049443f
0049443f  fdivr      st(0)
00494441  add        esp, 0x2c
00494444  ret        

// block 00494445
00494445  add        esp, 0x2c
00494448  int        6

// block 0049444a
0049444a  fdiv       st(0)
0049444c  add        esp, 0x2c
0049444f  ret        

// block 00494450
00494450  fdivp      st(0)
00494452  add        esp, 0x2c
00494455  ret        

// block 00494456
00494456  fdivr      st(0)
00494458  add        esp, 0x2c
0049445b  ret        

// block 0049445c
0049445c  fdivrp     st(0)
0049445e  add        esp, 0x2c
00494461  ret        

// block 00494462
00494462  fstp       xword ptr [esp + 0xc]
00494466  fld        st(0)
00494468  fstp       xword ptr [esp]
0049446b  fstp       xword ptr [esp + 0x20]
0049446f  call       0x494310

// block 00494474
00494474  fld        xword ptr [esp + 0x20]
00494478  fxch       st(1)
0049447a  add        esp, 0x2c
0049447d  ret        

// block 0049447e
0049447e  add        esp, 0x2c
00494481  int        6

// block 00494483
00494483  fstp       xword ptr [esp]
00494486  fstp       xword ptr [esp + 0xc]
0049448a  call       0x494310

// block 0049448f
0049448f  fld        xword ptr [esp + 0xc]
00494493  fxch       st(1)
00494495  add        esp, 0x2c
00494498  ret        

// block 00494499
00494499  add        esp, 0x2c
0049449c  int        6

// block 0049449e
0049449e  fxch       st(1)
004944a0  fstp       xword ptr [esp + 0xc]
004944a4  fld        st(0)
004944a6  fstp       xword ptr [esp]
004944a9  fstp       xword ptr [esp + 0x20]
004944ad  call       0x494310

// block 004944b2
004944b2  fld        xword ptr [esp + 0x20]
004944b6  add        esp, 0x2c
004944b9  ret        

// block 004944ba
004944ba  fstp       xword ptr [esp]
004944bd  fstp       xword ptr [esp + 0xc]
004944c1  call       0x494310

// block 004944c6
004944c6  add        esp, 0x2c
004944c9  ret        

// block 004944ca
004944ca  fstp       xword ptr [esp + 0xc]
004944ce  fstp       xword ptr [esp]
004944d1  call       0x494310

// block 004944d6
004944d6  fld        xword ptr [esp + 0xc]
004944da  add        esp, 0x2c
004944dd  ret        

// block 004944de
004944de  fstp       xword ptr [esp + 0xc]
004944e2  fstp       xword ptr [esp]
004944e5  call       0x494310

// block 004944ea
004944ea  add        esp, 0x2c
004944ed  ret        

// block 004944ee
004944ee  fstp       xword ptr [esp + 0xc]
004944f2  fxch       st(1)
004944f4  fld        st(0)
004944f6  fstp       xword ptr [esp]
004944f9  fstp       xword ptr [esp + 0x20]
004944fd  call       0x494310

// block 00494502
00494502  fxch       st(1)
00494504  fld        xword ptr [esp + 0x20]
00494508  fxch       st(2)
0049450a  add        esp, 0x2c
0049450d  ret        

// block 0049450e
0049450e  add        esp, 0x2c
00494511  int        6

// block 00494513
00494513  fstp       xword ptr [esp]
00494516  fxch       st(1)
00494518  fstp       xword ptr [esp + 0xc]
0049451c  call       0x494310

// block 00494521
00494521  fxch       st(1)
00494523  fld        xword ptr [esp + 0xc]
00494527  fxch       st(2)
00494529  add        esp, 0x2c
0049452c  ret        

// block 0049452d
0049452d  add        esp, 0x2c
00494530  int        6

// block 00494532
00494532  fxch       st(2)
00494534  fstp       xword ptr [esp + 0xc]
00494538  fxch       st(1)
0049453a  fld        st(0)
0049453c  fstp       xword ptr [esp]
0049453f  fstp       xword ptr [esp + 0x20]
00494543  call       0x494310

// block 00494548
00494548  fxch       st(1)
0049454a  fld        xword ptr [esp + 0x20]
0049454e  add        esp, 0x2c
00494551  ret        

// block 00494552
00494552  fstp       xword ptr [esp]
00494555  fxch       st(1)
00494557  fstp       xword ptr [esp + 0xc]
0049455b  call       0x494310

// block 00494560
00494560  fxch       st(1)
00494562  add        esp, 0x2c
00494565  ret        

// block 00494566
00494566  fstp       xword ptr [esp + 0xc]
0049456a  fxch       st(1)
0049456c  fstp       xword ptr [esp]
0049456f  call       0x494310

// block 00494574
00494574  fxch       st(1)
00494576  fld        xword ptr [esp + 0xc]
0049457a  add        esp, 0x2c
0049457d  ret        

// block 0049457e
0049457e  fstp       xword ptr [esp + 0xc]
00494582  fxch       st(1)
00494584  fstp       xword ptr [esp]
00494587  call       0x494310

// block 0049458c
0049458c  fxch       st(1)
0049458e  add        esp, 0x2c
00494591  ret        

// block 00494592
00494592  fstp       xword ptr [esp + 0xc]
00494596  fxch       st(2)
00494598  fld        st(0)
0049459a  fstp       xword ptr [esp]
0049459d  fstp       xword ptr [esp + 0x20]
004945a1  call       0x494310

// block 004945a6
004945a6  fxch       st(2)
004945a8  fld        xword ptr [esp + 0x20]
004945ac  fxch       st(3)
004945ae  add        esp, 0x2c
004945b1  ret        

// block 004945b2
004945b2  add        esp, 0x2c
004945b5  int        6

// block 004945b7
004945b7  fstp       xword ptr [esp]
004945ba  fxch       st(2)
004945bc  fstp       xword ptr [esp + 0xc]
004945c0  call       0x494310

// block 004945c5
004945c5  fxch       st(2)
004945c7  fld        xword ptr [esp + 0xc]
004945cb  fxch       st(3)
004945cd  add        esp, 0x2c
004945d0  ret        

// block 004945d1
004945d1  add        esp, 0x2c
004945d4  int        6

// block 004945d6
004945d6  fxch       st(3)
004945d8  fstp       xword ptr [esp + 0xc]
004945dc  fxch       st(2)
004945de  fld        st(0)
004945e0  fstp       xword ptr [esp]
004945e3  fstp       xword ptr [esp + 0x20]
004945e7  call       0x494310

// block 004945ec
004945ec  fxch       st(2)
004945ee  fld        xword ptr [esp + 0x20]
004945f2  add        esp, 0x2c
004945f5  ret        

// block 004945f6
004945f6  fstp       xword ptr [esp]
004945f9  fxch       st(2)
004945fb  fstp       xword ptr [esp + 0xc]
004945ff  call       0x494310

// block 00494604
00494604  fxch       st(2)
00494606  add        esp, 0x2c
00494609  ret        

// block 0049460a
0049460a  fstp       xword ptr [esp + 0xc]
0049460e  fxch       st(2)
00494610  fstp       xword ptr [esp]
00494613  call       0x494310

// block 00494618
00494618  fxch       st(2)
0049461a  fld        xword ptr [esp + 0xc]
0049461e  add        esp, 0x2c
00494621  ret        

// block 00494622
00494622  fstp       xword ptr [esp + 0xc]
00494626  fxch       st(2)
00494628  fstp       xword ptr [esp]
0049462b  call       0x494310

// block 00494630
00494630  fxch       st(2)
00494632  add        esp, 0x2c
00494635  ret        

// block 00494636
00494636  fstp       xword ptr [esp + 0xc]
0049463a  fxch       st(3)
0049463c  fld        st(0)
0049463e  fstp       xword ptr [esp]
00494641  fstp       xword ptr [esp + 0x20]
00494645  call       0x494310

// block 0049464a
0049464a  fxch       st(3)
0049464c  fld        xword ptr [esp + 0x20]
00494650  fxch       st(4)
00494652  add        esp, 0x2c
00494655  ret        

// block 00494656
00494656  add        esp, 0x2c
00494659  int        6

// block 0049465b
0049465b  fstp       xword ptr [esp]
0049465e  fxch       st(3)
00494660  fstp       xword ptr [esp + 0xc]
00494664  call       0x494310

// block 00494669
00494669  fxch       st(3)
0049466b  fld        xword ptr [esp + 0xc]
0049466f  fxch       st(4)
00494671  add        esp, 0x2c
00494674  ret        

// block 00494675
00494675  add        esp, 0x2c
00494678  int        6

// block 0049467a
0049467a  fxch       st(4)
0049467c  fstp       xword ptr [esp + 0xc]
00494680  fxch       st(3)
00494682  fld        st(0)
00494684  fstp       xword ptr [esp]
00494687  fstp       xword ptr [esp + 0x20]
0049468b  call       0x494310

// block 00494690
00494690  fxch       st(3)
00494692  fld        xword ptr [esp + 0x20]
00494696  add        esp, 0x2c
00494699  ret        

// block 0049469a
0049469a  fstp       xword ptr [esp]
0049469d  fxch       st(3)
0049469f  fstp       xword ptr [esp + 0xc]
004946a3  call       0x494310

// block 004946a8
004946a8  fxch       st(3)
004946aa  add        esp, 0x2c
004946ad  ret        

// block 004946ae
004946ae  fstp       xword ptr [esp + 0xc]
004946b2  fxch       st(3)
004946b4  fstp       xword ptr [esp]
004946b7  call       0x494310

// block 004946bc
004946bc  fxch       st(3)
004946be  fld        xword ptr [esp + 0xc]
004946c2  add        esp, 0x2c
004946c5  ret        

// block 004946c6
004946c6  fstp       xword ptr [esp + 0xc]
004946ca  fxch       st(3)
004946cc  fstp       xword ptr [esp]
004946cf  call       0x494310

// block 004946d4
004946d4  fxch       st(3)
004946d6  add        esp, 0x2c
004946d9  ret        

// block 004946da
004946da  fstp       xword ptr [esp + 0xc]
004946de  fxch       st(4)
004946e0  fld        st(0)
004946e2  fstp       xword ptr [esp]
004946e5  fstp       xword ptr [esp + 0x20]
004946e9  call       0x494310

// block 004946ee
004946ee  fxch       st(4)
004946f0  fld        xword ptr [esp + 0x20]
004946f4  fxch       st(5)
004946f6  add        esp, 0x2c
004946f9  ret        

// block 004946fa
004946fa  add        esp, 0x2c
004946fd  int        6

// block 004946ff
004946ff  fstp       xword ptr [esp]
00494702  fxch       st(4)
00494704  fstp       xword ptr [esp + 0xc]
00494708  call       0x494310

// block 0049470d
0049470d  fxch       st(4)
0049470f  fld        xword ptr [esp + 0xc]
00494713  fxch       st(5)
00494715  add        esp, 0x2c
00494718  ret        

// block 00494719
00494719  add        esp, 0x2c
0049471c  int        6

// block 0049471e
0049471e  fxch       st(5)
00494720  fstp       xword ptr [esp + 0xc]
00494724  fxch       st(4)
00494726  fld        st(0)
00494728  fstp       xword ptr [esp]
0049472b  fstp       xword ptr [esp + 0x20]
0049472f  call       0x494310

// block 00494734
00494734  fxch       st(4)
00494736  fld        xword ptr [esp + 0x20]
0049473a  add        esp, 0x2c
0049473d  ret        

// block 0049473e
0049473e  fstp       xword ptr [esp]
00494741  fxch       st(4)
00494743  fstp       xword ptr [esp + 0xc]
00494747  call       0x494310

// block 0049474c
0049474c  fxch       st(4)
0049474e  add        esp, 0x2c
00494751  ret        

// block 00494752
00494752  fstp       xword ptr [esp + 0xc]
00494756  fxch       st(4)
00494758  fstp       xword ptr [esp]
0049475b  call       0x494310

// block 00494760
00494760  fxch       st(4)
00494762  fld        xword ptr [esp + 0xc]
00494766  add        esp, 0x2c
00494769  ret        

// block 0049476a
0049476a  fstp       xword ptr [esp + 0xc]
0049476e  fxch       st(4)
00494770  fstp       xword ptr [esp]
00494773  call       0x494310

// block 00494778
00494778  fxch       st(4)
0049477a  add        esp, 0x2c
0049477d  ret        

// block 0049477e
0049477e  fstp       xword ptr [esp + 0xc]
00494782  fxch       st(5)
00494784  fld        st(0)
00494786  fstp       xword ptr [esp]
00494789  fstp       xword ptr [esp + 0x20]
0049478d  call       0x494310

// block 00494792
00494792  fxch       st(5)
00494794  fld        xword ptr [esp + 0x20]
00494798  fxch       st(6)
0049479a  add        esp, 0x2c
0049479d  ret        

// block 0049479e
0049479e  add        esp, 0x2c
004947a1  int        6

// block 004947a3
004947a3  fstp       xword ptr [esp]
004947a6  fxch       st(5)
004947a8  fstp       xword ptr [esp + 0xc]
004947ac  call       0x494310

// block 004947b1
004947b1  fxch       st(5)
004947b3  fld        xword ptr [esp + 0xc]
004947b7  fxch       st(6)
004947b9  add        esp, 0x2c
004947bc  ret        

// block 004947bd
004947bd  add        esp, 0x2c
004947c0  int        6

// block 004947c2
004947c2  fxch       st(6)
004947c4  fstp       xword ptr [esp + 0xc]
004947c8  fxch       st(5)
004947ca  fld        st(0)
004947cc  fstp       xword ptr [esp]
004947cf  fstp       xword ptr [esp + 0x20]
004947d3  call       0x494310

// block 004947d8
004947d8  fxch       st(5)
004947da  fld        xword ptr [esp + 0x20]
004947de  add        esp, 0x2c
004947e1  ret        

// block 004947e2
004947e2  fstp       xword ptr [esp]
004947e5  fxch       st(5)
004947e7  fstp       xword ptr [esp + 0xc]
004947eb  call       0x494310

// block 004947f0
004947f0  fxch       st(5)
004947f2  add        esp, 0x2c
004947f5  ret        

// block 004947f6
004947f6  fstp       xword ptr [esp + 0xc]
004947fa  fxch       st(5)
004947fc  fstp       xword ptr [esp]
004947ff  call       0x494310

// block 00494804
00494804  fxch       st(5)
00494806  fld        xword ptr [esp + 0xc]
0049480a  add        esp, 0x2c
0049480d  ret        

// block 0049480e
0049480e  fstp       xword ptr [esp + 0xc]
00494812  fxch       st(5)
00494814  fstp       xword ptr [esp]
00494817  call       0x494310

// block 0049481c
0049481c  fxch       st(5)
0049481e  add        esp, 0x2c
00494821  ret        

// block 00494822
00494822  fstp       xword ptr [esp + 0xc]
00494826  fxch       st(6)
00494828  fld        st(0)
0049482a  fstp       xword ptr [esp]
0049482d  fstp       xword ptr [esp + 0x20]
00494831  call       0x494310

// block 00494836
00494836  fxch       st(6)
00494838  fld        xword ptr [esp + 0x20]
0049483c  fxch       st(7)
0049483e  add        esp, 0x2c
00494841  ret        

// block 00494842
00494842  add        esp, 0x2c
00494845  int        6

// block 00494847
00494847  fstp       xword ptr [esp]
0049484a  fxch       st(6)
0049484c  fstp       xword ptr [esp + 0xc]
00494850  call       0x494310

// block 00494855
00494855  fxch       st(6)
00494857  fld        xword ptr [esp + 0xc]
0049485b  fxch       st(7)
0049485d  add        esp, 0x2c
00494860  ret        

// block 00494861
00494861  add        esp, 0x2c
00494864  int        6

// block 00494866
00494866  fxch       st(7)
00494868  fstp       xword ptr [esp + 0xc]
0049486c  fxch       st(6)
0049486e  fld        st(0)
00494870  fstp       xword ptr [esp]
00494873  fstp       xword ptr [esp + 0x20]
00494877  call       0x494310

// block 0049487c
0049487c  fxch       st(6)
0049487e  fld        xword ptr [esp + 0x20]
00494882  add        esp, 0x2c
00494885  ret        

// block 00494886
00494886  fstp       xword ptr [esp]
00494889  fxch       st(6)
0049488b  fstp       xword ptr [esp + 0xc]
0049488f  call       0x494310

// block 00494894
00494894  fxch       st(6)
00494896  add        esp, 0x2c
00494899  ret        

// block 0049489a
0049489a  fstp       xword ptr [esp + 0xc]
0049489e  fxch       st(6)
004948a0  fstp       xword ptr [esp]
004948a3  call       0x494310

// block 004948a8
004948a8  fxch       st(6)
004948aa  fld        xword ptr [esp + 0xc]
004948ae  add        esp, 0x2c
004948b1  ret        

// block 004948b2
004948b2  fstp       xword ptr [esp + 0xc]
004948b6  fxch       st(6)
004948b8  fstp       xword ptr [esp]
004948bb  call       0x494310

// block 004948c0
004948c0  fxch       st(6)
004948c2  add        esp, 0x2c
004948c5  ret        
