/*
   BLAKE2 reference source code package - b2sum tool

   Written in 2012 by Samuel Neves <sneves@dei.uc.pt>

   To the extent possible under law, the author(s) have dedicated all copyright
   and related and neighboring rights to this software to the public domain
   worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with
   this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>

#include "blake2.h"

/* This will help compatibility with coreutils */
int blake2s_stream( FILE *stream, void *resstream )
{
  int ret = -1;
  size_t sum, n;
  blake2s_state S[1];
  static const size_t buffer_length = 32768;
  uint8_t *buffer = ( uint8_t * )malloc( buffer_length );

  if( !buffer ) return -1;

  blake2s_init( S, BLAKE2S_OUTBYTES );

  while( 1 )
  {
    sum = 0;

    while( 1 )
    {
      n = fread( buffer + sum, 1, buffer_length - sum, stream );
      sum += n;

      if( buffer_length == sum )
        break;

      if( 0 == n )
      {
        if( ferror( stream ) )
          goto cleanup_buffer;

        goto final_process;
      }

      if( feof( stream ) )
        goto final_process;
    }

    blake2s_update( S, buffer, buffer_length );
  }

final_process:;

  if( sum > 0 ) blake2s_update( S, buffer, sum );

  blake2s_final( S, resstream, BLAKE2S_OUTBYTES );
  ret = 0;
cleanup_buffer:
  free( buffer );
  return ret;
}

int blake2b_stream( FILE *stream, void *resstream )
{
  int ret = -1;
  size_t sum, n;
  blake2b_state S[1];
  static const size_t buffer_length = 32768;
  uint8_t *buffer = ( uint8_t * )malloc( buffer_length );

  if( !buffer ) return -1;

  blake2b_init( S, BLAKE2B_OUTBYTES );

  while( 1 )
  {
    sum = 0;

    while( 1 )
    {
      n = fread( buffer + sum, 1, buffer_length - sum, stream );
      sum += n;

      if( buffer_length == sum )
        break;

      if( 0 == n )
      {
        if( ferror( stream ) )
          goto cleanup_buffer;

        goto final_process;
      }

      if( feof( stream ) )
        goto final_process;
    }

    blake2b_update( S, buffer, buffer_length );
  }

final_process:;

  if( sum > 0 ) blake2b_update( S, buffer, sum );

  blake2b_final( S, resstream, BLAKE2B_OUTBYTES );
  ret = 0;
cleanup_buffer:
  free( buffer );
  return ret;
}

int blake2sp_stream( FILE *stream, void *resstream )
{
  int ret = -1;
  size_t sum, n;
  blake2sp_state S[1];
  static const size_t buffer_length = 16 * ( 1UL << 20 );
  uint8_t *buffer = ( uint8_t * )malloc( buffer_length );

  if( !buffer ) return -1;

  blake2sp_init( S, BLAKE2S_OUTBYTES );

  while( 1 )
  {
    sum = 0;

    while( 1 )
    {
      n = fread( buffer + sum, 1, buffer_length - sum, stream );
      sum += n;

      if( buffer_length == sum )
        break;

      if( 0 == n )
      {
        if( ferror( stream ) )
          goto cleanup_buffer;

        goto final_process;
      }

      if( feof( stream ) )
        goto final_process;
    }

    blake2sp_update( S, buffer, buffer_length );
  }

final_process:;

  if( sum > 0 ) blake2sp_update( S, buffer, sum );

  blake2sp_final( S, resstream, BLAKE2S_OUTBYTES );
  ret = 0;
cleanup_buffer:
  free( buffer );
  return ret;
}


int blake2bp_stream( FILE *stream, void *resstream )
{
  int ret = -1;
  size_t sum, n;
  blake2bp_state S[1];
  static const size_t buffer_length = 16 * ( 1UL << 20 );
  uint8_t *buffer = ( uint8_t * )malloc( buffer_length );

  if( !buffer ) return -1;

  blake2bp_init( S, BLAKE2B_OUTBYTES );

  while( 1 )
  {
    sum = 0;

    while( 1 )
    {
      n = fread( buffer + sum, 1, buffer_length - sum, stream );
      sum += n;

      if( buffer_length == sum )
        break;

      if( 0 == n )
      {
        if( ferror( stream ) )
          goto cleanup_buffer;

        goto final_process;
      }

      if( feof( stream ) )
        goto final_process;
    }

    blake2bp_update( S, buffer, buffer_length );
  }

final_process:;

  if( sum > 0 ) blake2bp_update( S, buffer, sum );

  blake2bp_final( S, resstream, BLAKE2B_OUTBYTES );
  ret = 0;
cleanup_buffer:
  free( buffer );
  return ret;
}

