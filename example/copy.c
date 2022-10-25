#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int copy(const char *from, const char *to) {
  int fd_to, fd_from;
  char buf[4096];
  ssize_t nread;

  fd_from = open(from, O_RDONLY);
  if (fd_from < 0) {
    fprintf( stderr, "Could not open the file %s: %s\n", 
	     from, strerror(errno) );
    return -1;
  }

  fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0600);
  if (fd_to < 0) {
    int savedError =errno;
    close( fd_from );
    fprintf( stderr, "Could not open the file %s: %s\n", 
	     to, strerror(savedError) );
    return -1;
  }

  while( nread = read(fd_from, buf, sizeof buf), nread > 0 ) {
    char *out_ptr = buf;
    ssize_t nwritten;
    do {
      nwritten = write(fd_to, out_ptr, nread);
      if (nwritten >= 0) {
	    nread -= nwritten;
	    out_ptr += nwritten;
      } else if (errno != EINTR) {
	    int savedError =errno;
	    close( fd_from );
	    close( fd_to );
	    fprintf( stderr, "Could not copy  %s to %s: %s\n", 
		     from, to, strerror(savedError) );
	    return -1;
      }
    } while (nread > 0);
  }

  if (nread != 0) {
    int savedError =errno;
    close( fd_from );
    close( fd_to );
    fprintf( stderr, "Could not read %s: %s\n", 
	     from, strerror(savedError) );
    return -1;
  }

  close( fd_from );
  close( fd_to );
  //Il faudrait gérer les erreurs de fermeture

  return 0;
}

int main( int argc, char **argv ) {

  if( argc != 3 ) {
    printf( "Usage: %s <infile> <outfile>\n", argv[0] );
    exit( EXIT_FAILURE );
  }

  char *in = argv[1];
  char *out = argv[2];
  
  if( copy( in, out ) < 0 ) {
    exit( EXIT_FAILURE );
  } else {
    exit( EXIT_SUCCESS );
  }

}
