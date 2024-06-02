// recursive gz, tgz, zip files
// russ cox, march 2010 <rsc@swtch.com>

package main

import (
	"bufio"
	"bytes"
	"compress/flate"
	"fmt"
	"hash/crc32"
	"io"
	"os"
	"strconv"
)

var debugFlate = false

func main() {
//	makeGz()
//	makeTargz()
//	makeZip()
}

func makeGz() {
	// gzip header
	head := []byte{
		0x1f, 0x8b, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		'r', 'e', 'c', 'u', 'r', 's', 'i', 'v', 'e', 0x00,
	}
	zhead := deflate(head, true, false)
	
	// ztail is literal block
	ztail := make([]byte, 5+8)
	ztail[0] = 1	// final
	ztail[1] = 8
	ztail[2] = 0
	ztail[3] = ^byte(8)
	ztail[4] = ^byte(0)
	tail := ztail[5:]

	tail[0] = 0xaa
	tail[1] = 0xbb
	tail[2] = 0xcc
	tail[3] = 0xdd

	_, whole := makeGeneric(zhead, head, ztail, tail, nil)
	n := len(whole)
	tail[4] = byte(n)
	tail[5] = byte(n>>8)
	tail[6] = byte(n>>16)
	tail[7] = byte(n>>24)

	_, whole = makeGeneric(zhead, head, ztail, tail, tail[0:4])
	if n != len(whole) {
		fmt.Println("no converge!", n, len(whole))
		return
	}

	f, _ := os.Open("recursive.gz", os.O_CREAT|os.O_WRONLY, 0666)
	f.Write(whole)
	f.Close()
}

func makeTargz() {
	head := make([]byte, 512+10)
	
	// tar header
	copy(head[0:], []byte("r/r.tar.gz"))
	copy(head[100:], []byte("0000644"))	// mode
	copy(head[108:], []byte("007"))	// uid
	copy(head[116:], []byte("0"))	// gid
	copy(head[124:], []byte("0000000"))	// size placeholder
	copy(head[136:], []byte("1"))	// time
	head[156] = '0'	// type (regular)

	// gzip header
	copy(head[512:], []byte{0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00})

	// tail
	tail := make([]byte, 8+512*5)	// pad out to zero block, then two more to end tar file
	gziptail := tail[0:8]
	ztail := make([]byte, 5+8, 5+8+100)	// compressed tail w/ literal gzip trailer
	gziptail[0] = 0xaa
	gziptail[1] = 0xbb
	gziptail[2] = 0xcc
	gziptail[3] = 0xdd
	ztail[0] = 0
	ztail[1] = 8
	ztail[2] = 0
	ztail[3] = ^ztail[1]
	ztail[4] = ^ztail[2]

debugFlate = false
	// build header, tail with correct size, header sum
	var zhead, whole []byte
	var n int
Outer:
	for nz := 2*512; nz < 3*512; nz++ {
		println("----NZ", nz)
		tail = tail[0:8+nz]
		zzero := deflate(tail[8:], false, true)	// compressed tar trailer
		ztail = ztail[0:5+8+len(zzero)]
		copy(ztail[5+8:], zzero)
		n = 5*512 - 1
		for i := 0;; i++ {
			copy(head[124:], []byte(fmt.Sprintf("%07o", n - 512 - nz)))
			copy(head[148:], []byte("        "))	// sum placeholder
			sum := 0
			for _, v := range head[0:512] {
				sum += int(v)
			}
			copy(head[148:], []byte(fmt.Sprintf("%06o\x00 ", sum)))
			zhead = deflate(head, true, false)
fmt.Printf("zhead: %x\n", zhead)
			if len(zhead) > 64-5 {
				fmt.Println("zhead too big; abort", len(zhead))
				continue Outer
			}
			gziptail[4] = byte(n)
			gziptail[5] = byte(n>>8)
			gziptail[6] = byte(n>>16)
			gziptail[7] = byte(n>>24)
			copy(ztail[5:], gziptail[0:8])
			_, whole = makeGeneric(zhead, head, ztail, tail, nil)
			if n == len(whole) {
println("converged on", n)
				break
			}
			println(n)
			if i > 20 {
				fmt.Printf("looping in header %d %d %d\n", n, len(whole), nz)
				continue Outer
			}
			n = len(whole)
		}
		if n%512 == 0 {
			goto good
		}
	}
	fmt.Printf("failed\n")
	return

good:
	_, whole = makeGeneric(zhead, head, ztail, tail, gziptail[0:4])
	if n != len(whole) {
		fmt.Println("no converge!", n, len(whole))
		return
	}

println("writing", n)
	f, _ := os.Open("recursive.tar", os.O_CREAT|os.O_TRUNC|os.O_WRONLY, 0666)
	f.Write(whole)
	f.Close()
}

