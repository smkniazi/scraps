pub const true_: u32 = 1;
pub const false_: u32 = 0;
pub const __bool_true_false_are_defined: u32 = 1;
extern "C" {
    pub fn do_write(key: ::std::os::raw::c_longlong, value: *mut ::std::os::raw::c_char) -> bool;
}
extern "C" {
    pub fn do_read(
        key: ::std::os::raw::c_longlong,
        first_byte: *mut ::std::os::raw::c_char,
    ) -> ::std::os::raw::c_longlong;
}
extern "C" {
    pub fn do_delete(key: ::std::os::raw::c_longlong) -> bool;
}
extern "C" {
    pub fn initialize(connectstring: *const ::std::os::raw::c_char) -> bool;
}
extern "C" {
    pub fn shutdown();
}
extern "C" {
    pub fn hello_world();
}

const bufferlen: usize = 4096+2;
const iterations: i32 = 500000;
fn main() {
    unsafe{
        let mut data: [i8; bufferlen] = [1; bufferlen];
	    data[0] = (4096 % 256) as i8;
	    data[1] = (4096 / 256) as i8;

        let addressStr = "localhost";
        let mut addressCStr: [i8; 1024] = [0; 1024];

        for (i, c) in addressStr.chars().enumerate() {
            addressCStr[i] = 0;
        }

        initialize(addressCStr.as_mut_ptr());

        // write
        for i in 0..iterations {
           let mut mydata: [i8; bufferlen] = [0; bufferlen];
           mydata[..].clone_from_slice(&data);
           do_write(i as i64, mydata.as_mut_ptr());
        }

        for i in 0..iterations {
           let mut mydata: [i8; 4096] = [0; 4096];
           let retSize = do_read(i as i64, mydata.as_mut_ptr());
           if  retSize != 4096 {
                panic!("Wrong data read");
           }
        }
    }
}

fn sumArray(data: &[i8]) {
    // println!("Length {}", arr.len())

     let mut sum:i64 = 0;
    for c in data.iter() {
        sum += (*c as i64);
    }
    println!("Total sum {}", sum);
}
