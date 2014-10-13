#ifndef DmpAlgTrk_H
#define DmpAlgTrk_H

#include "DmpVAlg.h"

class DmpEvtBgoHits;
class TH2D;

class DmpAlgTrk : public DmpVAlg{
/*
 *  DmpAlgTrk
 *
 */
public:
  DmpAlgTrk();
  ~DmpAlgTrk();

  //void Set(const std::string &type,const std::string &value);
  // if you need to set some options for your algorithm at run time. Overload Set()
  bool Initialize();
  bool ProcessThisEvent();    // only for algorithm
  bool Finalize();

private:
  int            eventNumber;
	double         hitThreshold;
  DmpEvtBgoHits *bgoHits;
  TH2D          *trackXZ;
  TH2D          *trackYZ;
};

#endif
