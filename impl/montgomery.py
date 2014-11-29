#!/usr/bin/python2
import math
MASK = 0xFFFFFFFFFFFFFFFF
BIGMASK = 2**2048 - 1
def egcd(a, b):
    if a == 0:
        return (b, 0, 1)
    else:
        g, y, x = egcd(b % a, a)
        return (g, x - (b // a) * y, y)

def modinv(a, m):
    g, x, y = egcd(a, m)
    if g != 1:
        raise Exception('modular inverse does not exist')
    else:
        return x % m

def byte(a,word):
    return (a>>(word*64)) & MASK
def decompose_test(a):
    x = 0
    print math.log(a,2)
    assert byte(a,31) == 0
    for i in reversed(range(2048/64)):
        x= x| (byte(a,i) << i*64)
    assert (x==a)
decompose_test(1)
def mulmont(a,b,m,mp):
    c=0
    mp = -modinv(m,2**64) % 2**64
    print "mp =", mp
    print "modulo = ",mp*m % 2**64
    print "modulo = ",mp*-m % 2**64
    decompose_test(a)
    for i in range(2048/64):
        c = (c+ byte(a,i) * b) & BIGMASK
       # q_2 = (mp * c) & MASK
        q = (mp * byte(c,0)) & MASK
        # assert q == q_2
        c = ((c+q*m) >> 64) & BIGMASK
    if(c>=m):
        c = c-m
    return c


    
def mulmod(a,b,m):
    return (a*b)%m
a=0x37D1C2A33BF3CEE869045D92DC7591CC1734ADBF6A9054AEBDE32D3DD516727A99B3DA37BDD087D4FC1844A19AB4BEE39C5762DA484221B736E18887895D442A2F3504EA5CD492CD42E87B6F5F683192AE72109B35B76B2D7B20A207AA33760C39CC0D5BBEBA8D89E08DA17341BCCC96B87FFEE9CEEBE9F2B40472DD516A4EB5
b=0x6B245AA7A7688FF0AC45EF74424BFB58728E864567D047108A250771C174508DCEA0120EB40EF7C9F46127BAFAFFE6DE74E4B40481F15357DBB94DFE5A68389BA86F06C0D3AC9DE42AA205E9AA259E11A6157F49F1CFD62299CF589BE1E6B582044FBB04AAA69C082E20D5EB0B3D8D1CF2E3B6E6D8AEE8D8C710E5DC46D734D9
m=0xF6DCB8A197DD9E96880F824A9A0839F8AC628460D9CDC2F49600A67D97784A76E3636AF460B4FA4491AAF737EDD88C1C811D09B93106B23F82215A1368D142F00B9BB20DB46848902E3FD64F5C0BC0B052D3FF8C3CF18B66E7B4036B0D94B167A72EBDC50248E8F43B768CD50BE7B88F449B3DDF6513235CC6D8244DFE788215
multiply_out=0x1
mp=0xBE5DC2C3

#print mulmont(a,b,m,mp) >= m
a = 3
b = 9
m = 21
mm = mulmont(a,b,m,mp)
mm = mm * (2**2048 % m ) % m
print "got =", mm
real  = mulmod(a,b,m)
print "real=", real

assert mm == real
