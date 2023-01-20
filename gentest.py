import secrets
import hashlib

p = 0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffed
n = 0x1000000000000000000000000000000014def9dea2f79cd65812631a5cf5d3ed
d = 0x52036cee2b6ffe738cc740797779e89800700a4d4141d8ab75eb4dca135978a3
x = 0x216936d3cd6e53fec0a4e231fdd6dc5c692cc7609525a7b2c9562d608f25d51a
y = 0x6666666666666666666666666666666666666666666666666666666666666658
m = 0x4000000000000000000000000000000000000000000000000000000000000000
t = 0x0ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffd
dxy = (d * x * y) % p


def inv(b):
  if b < 0:
    b += p
  a = p
  x = 1
  y = 0
  m = 0
  n = 1
  while b:
    q, r = divmod(a, b)
    x, m = m, x - q * m
    y, n = n, y - q * n
    a, b = b, r
  
  if y < 0:
    y += p
  return y


class GroupElement:
  def __init__(self, a) -> None:
    self.x = 0
    self.y = 1

    z = a
    w = z & z - 1
    while w:
      z = w
      w = z & z - 1
    
    while z:
      self.double()
      if z & a:
        self.inc()
      z >>= 1
  
  def inc(self):
    z = dxy * self.x * self.y % p
    nx = (self.x * y + self.y * x) * inv(z+1) % p
    ny = (self.x * x + self.y * y) * inv(1-z) % p
    self.x = nx
    self.y = ny
  
  def double(self):
    a = self.x * self.y
    z = d * a * a % p
    nx = a * 2 * inv(z+1) % p
    ny = (self.x * self.x + self.y * self.y) * inv(1-z) % p
    self.x = nx
    self.y = ny
  
  def __str__(self):
    # return f'{self.x:064x} {self.y:064x}'
    z = self.x & 1
    z <<= 255
    z |= self.y
    return f'{z:064x}'
  
  def __int__(self):
    return (self.y + 1) * inv(1 - self.y) % p


def sc_reduce(z):
  for _ in range(z):
    a = secrets.randbits(512)
    r = a % n
    print(f'{a:0128x} {r:064x}\n', end='')


def sc_reduce_2(z):
  for _ in range(z):
    a = secrets.randbits(260)
    r = secrets.randbits(125)
    a = a * n - r
    r = n - r
    print(f'{a:0128x} {r:064x}\n', end='')


def sc_muladd(z):
  for _ in range(z):
    a = secrets.randbits(252) << 3
    a |= m
    b = secrets.randbits(256) % n
    c = secrets.randbits(256) % n
    r = (a * b + c) % n
    print(f'{a:064x} {b:064x} {c:064x} {r:064x}\n', end='')


def sc_muladd_2(z):
  for _ in range(z):
    a = secrets.randbits(252) << 3
    a |= m
    b = secrets.randbits(256) % n
    k = a * b
    c = (-k - secrets.randbits(125)) % n
    r = (k + c) % n
    print(f'{a:064x} {b:064x} {c:064x} {r:064x}\n', end='')


def fe_mul(z):
  for _ in range(z):
    a = secrets.randbits(255) % p
    b = secrets.randbits(255) % p
    r = (a * b) % p
    print(f'{a:064x} {b:064x} {r:064x}\n', end='')


def fe_sqr(z):
  for _ in range(z):
    a = secrets.randbits(255) % p
    r = pow(a, 2, p)
    print(f'{a:064x} {r:064x}\n', end='')


def fe_inv(z):
  for _ in range(z):
    a = secrets.randbits(255) % p
    r = pow(a, p - 2, p)
    print(f'{a:064x} {r:064x}\n', end='')


def fe_powt(z):
  for _ in range(z):
    a = secrets.randbits(255) % p
    r = pow(a, t, p)
    print(f'{a:064x} {r:064x}\n', end='')


def ge_scmul(z):
  for _ in range(z):
    a = secrets.randbits(255)
    r = GroupElement(a)
    print(f'{a:064x} {r}\n', end='')


def ge_scmulsub(z):
  for _ in range(z):
    a = secrets.randbits(255)
    b = secrets.randbits(255)
    c = secrets.randbits(255)
    g = GroupElement(c)
    r = GroupElement((a-b*c)%n)
    print(f'{a:064x} {b:064x} {g} {r}\n', end='')


def x25519_from_private(z):
  for _ in range(z):
    a = secrets.randbits(255)
    c = a | (1 << 254)
    c &= (1 << 255) - 8
    r = GroupElement(c)
    print(f'{a:064x} {int(r):064x}\n', end='')


def x25519_xchg(z):
  for _ in range(z):
    a = secrets.randbits(255)
    b = secrets.randbits(255)
    g = GroupElement(b)
    c = a | (1 << 254)
    c &= (1 << 255) - 8
    r = GroupElement(c*b%n)
    print(f'{a:064x} {int(g):064x} {int(r):064x}\n', end='')


def ed25519_sign_verify(z):
  for _ in range(z):
    size = secrets.randbelow(119) + 10
    msg = secrets.token_bytes(size)

    a = secrets.token_bytes(32)

    x = hashlib.sha512(a).digest()

    az = int(x[:32][::-1].hex(), 16)

    az |= (1 << 254)
    az &= (1 << 255) - 8
    pub = GroupElement(az)
    pub = bytes.fromhex(str(pub))[::-1]

    r = hashlib.sha512(x[32:] + msg).digest()
    r = int(r[::-1].hex(), 16) % n
    R = GroupElement(r)
    R = bytes.fromhex(str(R))[::-1]
  
    h = hashlib.sha512(R + pub + msg).digest()
    h = int(h[::-1].hex(), 16) % n
    s = (az * h + r) % n
    s = bytes.fromhex(f'{s:064x}')[::-1]
    sig = R + s
    print(f'{a.hex()} {pub.hex()} {size} {msg.hex()} {sig.hex()}\n', end='')



print(100)
sc_reduce(30)
sc_reduce_2(70)
print(100)
sc_muladd(30)
sc_muladd_2(70)

print(50)
fe_mul(50)
print(30)
fe_sqr(30)
print(50)
fe_inv(50)
print(30)
fe_powt(30)

print(50)
ge_scmul(50)
print(50)
ge_scmulsub(50)

print(50)
x25519_from_private(50)
print(50)
x25519_xchg(50)

print(100)
ed25519_sign_verify(100)
