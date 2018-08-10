use std::env;
use std::io;
use std::io::{Read};
use std::net::{ToSocketAddrs, TcpStream};

fn main() ->io::Result<()> {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        eprintln!("Incorrect number of arguments. usage: {}:<domain_name>", args[0]);
        return Ok(())
    }

    let sock_addr_iter = args[1].to_socket_addrs()?;
    let sock_addr = sock_addr_iter.last().unwrap();

    println!("{:?}", sock_addr);
    let mut tcp_stream = TcpStream::connect(sock_addr).unwrap();
    let mut time_string  = String::new();
    let bytes_read = tcp_stream.read_to_string(&mut time_string).unwrap();
    println!("nbytes:{} time:{}", bytes_read, time_string);
    Ok(())
}