func makeZip() {
	csize := 0
	uncsize := 0
	sufpos := 0
	zhead := []byte{
		0x00, 37, 0, ^byte(37), 0xFF,	// 37-byte literal

		0x50, 0x4b, 0x03, 0x04,	// ZHeader
		0x14,	// extvers
		0x00,	// extos
		0x00, 0x00,	// flags
		0x08, 0x00,	// meth
		0x08, 0x03,	// modtime
		0x64, 0x3c,	// moddate
		0xaa, 0xbb, 0xcc, 0xdd,	// crc
		byte(csize), byte(csize>>8), 0, 0,	// csize
		byte(uncsize), byte(uncsize>>8), 0, 0,	// uncsize
		0x07, 0x00,	// flen
		0x00, 0x00,	// xlen
		'r', '/', 'r', '.', 'z', 'i', 'p',	// file name
	}
	head := zhead[5:]
	headsize := head[14:26]
	
	tail := []byte{
		0x50, 0x4b, 0x01, 0x02,	// ZCHeader
		0x14,	// madevers
		0x00,	// madeos
		0x14,	// extvers
		0x00,	// extos
		0x00, 0x00,	// flags
		0x08, 0x00,	// meth
		0x08, 0x03,	// modtime
		0x64, 0x3c,	// moddate
		0xaa, 0xbb, 0xcc, 0xdd,	// crc
		byte(csize), byte(csize>>8), 0, 0,	// csize
		byte(uncsize), byte(uncsize>>8), 0, 0,	// uncsize
		0x07, 0x00,	// flen
		0x00, 0x00,	// xlen
		0x00, 0x00,	// fclen
		0x00, 0x00,	// disk start
		0x00, 0x00,	// iattr
		0x00, 0x00, 0x00, 0x00,	// eattr
		0x00, 0x00, 0x00, 0x00,	// off
		'r', '/', 'r', '.', 'z', 'i', 'p',	// file name
		0x50, 0x4b, 0x05, 0x06,	// ZECHeader
		0x00, 0x00,	// dn
		0x00, 0x00,	// ds
		0x01, 0x00,	// de
		0x01, 0x00,	// entries
		53, 0x00,	0x00, 0x00, // size
		byte(sufpos), byte(sufpos>>8), 0x00, 0x00,	// off
		0x00, 0x00,	// zclen
	}

	// hand-compressed tail, to squeeze space.
	// must come in under 59 bytes.  not easy.
	var b wbuf
	var zero [12]byte
	b.writeBits(0, 1, false)	// non-final huffman block
	b.writeBits(1, 2, false)
	b.writeBits(0x50+48, 8, true)	// ZCHeader
	b.writeBits(0x4b+48, 8, true)
	b.writeBits(0x01+48, 8, true)
	b.writeBits(0x02+48, 8, true)
	b.writeBits(0x14+48, 8, true)	// madevers
	b.writeBits(0x00+48, 8, true)	// madeos
	// copy 24 bytes from 367 bytes back
	// covers extvers through flen
	// we just happen to know that 367 is the
	// right number to go back to get to the
	// right field in the header (tried and counted).
	b.writeBits(270-256, 7, true)
	b.writeBits(1, 2, false)
	b.writeBits(16, 5, true)
	b.writeBits(367-256-1, 7, false)

	// copy 16 zero bytes from 1 byte back
	b.writeBits(267-256, 7, true)	
	b.writeBits(1, 1, false)
	b.writeBits(0, 5, true)
	/*
		b.writeBits(0x00+48, 8, true)	// xlen
		b.writeBits(0x00+48, 8, true)
		b.writeBits(0x00+48, 8, true)	// fclen
		b.writeBits(0x00+48, 8, true)
		b.writeBits(0x00+48, 8, true)	// disk start
		b.writeBits(0x00+48, 8, true)
		b.writeBits(0x00+48, 8, true)	// iattr
		b.writeBits(0x00+48, 8, true)
		b.writeBits(0x00+48, 8, true)	// eattr
		b.writeBits(0x00+48, 8, true)
		b.writeBits(0x00+48, 8, true)
		b.writeBits(0x00+48, 8, true)
		b.writeBits(0x00+48, 8, true)	// off
		b.writeBits(0x00+48, 8, true)
		b.writeBits(0x00+48, 8, true)
		b.writeBits(0x00+48, 8, true)
	*/
	b.writeBits('r'+48, 8, true)	// file name
	b.writeBits('/'+48, 8, true)
	b.writeBits('r'+48, 8, true)
	b.writeBits('.'+48, 8, true)
	b.writeBits('z'+48, 8, true)
	b.writeBits('i'+48, 8, true)
	b.writeBits('p'+48, 8, true)
	b.writeBits(0x50+48, 8, true)	// ZECHeader
	b.writeBits(0x4b+48, 8, true)
	b.writeBits(0x05+48, 8, true)
	b.writeBits(0x06+48, 8, true)
	b.writeBits(4-2, 7, true)	// copy 4 zero bytes from 16 bytes back
	b.writeBits(7, 5, true)
	b.writeBits(3, 2, false)
	/*
		b.writeBits(0x00+48, 8, true)	// dn
		b.writeBits(0x00+48, 8, true)
		b.writeBits(0x00+48, 8, true)	// ds
		b.writeBits(0x00+48, 8, true)
	*/
	b.writeBits(0x01+48, 8, true)	// de
	b.writeBits(3-2, 7, true)	// copy 3 bytes from 2 bytes back
	b.writeBits(2-1, 5, true)
	/*
		b.writeBits(0x00+48, 8, true)
		b.writeBits(0x01+48, 8, true)	// entries
		b.writeBits(0x00+48, 8, true)
	*/
	b.writeBits(53+48, 8, true)	// size
	b.writeBits(0x00+48, 8, true)
	b.writeBits(0x00+48, 8, true)
	b.writeBits(0x00+48, 8, true)
	b.writeBits(0, 7, true)
	
	b.writeBits(1, 1, false)	// final literal block
	b.writeBits(0, 2, false)
	b.flushBits()
	b.bytes.WriteByte(6)
	b.bytes.WriteByte(0)
	b.bytes.WriteByte(^byte(6))
	b.bytes.WriteByte(^byte(0))
	tailsufOffset := b.bytes.Len()
	b.bytes.Write(zero[0:6])

	println(b.bytes.Len())
	ztail := b.bytes.Bytes()

	tailsuf := ztail[tailsufOffset:tailsufOffset+4]

	_, whole := makeGeneric(zhead, head, ztail, tail, nil)

	csize = len(whole) - len(head) - len(tail)
	uncsize = len(whole)
	headsize[4+0] = byte(csize) 
	headsize[4+1] = byte(csize>>8)
	headsize[8+0] = byte(uncsize)
	headsize[8+1] = byte(uncsize>>8)
	
	tail[20] = byte(csize)
	tail[21] = byte(csize>>8)
	tail[24] = byte(uncsize)
	tail[25] = byte(uncsize>>8)
	
	sufpos = len(head) + csize
	tailsuf[0+0] = byte(sufpos)
	tailsuf[0+1] = byte(sufpos>>8)
	tail[len(tail)-6+0] = byte(sufpos)
	tail[len(tail)-6+1] = byte(sufpos>>8)
	

	_, whole = makeGeneric(zhead, head, ztail, tail, nil)
	if uncsize != len(whole) {
		fmt.Println("no converge!", uncsize, len(whole))
		return
	}
	
	_, whole = makeGeneric(zhead, head, ztail, tail, headsize[0:4])
	if uncsize != len(whole) {
		fmt.Println("no converge2!", uncsize, len(whole))
		return
	}
	
	println("writing", len(whole))
	f, _ := os.Open("r.zip", os.O_CREAT|os.O_TRUNC|os.O_WRONLY, 0666)
	f.Write(whole)
	f.Close()
}


