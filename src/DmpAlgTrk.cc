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
  trackXZ = new TH2D("trackXOZ","trackXOZ",24,-330,330,16,15,479);// should be modified, x gap 27.5mm y gap 29.0mm
  trackYZ = new TH2D("trackYOZ","trackYOZ",24,-330,330,16,15,479);// first z is 58.5, last z is 435.5
  trackXZ2 = new TH2D("trackXOZ2","trackXOZ",24,-1,23,16,-1,15);
  trackYZ2 = new TH2D("trackYOZ2","trackYOZ",24,-1,23,16,-1,15);
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgTrk::ProcessThisEvent(){
	trackXZ->Reset();
	trackYZ->Reset();
	trackXZ2->Reset();
	trackYZ2->Reset();
	short hitbars=bgoHits->GetHittedBarNumber();
	if(hitbars==0){ 
		return false;
		DmpLogInfo<<"no hits for this event."<<DmpLogEndl;
	}
	else
	  eventNumber++;
  
	//show crystal
	for (short i=0;i<22;i++){
	   for (short j=0;j<14;j++){
		  j%2==0 ? trackYZ->Fill(-288.75+i*27.5, 435.5-j*29, 0.1):
			         trackXZ->Fill(-288.75+i*27.5, 435.5-j*29, 0.1);
							 // position x(or y)=-330+27.5*1.5+i*27.5, z=58.5+435.5-58.5-j*29
		  j%2==0 ? trackYZ2->Fill(i,13-j,0.1):trackXZ2->Fill(i,13-j,0.1);
		}
	} 
	// fill track
  for (short i=0;i<bgoHits->GetHittedBarNumber();i++){
	  double weight = bgoHits->fEnergy.at(i);
		//DmpLogInfo<<"energy of this hit is "<<weight<<DmpLogEndl;
		if (weight < hitThreshold){
		  continue;
		}
		short layer = DmpBgoBase::GetLayerID(bgoHits->fGlobalBarID.at(i));
		short bar   = DmpBgoBase::GetBarID(bgoHits->fGlobalBarID.at(i));
		double posx = DmpBgoBase::Parameter()->BarCenter(bgoHits->fGlobalBarID.at(i)).x();
		double posy = DmpBgoBase::Parameter()->BarCenter(bgoHits->fGlobalBarID.at(i)).y();
		double posz = DmpBgoBase::Parameter()->LayerCenter(bgoHits->fGlobalBarID.at(i)).z();
		//DmpLogInfo<<"z is "<<posz<<DmpLogEndl;
		if (layer%2 == 0){
		  trackYZ->Fill(posy,494-posz,weight);
		  trackYZ2->Fill(bar,13-layer,weight);
		}
		else{
		  trackXZ->Fill(posx,494-posz,weight);
		  trackXZ2->Fill(bar,13-layer,weight);
		}
	}   

	// Draw track
  TCanvas *c1 = new TCanvas("c1","track",1000,800);
	gStyle->SetOptStat(0);
	c1->Divide(2,2);
	c1->cd(1);
	trackXZ->Draw("colz");
	ReverseYAxis(trackXZ);
	c1->cd(2);
	trackYZ->Draw("colz");
  ReverseYAxis(trackYZ);
  c1->cd(3);
	trackXZ2->Draw("colz");
	ReverseYAxis(trackXZ2);
	c1->cd(4);
	trackYZ2->Draw("colz");
  ReverseYAxis(trackYZ2);

	// save canvas
	char trackname[100];
	sprintf(trackname,"track_eventid_%d.eps",eventID);
	c1->Print(trackname);
	delete c1;
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

