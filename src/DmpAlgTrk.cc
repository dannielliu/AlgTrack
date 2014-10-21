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
  trackXZ0 = new TH2D("trackXOZ0","trackXOZ_Side0",46,-605,605,26,-275,479);
  trackYZ0 = new TH2D("trackYOZ0","trackYOZ_Side0",46,-605,605,26,-275,479);
  trackXZ1 = new TH2D("trackXOZ1","trackXOZ_Side1",46,-605,605,26,-275,479);
  trackYZ1 = new TH2D("trackYOZ1","trackYOZ_Side1",46,-605,605,26,-275,479);
  //trackXZ2 = new TH2D("trackXOZ2","trackXOZ",24,-1,23,16,-1,15);
  //trackYZ2 = new TH2D("trackYOZ2","trackYOZ",24,-1,23,16,-1,15);
  // x gap 27.5mm y gap 29.0mm
  // first z is 58.5, last z is 435.5
  // 46=(22+2)+20, 605=330+27.5*10,26=(14+2)+10,-275=15-10*29

  return true;
}

//-------------------------------------------------------------------
bool DmpAlgTrk::ProcessThisEvent(){
	trackXZ0->Reset();
	trackYZ0->Reset();
	trackXZ1->Reset();
	trackYZ1->Reset();
	short hitbars=bgoHits->GetHittedBarNumber();
	if(hitbars==0){ 
		return false;
		DmpLogInfo<<"no hits for this event."<<DmpLogEndl;
	}
	else
	  eventNumber++;
  
	//show crystal
	for (short i=0;i<22;i++){ 
	   for (short j=0;j<14;j++ ){
		  j%2==0 ? (trackYZ0->Fill(-288.75+i*27.5, 435.5-j*29-10*29, 0.1),
		           trackYZ1->Fill(-288.75+i*27.5, 435.5-j*29-10*29, 0.1)):
		           (trackXZ0->Fill(-288.75+i*27.5, 435.5-j*29-10*29, 0.1),
				   trackXZ1->Fill(-288.75+i*27.5, 435.5-j*29-10*29, 0.1));
		  // position x(or y)=-330+27.5*1.5+i*27.5, z=58.5+435.5-58.5-j*29
		  //j%2==0 ? trackYZ2->Fill(i,13-j,0.1):trackXZ2->Fill(i,13-j,0.1);
		}
	} 
	// fill track
  for (short i=0;i<bgoHits->GetHittedBarNumber();i++){
	  double weight = bgoHits->fEnergy.at(i);
	  double weight0 = bgoHits->fES0.at(i);
	  double weight1 = bgoHits->fES1.at(i);
		//DmpLogInfo<<"energy of this hit is "<<weight<<' '<<weight0<<' '<<weight1<<DmpLogEndl;
		if (weight < hitThreshold){
		  continue;
		}
		short layer = DmpBgoBase::GetLayerID(bgoHits->fGlobalBarID.at(i));
		//short bar   = DmpBgoBase::GetBarID(bgoHits->fGlobalBarID.at(i));
		double posx = DmpBgoBase::Parameter()->BarCenter(bgoHits->fGlobalBarID.at(i)).x();
		double posy = DmpBgoBase::Parameter()->BarCenter(bgoHits->fGlobalBarID.at(i)).y();
		double posz = DmpBgoBase::Parameter()->LayerCenter(bgoHits->fGlobalBarID.at(i)).z();
		//DmpLogInfo<<"z is "<<posz<<DmpLogEndl;
		if(layer%2 == 0){
		  trackYZ0->Fill(posy,494-posz-10*29,weight0);
		  trackYZ1->Fill(posy,494-posz-10*29,weight1);
		  //trackYZ2->Fill(bar,13-layer,weight);
		}
		else{
		  trackXZ0->Fill(posx,494-posz-29*10,weight0);
		  trackXZ1->Fill(posx,494-posz-29*10,weight1);
		  //trackXZ2->Fill(bar,13-layer,weight);
		}
	}

	// Draw track
  TCanvas *c1 = new TCanvas("c1","track",1000,800);
  gStyle->SetOptStat(0);
  c1->Divide(2,2);
  c1->cd(1);
  trackXZ0->Draw("colz");
  ReverseYAxis(trackXZ0);
  c1->cd(2);
  trackYZ0->Draw("colz");
  ReverseYAxis(trackYZ0);
  c1->cd(3);
  trackXZ1->Draw("colz");
  //trackXZ2->Draw("colz");
  ReverseYAxis(trackXZ1);
  c1->cd(4);
  trackYZ1->Draw("colz");
  //trackYZ2->Draw("colz");
  ReverseYAxis(trackYZ1);

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

