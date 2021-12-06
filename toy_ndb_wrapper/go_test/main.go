package main

import (
	"fmt"
	"log"
	"unsafe"
)

/*
#cgo CFLAGS: -g -Wall
#cgo LDFLAGS: -L./../lib/build -lndbapiwrapper
#cgo LDFLAGS: -L/usr/local/mysql/lib -lndbclient
#include <stdlib.h>
#include "../lib/ndb_api_wrapper.h"
#include "dummy.cc"
*/
import "C"

func main() {
	if !initialize("localhost") {
		log.Fatalf("Failed to connect")
	}

	iterations := 100000

	data := make([]byte, 4096+2)
	data[0] = 4096 % 256
	data[1] = 4096 / 256

	for i := 2; i < len(data); i++ {
		data[i] = 1
	}

	// sumArray(data[:], 2, 4098)

	for i := 0; i < iterations; i++ {
		if !do_write(i, data) {
			log.Fatalf("Failed to write item %d", i)
		}
	}

	for i := 0; i < iterations; i++ {
		read := do_read(i)
		if read < 0 || read != 4096 {
			log.Fatalf("Failed to read item %d", i)
		}
	}

	shutdown()

}

func initialize(connStr string) bool {
	cs := C.CString(connStr)
	b := C.initialize(cs)
	C.free(unsafe.Pointer(cs))
	return bool(b)
}

func do_write(key int, data []byte) bool {
	p := C.malloc(C.size_t(len(data)))
	defer C.free(p)

	const size int = 4096 + 2
	// copy the data into the buffer, by converting it to a Go array
	cBuf := (*[size]byte)(p)
	copy(cBuf[:], data)

	ck := C.longlong(key)
	b := C.do_write(ck, (*C.char)(p))
	return bool(b)
}

func do_read(key int) int64 {
	p := C.malloc(C.size_t(4096 + 2))
	defer C.free(p)

	const size int = 4096 + 2
	// copy the data into the buffer, by converting it to a Go array

	ck := C.longlong(key)
	v := C.do_read(ck, (*C.char)(p))
	// cBuf := (*[size]byte)(p)
	// sumArray(cBuf[:], 0, 4096)

	return int64(v)
}

func sumArray(data []byte, start int, end int) {

	var sum int = 0
	for i := start; i < end; i++ {
		fmt.Printf("%d , ", int(data[i]))
		sum += int(data[i])
	}
	fmt.Println()

	fmt.Println("Total of data: ", sum)

}

func shutdown() {
	// C.shutdown()
}
