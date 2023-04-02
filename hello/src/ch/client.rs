use socket2::{Domain, Protocol, Socket, Type};
use std::{
    io::{IoSlice, Write},
    net::SocketAddr,
};

/** soime stuff */
const IPV: Domain = Domain::IPV4;
const SOCK_TYPE: socket2::Type = Type::STREAM;
const PROT: Protocol = Protocol::TCP;

pub fn default_sock() -> Socket {
    Socket::new(IPV, SOCK_TYPE, Some(PROT)).expect("could not create socket")
}
pub fn main() {
    //let port = unimplemented!();
    let port = "8080";
    let socket = default_sock();
    //let socket_addr = SocketAddrV4::new(Ipv4Addr::new(127, 0, 0, 1), 8080);

    let sock_addr: SocketAddr = format!("127.0.0.1:{port}")
        .parse()
        .expect("cannot create adddr");

    // socket
    //     .connect(&sock_addr.into())
    //     .expect(&format!("cannot connect to addr {:?}", sock_addr));

    let msg1 = vec!["Hello, World!"];
    let msg2 = vec!["How", "are", "you?"];

    let msg = "This is a message";
    socket.send(msg.as_bytes()).expect("");
    //socket.write(msg.as_bytes()).expect("could not write to socket");

    println!("DONE");
}

//let address: SocketAddr = "[::1]:12345".parse().unwrap();
//let address = address.into();
//socket.bind(&address)?;
//socket.listen(128)?;

//let listener: TcpListener = socket.into();
