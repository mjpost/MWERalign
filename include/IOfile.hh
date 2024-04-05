/*--------------------------------------------------------------
 Copyright 2006 (c) by RWTH Aachen - Lehrstuhl fuer Informatik 6
 Richard Zens, Evgeny Matusov, Gregor Leusch
 ---------------------------------------------------------------

This header file is part of the MwerAlign C++ Library.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.
*/
#ifndef IOFILE_HH_
#define IOFILE_HH_
#include <iostream>
#include <string>
#include <assert.h>

class Ifile {
public:
  Ifile() : zip(0),s(0) {}
  Ifile(const char* f,bool z=1) : s(0) {open(f,z);}
  Ifile(const std::string f,bool z=1) : s(0) {open(f,z);}
  ~Ifile() {close();}

  std::istream* open(const char* f,bool z=1);
  std::istream* open(const std::string f,bool z=1) {return open(f.c_str(),z);}
  void close();
  operator std::istream&() {assert(s); return *s;}
  operator std::istream*() {return s;}
  std::string getname() const {return name;}
  bool iszipped() const {return zip;}
  bool eof() { return s->eof(); }
  bool is_open();

  friend std::istream& getline(Ifile& is, std::string& str, char delim = '\n') {
    return getline(*(is.s),str,delim);}

  // attention: operator>> will return a std::istream& ! (typically this is ok)
  template<class TTT> friend std::istream& operator>>(Ifile& in,TTT& x) {
    return (*(in.s)>>x);}

private:
  Ifile(const Ifile&);
  void operator=(const Ifile&);

  bool zip;
  std::string name;
  std::istream* s;
};


class Ofile {
public:
  Ofile() : zip(0),s(0) {}
  Ofile(const char* f,bool z=0) : s(0) {open(f,z);}
  Ofile(const std::string f,bool z=0) : s(0) {open(f,z);}
  ~Ofile() {close();}

  std::ostream* open(const char* f,bool z=0);
  std::ostream* open(const std::string f,bool z=0) {return open(f.c_str(),z);}
  void close();
  operator std::ostream&() {assert(s); return *s;}
  operator std::ostream*() {return s;}

  std::string getname() const {return name;}
  bool iszipped() const {return zip;}

  template<class T> friend std::ostream& operator<<(Ofile& out,const T& x) {
    return (*(out.s))<<x;}

private:
  Ofile(const Ofile&);
  void operator=(const Ofile&);

  bool zip;
  std::string name;
  std::ostream* s;
};
#endif
