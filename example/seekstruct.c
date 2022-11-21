#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define MAX_DESC 16

struct location {
  long x;
  long y;
  char description[MAX_DESC];
};

typedef struct location location_t;

location_t *build( long x, long y, char* desc ) {
  location_t *p = malloc( sizeof(location_t) );
  if( p != NULL ) {
    p->x = x;
    p->y = y;
    strncpy(p->description, desc, MAX_DESC-1);
  }
  return p;
}

void print( location_t *loc ) {
  printf( "%s <%ld:%ld>\n", loc->description, loc->x, loc->y );
}

int append( int fd, location_t *loc ) {
  int size = sizeof( *loc );
  char *bytes = (char*) loc;
  int wrote;
  int off = lseek( fd, 0, SEEK_END );
  if( off < 0 ) return off;
   errno = 0;
   while (size) {
     wrote = write(fd, bytes, size);
      if (wrote <= 0) {
         if (errno != EINTR && errno != EAGAIN) {
            return -1;
         }
         errno = 0;
         continue;
      }
      size -= wrote;
      bytes += wrote;
   }
   return 0;
}

int readN( int fd, int nth, location_t *loc ) {
  int size = sizeof( *loc );
  char *buffer = malloc(size);
  int rd;
  int rem = size;
  char *curr = buffer;
  if( buffer == NULL ) return -1;
  int off = lseek( fd, nth*size, SEEK_SET );
  if( off < 0 ) return off;
  while( (rd = read( fd, curr, rem ) ) > 0 ) {
    curr += rd;
    rem -= rd;
  }
  memcpy( loc, buffer, size );
  free( buffer );
  return 0;
}


int main( void ) { 
  location_t *l1 = build( 12, 34, "Home" );
  location_t *l2 = build( 56, 78, "Office" );
  location_t loc;
  int fd = open( "test.out", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR );
  print( l1 );
  print( l2 );
  append( fd, l1 );
  append( fd, l2 );
  readN(fd, 1, &loc);
  print( &loc );
  readN(fd, 1, &loc);
  print( &loc );
  close(fd);
  free(l1);
  free(l2);
  return 0;
}