func makeGeneric(zhead, head, ztail, tail, crc []byte) (z, whole []byte) {
	if false {
		head0 := inflate(bytes.Add(zhead, []byte{0, 0, 0, 0xff, 0xff, 1, 0, 0, 0xff, 0xff}))
		if bytes.Compare(head, head0) != 0 {
			fmt.Println("zhead/head mismatch")
			fmt.Printf("head0: %x\n", head0)
			fmt.Printf("head1: %x\n", head)
			os.Exit(2)
		}
	}
	
	if false {
		tail0 := inflate(ztail)
		if bytes.Compare(tail, tail0) != 0 {
			fmt.Printf("ztail/tail mismatch\n%x\n%x", tail, tail0)
			os.Exit(2)
		}
	}

	const unit = 5
	
	// zhead
	var b wbuf
	b.bytes.Write(zhead)
	
	// LITn+1 zhead LITn+1
	b.lit(len(zhead)+unit)
	b.bytes.Write(zhead)
	b.lit(len(zhead)+unit)

	// REPn+1
	b.rep(len(zhead)+unit)
	
	// LIT1 REPn+1
	b.lit(unit)
	b.rep(len(zhead)+unit)
	
	// LIT1 LIT1
	b.lit(unit)
	b.lit(unit)

	// LIT4 REPn+1 LIT1 LIT1 LIT4
	b.lit(4*unit)
	b.rep(len(zhead)+unit)
	b.lit(unit)
	b.lit(unit)
	b.lit(4*unit)

	// REP4
	b.rep(4*unit)

	// LIT4 REP4 LIT4 REP4 LIT4
	b.lit(4*unit)
	b.rep(4*unit)
	b.lit(4*unit)
	b.rep(4*unit)
	b.lit(4*unit)

	// REP4
	b.rep(4*unit)

	// LIT4 REP4 NOP NOP LITm+1
	b.lit(4*unit)
	b.rep(4*unit)
	b.lit(0)
	b.lit(0)
	b.lit(len(ztail)+2*unit)

	// REP4
	b.rep(4*unit)
	
	// NOP NOP LITm+1 REPm+1 suffix
	b.lit(0)
	b.lit(0)
	b.lit(len(ztail)+2*unit)
	b.rep(len(ztail)+2*unit)
	b.lit(0)
	b.bytes.Write(ztail)
	
	// REPm+1
	b.rep(len(ztail)+2*unit)

	// suffix
	b.lit(0)
	b.bytes.Write(ztail)

	out := b.bytes.Bytes()
	fmt.Printf("enc: %x\n", out)

	// double-check
	{
	//	debugFlate = true
		r := NewInflater(bytes.NewBuffer(out))
		var b1 bytes.Buffer
		_, err := io.Copy(&b1, r)
		if err != nil {
			fmt.Printf("ERROR: %s\n", err)
			os.Exit(2)
		}
		r.Close()
	
		var b2 bytes.Buffer
		b2.Write(head)
		b2.Write(out)
		b2.Write(tail)
		if bytes.Compare(b1.Bytes(), b2.Bytes()) != 0 {
			fmt.Printf("have %d: %x\n", len(b1.Bytes()), b1.Bytes())
			fmt.Printf("want %d: %x\n", len(b2.Bytes()), b2.Bytes())
			os.Exit(2)
		}	
		whole = b1.Bytes()
	}

	// force crc
	if crc != nil {
		n := bytes.Count(whole, crc)	// look for crc
		embed := make([]int, n)
		off := 0
		for i := 0; i < n; i++ {
			j := bytes.Index(whole[off:], crc)
			if j < 0 {
				fmt.Println("missing crcs")
				return
			}
			off += j
			embed[i] = off
			off += 4
		}
fmt.Printf("embedded crc at %v (first=%d/%d)\n", embed, embed[0], len(whole))
		crc0 := uint32(0)
//crc0 = 0x8520b13d // gzip
		crcbase := crc32.ChecksumIEEE(whole[0:embed[0]])
		for {
			if crc0&0xfffff == 0 {
				fmt.Printf("%#x...", crc0)
			}
			for _, i := range embed {
				whole[i+0] = byte(crc0)
				whole[i+1] = byte(crc0>>8)
				whole[i+2] = byte(crc0>>16)
				whole[i+3] = byte(crc0>>24)
			}
			crc1 := crc32.Update(crcbase, crc32.IEEETable, whole[embed[0]:])
			if crc0 == crc1 {
				break
			}
			if crc0++; crc0 == 0 {
				fmt.Println("\nFAIL!\n")
				os.Exit(2)
			}
		}
		
		fmt.Printf("\nSUCCESS: %#x\n", crc0)
	}

	// double double-check
	{
	//	debugFlate = true
		r := NewInflater(bytes.NewBuffer(whole[len(head):len(head)+len(out)]))
		var b1 bytes.Buffer
		_, err := io.Copy(&b1, r)
		if err != nil {
			fmt.Printf("ERROR: %s\n", err)
			os.Exit(2)
		}
		r.Close()

		if bytes.Compare(b1.Bytes(), whole) != 0 {
			fmt.Printf("have %d %x\n", len(b1.Bytes()), b1.Bytes())
			fmt.Printf("want %d: %x\n", len(whole), whole)
			os.Exit(2)
		}	
		whole = b1.Bytes()
	}

	return out, whole
}

