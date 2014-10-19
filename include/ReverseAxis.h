#ifndef ReverseAxis_h
#define ReverseAxis_h

//#include "TH1.h"
//#include "TH2.h"
#include "TGaxis.h"

void ReverseXAxis (TH1 *h)
{
  // Remove the current axis
  h->GetXaxis()->SetLabelOffset(999);
  h->GetXaxis()->SetTickLength(0);
  
  // Redraw the new axis
  gPad->Update();
  TGaxis *newaxis = new TGaxis(gPad->GetUxmax(),
                               gPad->GetUymin(),
                               gPad->GetUxmin(),
                               gPad->GetUymin(),
                               h->GetXaxis()->GetXmin(),
                               h->GetXaxis()->GetXmax(),
                               510,"-");
  newaxis->SetLabelOffset(-0.03);
  newaxis->Draw();
}

void ReverseYAxis (TH1 *h)
{ 
  // Remove the current axis
  h->GetYaxis()->SetLabelOffset(999);
  h->GetYaxis()->SetTickLength(0);

  // Redraw the new axis
  gPad->Update();
  TGaxis *newaxis = new TGaxis(gPad->GetUxmin()-0.001,
                               gPad->GetUymax(),
                               gPad->GetUxmin(),
                               gPad->GetUymin(),
                               h->GetYaxis()->GetXmin(),
                               h->GetYaxis()->GetXmax(),
                               510,"");
  newaxis->SetLabelOffset(-0.03);
  //newaxis->LabelsOption("u");
  //newaxis->CenterLabels();
  newaxis->Draw();
}

#endif
