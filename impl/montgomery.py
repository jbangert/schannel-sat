#!/usr/bin/python2
import math
MASK = 0xFFFFFFFFFFFFFFFF
SIZE = 1024 #+ 64
WORDS= SIZE/64 
BIGMASK = 2**SIZE - 1

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
def decompose(a):
    return [byte(a,i) for i in range(WORDS)]
def compose(a):
    x=0
    for i in range(WORDS):
        x+= a[i] << 64*i
    return x
def decompose_test(a):
    x = 0
    assert byte(a,31) == 0
    compose(decompose(a)) == a
decompose_test(1)
def mulmont(a,b,m,mp):
    c=0
    pmp = mp
    mp = -modinv(m,2**64) % 2**64

    print "m=", hex(m)
    print "mp =", hex(mp)
#    print "modulo = ",mp*m % 2**64
#    print "modulo = ",mp*-m % 2**64
    decompose_test(a)
    for i in range(WORDS):
        c = (c+ byte(a,i) * b) & BIGMASK
       # q_2 = (mp * c) & MASK
        q = (mp * byte(c,0)) & MASK
        # assert q == q_2
        c = ((c+q*m) >> 64) & BIGMASK
    if(c>=m):
        c = c-m
    return c
def mulmont2(a,b,m,mp):
    print "a=", hex(a)
    print "b=", hex(b)
    print "m=", hex(m)
    a= decompose(a)
    b= decompose(b)
    mp = modinv(m,2**64) % 2**64
    m= decompose(m)
    d = [0] * WORDS
    e = [0] * WORDS
   
    print "mp =", hex(mp)
    
    for j in range(WORDS):
        print "a0 =", a[0]
        q = (mp * b[0] * a[j] + mp * (d[0] - e[0])) & MASK
        print "q = ",q
        print "aj", a[j]
        print "b0", b[0]
        print "d0", d[0]
        t0 = a[j] * b[0] + d[0]
        print "t0 =", hex(t0)
        t0 = t0>>64
        t1 = q * m[0] + e[0]
        t1 = t1>>64
        for i in range(1,WORDS):
            p0 = a[j] * b[i] + t0 + d[i]
            p1 = q * m[i] + t1 + e[i]
            t0 = p0 >> 64
            t1 = p1 >> 64
            d[i-1] = p0 & MASK
            e[i-1] = p1 & MASK
        d[WORDS-1] = t0
        e[WORDS-1] = t1
        print "d=", hex(compose(d))
        print "e=", hex(compose(e))
        
    d = compose(d)
    e = compose(e)
    m = compose(m)
    c = d-e
    if(c<0):
        c = c+m
    print "got =", hex(c)
    return c
def mulmod(a,b,m):
    return (a*b)%m
# a=0x37D1C2A33BF3CEE869045D92DC7591CC1734ADBF6A9054AEBDE32D3DD516727A99B3DA37BDD087D4FC1844A19AB4BEE39C5762DA484221B736E18887895D442A2F3504EA5CD492CD42E87B6F5F683192AE72109B35B76B2D7B20A207AA33760C39CC0D5BBEBA8D89E08DA17341BCCC96B87FFEE9CEEBE9F2B40472DD516A4EB5
# b=0x6B245AA7A7688FF0AC45EF74424BFB58728E864567D047108A250771C174508DCEA0120EB40EF7C9F46127BAFAFFE6DE74E4B40481F15357DBB94DFE5A68389BA86F06C0D3AC9DE42AA205E9AA259E11A6157F49F1CFD62299CF589BE1E6B582044FBB04AAA69C082E20D5EB0B3D8D1CF2E3B6E6D8AEE8D8C710E5DC46D734D9
# m=0xF6DCB8A197DD9E96880F824A9A0839F8AC628460D9CDC2F49600A67D97784A76E3636AF460B4FA4491AAF737EDD88C1C811D09B93106B23F82215A1368D142F00B9BB20DB46848902E3FD64F5C0BC0B052D3FF8C3CF18B66E7B4036B0D94B167A72EBDC50248E8F43B768CD50BE7B88F449B3DDF6513235CC6D8244DFE788215
# multiply_out=0x1
# mp=0x2A993E0641A23D3D


a=0x31FCD179F567F65A13CE1B4F02B4AB9073BFFBC1C2D13C270B6270D0C22E61778F6B11E508A355E3D19326138740E2029D7FFFD5373CF1328D25D1B3A65D756DB7FB75551D57059D808E41316863140607111D42A6B2B5B0D186E9B8A8C3813E0342CDF2C7DA41C0575E8368F790251A908F7931AED47E33D01BBE9ED8F13A17
b=0x31FCD179F567F65A13CE1B4F02B4AB9073BFFBC1C2D13C270B6270D0C22E61778F6B11E508A355E3D19326138740E2029D7FFFD5373CF1328D25D1B3A65D756DB7FB75551D57059D808E41316863140607111D42A6B2B5B0D186E9B8A8C3813E0342CDF2C7DA41C0575E8368F790251A908F7931AED47E33D01BBE9ED8F13A17
m=0xCE032E860A9809A5EC31E4B0FD4B546F8C40043E3D2EC3D8F49D8F2F3DD19E887094EE1AF75CAA1C2E6CD9EC78BF1DFD6280002AC8C30ECD72DA2E4C59A28A9248048AAAE2A8FA627F71BECE979CEBF9F8EEE2BD594D4A4F2E791647573C7EC1FCBD320D3825BE3FA8A17C97086FDAE56F7086CE512B81CC2FE44161270EC5E9
mp = 0


def num_to_mont(a,m,mp):
    val =   (a * (2**SIZE % m)) % m
    v2 = mulmont2(a, 2**(2*SIZE) % m, m,mp)
    
#    v2 = a
#    print "vx=", ((v2*mp) & BIGMASK) *m
#    v2 = (v2+ ((v2*mp) & BIGMASK)*m)>>SIZE
    
    assert val == v2
    return val
def mont_red(T,N,mp):
    g,rp,np = egcd(2**SIZE, N)
    np = -np
  #  print "mont_normalization=",np

    assert rp * (2**SIZE) - np *N == 1
    #np =  modinv(N,2**64) % 2**64
    m = (T * np) & BIGMASK
    t = (T+m*N) / 2**SIZE
    if t>=N:
        return t-N
    else:
        return t

mm = mulmont2(a,b,m,mp) 
#a = 3
#b = 9
#m = 19
#assert mont_red(num_to_mont(a,m,mp),m,mp) - a == 0
#mm = mulmont2(num_to_mont(a,m,mp),num_to_mont(b,m,mp),m,mp)
#mm = mont_red(mm,m,mp)
#mm= mont_red(mm,a,mp)
#mm = num_to_mont(mm,m,mp)
print "product =",hex(mulmod(a,b,m))
real  = mont_red(mulmod(a,b,m),m,mp)
#real = mulmod(a,b,m)
print "real=", hex(real)
assert mm == real
