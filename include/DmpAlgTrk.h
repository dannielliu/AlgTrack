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
  bool ProcessEvent(long eventID);
  //bool ProcessEvents(long *eventID);
  bool ProcessEvents(std::string eventID); // eventiD = "numberA:numberB", from numberA to numberB
  bool Finalize();

private:
  int            eventNumber;
	int            eventID;
	double         hitThreshold;
  DmpEvtBgoHits *bgoHits;
  TH2D          *trackXZ;
  TH2D          *trackXZ2;
  TH2D          *trackYZ;
  TH2D          *trackYZ2;
};

#endif
