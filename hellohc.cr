lib NoLib
  fun _ = syscall(1, 1, 1, 1) : Int64
end

{% begin %}
  tbl = [] of UInt8
  128.times do |i|
    tbl << (i < 32 ? 0_u8 : i.to_u8)
  end

  h = (tbl[64] ^ tbl[40])
  e = (tbl[64] ^ tbl[37])
  l = (tbl[64] ^ tbl[44])
  o = (tbl[64] ^ tbl[47])
  c = (tbl[64] ^ tbl[84])
  s = (tbl[64] ^ tbl[96])
  w = (tbl[64] ^ tbl[55])
  r = (tbl[64] ^ tbl[50])
  d = (tbl[64] ^ tbl[36])
  n = (tbl[64] ^ tbl[74])

  ptr = Pointer(UInt8).new(0x1_0000_0000 - 0xFFFF_FFE7)
  ptr[ 0] = h
  ptr[ 1] = e
  ptr[ 2] = l
  ptr[ 3] = l
  ptr[ 4] = o
  ptr[ 5] = c
  ptr[ 6] = s
  ptr[ 7] = w
  ptr[ 8] = o
  ptr[ 9] = r
  ptr[10] = l
  ptr[11] = d
  ptr[12] = n

  {% if flag?(:linux) && flag?(:x86_64) %}
    asm("movq $1, %rax; movq $1, %rdi; movq $0, %rsi; movq $13, %rdx")
    asm("movq $#{ptr.address}, %rsi")
    asm("syscall")
  {% else %}
    NoLib.syscall(1, 1, ptr.address, 13)
  {% end %}
{% end %}

__crystal_main = ->{ }