// A wbuf is a write buffer for bit-oriented data like deflate.
type wbuf struct {
	bytes bytes.Buffer
	bit uint32
	nbit uint
	final uint32
}

func (b *wbuf) writeBits(bit uint32, nbit uint, rev bool) {
	// reverse, for huffman codes
	if rev {
		br := uint32(0)
		for i := uint(0); i < nbit; i++ {
			if bit&(1<<i) != 0 {
				br |= 1<<(nbit-1-i)
			}
		}
		bit = br
	}
	b.bit |= bit << b.nbit
	b.nbit += nbit
	for b.nbit >= 8 {
		b.bytes.WriteByte(byte(b.bit))
		b.bit >>= 8
		b.nbit -= 8
	}
}

func (b *wbuf) flushBits() {
	if b.nbit > 0 {
		b.bytes.WriteByte(byte(b.bit))
		b.nbit = 0
		b.bit = 0
	}
}

func (b *wbuf) lit(n int) {
	b.writeBits(b.final, 1, false)
	b.writeBits(0, 2, false)	// data block
	b.flushBits()
	b1 := byte(n)
	b2 := byte(n>>8)
	b.bytes.WriteByte(b1)	// len
	b.bytes.WriteByte(b2)
	b.bytes.WriteByte(^b1)	// ^len
	b.bytes.WriteByte(^b2)
}

