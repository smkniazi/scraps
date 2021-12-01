package main

import (
	"log"
)

/*
  #cgo CXXFLAGS: -I/usr/local/mysql/include
  #cgo CXXFLAGS: -I/usr/local/mysql/include/storage/ndb
  #cgo CXXFLAGS: -I/usr/local/mysql/include/storage/ndb/ndbapi
  #cgo CXXFLAGS: -I/usr/local/mysql/include/storage/ndb/mgmapi
  //#cgo CXXFLAGS: -g -Wall
  // #cgo LDFLAGS: -lstdc++
  #cgo LDFLAGS: -L/usr/local/mysql/lib/libndbclient.so.6.1.0
  #include "ndb_api_wrapper.h"
  #include "dummy.cc"
*/
import "C"

func main() {
	//Call to void function without params
	err := Hello()
	if err != nil {
		log.Fatal(err)
	}
}

func Hello() error {
  C.hello_world() //We ignore first result as it is a void function
	return nil
}

