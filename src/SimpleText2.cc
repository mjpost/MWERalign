/* ---------------------------------------------------------------- */
/* Copyright 2003 (c) by RWTH Aachen - Lehrstuhl fuer Informatik VI */
/* Richard Zens                                                     */
/* ---------------------------------------------------------------- */
#include "SimpleText2.hh"

std::istream &operator>>(std::istream &in, TextNS::Sentence &x)
{
    x.clear();
    std::string line;
    getline(in, line);
    std::istringstream is(line);
    typedef std::istream_iterator<std::string> iter;
    std::copy(iter(is), iter(), std::back_inserter(x));
    return in;
}

std::ostream &operator<<(std::ostream &out, const TextNS::Sentence &x)
{
    std::copy(x.begin(), x.end(), std::ostream_iterator<std::string>(out, " "));
    return out;
}

std::ostream &operator<<(std::ostream &out, const TextNS::VectorText &x)
{
    TextNS::write(out, x);
    return out;
}

std::istream &operator>>(std::istream &in, TextNS::VectorText &x)
{
    TextNS::read(in, x);
    return in;
}

std::ostream &operator<<(std::ostream &out, const TextNS::DequeText &x)
{
    TextNS::write(out, x);
    return out;
}

std::istream &operator>>(std::istream &in, TextNS::DequeText &x)
{
    TextNS::read(in, x);
    return in;
}
