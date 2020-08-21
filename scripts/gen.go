package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"math/rand"
	"net"
	"os"
	"path/filepath"
	"strconv"
	"time"

	"github.com/juju/ratelimit"
)

type Addr struct {
	IP   [4]byte
	Port uint16
}

func getAddr(str string) Addr {
	var a Addr
	host, port, _ := net.SplitHostPort(str)

	p, _ := strconv.ParseUint(port, 10, 64)
	a.Port = uint16(p)

	copy(a.IP[:], net.ParseIP(host).To4())

	return a
}

var locals = []Addr{
	getAddr("192.168.67.236:45678"),
	getAddr("192.168.67.236:54321"),
	getAddr("192.168.67.236:11122"),
	getAddr("192.168.67.181:56781"),
	getAddr("192.168.67.181:34123"),
}
var remotes = []Addr{
	getAddr("10.1.12.30:22"),
	getAddr("10.1.12.31:22"),
	getAddr("139.145.19.5:2222"),
	getAddr("8.8.8.8:22"),
	getAddr("10.1.13.30:2222"),
	getAddr("10.1.13.31:2222"),
	getAddr("18.19.1.127:22"),
	getAddr("100.99.87.1:2222"),
}

func Local() Addr {
	n := rand.Intn(len(locals))
	return locals[n]
}

func Remote() Addr {
	n := rand.Intn(len(remotes))
	return remotes[n]
}

func init() {
	rand.Seed(time.Now().Unix())
}

func main() {
	flag.Parse()
	filepath.Walk(flag.Arg(1), func(file string, i os.FileInfo, err error) error {
		if err != nil || i.IsDir() {
			return err
		}
		time.Sleep(time.Millisecond * 100)
		return readFile(flag.Arg(0), file)
	})
}

func readFile(addr, file string) error {
	r, err := os.Open(file)
	if err != nil {
		return err
	}
	defer r.Close()

	s, err := r.Stat()
	if err != nil {
		return err
	}

	c, err := Dial(addr, file, s.Size())
	if err != nil {
		return err
	}
	defer c.Close()

	var (
		writer = ratelimit.Writer(c, ratelimit.NewBucketWithRate(4<<20, 4<<20))
		buffer = make([]byte, 1<<15)
	)

	now := time.Now()
	_, err = io.CopyBuffer(writer, r, buffer)
	fmt.Printf("done copying %s (%s) - %v\n", file, time.Since(now), err)
	return err
}

type conn struct {
	net.Conn

	state struct {
		File    [256]byte
		Size    int64
		Curr    int64
		Written int64
		Src     Addr
		Dst     Addr
	}
}

func Dial(addr, file string, size int64) (net.Conn, error) {
	c, err := net.Dial("udp", addr)
	if err != nil {
		return nil, err
	}
	co := conn{
		Conn: c,
	}
	co.state.Size = size
	co.state.Src = Local()
	co.state.Dst = Remote()
	copy(co.state.File[:], filepath.Clean(file))

	return &co, binary.Write(co.Conn, binary.LittleEndian, co.state)
}

func (c *conn) Write(b []byte) (int, error) {
	n := len(b)

	c.state.Written = int64(n)
	c.state.Curr += c.state.Written
	if err := binary.Write(c.Conn, binary.LittleEndian, c.state); err != nil {
		return 0, err
	}
	time.Sleep(150 * time.Microsecond)
	return n, nil
}
