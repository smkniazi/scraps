package main

import (
	"log"
	"unsafe"
)

/*
#cgo CFLAGS: -g -Wall
#cgo LDFLAGS: -L./lib/build -lndbapiwrapper
#cgo LDFLAGS: -L/usr/local/mysql/lib -lndbclient
#include <stdlib.h>
#include "lib/ndb_api_wrapper.h"
#include "dummy.cc"
*/
import "C"

func main() {
	if !initialize("localhost") {
		log.Fatalf("Failed to connect")
	}

	iterations := 100000

	for i := 0; i < iterations; i++ {
		if !do_write(i, i) {
			log.Fatalf("Failed to write item %d", i)
		}
	}

	for i := 0; i < iterations; i++ {
		read := do_read(i)
		if read < 0 {
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

func do_write(key int, val int) bool {
	ck := C.longlong(key)
	cv := C.longlong(val)
	b := C.do_write(ck, cv)
	return bool(b)
}

func do_read(key int) int64 {
	ck := C.longlong(key)
	v := C.do_read(ck)
	return int64(v)
}

func shutdown() {
	// C.shutdown()
}

//func Hello() error {
//	C.hello_world()
//	return nil
//}

//#cgo CXXFLAGS: -I/usr/local/mysql/include
//#cgo CXXFLAGS: -I/usr/local/mysql/include/storage/ndb
//#cgo CXXFLAGS: -I/usr/local/mysql/include/storage/ndb/ndbapi
//#cgo CXXFLAGS: -I/usr/local/mysql/include/storage/ndb/mgmapi
