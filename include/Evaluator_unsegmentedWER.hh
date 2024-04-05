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
#ifndef EVALUATOR_UNSEGMENTEDWER_HH_
#define EVALUATOR_UNSEGMENTEDWER_HH_
#include "Evaluator.hh"
#include <map>
#include <set>

using namespace std;

class Evaluator_unsegmentedWER : public Evaluator {
public:
  typedef Evaluator::hyptype hyptype;
  typedef Evaluator::HypContainer HypContainer;
  typedef struct DP_ { 
    unsigned int cost;
    unsigned int bp;
  } DP;
  typedef std::vector<std::vector<DP> > Matrix;
  
  Evaluator_unsegmentedWER();
  Evaluator_unsegmentedWER(const std::string& refFile);

  ~Evaluator_unsegmentedWER() {}
  
  double _evaluate2(const HypContainer& hyps, std::ostream& out) const;

  double _evaluate(const HypContainer& hyps) const {
    return _evaluate2(hyps, std::cout);
  }

  double _evaluate_abs(const HypContainer&) const { return 0.0; };
  
  std::string name() const;

  void setInsertionCosts(double x) {ins_=x;}
  void setDeletionCosts(double x) {del_=x;}
  
  virtual bool initrefs() { return true; }
  void setOptions(const double maxER, const bool human) {
    maxER_ = maxER;
    human_ = human;
  }

 
protected:
  double computeSpecialWER(const std::vector<std::vector<unsigned int> >& A,
                           const std::vector<unsigned int>& B, unsigned int nSegments) const;
  unsigned int getVocIndex(const std::string& word) const;
  unsigned int getSubstitutionCosts(const uint a, const uint b) const;
  unsigned int getInsertionCosts(const uint w) const;
  unsigned int additionalInsertionCosts(const uint w, const uint w2=0) const;
  unsigned int getDeletionCosts(const uint w) const;
  double ins_,del_;
  void fillPunctuationSet();
private:
  unsigned int segmentationWord;
  double maxER_;
  bool human_;
  mutable unsigned int refLength_;
  mutable unsigned int vocCounter_;
  mutable std::map<std::string,unsigned int> vocMap_;
  mutable std::set<unsigned int> punctuationSet_;
  mutable std::vector<unsigned int> boundary;
  mutable std::vector<unsigned int> sentCosts;
};

#endif

