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

void drawUSRot();

int main() {
  drawUSRot();
}

void drawUSRot(){
  // auto Runs = runlist();
  rapidjson::Document doc;
  auto& Runs = runlist::load(doc);

  // for (auto &[runName, runInfos] : Runs.items()) {
  for (auto itr = Runs.MemberBegin(); itr != Runs.MemberEnd(); ++itr) {
    // std::cout << "Run number: " << runName << std::endl;
    // std::cout << "Run infos: " << runInfos << std::endl;
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

    //normalization for rotational background
    double norm_min = 1.055;
    double norm_max = 1.075;
    if (runInfos.HasMember("norm")) {
      norm_min = runInfos["norm"].GetArray()[0].GetDouble();
      norm_max = runInfos["norm"].GetArray()[1].GetDouble();
    }
    
    //2nd normalization for rotational background
    double norm2min = 0.0;
    double norm2max = 0.0;
    if (runInfos.HasMember("norm2")) {
      norm2min = runInfos["norm2"].GetArray()[0].GetDouble();
      norm2max = runInfos["norm2"].GetArray()[1].GetDouble();
    }

    TFile *fin = new TFile((DIRECTORY + runName + results).c_str(), "READ");
    if (!fin || fin->IsZombie()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }
    // fin->ls();

    TH1D* hProjInvMassUS[nmult][npt];
    TH1D* hProjInvMassUSRot[nmult][npt];

    for (int j = 0; j < nmult; j++) {
      TCanvas *cUSRot = new TCanvas(Form("cUSRot_%d", j), Form("Canvas_USRot %d", j), 6*200, (npt+5)/6*160);
      cUSRot->Divide(6, (npt+5)/6, 0.001, 0.001);
      cUSRot->SetGrid();
      
      for (int k = 0; k < npt; k++) {
      const int cIdx = k + 1;
      cUSRot->cd(cIdx);
      cUSRot->GetPad(cIdx)->SetRightMargin(0.05);
      gStyle->SetOptTitle(0); // SetOptTitle(0) is used to disable the title of the histogram.
      gStyle->SetOptStat(0); // SetOptStat(0) is used to disable the statistics box of the histogram.

      hProjInvMassUS[j][k] = (TH1D*)fin->Get(Form("f0980_US_EPA/hProjInvMassUS_%d_%d", j, k));
      hProjInvMassUSRot[j][k] = (TH1D*)fin->Get(Form("f0980_USRot_EPA/hProjInvMassUSRot_%d_%d", j, k));
      if (!hProjInvMassUS[j][k] || !hProjInvMassUSRot[j][k]) {
        std::cerr << "Error opening histogram." << std::endl;
        return;
      }

      if (colName == "pp") {
          hProjInvMassUS[j][k]->Rebin(2); // 2 : 5MeV
          hProjInvMassUSRot[j][k]->Rebin(2); // 2 : 5MeV
      }
      else if (colName == "pbpb_kaon") {
          hProjInvMassUS[j][k]->Rebin(2); // 2 : 2MeV
          hProjInvMassUSRot[j][k]->Rebin(2); // 2 : 52MeV
      }

      hProjInvMassUS[j][k]->Draw("");
      // hProjInvMassUS[j][k]->Draw("hist p");
      hProjInvMassUS[j][k]->SetMarkerColor(kBlack);
      hProjInvMassUS[j][k]->SetLineColor(kBlue);
      // hProjInvMassUS[j][k]->SetMarkerStyle(4);
      // hProjInvMassUS[j][k]->SetMarkerSize(0.5);
      hProjInvMassUS[j][k]->GetXaxis()->SetTitle("M_{#pi#pi} (GeV/c^{2})");
      hProjInvMassUS[j][k]->GetXaxis()->SetRangeUser(0.99, 1.08); // mass range
      hProjInvMassUS[j][k]->GetYaxis()->SetTitle("Counts");


      // normalization range box
      // hProjInvMassUSRot[j][k]->GetXaxis()->SetRangeUser(0.99, 1.08); // mass range
      TH1D* hNormRegion = (TH1D*)hProjInvMassUSRot[j][k]->Clone("hNormRegion");
      int nbins = hNormRegion->GetNbinsX();

      for (int ibin = 1; ibin <= nbins; ++ibin) {
        double bin_center = hNormRegion->GetBinCenter(ibin);
        bool inNorm1 = (bin_center >= norm_min && bin_center <= norm_max);
        bool inNorm2 = runInfos.HasMember("norm2") && (bin_center >= norm2min && bin_center <= norm2max);

        if (!(inNorm1 || inNorm2)) {
          hNormRegion->SetBinContent(ibin, 0);
        }
      }
      hNormRegion->SetFillColorAlpha(kGreen, 0.3);
      hNormRegion->SetLineColorAlpha(kGreen, 0.0);
      hNormRegion->Draw("hist same");


      hProjInvMassUSRot[j][k]->Draw("same");
      // hProjInvMassUSRot[j][k]->Draw("hist p same");
      hProjInvMassUSRot[j][k]->SetMarkerColor(kRed);
      hProjInvMassUSRot[j][k]->SetLineColor(kYellow);
      // hProjInvMassUSRot[j][k]->SetMarkerStyle(4);
      // hProjInvMassUSRot[j][k]->SetMarkerSize(0.5);

      // TLegend *leg = new TLegend(0.5, 0.6, 0.88, 0.88);
      // TLegend *leg = new TLegend(0.4, 0.6, 0.88, 0.88);
      TLegend *leg = new TLegend(0.48, 0.15, 0.88, 0.45);
      // leg->SetTextFont(43);
      leg->SetTextSize(0.045);
      leg->SetLineWidth(0.0);
      leg->SetFillStyle(0);
      leg->AddEntry((TObject *)0, dataSet.c_str(),"");
      leg->AddEntry((TObject *)0, Form("train number : %s", runName.c_str()), "");
      leg->AddEntry((TObject *)0, Form("FT0C %d#font[122]{-}%d %%", m_min[j], m_max[j]), "");
      leg->AddEntry((TObject *)0, "PbPb 5.36 TeV, |#it{y}| < 0.5", "");
      // leg->AddEntry( (TObject*)0, Form("#it{p}_{T,lead} > 5 GeV/#it{c},%s",trnsName[r]), "");
      leg->AddEntry((TObject *)0, Form("%.1lf < #it{p}_{T} < %.1lf GeV/#it{c}", p_min[k], p_max[k]), "");
      leg->Draw();
      }


      //   TString fileName = Form("plot/Invmass_mult_%d_%d.pdf", m_min[j], m_max[j]); 
      TString fileName = Form((DIRECTORY + runName + "/" + "plot_c/USRot/Invmass_USRot_%d_%d.pdf").c_str(), m_min[j], m_max[j]);
      // TString fileName = Form((DIRECTORY + runName + "/" + "plot_c/USLS/rebin_Invmass_USLS_%d_%d.pdf").c_str(), m_min[j], m_max[j]);
      // TString fileName = Form((DIRECTORY + runName + "/" + "plot_c/USLS_N/rebin_Invmass_USLS_%d_%d.pdf").c_str(), m_min[j], m_max[j]);
      std::cout << (DIRECTORY + runName + "/" + "plot_c").c_str() << std::endl;
      cUSRot->SaveAs(fileName);
      delete cUSRot;
    }
  }
}