typedef int ( *blake2fn )( FILE *, void * );


static void usage( char **argv, int errcode )
{
  FILE *out = errcode ? stderr : stdout;
  fprintf( out, "Usage: %s [OPTION]... [FILE]...\n", argv[0] );
  fprintf( out, "\n" );
  fprintf( out, "With no FILE, or when FILE is -, read standard input.\n" );
  fprintf( out, "\n" );
  fprintf( out, "  -a <algo>    hash algorithm (blake2b is default): \n"
                "               [blake2b|blake2s|blake2bp|blake2sp]\n" );
  fprintf( out, "  --tag        create a BSD-style checksum\n" );
  fprintf( out, "  --help       display this help and exit\n" );
  exit( errcode );
}


int main( int argc, char **argv )
{
  blake2fn blake2_stream = blake2b_stream;
  size_t outlen   = BLAKE2B_OUTBYTES;
  unsigned char hash[BLAKE2B_OUTBYTES] = {0};
  bool bsdstyle = false;
  int c;
  opterr = 1;

  while( 1 )
  {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      { "help",  no_argument, 0,  0  },
      { "tag",   no_argument, 0,  0  },
      { NULL, 0, NULL, 0 }
    };

    c = getopt_long( argc, argv, "a:", long_options, &option_index );
    if( c == -1 ) break;
    switch( c )
    {
    case 'a':
      if( 0 == strcmp( optarg, "blake2b" ) )
      {
        blake2_stream = blake2b_stream;
        outlen = BLAKE2B_OUTBYTES;
      }
      else if ( 0 == strcmp( optarg, "blake2s" ) )
      {
        blake2_stream = blake2s_stream;
        outlen = BLAKE2S_OUTBYTES;
      }
      else if ( 0 == strcmp( optarg, "blake2bp" ) )
      {
        blake2_stream = blake2bp_stream;
        outlen = BLAKE2B_OUTBYTES;
      }
      else if ( 0 == strcmp( optarg, "blake2sp" ) )
      {
        blake2_stream = blake2sp_stream;
        outlen = BLAKE2S_OUTBYTES;
      }
      else
      {
        printf( "Invalid function name: `%s'\n", optarg );
        usage( argv, 111 );
      }

      break;

    case 0:
      if( 0 == strcmp( "help", long_options[option_index].name ) )
        usage( argv, 0 );
      else if( 0 == strcmp( "tag", long_options[option_index].name ) )
        bsdstyle = true;
      break;

    case '?':
      usage( argv, 1 );
      break;
    }
  }

  if( optind == argc )
    argv[argc++] = (char *) "-";

  for( int i = optind; i < argc; ++i )
  {
    FILE *f = NULL;
    if( argv[i][0] == '-' && argv[i][1] == '\0' )
      f = stdin;
    else
      f = fopen( argv[i], "rb" );

    if( !f )
    {
      fprintf( stderr, "Could not open `%s': %s\n", argv[i], strerror( errno ) );
      continue;
    }

    if( blake2_stream( f, hash ) < 0 )
    {
      fprintf( stderr, "Failed to hash `%s'\n", argv[i] );
    }
    else
    {
      if( bsdstyle )
      {
        if( blake2_stream == blake2b_stream ) printf( "BLAKE2b" );
        else if( blake2_stream == blake2bp_stream ) printf( "BLAKE2bp" );
        else if( blake2_stream == blake2s_stream ) printf( "BLAKE2s" );
        else if( blake2_stream == blake2sp_stream ) printf( "BLAKE2sp" );
        printf( " (%s) = ", argv[i] );
      }

      for( int j = 0; j < outlen; ++j )
        printf( "%02x", hash[j] );

      if( bsdstyle )
        printf( "\n" );
      else
        printf( "  %s\n", argv[i] );
    }

    if( f != stdin ) fclose( f );
  }

  return 0;
}

