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
#ifndef EVALUATOR_HH_
#define EVALUATOR_HH_
#include <string>
#include <vector>
#include <sstream>
#include "SimpleText2.hh"



class Evaluator 
{
   public:
      
      /** A candidate (hypothesis) sentence */
      typedef TextNS::Sentence hyptype;
      
      /** A candidate (hypothesis) corpus */
      typedef TextNS::SimpleText HypContainer;
      
      /** Multiple reference sentences for a candidate sentence */
      typedef std::vector<hyptype> mreftype;
      
      /** Corpus multiple reference sentences for a candidate corpus */    
      typedef std::vector<mreftype> MRefContainer;
      
      /** General evaluation exception */
      class EvaluationException {};

      /** Exception: Thrown by evaluate() when called without having properly initialized references **/
      class InvalidReferencesException : public EvaluationException {};

      /** Exception: Thrown if this kind of evaluation is not possible (e.g. _abs with BLEU, NIST) **/
      class InvalidMethodException : public EvaluationException {};
    

    Evaluator();
    virtual ~Evaluator() {}

    /** return normalized number of errors (= error rate)
      * \param sentence hyps Candidate corpus to evaluate
     **/
    double evaluate(const HypContainer& hyps) const;
    
    /** return absolute number of errors
      * \param hyps Candidate corpus to evaluate
     **/
    double evaluate_abs(const HypContainer& hyps) const;
    
    /** return name of criterion **/
    virtual std::string name() const=0;
  
    /** write detailed evaluation information to output stream 
      * \param out Output stream to write evaluation to
      * \param hyps Candidate corpus to evaluate
     **/
    virtual void detailed_evaluation(std::ostream&, const HypContainer&) const {};

    /** set flag for case sensitivity
      * \param b \em true: regard case information; \em false: neglect case information
     **/
    virtual void setcase(bool b) {usecase=b;}

    /** set flag for tokenization
      * \param b \em true: Tokenize \b references \em false: do not tokenize references
     **/
    virtual void settokenize(bool b) {tokenize=b;}

    /** Load reference sentences from file in mref format
      * (i.e. multiple refererences separated by a '#' in each line)
      * Initialize then all necessary reference data structures.
      * Must be called \b before evaluation.
      * 
      * The default implementation loads the sentences into the mref container 
      * and calls \see initrefs() afterwards. 
      * It is recommended to redefine \see initrefs() instead of loadrefs when inheriting.
      *
      * \param filename MRef file name 
      * \return true iff loading was successfull 
     **/
    virtual bool loadrefs(const std::string& filename);

    bool loadrefsFromStream(std::istream& in);

    /** Load reference sentences from MRefContainer
      * Initialize then all necessary reference data structures.
      * Must be called \b before evaluation.
      * 
      * The default implementation loads the sentences into the mref container 
      * and calls \see initrefs() afterwards. 
      * It is recommended to redefine \see initrefs() instead of loadrefs when inheriting.
      *
      * \param references Reference sentences
      **/
    virtual bool loadrefs(const MRefContainer & references);

    

   protected:


      /** Return normalized number of errors (= error rate)
        * Protected virtual method to be called from \see evaluate().
        * \param sentence hyps Candidate corpus to evaluate
        * \exception InvalidReferencesException Reference sentences had not been initialized properly
       **/
      virtual double _evaluate(const HypContainer& hyps) const=0;
      
      /** return absolute number of errors
        * Protected virtual method to be called from \see evaluate().
	* \param hyps Candidate corpus to evaluate
	* \exception InvalidReferencesException Reference sentences had not been initialized properly
       **/
      virtual double _evaluate_abs(const HypContainer& hyps) const=0;


    /** Init internal reference sentence structures.
      * To be called from loadRefs(), after reference sentences 
      * have been loaded.
      *
      * Overwrite this method and not loadRefs() if possible.
      *
      * \return true iff loading was successfull 
     **/    
    virtual bool initrefs();


    /** Use case information? */
    bool usecase;

    /** Tokenize punctuation? */
    bool tokenize;

    /** Reference data initialized?*/
    bool referencesAreOk;

    /** Container for the reference sentences **/
    MRefContainer mref;
};


inline std::ostream& operator<<(std::ostream& out,const Evaluator::hyptype& x) {
  std::copy(x.begin(),x.end(),std::ostream_iterator<std::string>(out," "));
  return out;
};


template <class T,int myMax> class InvEvaluator : public T {
  typedef T MyBase;
public:
  InvEvaluator() : MyBase() {}
  InvEvaluator(const MyBase& e) : MyBase(e) {}
  ~InvEvaluator() {}

  std::string name() const {
    std::ostringstream s; s<<myMax<<"-"<<MyBase::name(); return s.str();}

protected:
  double _evaluate(const Evaluator::HypContainer& hyps) const {
    return myMax - MyBase::_evaluate(hyps);}

  double _evaluate_abs(const Evaluator::HypContainer& hyps) const {
    return myMax - MyBase::_evaluate_abs(hyps);}
};

#endif
