/* ---------------------------------------------------------------- */
/* Copyright 2003 (c) by RWTH Aachen - Lehrstuhl fuer Informatik VI */
/* Richard Zens                                                     */
/* ---------------------------------------------------------------- */
#include "Evaluator_unsegmentedWER.hh"
using namespace std;
// const double MAX_SENT_ER=0.9;

double Evaluator_unsegmentedWER::_evaluate2(const HypContainer& hyps, std::ostream& out) const {
  size_t HS=hyps.size();
  unsigned int epsilon = 0;
  double rv=0;
  std::vector<std::vector<unsigned int> > A;
  std::vector<unsigned int> B;
  std::vector<std::string> stringB;

  A.resize(mref[0].size()); /** NOTE: different segments can have different number of references!
                             -> some sents must have "double" same references, before this can be used! **/
  for(size_t i=0;i<mref.size();++i) {
    unsigned int maxRefLength =0;
    for(HypContainer::const_iterator r=mref[i].begin();r!=mref[i].end();++r) 
      if(r->size() > maxRefLength) maxRefLength=r->size();
    for(size_t r=0;r<mref[i].size();++r)
    {
      for(size_t k=0;k<maxRefLength;++k)
      {
        if(k<mref[i][r].size())
          (A[r]).push_back(getVocIndex(mref[i][r][k]));
        else (A[r]).push_back(epsilon);
      }
      (A[r]).push_back(segmentationWord);
    }
  } 
  for(size_t i=0;i<HS;++i) {
    for(size_t j=0;j<hyps[i].size();++j) 
    {
      B.push_back(getVocIndex(hyps[i][j])); 
      stringB.push_back(hyps[i][j]);
    }
  }
  rv = computeSpecialWER(A, B, mref.size()); // compute the edit distance
  size_t beg = 1;
  size_t end = 0;
  // Ofile segOut("__segments");
  for(size_t s=2;s<boundary.size();++s)
  {
    end=boundary[s];
    size_t sentLength=0;
    for(size_t j=beg; j<=std::min(end, stringB.size()); ++j)
    {
      out << stringB[j-1] << " ";
      ++sentLength;
    }
    out << "\n";
  double thisSentCosts = double(sentCosts[s-1] - sentCosts[s-2])/double(sentLength);
  if((maxER_>=0)&&(thisSentCosts > maxER_)) std::cerr << "WARNING: check the alignment for segment " << s-1
                                            << " manually (WER: " << thisSentCosts << " )!\n";
    beg = end + 1;
  }
  // for the last segment:
  size_t sentLength=0;
  for(size_t j=beg; j<=stringB.size(); ++j)
  {
    out << stringB[j-1] << " ";
    ++sentLength;
  }
  out << "\n";
  double thisSentCosts = double(sentCosts[sentCosts.size()-1] - sentCosts[sentCosts.size()-2])/double(sentLength);
  if((maxER_>=0)&&(thisSentCosts > maxER_)) std::cerr << "WARNING: check the alignment for segment " << sentCosts.size()-1
                                             << " manually (WER: " << thisSentCosts << " )!\n";
  return rv/refLength_;
}

unsigned int Evaluator_unsegmentedWER::getVocIndex(const std::string& word) const
{
  std::string wlc = TextNS::makelowerstring(word);
  std::map<std::string, unsigned int>::const_iterator p = vocMap_.find(wlc); 
  if(p != vocMap_.end()) return p->second;
  ++vocCounter_;
  vocMap_[wlc] = vocCounter_;
  return vocCounter_;
}

unsigned int Evaluator_unsegmentedWER::getSubstitutionCosts(const uint a, const uint b) const
{
  if(a==b) return 0;
  if(!human_) return 1;
  bool aIsPunc = (punctuationSet_.find(a) != punctuationSet_.end());
  bool bIsPunc = (punctuationSet_.find(b) != punctuationSet_.end());
  if(aIsPunc&&bIsPunc) return 1; /** substitution of a punc with another punc **/
  if(aIsPunc||bIsPunc) return 2;
  return 1;
}

unsigned int Evaluator_unsegmentedWER::getDeletionCosts(const uint w) const
{
  /** additional costs for deletion if the word is a punctuation **/
  if(!human_||(punctuationSet_.find(w) == punctuationSet_.end()))
    return (unsigned int)(del_);
  else return 2;
}

unsigned int Evaluator_unsegmentedWER::getInsertionCosts(const uint w) const
{
  /** additional costs for insertion if the word is a punctuation **/
  if(!human_||(punctuationSet_.find(w) == punctuationSet_.end()))
    return (unsigned int)(ins_);
  else return 2;
}

unsigned int Evaluator_unsegmentedWER::additionalInsertionCosts(const uint w1, const uint w2) const
{
  
  if(human_&&w2&&(punctuationSet_.find(w2) != punctuationSet_.end())) return 2;
  /** additional costs for insertion if the next word is a segment boundary **/
  if(human_&&(w1==segmentationWord)) return 2;
  else return 0;
}

