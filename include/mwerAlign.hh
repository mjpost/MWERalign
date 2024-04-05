/* ---------------------------------------------------------------- */
/* Copyright 2003 (c) by RWTH Aachen - Lehrstuhl fuer Informatik VI */
/* Richard Zens                                                     */
/* ---------------------------------------------------------------- */
#ifndef MWERALIGN_HH_
#define MWERALIGN_HH_
#include <string>
#include <vector>
#include <sstream>
#include "Evaluator_unsegmentedWER.hh"
#include "SimpleText2.hh"

typedef TextNS::SimpleText Text;

class MwerSegmenter {

private:

  double maxER_;
  bool human_;
  Evaluator_unsegmentedWER* E;


public:

  MwerSegmenter(double maxER, bool human): maxER_(maxER), human_(human)
  {
    E = new Evaluator_unsegmentedWER();
  }
  ~MwerSegmenter() { delete(E); }
  void mwerAlign(const std::string& ref, const std::string& hyp, std::string& result);

};


#endif
