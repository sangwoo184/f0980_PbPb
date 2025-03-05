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
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include "runlist.h"

using json = nlohmann::json;

const std::string DIRECTORY = "/Users/sangwoo/cernbox/workspace/f0_draw/pass4_small/";
// const std::string results = "/InvMassOut_c.root";
const std::string results = "/InvMassOutAll.root";
// const std::string results = "/InvMassOutRot.root";

void drawUSRot();

int main() {
  drawUSRot();
}

void drawUSRot(){
  auto Runs = runlist();

  for (auto &[runName, runInfos] : Runs.items()) {
    // std::cout << "Run number: " << runName << std::endl;
    // std::cout << "Run infos: " << runInfos << std::endl;

    //configuration
    const std::string colName = runInfos["colName"];
    const int nmult = runInfos["nmult"];
    const int npt = runInfos["npt"];
    const double mass_min = runInfos["mass"][0];
    const double mass_max = runInfos["mass"][1];
    //	centralitys
    std::vector<int> m_min = runInfos["m_min"];
    std::vector<int> m_max = runInfos["m_max"];
    //	pt
    std::vector<double> p_min = runInfos["p_min"];
    std::vector<double> p_max = runInfos["p_max"];

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
      // else if (colName == "pbpb") {
      //     hProjInvMassSub[j][k]->Rebin(2); //
      // }

      hProjInvMassUS[j][k]->Draw("");
      // hProjInvMassUS[j][k]->Draw("hist p");
      hProjInvMassUS[j][k]->SetMarkerColor(kBlack);
      hProjInvMassUS[j][k]->SetLineColor(kBlue);
      // hProjInvMassUS[j][k]->SetMarkerStyle(4);
      // hProjInvMassUS[j][k]->SetMarkerSize(0.5);
      hProjInvMassUS[j][k]->GetXaxis()->SetTitle("M_{#pi#pi} (GeV/c^{2})");
      hProjInvMassUS[j][k]->GetXaxis()->SetRangeUser(mass_min, mass_max);
      hProjInvMassUS[j][k]->GetYaxis()->SetTitle("Counts");

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
      leg->AddEntry((TObject *)0, "LHC23zzh_pass4_small", "");
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