double Evaluator_unsegmentedWER::computeSpecialWER(const std::vector<std::vector<unsigned int> >& A, const std::vector<unsigned int>& B,
                                                   unsigned int nSegments) const
{ 
  unsigned int R=A.size();
  unsigned int I=A[0].size(); // the length is the same for all references due to epsilon entries
  unsigned int J=B.size();
  unsigned int S=nSegments;
  std::vector<std::vector<unsigned int> > BP(J+1), BC(J+1);
  std::vector<std::vector<unsigned short> > BR(J+1);
  std::vector<std::vector<DP> > m(R), mnew(R);
  boundary.resize(S+1);
  sentCosts.resize(S+1);
  unsigned int cSUB=1, cDEL=(unsigned int)(del_), cINS=(unsigned int)(ins_), epsilon=0;
  unsigned int s, sub, del, ins, k, min=10000000, argmin=0, bestRef=0;
  bool merge;

  for(size_t r=0;r<R;++r) // initialization along reference axis i for all references
  {
    m[r].resize(I+1);
    for(size_t i=0;i<=I;++i) m[r][i].cost = i;
    mnew[r].resize(2);
  }

  for(size_t j=1;j<=J;++j) // main loop over hyp positions j
  {
    BP[j].resize(S+1);
    BR[j].resize(S+1);
    BC[j].resize(S+1);
    s=0;
    for(size_t r=0;r<R;++r) // initialization along axis j for all references
    { 
      m[r][0].cost = j-1;
      m[r][0].bp = 0;
      mnew[r][0].cost = j;
      mnew[r][0].bp = 0;
    }
    for(size_t i=1;i<=I;++i) // main loop over ref positions i
    {
      if(A[0][i-1]==segmentationWord) 
      {
        merge=true; 
        min = 100000000;
        argmin =0;
        bestRef=0;
      }
      else merge=false;

      for(size_t r=0;r<R;++r) // loop over references
      {
        if(merge||(A[r][i-1]==epsilon)) // just move the previous entry up without any additional costs
          m[r][i-1] = mnew[r][0];
        else  // do compute next step in the LEVENSHTEIN distance
        {
          del= mnew[r][0].cost + getDeletionCosts(A[r][i-1]);
          ins= m[r][i].cost    + getInsertionCosts(B[j-1]) + additionalInsertionCosts(A[r][i], A[r][i-1]);
          sub= m[r][i-1].cost  + getSubstitutionCosts(A[r][i-1], B[j-1]); // ((A[r][i-1]==B[j-1]) ? 0 : cSUB);
  //        std::cerr << j << " " << i << "\n";
  //        std::cerr << del << " " << ins << " " << sub << "\n";
          if(sub < del) // do not appreciate substitutions (that is why <, not <=)
            if(sub < ins) { mnew[r][1].cost = sub; mnew[r][1].bp = m[r][i-1].bp; }
            else { mnew[r][1].cost=ins; mnew[r][1].bp = m[r][i].bp; }
          else 
            if(del <= ins) { mnew[r][1].cost=del; mnew[r][1].bp = mnew[r][0].bp; }
            else { mnew[r][1].cost=ins; mnew[r][1].bp = m[r][i].bp; }
          m[r][i-1] = mnew[r][0];  // finalize saving of the previous entry
          mnew[r][0] = mnew[r][1];  // move the current entry up the stack
        }
        if(merge) // segmentation word is the same in all references
          if(mnew[r][0].cost < min) 
          {
            min=mnew[r][0].cost;
            argmin=mnew[r][0].bp;
            bestRef = r;            // HERE: also save "r"  (reference number, to count reference length!)
          }
      } // end of loop over references
      if(merge) // segment end, merge
      {
        ++s;
        BC[j][s] = min;
        BP[j][s] = argmin;
        BR[j][s] = bestRef;
   //     std::cerr << "MERGE: " << min << " " << argmin << "\n";
        for(size_t r=0;r<R;++r)
        {
          mnew[r][0].cost = min; 
          mnew[r][0].bp = j; 
        }
      }
    } // end of loop over i
    for(size_t r=0;r<R;++r) m[r][I] = mnew[r][0];  // make the stack empty by filling in the last values
  }
  // Backtracing from here:
  s = S; // S - total number of segments
  k = J; 
  unsigned int refNo=0;
  do
  {
      boundary[s] = BP[k][s];
      sentCosts[s] = BC[k][s];
      refNo = BR[k][s];
      refLength_ += mref[s-1][refNo].size(); // add up the length of the best aligned references
      k = BP[k][s];
      s = s-1;
  }
  while(s > 0);
  return m[0][I].cost; // total costs - the same for all references (since a merge is always the last step)
}

void Evaluator_unsegmentedWER::fillPunctuationSet()
{
  std::string period = "</s>";
  segmentationWord = getVocIndex(period);
  punctuationSet_.insert(getVocIndex("."));
  punctuationSet_.insert(getVocIndex(","));
  punctuationSet_.insert(getVocIndex(";"));
  punctuationSet_.insert(getVocIndex("?"));
  punctuationSet_.insert(getVocIndex("!"));
  punctuationSet_.insert(getVocIndex("-"));
  punctuationSet_.insert(getVocIndex(":"));
  punctuationSet_.insert(getVocIndex("/"));
  punctuationSet_.insert(getVocIndex(")"));
  punctuationSet_.insert(getVocIndex("("));
  punctuationSet_.insert(getVocIndex("\""));
}

Evaluator_unsegmentedWER::Evaluator_unsegmentedWER() 
  : Evaluator(),ins_(1),del_(1), refLength_(0), vocCounter_(0), maxER_(-1.0), human_(false) { fillPunctuationSet(); }

Evaluator_unsegmentedWER::Evaluator_unsegmentedWER(const std::string& refFile)
  : Evaluator(),ins_(1),del_(1), refLength_(0), vocCounter_(0), maxER_(-1.0), human_(false) {
  fillPunctuationSet();
  loadrefs(refFile);
}

std::string Evaluator_unsegmentedWER::name() const {return std::string("mnsWER");}