func (b *wbuf) rep(n int) {
	// generate copy n bytes at n bytes back.
	// must take 5 bytes to do it.  padding okay in last byte.

	b.writeBits(b.final, 1, false)
	b.writeBits(1, 2, false)	// compressed, fixed Huffman tables
	
	steal := uint(0)	// can steal at most 5

	// want 38-45 bits total.  have 3 above, 7 below.
	// leaves 28-35.
	switch {
	case 9 <= n && n <= 12:
		// length n/2 distance n
		b.writeBits(uint32(254+n/2)-256, 7, true)
		b.writeBits(6, 5, true)
		b.writeBits(uint32(n-8-1), 2, false)

		// length n-n/2 distance n
		b.writeBits(uint32(254+n-n/2)-256, 7, true)
		b.writeBits(6, 5, true)
		b.writeBits(uint32(n-8-1), 2, false)

	case 13 <= n && n <= 16:
		// length n/2 distance n
		b.writeBits(uint32(254+n/2)-256, 7, true)
		b.writeBits(7, 5, true)
		b.writeBits(uint32(n-12-1), 2, false)

		// length n-n/2 distance n
		b.writeBits(uint32(254+n-n/2)-256, 7, true)
		b.writeBits(7, 5, true)
		b.writeBits(uint32(n-12-1), 2, false)

	case 17 <= n && n <= 20:
		// length n/2 distance n
		b.writeBits(uint32(254+n/2)-256, 7, true)
		b.writeBits(8, 5, true)
		b.writeBits(uint32(n-16-1), 3, false)
	
		// length n-n/2 distance n
		b.writeBits(uint32(254+n-n/2)-256, 7, true)
		b.writeBits(8, 5, true)
		b.writeBits(uint32(n-16-1), 3, false)
	
	case n == 21:
		// length 10 distance 21
		b.writeBits(uint32(254+10)-256, 7, true)
		b.writeBits(8, 5, true)
		b.writeBits(uint32(n-16-1), 3, false)
		
		// length 11 distance 21
		b.writeBits(uint32(265)-256, 7, true)
		b.writeBits(0, 1, true)
		b.writeBits(8, 5, true)
		b.writeBits(uint32(n-16-1), 3, false)
		steal = 1

	case 22 <= n && n <= 24:
		// length n/2 distance n
		b.writeBits(uint32(265+(n/2-11)>>1)-256, 7, true)
		b.writeBits(uint32(n/2-11)&1, 1, false)
		b.writeBits(8, 5, true)
		b.writeBits(uint32(n-16-1), 3, false)
		
		// length n-n/2 distance n
		b.writeBits(uint32(265+(n-n/2-11)>>1)-256, 7, true)
		b.writeBits(uint32(n-n/2-11)&1, 1, false)
		b.writeBits(8, 5, true)
		b.writeBits(uint32(n-16-1), 3, false)
		steal = 2
	
	case 25 <= n && n <= 32:
		// length n/2 distance n
		b.writeBits(uint32(265+(n/2-11)>>1)-256, 7, true)
		b.writeBits(uint32(n/2-11)&1, 1, false)
		b.writeBits(9, 5, true)
		b.writeBits(uint32(n-24-1), 3, false)
		
		// length n-n/2 distance n
		b.writeBits(uint32(265+(n-n/2-11)>>1)-256, 7, true)
		b.writeBits(uint32(n-n/2-11)&1, 1, false)
		b.writeBits(9, 5, true)
		b.writeBits(uint32(n-24-1), 3, false)
		steal = 2

	case 33 <= n && n <= 36:
		// length n/2 distance n
		b.writeBits(uint32(265+(n/2-11)>>1)-256, 7, true)
		b.writeBits(uint32(n/2-11)&1, 1, false)
		b.writeBits(10, 5, true)
		b.writeBits(uint32(n-32-1), 4, false)

		// length n-n/2 distance n
		b.writeBits(uint32(265+(n-n/2-11)>>1)-256, 7, true)
		b.writeBits(uint32(n-n/2-11)&1, 1, false)
		b.writeBits(10, 5, true)
		b.writeBits(uint32(n-32-1), 4, false)
		steal = 4

	case 37 <= n && n <= 48:
		// length 18 distance n
		b.writeBits(uint32(265+(18-11)>>1)-256, 7, true)
		b.writeBits(uint32(18-11)&1, 1, false)
		b.writeBits(10, 5, true)
		b.writeBits(uint32(n-32-1), 4, false)
		
		// length n-18 distance n
		b.writeBits(uint32(269+(n-18-19)>>2)-256, 7, true)
		b.writeBits(uint32(n-18-19)&3, 2, false)
		b.writeBits(10, 5, true)
		b.writeBits(uint32(n-32-1), 4, false)
		steal = 5

	case 49 <= n && n <= 64:
		// length 10 distance n
		b.writeBits(uint32(254+10)-256, 7, true)
		b.writeBits(11, 5, true)
		b.writeBits(uint32(n-48-1), 4, false)

		// length n-10 distance n
		b.writeBits(uint32(273+(n-10-35)>>3)-256, 7, true)
		b.writeBits(uint32(n-10-35)&7, 3, false)

		b.writeBits(11, 5, true)
		b.writeBits(uint32(n-48-1), 4, false)
		steal = 5

	default:
		panic("cannot encode REP", n)
	}

	b.writeBits(0, 7-steal, true)	// 256: end of block
}

var inflateO, inflateB int

func deflate(data []byte, litNext bool, final bool) []byte {
	var buf bytes.Buffer
	w := flate.NewDeflater(&buf, 9)
	w.Write(data)
	w.Close()
	z := buf.Bytes()
	if final {
		return z
	}

	b1 := bytes.NewBuffer(z)
	var b2 bytes.Buffer
	r := NewInflater(b1)
	io.Copy(&b2, r)
	r.Close()

	if inflateB == 0 {
		return z[0:inflateO]
	}
	// otherwise we have to clear the final bit
	z[inflateO] ^= 1<<uint(inflateB)

println("inflated at", inflateO, inflateB, z[inflateO])
	if litNext {
		// literal begins with three zero bits and then flushes byte.
		// if this block ends at offset 6 or 7 we can round up.
		if inflateB >= 6 && len(z) == inflateO+1+5 && z[inflateO+1] == 0 && z[inflateO+2] == 0 && z[inflateO+3] == 0 && z[inflateO+4] == 0xff && z[inflateO+5] == 0xff {
			return z[0:inflateO+1]
		}
		// if this block ends before that and is zero, we can round down.
		if inflateB <= 5 && z[inflateO] == 0 {
			return z[0:inflateO]
		}
	}

	return z
}

func inflate(data []byte) []byte {
	r := NewInflater(bytes.NewBuffer(data))
	var b bytes.Buffer
	_, err := io.Copy(&b, r)
	r.Close()
	if err != nil {
		fmt.Println("INFLATE:", err)
	}

	return b.Bytes()
}


//-----------------------------------------------------------
// copy of inflate.go with debugging prints added
// (for debugging the input, not the code).

// Copyright 2009 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// The flate package implements the DEFLATE compressed data
// format, described in RFC 1951.  The gzip and zlib packages
// implement access to DEFLATE-based file formats.

const (
	maxCodeLen = 16    // max length of Huffman code
	maxHist    = 32768 // max history required
	maxLit     = 286
	maxDist    = 32
	numCodes   = 19 // number of codes in Huffman meta-code
)

// A CorruptInputError reports the presence of corrupt input at a given offset.
type CorruptInputError int64

func (e CorruptInputError) String() string {
	return "flate: corrupt input before offset " + strconv.Itoa64(int64(e))
}

// An InternalError reports an error in the flate code itself.
type InternalError string

func (e InternalError) String() string { return "flate: internal error: " + string(e) }

// A ReadError reports an error encountered while reading input.
type ReadError struct {
	Offset int64    // byte offset where error occurred
	Error  os.Error // error returned by underlying Read
}

