/* ---------------------------------------------------------------- */
/* Copyright 2003 (c) by RWTH Aachen - Lehrstuhl fuer Informatik VI */
/* Richard Zens                                                     */
/* ---------------------------------------------------------------- */
#include "IOfile.hh"
#include "gzstream.hh"
#include <fstream>

std::istream *Ifile::open(const char *f, bool z)
{
    close();
    name = std::string(f);
    zip = z;
    if (zip)
        if (name != "")
            s = new igzstream(f);
        else {
            s = new igzstream("stdin");
        }
    else if (name != "")
        s = new std::ifstream(f);
    else
        s = &std::cin;
    return s;
}

bool Ifile::is_open()
{
    if (zip) {
        igzstream *zipstream = dynamic_cast<igzstream *>(s);
        return (zipstream->rdbuf()->is_open() != 0);
    } else if (s != &std::cin) {
        std::ifstream *fs = dynamic_cast<std::ifstream *>(s);
        return fs->is_open();
    } else
        return true;
}

void Ifile::close()
{
    if (s) {
        if (zip)
            static_cast<igzstream *>(s)->close();
        if (name != "")
            delete s;
        s = 0;
    }
}

std::ostream *Ofile::open(const char *f, bool z)
{
    close();
    name = std::string(f);
    zip = z;
    if (zip)
        if (name != "")
            s = new ogzstream(f);
        else
            s = new ogzstream("stdout");
    else if (name != "" && name != "stdout")
        s = new std::ofstream(f);
    else
        s = &std::cout;
    return s;
}

void Ofile::close()
{
    if (s) {
        if (zip)
            static_cast<ogzstream *>(s)->close();
        if (name != "" && name != "stdout")
            delete s;
        s = 0;
    }
}
