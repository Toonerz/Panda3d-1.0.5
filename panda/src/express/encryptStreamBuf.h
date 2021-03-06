// Filename: encryptStreamBuf.h
// Created by:  drose (01Sep04)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001 - 2004, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://etc.cmu.edu/panda3d/docs/license/ .
//
// To contact the maintainers of this program write to
// panda3d-general@lists.sourceforge.net .
//
////////////////////////////////////////////////////////////////////

#ifndef ENCRYPTSTREAMBUF_H
#define ENCRYPTSTREAMBUF_H

#include "pandabase.h"

// This module is not compiled if OpenSSL is not available.
#ifdef HAVE_SSL

#include <openssl/evp.h>

////////////////////////////////////////////////////////////////////
//       Class : EncryptStreamBuf
// Description : The streambuf object that implements
//               IDecompressStream and OCompressStream.
////////////////////////////////////////////////////////////////////
class EXPCL_PANDAEXPRESS EncryptStreamBuf : public streambuf {
public:
  EncryptStreamBuf();
  virtual ~EncryptStreamBuf();

  void open_read(istream *source, bool owns_source, const string &password);
  void close_read();

  void open_write(ostream *dest, bool owns_dest, const string &password);
  void close_write();

  INLINE void set_algorithm(const string &algorithm);
  INLINE const string &get_algorithm() const;

  INLINE void set_key_length(int key_length);
  INLINE int get_key_length() const;

  INLINE void set_iteration_count(int iteration_count);
  INLINE int get_iteration_count() const;

protected:
  virtual int overflow(int c);
  virtual int sync(void);
  virtual int underflow(void);

private:
  size_t read_chars(char *start, size_t length);
  void write_chars(const char *start, size_t length);

private:
  istream *_source;
  bool _owns_source;

  ostream *_dest;
  bool _owns_dest;

  string _algorithm;
  int _key_length;
  int _iteration_count;
  
  bool _read_valid;
  EVP_CIPHER_CTX _read_ctx;
  size_t _read_block_size;
  unsigned char *_read_overflow_buffer;
  size_t _in_read_overflow_buffer;

  bool _write_valid;
  EVP_CIPHER_CTX _write_ctx;
  size_t _write_block_size;
};

#include "encryptStreamBuf.I"

#endif  // HAVE_SSL

#endif