func (e *ReadError) String() string {
	return "flate: read error at offset " + strconv.Itoa64(e.Offset) + ": " + e.Error.String()
}

// A WriteError reports an error encountered while writing output.
type WriteError struct {
	Offset int64    // byte offset where error occurred
	Error  os.Error // error returned by underlying Read
}

func (e *WriteError) String() string {
	return "flate: write error at offset " + strconv.Itoa64(e.Offset) + ": " + e.Error.String()
}

// Huffman decoder is based on
// J. Brian Connell, ``A Huffman-Shannon-Fano Code,''
// Proceedings of the IEEE, 61(7) (July 1973), pp 1046-1047.
type huffmanDecoder struct {
	// min, max code length
	min, max int

	// limit[i] = largest code word of length i
	// Given code v of length n,
	// need more bits if v > limit[n].
	limit [maxCodeLen + 1]int

	// base[i] = smallest code word of length i - seq number
	base [maxCodeLen + 1]int

	// codes[seq number] = output code.
	// Given code v of length n, value is
	// codes[v - base[n]].
	codes []int
}

// Initialize Huffman decoding tables from array of code lengths.
func (h *huffmanDecoder) init(bits []int) bool {
	// TODO(rsc): Return false sometimes.

	// Count number of codes of each length,
	// compute min and max length.
	var count [maxCodeLen + 1]int
	var min, max int
	for _, n := range bits {
		if n == 0 {
			continue
		}
		if min == 0 || n < min {
			min = n
		}
		if n > max {
			max = n
		}
		count[n]++
	}
	if max == 0 {
		return false
	}

	h.min = min
	h.max = max


	// For each code range, compute
	// nextcode (first code of that length),
	// limit (last code of that length), and
	// base (offset from first code to sequence number).
	code := 0
	seq := 0
	var nextcode [maxCodeLen]int
	for i := min; i <= max; i++ {
		n := count[i]
		nextcode[i] = code
		h.base[i] = code - seq
		code += n
		seq += n
		h.limit[i] = code - 1
		code <<= 1
	}

	// Make array mapping sequence numbers to codes.
	if len(h.codes) < len(bits) {
		h.codes = make([]int, len(bits))
	}
	for i, n := range bits {
		if n == 0 {
			continue
		}
		code := nextcode[n]
		nextcode[n]++
		seq := code - h.base[n]
		h.codes[seq] = i
	}
	return true
}

// Hard-coded Huffman tables for DEFLATE algorithm.
// See RFC 1951, section 3.2.6.
var fixedHuffmanDecoder = huffmanDecoder{
	7, 9,
	[maxCodeLen + 1]int{7: 23, 199, 511},
	[maxCodeLen + 1]int{7: 0, 24, 224},
	[]int{
		// length 7: 256-279
		256, 257, 258, 259, 260, 261, 262,
		263, 264, 265, 266, 267, 268, 269,
		270, 271, 272, 273, 274, 275, 276,
		277, 278, 279,

		// length 8: 0-143
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
		12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
		22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
		32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
		42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
		62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
		72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
		82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
		92, 93, 94, 95, 96, 97, 98, 99, 100,
		101, 102, 103, 104, 105, 106, 107, 108,
		109, 110, 111, 112, 113, 114, 115, 116,
		117, 118, 119, 120, 121, 122, 123, 124,
		125, 126, 127, 128, 129, 130, 131, 132,
		133, 134, 135, 136, 137, 138, 139, 140,
		141, 142, 143,

		// length 8: 280-287
		280, 281, 282, 283, 284, 285, 286, 287,

		// length 9: 144-255
		144, 145, 146, 147, 148, 149, 150, 151,
		152, 153, 154, 155, 156, 157, 158, 159,
		160, 161, 162, 163, 164, 165, 166, 167,
		168, 169, 170, 171, 172, 173, 174, 175,
		176, 177, 178, 179, 180, 181, 182, 183,
		184, 185, 186, 187, 188, 189, 190, 191,
		192, 193, 194, 195, 196, 197, 198, 199,
		200, 201, 202, 203, 204, 205, 206, 207,
		208, 209, 210, 211, 212, 213, 214, 215,
		216, 217, 218, 219, 220, 221, 222, 223,
		224, 225, 226, 227, 228, 229, 230, 231,
		232, 233, 234, 235, 236, 237, 238, 239,
		240, 241, 242, 243, 244, 245, 246, 247,
		248, 249, 250, 251, 252, 253, 254, 255,
	},
}

// The actual read interface needed by NewInflater.
// If the passed in io.Reader does not also have ReadByte,
// the NewInflater will introduce its own buffering.
type Reader interface {
	io.Reader
	ReadByte() (c byte, err os.Error)
}

// Inflate state.
type inflater struct {
	// Input/output sources.
	r       Reader
	w       io.Writer
	roffset int64
	woffset int64

	// Input bits, in top of b.
	b  uint32
	nb uint

	// Huffman decoders for literal/length, distance.
	h1, h2 huffmanDecoder

	// Length arrays used to define Huffman codes.
	bits     [maxLit + maxDist]int
	codebits [numCodes]int

	// Output history, buffer.
	hist  [maxHist]byte
	hp    int  // current output position in buffer
	hfull bool // buffer has filled at least once

	// Temporary buffer (avoids repeated allocation).
	buf [4]byte
}

