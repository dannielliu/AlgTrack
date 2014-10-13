#include "DmpAlgTrk.h"
#include "DmpBgoBase.h"
#include "DmpEvtBgoHits.h"
#include "DmpDataBuffer.h"
#include "TH2D.h"
#include "TCanvas.h"

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
  hitThreshold = 100.;
  gDataBuffer->LinkRootFile("",bgoHits);
	bgoHits = dynamic_cast<DmpEvtBgoHits*>(gDataBuffer->ReadObject(""));
  trackXZ = new TH2D("trackXOZ","trackXOZ",22,0,22,14,0,14);// should be modified
  trackYZ = new TH2D("trackYOZ","trackYOZ",22,0,22,14,0,14);
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgTrk::ProcessThisEvent(){
  if(eventNumber>0){ 
	  return false;
	}
  for (short i=0;i<bg oHits->GetHittedBarNumber();i++){
	  double weight = bgoHits->fEnergy.at(i);
		if (weight < hitThreshold){
		  continue;
		}

		//there is some problem here, should not fill posxx
		double posx = bgoHits->fPosition.at(i).x();
		double posy = bgoHits->fPosition.at(i).y();
		double posz = bgoHits->fPosition.at(i).z();
		short layer = DmpBgoBase::GetLayerID(bgoHits->fGlobalBarID.at(i));
		if (layer%2 == 0){
		  trackXZ->Fill(posx,posz,weight);
		} 
		else{
		  trackYZ->Fill(posy,posz,weight);
		}
	}
	eventNumber++;

	// Draw track
  TCanvas *c1 = new TCanvas("c1","track",600,800);
	c1->Divide(1,2);
	c1->cd(1);
	trackXZ->Draw("colz");
	c1->cd(2);
	trackYZ->Draw("colz");
	c1->Print("track.eps");
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgTrk::Finalize(){
  return true;
}

