use std::net::{SocketAddr, TcpListener};
use socket2::{Socket, Domain, Type, Protocol};

/** soime stuff */
const IPV: Domain = Domain::IPV4;
const SOCK_TYPE: socket2::Type = Type::STREAM;
const PROT: Protocol = Protocol::TCP;

fn default_sock() {
    Socket::new(IPV, SOCK_TYPE, PROT)
}

fn main() {
    let port = unimplemented!();

    let socket = default_sock();
    
    let sock_addr = "127.0.0.1".parse();
}


//let address: SocketAddr = "[::1]:12345".parse().unwrap();
//let address = address.into();
//socket.bind(&address)?;
//socket.listen(128)?;

//let listener: TcpListener = socket.into();