/* ---------------------------------------------------------------- */
/* Copyright 2003 (c) by RWTH Aachen - Lehrstuhl fuer Informatik VI */
/* Richard Zens                                                     */
/* ---------------------------------------------------------------- */
#include "Evaluator.hh"
#include <iostream>
#include <fstream>
#include <sstream>

Evaluator::Evaluator() : usecase(true), tokenize(false), referencesAreOk(false) {}








bool Evaluator::loadrefs(const std::string& filename) 
{
  //std::cerr<<"loading reference file: "<<filename<<" case sensitive: "<<usecase<<std::endl;
   std::ifstream in(filename.c_str());
   if (!in)
     return (referencesAreOk = false);
   return loadrefsFromStream(in);
}

bool Evaluator::loadrefsFromStream(std::istream& in)
{
   std::string line,w;
   while(getline(in,line)) 
   {
      mreftype refs;
      hyptype h;

      hyptype h_m = tokenize ? TextNS::makeTokenSent(usecase ? line : TextNS::makelowerstring(line)) 
	 : TextNS::makeSent(usecase ? line : TextNS::makelowerstring(line));
     

      for (hyptype::const_iterator i = h_m.begin(); i != h_m.end(); ++i)
      {
	 if (*i == "#") 
	 {	   
	    if (!h.empty()) 
	       refs.push_back(h);
	   
	    h.clear();
	 }
	 else 
	 {
	    h.push_back(*i);
	 }
      }

      if (!h.empty()) 
	 refs.push_back(h);

      mref.push_back(refs);
   }

   /* prepare tables and hashes */
   referencesAreOk = initrefs();
   return referencesAreOk;
}

bool Evaluator::loadrefs(const MRefContainer & references)
{
  if (references.empty())
    return (referencesAreOk = false);

   if (tokenize)
   {
      mref.reserve(references.size());

      for (MRefContainer::const_iterator i = references.begin(); i != references.end(); ++i) 
      {
	 HypContainer hc;
	 hc.reserve(i->size());
	 for (HypContainer::const_iterator j = i->begin(); j != i->end(); ++j)
	 {
	    /* Re-tokenize */
	    std::ostringstream os;
	    std::copy(j->begin(), j->end(),
		      std::ostream_iterator<std::string>(os, " "));

	    hc.push_back(TextNS::makeTokenSent(usecase ? os.str() : TextNS::makelowerstring(os.str())));
	 }	 
	 mref.push_back(hc);
      }
   } 
   else
   {
      if (usecase)
      {
	 mref = references;
      }
      else
      {
	 mref.reserve(references.size());

	 for (MRefContainer::const_iterator i = references.begin(); i != references.end(); ++i) 
	 {
	    HypContainer hc;
	    hc.reserve(i->size());
	    for (HypContainer::const_iterator j = i->begin(); j != i->end(); ++j)
	    {
	       hyptype hy;
	       hy.reserve(j->size());
	       for (hyptype::const_iterator k = j->begin(); k != j->end(); ++k)
		  hy.push_back(TextNS::makelowerstring(*k));

	       hc.push_back(hy);
	    }
	    mref.push_back(hc);
	 }
      }
   }

   /* prepare tables and hashes */
   referencesAreOk = initrefs();
   return referencesAreOk;
}

bool Evaluator::initrefs()
{
  return false;
}

#if 0
double Evaluator::_evaluate(const HypContainer&) const
{
  InvalidMethodException e;
  throw(e);
}

double Evaluator::_evaluate_abs(const HypContainer&) const
{
  InvalidMethodException e;
  throw(e);
}
#endif

double Evaluator::evaluate(const HypContainer& hyps) const {
  InvalidReferencesException e;
  if (!referencesAreOk)
    throw(e);

  if(usecase) return _evaluate(hyps);

  HypContainer lower;
  TextNS::makelowertext(hyps,lower);
  return _evaluate(lower);
}

double Evaluator::evaluate_abs(const HypContainer& hyps) const {
  InvalidReferencesException e;
  if (!referencesAreOk)
    throw(e);

  if(usecase) return _evaluate_abs(hyps);
  HypContainer lower;
  TextNS::makelowertext(hyps,lower);
  return _evaluate_abs(lower);
}