func (f *inflater) inflate() (err os.Error) {
	final := false
	for err == nil && !final {
		for f.nb < 1+2 {
			if err = f.moreBits(); err != nil {
				return
			}
		}
		final = f.b&1 == 1
		f.b >>= 1
		typ := f.b & 3
		f.b >>= 2
		f.nb -= 1 + 2
		if final {
			o := int(f.roffset) - 1
			b := 8 - int(f.nb) - 3
			if b < 0 {
				o--
				b += 8
			}
			inflateO = o
			inflateB = b
		}
		switch typ {
		case 0:
			err = f.dataBlock()
		case 1:
			// compressed, fixed Huffman tables
			err = f.decodeBlock(&fixedHuffmanDecoder, nil)
		case 2:
			// compressed, dynamic Huffman tables
			if err = f.readHuffman(); err == nil {
				err = f.decodeBlock(&f.h1, &f.h2)
			}
		default:
			// 3 is reserved.
			err = CorruptInputError(f.roffset)
		}
	}
	return
}

// RFC 1951 section 3.2.7.
// Compression with dynamic Huffman codes

var codeOrder = [...]int{16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15}

func (f *inflater) readHuffman() os.Error {
	// HLIT[5], HDIST[5], HCLEN[4].
	for f.nb < 5+5+4 {
		if err := f.moreBits(); err != nil {
			return err
		}
	}
	nlit := int(f.b&0x1F) + 257
	f.b >>= 5
	ndist := int(f.b&0x1F) + 1
	f.b >>= 5
	nclen := int(f.b&0xF) + 4
	f.b >>= 4
	f.nb -= 5 + 5 + 4

	// (HCLEN+4)*3 bits: code lengths in the magic codeOrder order.
	for i := 0; i < nclen; i++ {
		for f.nb < 3 {
			if err := f.moreBits(); err != nil {
				return err
			}
		}
		f.codebits[codeOrder[i]] = int(f.b & 0x7)
		f.b >>= 3
		f.nb -= 3
	}
	for i := nclen; i < len(codeOrder); i++ {
		f.codebits[codeOrder[i]] = 0
	}
	if !f.h1.init(&f.codebits) {
		return CorruptInputError(f.roffset)
	}

	// HLIT + 257 code lengths, HDIST + 1 code lengths,
	// using the code length Huffman code.
	for i, n := 0, nlit+ndist; i < n; {
		x, err := f.huffSym(&f.h1)
		if err != nil {
			return err
		}
		if x < 16 {
			// Actual length.
			f.bits[i] = x
			i++
			continue
		}
		// Repeat previous length or zero.
		var rep int
		var nb uint
		var b int
		switch x {
		default:
			return InternalError("unexpected length code")
		case 16:
			rep = 3
			nb = 2
			if i == 0 {
				return CorruptInputError(f.roffset)
			}
			b = f.bits[i-1]
		case 17:
			rep = 3
			nb = 3
			b = 0
		case 18:
			rep = 11
			nb = 7
			b = 0
		}
		for f.nb < nb {
			if err := f.moreBits(); err != nil {
				return err
			}
		}
		rep += int(f.b & uint32(1<<nb-1))
		f.b >>= nb
		f.nb -= nb
		if i+rep > n {
			return CorruptInputError(f.roffset)
		}
		for j := 0; j < rep; j++ {
			f.bits[i] = b
			i++
		}
	}

	if !f.h1.init(f.bits[0:nlit]) || !f.h2.init(f.bits[nlit:nlit+ndist]) {
		return CorruptInputError(f.roffset)
	}

	return nil
}

// Decode a single Huffman block from f.
// hl and hd are the Huffman states for the lit/length values
// and the distance values, respectively.  If hd == nil, using the
// fixed distance encoding associated with fixed Huffman blocks.
func (f *inflater) decodeBlock(hl, hd *huffmanDecoder) os.Error {
	for {
		v, err := f.huffSym(hl)
		if err != nil {
			return err
		}
		var n uint // number of bits extra
		var length int
		switch {
		case v < 256:
			if debugFlate {
				fmt.Println("BYTE", v)
			}
			f.hist[f.hp] = byte(v)
			f.hp++
			if f.hp == len(f.hist) {
				if err = f.flush(); err != nil {
					return err
				}
			}
			continue
		case v == 256:
			return nil
		// otherwise, reference to older data
		case v < 265:
			length = v - (257 - 3)
			n = 0
		case v < 269:
			length = v*2 - (265*2 - 11)
			n = 1
		case v < 273:
			length = v*4 - (269*4 - 19)
			n = 2
		case v < 277:
			length = v*8 - (273*8 - 35)
			n = 3
		case v < 281:
			length = v*16 - (277*16 - 67)
			n = 4
		case v < 285:
			length = v*32 - (281*32 - 131)
			n = 5
		default:
			length = 258
			n = 0
		}
		if n > 0 {
			for f.nb < n {
				if err = f.moreBits(); err != nil {
					return err
				}
			}
			length += int(f.b & uint32(1<<n-1))
			f.b >>= n
			f.nb -= n
		}

		var dist int
		if hd == nil {
			for f.nb < 5 {
				if err = f.moreBits(); err != nil {
					return err
				}
			}
			dist = int(reverseByte[(f.b&0x1F)<<3])
			f.b >>= 5
			f.nb -= 5
		} else {
			if dist, err = f.huffSym(hd); err != nil {
				return err
			}
		}

		switch {
		case dist < 4:
			dist++
		case dist >= 30:
			return CorruptInputError(f.roffset)
		default:
			nb := uint(dist-2) >> 1
			// have 1 bit in bottom of dist, need nb more.
			extra := (dist & 1) << nb
			for f.nb < nb {
				if err = f.moreBits(); err != nil {
					return err
				}
			}
			extra |= int(f.b & uint32(1<<nb-1))
			f.b >>= nb
			f.nb -= nb
			dist = 1<<(nb+1) + 1 + extra
		}
		
		if debugFlate {
			fmt.Println("REP", dist, length)
		}
		// Copy history[-dist:-dist+length] into output.
		if dist > len(f.hist) {
			return InternalError("bad history distance")
		}

		// No check on length; encoding can be prescient.
		if !f.hfull && dist > f.hp {
			return CorruptInputError(f.roffset)
		}

		p := f.hp - dist
		if p < 0 {
			p += len(f.hist)
		}
		for i := 0; i < length; i++ {
			f.hist[f.hp] = f.hist[p]
			f.hp++
			p++
			if f.hp == len(f.hist) {
				if err = f.flush(); err != nil {
					return err
				}
			}
			if p == len(f.hist) {
				p = 0
			}
		}
	}
	panic("unreached")
}

