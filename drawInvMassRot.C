#include "TCanvas.h"
#include "TLatex.h"
#include "TString.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TROOT.h"
#include "THnSparse.h"
#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
// #include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include "runlist.h"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>


// using json = nlohmann::json;

const std::string DIRECTORY = "/Users/sangwoo/cernbox/workspace/f0_draw/pass4_small/";
// const std::string results = "/InvMassOut_c.root";
const std::string results = "/InvMassOutAll.root";
// const std::string results = "/InvMassOutRot.root";

void drawInvMassRot();

int main() {
  drawInvMassRot();
}

void drawInvMassRot(){
  // auto Runs = runlist();
  rapidjson::Document doc;
  auto& Runs = runlist::load(doc);

  // for (auto &[runName, runInfos] : Runs.items()) {
  for (auto itr = Runs.MemberBegin(); itr != Runs.MemberEnd(); ++itr) {
    std::string runName = itr->name.GetString();
    auto& runInfos = itr->value;
    std::cout << "Run number: " << runName << std::endl;

    //configuration
    const std::string colName = runInfos["colName"].GetString();
    const std::string dataSet = runInfos["dataSet"].GetString();
    const int nmult = runInfos["nmult"].GetInt();
    const int npt = runInfos["npt"].GetInt();
    const double mass_min = runInfos["mass"].GetArray()[0].GetDouble();
    const double mass_max = runInfos["mass"].GetArray()[1].GetDouble();
    //	centralitys
    // std::vector<int> m_min = runInfos["m_min"];
    // std::vector<int> m_max = runInfos["m_max"];
    std::vector<int> m_min;
    for (auto& v : runInfos["m_min"].GetArray()) m_min.push_back(v.GetInt());
    std::vector<int> m_max;
    for (auto& v : runInfos["m_max"].GetArray()) m_max.push_back(v.GetInt());
    //	pt
    // std::vector<double> p_min = runInfos["p_min"];
    // std::vector<double> p_max = runInfos["p_max"];
    std::vector<double> p_min;
    for (auto& v : runInfos["p_min"].GetArray()) p_min.push_back(v.GetDouble());
    std::vector<double> p_max;
    for (auto& v : runInfos["p_max"].GetArray()) p_max.push_back(v.GetDouble());

    TFile *fin = new TFile((DIRECTORY + runName + results).c_str(), "READ");
    if (!fin || fin->IsZombie()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    TH1D* hProjInvMassSubRot[nmult][npt];

    

    for (int j = 0; j < nmult; j++) {
      TCanvas *c = new TCanvas(Form("c_%d", j), Form("Canvas %d", j), 6*200, (npt+5)/6*160);
      c->Divide(6, (npt+5)/6, 0.001, 0.001);
      c->SetGrid();

      for (int k = 0; k < npt; k++) {
      const int cIdx = k + 1;
      c->cd(cIdx);
      gStyle->SetOptTitle(0);
      gStyle->SetOptStat(0);

      hProjInvMassSubRot[j][k] = (TH1D*)fin->Get(Form("f0980_SUBRot_EPA/hProjInvMassSubRot_%d_%d", j, k));

      if (!hProjInvMassSubRot[j][k]) {
        std::cerr << "Error opening histogram." << std::endl;
        return;
      }

      if (colName == "pp") {
          hProjInvMassSubRot[j][k]->Rebin(2); // 2 : 5 MeV
      }
      else if (colName == "pbpb_kaon") {
          hProjInvMassSubRot[j][k]->Rebin(2); // 2 : 2 MeV 
      }

      hProjInvMassSubRot[j][k]->SetLineWidth(1);
      hProjInvMassSubRot[j][k]->Draw("");
      hProjInvMassSubRot[j][k]->GetXaxis()->SetTitle("M_{#pi#pi} (GeV/c^{2})");
      hProjInvMassSubRot[j][k]->GetXaxis()->SetRangeUser(0.99, 1.08); // mass range

      //  TLegend *leg = new TLegend(0.4, 0.6, 0.7, 0.88);
      // TLegend *leg = new TLegend(0.42, 0.6, 0.88, 0.88);
      TLegend *leg = new TLegend(0.43, 0.66, 0.85, 0.88);
      leg->SetTextSize(0.045);
      leg->SetLineWidth(0.0);
      leg->SetFillStyle(0);
      leg->AddEntry((TObject *)0, dataSet.c_str(), "");
      leg->AddEntry((TObject *)0, Form("train number : %s", runName.c_str()), "");
      leg->AddEntry((TObject *)0, Form("FT0C %d#font[122]{-}%d %%", m_min[j], m_max[j]), "");
      leg->AddEntry((TObject *)0, "PbPb 5.36 TeV, |#it{y}| < 0.5", "");
      // leg->AddEntry( (TObject*)0, Form("#it{p}_{T,lead} > 5 GeV/#it{c},%s",trnsName[r]), "");
      leg->AddEntry((TObject *)0, Form("%.1lf < #it{p}_{T} < %.1lf GeV/#it{c}", p_min[k], p_max[k]), "");
      leg->Draw();
      }
      // TString fileName = Form("plot/Invmass_mult_%d_%d.pdf", m_min[j], m_max[j]); 
      // TString fileName = Form((DIRECTORY + runName + "/" + "plot_c/Invmass_mult_%d_%d.pdf").c_str(), m_min[j], m_max[j]);
      // TString fileName = Form((DIRECTORY + runName + "/" + "plot_c/rebin_Invmass_mult_%d_%d.pdf").c_str(), m_min[j], m_max[j]);
      TString fileName = Form((DIRECTORY + runName + "/" + "plot_c/InvmassRot_mult_%d_%d.pdf").c_str(), m_min[j], m_max[j]);
      // TString fileName = Form((DIRECTORY + runName + "/" + "plot_c/InvmassRot_mult_rebin_%d_%d.pdf").c_str(), m_min[j], m_max[j]);
      std::cout << (DIRECTORY + runName + "/" + "plot_c").c_str() << std::endl;
      c->SaveAs(fileName);
      delete c;
    }
  }
}


