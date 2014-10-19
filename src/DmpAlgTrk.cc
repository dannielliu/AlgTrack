#include "DmpAlgTrk.h"
#include "DmpBgoBase.h"
#include "DmpEvtBgoHits.h"
#include "DmpDataBuffer.h"
//#include "DmpRootIOSvc.h"
#include "DmpCore.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "ReverseAxis.h"

//-------------------------------------------------------------------
DmpAlgTrk::DmpAlgTrk()
 :DmpVAlg("Trk")
{
}

//-------------------------------------------------------------------
DmpAlgTrk::~DmpAlgTrk(){
}

//-------------------------------------------------------------------
bool DmpAlgTrk::Initialize(){
  eventNumber=0;
	eventID=gCore->GetCurrentEventID();
  hitThreshold = 1.0;
	bgoHits=new DmpEvtBgoHits();
  gDataBuffer->LinkRootFile("Event/Cal/Hits",bgoHits);
	bgoHits = dynamic_cast<DmpEvtBgoHits*>(gDataBuffer->ReadObject("Event/Cal/Hits"));
  trackXZ = new TH2D("trackXOZ","trackXOZ",22,0,22,14,0,14);// should be modified
  trackYZ = new TH2D("trackYOZ","trackYOZ",22,0,22,14,0,14);
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgTrk::ProcessThisEvent(){
	trackXZ->Reset();
	trackYZ->Reset();
	short hitbars=bgoHits->GetHittedBarNumber();
  std::cout<<"hit bars is "<<hitbars<<std::endl;
	if(hitbars==0){
		return false;
		DmpLogInfo<<"no hits for this event."<<DmpLogEndl;
	}
	else
	  eventNumber++;
  
	//show crystal
	for (short i=0;i<22;i++){
	  for (short j=0;j<14;j++){
		  j%2==1 ? trackXZ->Fill(i,j,0.1):trackYZ->Fill(i,j,0.1);
		}
	}
	// fill track
  for (short i=0;i< bgoHits->GetHittedBarNumber();i++){
	  double weight = bgoHits->fEnergy.at(i);
		DmpLogInfo<<"energy of this hit is "<<weight<<DmpLogEndl;
		if (weight < hitThreshold){
		  continue;
		}
		//there is some problem here, should not fill posxx
		/*//double posx = bgoHits->fPosition.at(i).x();
		//double posy = bgoHits->fPosition.at(i).y();
		//double posz = bgoHits->fPosition.at(i).z();*/
		short layer = DmpBgoBase::GetLayerID(bgoHits->fGlobalBarID.at(i));
		short bar   = DmpBgoBase::GetBarID(bgoHits->fGlobalBarID.at(i));
		if (layer%2 == 0){
		  trackXZ->Fill(bar,13-layer,weight);
		}  
		else{
		  trackYZ->Fill(bar,13-layer,weight);
		}   
	} 

	// Draw track
  TCanvas *c1 = new TCanvas("c1","track",1000,400);
	gStyle->SetOptStat(0);
	c1->Divide(2,1);
	c1->cd(1);
	trackXZ->Draw("colz");
	ReverseYAxis(trackXZ);
	c1->cd(2);
	trackYZ->Draw("colz");
  ReverseYAxis(trackYZ);

	// save canvas
	char trackname[100];
	sprintf(trackname,"track_eventid_%d.eps",eventID);
	c1->Print(trackname);
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgTrk::ProcessEvent(long eventID)
{
  DmpAlgTrk::eventID=eventID;
  gRootIOSvc->PrepareEvent(eventID);
	return ProcessThisEvent();
}

//-------------------------------------------------------------------
#include <boost/algorithm/string.hpp>
//#include <vector>
//#include <string>
//#include <stdlib.h>
bool DmpAlgTrk::ProcessEvents(std::string eventID)
{
  // analyse the input string
	std::vector<long> eventIDs;
  std::vector<std::string> tmp;
  boost::split(tmp,eventID,boost::is_any_of(":"));
	if (tmp.size()==2){
	  long startid, stopid;
	  tmp.at(0)!=""?startid = atoi(tmp.at(0).c_str()):startid=0;
	  tmp.at(1)!=""?stopid = atoi(tmp.at(1).c_str()): stopid = gCore->GetMaxEventNumber(); 
	  for (long id=startid;id<=stopid;id++){
	    eventIDs.push_back(id);
	  }
	}
	else{
	  tmp.clear();
		boost::split(tmp,eventID,boost::is_any_of(" "));
		for (long i=0;i<tmp.size();i++){
		  eventIDs.push_back(atoi(tmp.at(i).c_str()));
		}
	}
	// analyse end

  // draw track/tracks
	for (long index=0;index<eventIDs.size();index++){
	  ProcessEvent(eventIDs.at(index));
	}
	return true;
}

//-------------------------------------------------------------------
bool DmpAlgTrk::Finalize(){
  return true;
}