// Copy a single uncompressed data block from input to output.
func (f *inflater) dataBlock() os.Error {
	// Uncompressed.
	// Discard current half-byte.
	f.nb = 0
	f.b = 0

	// Length then ones-complement of length.
	nr, err := io.ReadFull(f.r, f.buf[0:4])
	f.roffset += int64(nr)
	if err != nil {
		return &ReadError{f.roffset, err}
	}
	n := int(f.buf[0]) | int(f.buf[1])<<8
	nn := int(f.buf[2]) | int(f.buf[3])<<8
	if uint16(nn) != uint16(^n) {
		return CorruptInputError(f.roffset)
	}

	if debugFlate {
		fmt.Println("LIT", n)
	}
	// Read len bytes into history,
	// writing as history fills.
	for n > 0 {
		m := len(f.hist) - f.hp
		if m > n {
			m = n
		}
		m, err := io.ReadFull(f.r, f.hist[f.hp:f.hp+m])
		f.roffset += int64(m)
		if err != nil {
			return &ReadError{f.roffset, err}
		}
		n -= m
		f.hp += m
		if f.hp == len(f.hist) {
			if err = f.flush(); err != nil {
				return err
			}
		}
	}
	return nil
}

func (f *inflater) moreBits() os.Error {
	c, err := f.r.ReadByte()
	if err != nil {
		if err == os.EOF {
			err = io.ErrUnexpectedEOF
		}
		return err
	}
	f.roffset++
	f.b |= uint32(c) << f.nb
	f.nb += 8
	return nil
}

// Read the next Huffman-encoded symbol from f according to h.
func (f *inflater) huffSym(h *huffmanDecoder) (int, os.Error) {
	for n := uint(h.min); n <= uint(h.max); n++ {
		lim := h.limit[n]
		if lim == -1 {
			continue
		}
		for f.nb < n {
			if err := f.moreBits(); err != nil {
				return 0, err
			}
		}
		v := int(f.b & uint32(1<<n-1))
		v <<= 16 - n
		v = int(reverseByte[v>>8]) | int(reverseByte[v&0xFF])<<8 // reverse bits
		if v <= lim {
			f.b >>= n
			f.nb -= n
			return h.codes[v-h.base[n]], nil
		}
	}
	return 0, CorruptInputError(f.roffset)
}

// Flush any buffered output to the underlying writer.
func (f *inflater) flush() os.Error {
	if f.hp == 0 {
		return nil
	}
	n, err := f.w.Write(f.hist[0:f.hp])
	if n != f.hp && err == nil {
		err = io.ErrShortWrite
	}
	if err != nil {
		return &WriteError{f.woffset, err}
	}
	f.woffset += int64(f.hp)
	f.hp = 0
	f.hfull = true
	return nil
}

func makeReader(r io.Reader) Reader {
	if rr, ok := r.(Reader); ok {
		return rr
	}
	return bufio.NewReader(r)
}

// Inflate reads DEFLATE-compressed data from r and writes
// the uncompressed data to w.
func (f *inflater) inflater(r io.Reader, w io.Writer) os.Error {
	f.r = makeReader(r)
	f.w = w
	f.woffset = 0
	if err := f.inflate(); err != nil {
		return err
	}
	if err := f.flush(); err != nil {
		return err
	}
	return nil
}

// NewInflater returns a new ReadCloser that can be used
// to read the uncompressed version of r.  It is the caller's
// responsibility to call Close on the ReadCloser when
// finished reading.
func NewInflater(r io.Reader) io.ReadCloser {
	var f inflater
	pr, pw := io.Pipe()
	go func() { pw.CloseWithError(f.inflater(r, pw)) }()
	return pr
}

var reverseByte = [256]byte{
	0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
	0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
	0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
	0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
	0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
	0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
	0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
	0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
	0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
	0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
	0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
	0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
	0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
	0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
	0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
	0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
	0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
	0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
	0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
	0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
	0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
	0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
	0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
	0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
	0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
	0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
	0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
	0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
	0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
	0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
	0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
	0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
}

func reverseUint16(v uint16) uint16 {
	return uint16(reverseByte[v>>8]) | uint16(reverseByte[v&0xFF])<<8
}

func reverseBits(number uint16, bitLength byte) uint16 {
	return reverseUint16(number << uint8(16-bitLength))
}
