
#pragma once

#include <algorithm>
#include <math.h>

#include "BaseOperator.h"

template <class EventClass> class EventWriterOperator : public BaseOperator<EventClass> {

  public:
 
    bool root_;
    std::string dir_;
    float n_ev_; 

    //count histos
    TH1D h_nevts {"h_nevts", "number of events", 1, 0., 1.};

    // variables to save in branches
    std::vector<alp::Jet> * jets_ptr = nullptr;
    std::vector<alp::PtEtaPhiEVector> * dijets_ptr = nullptr;
    std::vector<float> * muons_pt_ptr = nullptr;
    std::vector<float> * muons_pfiso03_ptr = nullptr;
    float met_pt;
    float w_btag;
    bool hlt0;
    bool hlt1;
    bool hlt2;
    bool hlt3;
    bool hltOr01;
    bool hltOr23;
    bool hltOr0123;

    TTree tree_{"tree","Tree using simplified mut::dataformats"};

     EventWriterOperator(bool root = false, std::string dir = "") :
      root_(root),
      dir_(dir) {
        n_ev_ = 0.;
        met_pt = 0.;
        w_btag = 0.;
        hlt0 = false;
        hlt1 = false;
        hlt2 = false;
        hlt3 = false;
        hltOr01 = false;
        hltOr23 = false;
        hltOr0123 = false;
      }
    virtual ~EventWriterOperator() {}

    virtual void init(TDirectory * tdir) {
      if (root_) {
        tdir = tdir->GetFile();
        auto ndir = tdir->mkdir(dir_.c_str());
        if (ndir == 0) {
          tdir = tdir->GetDirectory(dir_.c_str());
        } else {
          tdir = ndir;
        }
      }
      h_nevts.SetDirectory(tdir);

      tree_.Branch("Jets","std::vector<alp::Jet>",&jets_ptr, 64000, 2);
      tree_.Branch("DiJets","std::vector<alp::PtEtaPhiEVector>", &dijets_ptr, 64000, 2);

      tree_.Branch("muons.pt","std::vector<float>", &muons_pt_ptr, 64000, 2);
      tree_.Branch("muons.pfiso03","std::vector<float>", &muons_pfiso03_ptr, 64000, 2);
      tree_.Branch("met_pt",&met_pt,"met_pt/F");
      tree_.Branch("w_btag",&w_btag,"w_btag/F");
      tree_.Branch("hlt_0",&hlt0,"hlt_0/O");
      tree_.Branch("hlt_1",&hlt1,"hlt_1/O");
      tree_.Branch("hlt_2",&hlt2,"hlt_2/O");
      tree_.Branch("hlt_3",&hlt3,"hlt_3/O"); //DEBUG -- not fix number!
      tree_.Branch("hlt_or01",&hltOr01,"hlt_or01/O"); 
      tree_.Branch("hlt_or23",&hltOr23,"hlt_or23/O"); 
      tree_.Branch("hlt_or0123",&hltOr0123,"hlt_or0123/O"); 

      tree_.SetDirectory(tdir);
      tree_.AutoSave();

   }


    virtual bool process( EventClass & ev ) {

      n_ev_ += ev.w_btag_;

      // to fill tree redirect pointers
      jets_ptr = dynamic_cast<std::vector<alp::Jet> *>(&ev.jets_); 
      dijets_ptr = dynamic_cast<std::vector<alp::PtEtaPhiEVector> *>(&ev.dijets_); 
      //additional variables - to be changed to objects
      muons_pt_ptr = dynamic_cast<std::vector<float> *>(&ev.muons_pt_); 
      muons_pfiso03_ptr = dynamic_cast<std::vector<float> *>(&ev.muons_pfiso03_); 
      met_pt = ev.met_pt_;
      w_btag = ev.w_btag_;

      hlt0 = ev.hlt_bits_.at(0).second; //DEBUG 
      hlt1 = ev.hlt_bits_.at(1).second; //DEBUG 
      hlt2 = ev.hlt_bits_.at(2).second; //DEBUG 
      hlt3 = ev.hlt_bits_.at(3).second; //DEBUG 
      hltOr01 = hlt0 || hlt1; //DEBUG        
      hltOr23 = hlt2 || hlt3; //DEBUG        
      hltOr0123 = hlt0 || hlt1 || hlt2 || hlt3; //DEBUG    

      tree_.Fill();

      return true;
    }

    virtual bool output( TFile * tfile) {

     // tree_.Write("",TObject::kWriteDelete); // it does not solve double tree issue
      h_nevts.SetBinContent(1, n_ev_);

      return true;

    }

};
