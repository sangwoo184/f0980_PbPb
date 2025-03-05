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
const std::string results = "/AnalysisResults.root";

void InvMassLSM();

int main() {
	InvMassLSM();
}

void InvMassLSM() {
  // std::ifstream file("runlist.json");
  // json inputJson = json::parse(file);
  // // auto Runs = inputJson["runs"];
  // // auto Runs = inputJson["run"];
  // // auto Runs = inputJson["runWoInitial"];
  // // auto Runs = inputJson["singlerun"];
  // auto Runs = inputJson["local"];
  // // std::vector<std::string> runs;
  // std::cout << "Runs type: " << typeid(Runs).name() << std::endl;
  
  auto Runs = runlist();


  for (auto &[runName, runInfos] : Runs.items()) {
      // runs.push_back(DIRECTORY + runName); // push_back: append
    // std::cout << "Run number: " << runName << ", Run info: " << runInfos << std::endl;
    std::cout << "Run number: " << runName << std::endl;

    //configuration
    const std::string colName = runInfos["colName"];
    const int nmult = runInfos["nmult"];
    const int npt = runInfos["npt"];
    const double mass_min = runInfos["mass"][0];
    const double mass_max = runInfos["mass"][1];
    // std::cout << "mass : " << mass_min << ", " << mass_max << std::endl;
    // std::cout << colName << " collision" << std::endl;

    // TFile* fin = new TFile("/Volumes/Seul/HP_results/239395.root");
    TFile *fin = new TFile((DIRECTORY + runName + results).c_str(), "read");

    THnSparse* hInvMassUS;
    THnSparse* hInvMassLSpp; 
    THnSparse* hInvMassLSmm; //선언은 중괄호 안에 있으면 밖에 나와서 사라짐 그래서 if문 밖으로 뺌

    if (colName == "pp") {
        hInvMassUS = (THnSparse*)fin->Get("lf-f0980analysis/hInvMass_f0980_US_EPA");
        hInvMassLSpp = (THnSparse*)fin->Get("lf-f0980analysis/hInvMass_f0980_LSpp_EPA");
        hInvMassLSmm = (THnSparse*)fin->Get("lf-f0980analysis/hInvMass_f0980_LSmm_EPA");
    }
    else if (colName == "pbpb") {
        hInvMassUS = (THnSparse*)fin->Get("lf-f0980pbpbanalysis/hInvMass_f0980_US_EPA");
        hInvMassLSpp = (THnSparse*)fin->Get("lf-f0980pbpbanalysis/hInvMass_f0980_LSpp_EPA");
        hInvMassLSmm = (THnSparse*)fin->Get("lf-f0980pbpbanalysis/hInvMass_f0980_LSmm_EPA");
    }
    else{
      std::cout << "collision name error" << std::endl;
    } 

    //	centralitys
    std::vector<int> m_min = runInfos["m_min"];
    std::vector<int> m_max = runInfos["m_max"];
    //	pt
    std::vector<double> p_min = runInfos["p_min"];
    std::vector<double> p_max = runInfos["p_max"];

    // std::cout << "centrality : " << nmult <<std::endl;
    // std::cout << "m_min vector size: " << m_min.size() << std::endl;
    // std::cout << "m_min values: ";
    // for (int i = 0; i < m_min.size(); i++) {
    // 	std::cout << m_min[i];
    // 	if (i < m_min.size()-1) std::cout << ", ";
    // }
    // std::cout << std::endl;
    // // std::cout << m_max << std::endl;
    // std::cout << "npt : " << npt <<std::endl;
    // std::cout << p_min << std::endl;
    // std::cout << p_max << std::endl;

    TH1D* hProjInvMassUS[nmult][npt];
    TH1D* hProjInvMassLSpp[nmult][npt];
    TH1D* hProjInvMassLSmm[nmult][npt];
    TH1D* hProjInvMassLS[nmult][npt];
    TH1D* hProjInvMassSub[nmult][npt];
    TH1D* hProjInvMassSub_D[nmult][npt];
    TH1D* hProjInvMassSub_S[nmult][npt];

    TCanvas *c1 = new TCanvas(Form("c1_%s",runName.c_str()), "Invariant mass distribution", 1400, 600);
    c1->Divide(2, 1, 0.001, 0.001);

    TFile *fout = new TFile((DIRECTORY + runName + "/"+ "InvMassOutLSM.root").c_str(),"recreate");
    TDirectory *dir_US_EPA = fout -> mkdir("f0980_US_EPA");
    TDirectory *dir_LS_EPA = fout -> mkdir("f0980_LS_EPA");
    TDirectory *dir_SUB_EPA = fout -> mkdir("f0980_SUB_EPA");    

    //	centrality = j
    for (int j = 0; j < nmult; j++) {
      hInvMassUS->GetAxis(2)->SetRangeUser(m_min[j], m_max[j] - 0.001);
      hInvMassLSpp->GetAxis(2)->SetRangeUser(m_min[j], m_max[j] - 0.001);
      hInvMassLSmm->GetAxis(2)->SetRangeUser(m_min[j], m_max[j] - 0.001);
      // std::cout << m_min[j] << std::endl;

      for (int k = 0; k < npt; k++) {
        hInvMassUS->GetAxis(1)->SetRangeUser(p_min[k], p_max[k] - 0.001);
        hInvMassLSpp->GetAxis(1)->SetRangeUser(p_min[k], p_max[k] - 0.001);
        hInvMassLSmm->GetAxis(1)->SetRangeUser(p_min[k], p_max[k] - 0.001);

        hProjInvMassUS[j][k] = hInvMassUS->Projection(0, "e");
        hProjInvMassUS[j][k]->SetName(Form("hProjInvMassUS_%d_%d", j, k));
        hProjInvMassLSpp[j][k] = hInvMassLSpp->Projection(0, "e");
        hProjInvMassLSpp[j][k]->SetName(Form("hProjInvMassLSpp_%d_%d", j, k));
        hProjInvMassLSmm[j][k] = hInvMassLSmm->Projection(0, "e");
        hProjInvMassLSmm[j][k]->SetName(Form("hProjInvMassLSmm_%d_%d", j, k));
        hProjInvMassLS[j][k] = (TH1D *)hProjInvMassUS[j][k]->Clone();
        hProjInvMassLS[j][k]->SetName(Form("hProjInvMassLS_%d_%d", j, k));
        hProjInvMassLS[j][k]->Reset();

        for (int p = 0; p < hProjInvMassUS[j][k]->GetNbinsX(); p++) {
          hProjInvMassLS[j][k]->SetBinContent(p + 1, 2.0 * sqrt(hProjInvMassLSpp[j][k]->GetBinContent(p + 1) * hProjInvMassLSmm[j][k]->GetBinContent(p + 1)));
          hProjInvMassLS[j][k]->SetBinError(p + 1, sqrt(hProjInvMassLSpp[j][k]->GetBinContent(p + 1) * hProjInvMassLSmm[j][k]->GetBinContent(p + 1))
                        * (pow(hProjInvMassLSpp[j][k]->GetBinError(p + 1) / hProjInvMassLSpp[j][k]->GetBinContent(p + 1), 2)
                        + pow(hProjInvMassLSmm[j][k]->GetBinError(p + 1) / hProjInvMassLSmm[j][k]->GetBinContent(p + 1), 2)));
        }
        hProjInvMassSub[j][k] = (TH1D*)hProjInvMassUS[j][k]->Clone();
        hProjInvMassSub[j][k]->SetName(Form("hProjInvMassSub_%d_%d", j, k));
        hProjInvMassSub[j][k]->Add(hProjInvMassLS[j][k], -1.0);

        dir_US_EPA -> cd();
        hProjInvMassUS[j][k] -> Write();
        dir_LS_EPA -> cd();
        hProjInvMassLS[j][k] -> Write();
        dir_SUB_EPA -> cd();
        hProjInvMassSub[j][k] -> Write();


        //	Draw
        if (j == 8 && k == 3) {
          c1->cd(1);
          gStyle->SetOptTitle(0);
          gStyle->SetOptStat(0);

          hProjInvMassSub_D[j][k] = (TH1D *)hProjInvMassUS[j][k]->Clone();

          hProjInvMassSub_D[j][k]->Draw("");
          hProjInvMassSub_D[j][k]->SetMarkerColor(kBlack);
          hProjInvMassSub_D[j][k]->SetMarkerStyle(4);
          hProjInvMassSub_D[j][k]->SetMarkerSize(0.5);
          hProjInvMassSub_D[j][k]->GetXaxis()->SetTitle("M_{#pi#pi} (GeV/c^{2})");
          hProjInvMassSub_D[j][k]->GetXaxis()->SetRangeUser(mass_min, mass_max);
          hProjInvMassSub_D[j][k]->GetYaxis()->SetTitle("Counts");
          hProjInvMassLS[j][k]->Draw("same");
          hProjInvMassLS[j][k]->SetMarkerColor(kRed);
          hProjInvMassLS[j][k]->SetMarkerStyle(4);
          hProjInvMassLS[j][k]->SetMarkerSize(0.5);

          TLegend *leg = new TLegend(0.48, 0.13, 0.88, 0.45);
          leg->SetTextSize(0.038);
          leg->SetLineWidth(0.0);
          leg->SetFillStyle(0);
          leg->AddEntry((TObject *)0, "LHC23zzh_pass4_small", "");
          leg->AddEntry((TObject *)0, Form("train number : %s", runName.c_str()), "");
          leg->AddEntry((TObject *)0, Form("FT0C %d#font[122]{-}%d %%", m_min[j], m_max[j]), "");
          leg->AddEntry((TObject *)0, "PbPb 5.36 TeV, |#it{y}| < 0.5", "");
          // leg->AddEntry( (TObject*)0, Form("#it{p}_{T,lead} > 5 GeV/#it{c},
          // %s",trnsName[r]), "");
          leg->AddEntry((TObject *)0, Form("%.1lf < #it{p}_{T} < %.1lf GeV/#it{c}", p_min[k], p_max[k]), "");
          leg->AddEntry(hProjInvMassSub_D[j][k], "Unlike-sign pairs", "p");
          leg->AddEntry(hProjInvMassLS[j][k], "Like-sign pairs", "p");
          leg->Draw();
        }

        if (j == 8 && k == 3) {
          c1->cd(2);
          hProjInvMassSub_S[j][k] = (TH1D *)hProjInvMassSub[j][k]->Clone();
          hProjInvMassSub_S[j][k]->Draw("l");

          hProjInvMassSub_S[j][k]->GetXaxis()->SetTitle("M_{#pi#pi} (GeV/c^{2})");
          hProjInvMassSub_S[j][k]->GetXaxis()->SetRangeUser(mass_min, mass_max);
          hProjInvMassSub_S[j][k]->GetYaxis()->SetTitle("Counts");
          hProjInvMassSub_S[j][k]->SetMarkerColor(kBlue);
          hProjInvMassSub_S[j][k]->SetMarkerStyle(4);
          hProjInvMassSub_S[j][k]->SetMarkerSize(0.8);

        }
        // dir_US_EPA -> cd();
        // hProjInvMassUS[j][k] -> Write();
        // dir_LS_EPA -> cd();
        // hProjInvMassLS[j][k] -> Write();
        // dir_SUB_EPA -> cd();
        // hProjInvMassSub[j][k] -> Write();
      }
    }
    // fout->Close();
    c1->SaveAs((DIRECTORY + runName + "/" + "plot_c/bg_subtraction_LSM.pdf").c_str());

    // TLatex latex;

    // for (int j = 0; j < nmult; j++) {
    //   // TCanvas *c = new TCanvas(Form("c_%d_%d_%d", i, r, j), Form("Canvas
    //   // %d_%d_%d", i, r, j), 1000, 1000);
    //   TCanvas *c =
    //       new TCanvas(Form("c_%d", j), Form("Canvas %d", j), 1500, 700);
    //   c->Divide(6, 3, 0.001, 0.001);
    //   c->SetGrid();

    //   for (int k = 0; k < npt; k++) {
    //     const int cIdx = k + 1;
    //     c->cd(cIdx);
    //     gStyle->SetOptTitle(0);
    //     gStyle->SetOptStat(0);

    //     if (colName == "pp") {
    //         hProjInvMassSub[j][k]->Rebin(2); // 2 : 5MeV
    //     }
    //     // else if (colName == "pbpb") {
    //     //     hProjInvMassSub[j][k]->Rebin(2); //
    //     // }

    //     hProjInvMassSub[j][k]->Draw("hist");
    //     hProjInvMassSub[j][k]->GetXaxis()->SetTitle("M_{#pi#pi} (GeV/c^{2})");
    //     hProjInvMassSub[j][k]->GetXaxis()->SetRangeUser(mass_min, mass_max);

    //     TLegend *leg = new TLegend(0.4, 0.6, 0.9, 0.85);
    //     // leg->SetTextFont(43);
    //     leg->SetTextSize(0.045);
    //     leg->SetLineWidth(0.0);
    //     leg->SetFillStyle(0);
    //     leg->AddEntry((TObject *)0, "LHC22o apass6", "");
    //     leg->AddEntry((TObject *)0, Form("train number : %s", runName.c_str()), "");
    //     leg->AddEntry((TObject *)0, Form("FT0C %d#font[122]{-}%d %%", m_min[j], m_max[j]), "");
    //     leg->AddEntry((TObject *)0, "pp 13.6 TeV, |#it{y}| < 0.5", "");
    //     // leg->AddEntry( (TObject*)0, Form("#it{p}_{T,lead} > 5 GeV/#it{c},%s",trnsName[r]), "");
    //     leg->AddEntry((TObject *)0, Form("%.1lf < #it{p}_{T} < %.1lf GeV/#it{c}", p_min[k], p_max[k]), "");
    //     leg->Draw();
    //   }
    //   //   TString fileName = Form("plot/Invmass_mult_%d_%d.pdf", m_min[j], m_max[j]); 
    //   TString fileName = Form((DIRECTORY + runName + "/" + "plot_c/rebin_Invmass_mult_%d_%d.pdf").c_str(), m_min[j], m_max[j]);
    //   //   TString fileName = Form((DIRECTORY + runName + "/" + "plot_c/rebin_Invmass_mult_%d_%d.pdf").c_str(), m_min[j], m_max[j]);
    //   std::cout << (DIRECTORY + runName + "/" + "plot_c").c_str() << std::endl;
    //   c->SaveAs(fileName);
    //   delete c;
    // }
    // fout->Close();

    // TBrowser b("fout", fout);
    // b.Show();
    // std::cin.get();
  }
}