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
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include "runlist.h"


// using json = nlohmann::json;

const std::string DIRECTORY = "/Users/sangwoo/cernbox/workspace/f0_draw/pass4_small/";
const std::string results = "/AnalysisResults.root";

void InvMassAll();

int main() {
	InvMassAll();
}

void InvMassAll() {
  // auto Runs = runlist();
  rapidjson::Document doc;
  auto& Runs = runlist::load(doc);

  // for (auto &[runName, runInfos] : Runs.items()) {
  for (auto itr = Runs.MemberBegin(); itr != Runs.MemberEnd(); ++itr) {
    //   // runs.push_back(DIRECTORY + runName); // push_back: append
    // // std::cout << "Run number: " << runName << ", Run info: " << runInfos << std::endl;
    // std::cout << "Run number: " << runName << std::endl;
    std::string runName = itr->name.GetString();
    // std::ofstream normOut_txt((DIRECTORY + runName + "/" + "plot_h" + "/" + "norm_factor_Rot_initial.txt").c_str());
    // std::ofstream normOut_csv((DIRECTORY + runName + "/" + "plot_h" + "/" + "norm_factor_Rot_initial.csv").c_str());
    std::ofstream normOut_txt((DIRECTORY + runName + "/" + "plot_c" + "/" + "norm_factor_Rot_initial_" + runName + ".txt").c_str());
    std::ofstream normOut_csv((DIRECTORY + runName + "/" + "plot_c" + "/" + "norm_factor_Rot_initial_" + runName + ".csv").c_str());
    auto& runInfos = itr->value;

    std::cout << "Run number: " << runName << std::endl;

    //configuration
    const std::string colName = runInfos["colName"].GetString();
    const int nmult = runInfos["nmult"].GetInt();
    const int npt = runInfos["npt"].GetInt();
    const double mass_min = runInfos["mass"].GetArray()[0].GetDouble();
    const double mass_max = runInfos["mass"].GetArray()[1].GetDouble();
    
    //normalization for rotational background
    // double norm_min = 1.85;
    // double norm_max = 2.0;
    double norm_min = 1.055;
    double norm_max = 1.075;
    //low
    // double norm_min = 0.99;
    // double norm_max = 1.01;
    //high
    // double norm_min = 1.08;
    // double norm_max = 1.06;
    if (runInfos.HasMember("norm")) {
      norm_min = runInfos["norm"].GetArray()[0].GetDouble();
      norm_max = runInfos["norm"].GetArray()[1].GetDouble();
    }
    // 2nd normalization for rotational background
    double norm2min = 0.0;
    double norm2max = 0.0;
    if (runInfos.HasMember("norm2")) {
      norm2min = runInfos["norm2"].GetArray()[0].GetDouble();
      norm2max = runInfos["norm2"].GetArray()[1].GetDouble();
    }
    
    TFile *fin = new TFile((DIRECTORY + runName + results).c_str(), "read");

    THnSparse* hInvMassUS;
    THnSparse* hInvMassLSpp; 
    THnSparse* hInvMassLSmm; //선언은 중괄호 안에 있으면 밖에 나와서 사라짐 그래서 if문 밖으로 뺌
    THnSparse* hInvMassUSRot;

    if (colName == "pp") {
        hInvMassUS = (THnSparse*)fin->Get("lf-f0980analysis/hInvMass_f0980_US_EPA");
        hInvMassLSpp = (THnSparse*)fin->Get("lf-f0980analysis/hInvMass_f0980_LSpp_EPA");
        hInvMassLSmm = (THnSparse*)fin->Get("lf-f0980analysis/hInvMass_f0980_LSmm_EPA");
    }
    else if (colName == "pbpb" || colName == "pbpb_kaon") {
        // hInvMassUS = (THnSparse*)fin->Get("lf-f0980pbpbanalysis/hInvMass_f0980_US_EPA");
        // hInvMassLSpp = (THnSparse*)fin->Get("lf-f0980pbpbanalysis/hInvMass_f0980_LSpp_EPA");
        // hInvMassLSmm = (THnSparse*)fin->Get("lf-f0980pbpbanalysis/hInvMass_f0980_LSmm_EPA");
        // hInvMassUSRot = (THnSparse*)fin->Get("lf-f0980pbpbanalysis/hInvMass_f0980_USRot_EPA");
        hInvMassUS = (THnSparse*)fin->Get("f0980pbpbanalysis/hInvMass_f0980_US_EPA");
        hInvMassLSpp = (THnSparse*)fin->Get("f0980pbpbanalysis/hInvMass_f0980_LSpp_EPA");
        hInvMassLSmm = (THnSparse*)fin->Get("f0980pbpbanalysis/hInvMass_f0980_LSmm_EPA");
        hInvMassUSRot = (THnSparse*)fin->Get("f0980pbpbanalysis/hInvMass_f0980_USRot_EPA");

        if (!hInvMassUS || !hInvMassLSpp || !hInvMassLSmm || !hInvMassUSRot) {
          std::cerr << "[ERROR] Histogram(s) not found in file for run " << runName << std::endl;
          fin->Close();
          continue;
        }
    }
    else{
      std::cout << "collision name error" << std::endl;
    }

    // //	centralitys
    // std::vector<int> m_min = runInfos["m_min"];
    // std::vector<int> m_max = runInfos["m_max"];
    // //	pt
    // std::vector<double> p_min = runInfos["p_min"];
    // std::vector<double> p_max = runInfos["p_max"];
    
    std::vector<int> m_min;
    for (auto& v : runInfos["m_min"].GetArray()) m_min.push_back(v.GetInt());

    std::vector<int> m_max;
    for (auto& v : runInfos["m_max"].GetArray()) m_max.push_back(v.GetInt());

    std::vector<double> p_min;
    for (auto& v : runInfos["p_min"].GetArray()) p_min.push_back(v.GetDouble());

    std::vector<double> p_max;
    for (auto& v : runInfos["p_max"].GetArray()) p_max.push_back(v.GetDouble());

    TH1D* hProjInvMassUS[nmult][npt];
    TH1D* hProjInvMassLSpp[nmult][npt];
    TH1D* hProjInvMassLSmm[nmult][npt];
    TH1D* hProjInvMassLS[nmult][npt];
    TH1D* hProjInvMassSub[nmult][npt];
    TH1D* hProjInvMassSub_D[nmult][npt];
    TH1D* hProjInvMassSub_S[nmult][npt];
    TH1D* hProjInvMassUSRot[nmult][npt];
    TH1D* hProjInvMassSubRot[nmult][npt];
    TH1D* hProjInvMassSubRot_D[nmult][npt];
    TH1D* hProjInvMassSubRot_S[nmult][npt];
    // TH1D* hProjInvMassNLS[nmult][npt];
    // TH1D* hProjInvMassNSub[nmult][npt];

    TCanvas *c1 = new TCanvas(Form("c1_%s",runName.c_str()), "Invariant mass distribution", 1400, 600);
    c1->Divide(2, 1, 0.001, 0.001);
    TCanvas *c2 = new TCanvas(Form("c2_%s",runName.c_str()), "Invariant mass distribution Rotation", 1400, 600);
    c2->Divide(2, 1, 0.001, 0.001);

    TFile *fout = new TFile((DIRECTORY + runName + "/"+ "InvMassOutAll.root").c_str(),"recreate");
    TDirectory *dir_US_EPA = fout -> mkdir("f0980_US_EPA");
    TDirectory *dir_LS_EPA = fout -> mkdir("f0980_LS_EPA");
    TDirectory *dir_SUB_EPA = fout -> mkdir("f0980_SUB_EPA");
    TDirectory *dir_USRot_EPA = fout -> mkdir("f0980_USRot_EPA");
    TDirectory *dir_SUBRot_EPA = fout -> mkdir("f0980_SUBRot_EPA");
    // TDirectory *dir_NLS_EPA = fout -> mkdir("f0980_NLS_EPA");
    // TDirectory *dir_NSub_EPA = fout -> mkdir("f0980_NSub_EPA");

    //	centrality = j
    for (int j = 0; j < nmult; j++) {
      hInvMassUS->GetAxis(2)->SetRangeUser(m_min[j], m_max[j] - 0.001);
      hInvMassLSpp->GetAxis(2)->SetRangeUser(m_min[j], m_max[j] - 0.001);
      hInvMassLSmm->GetAxis(2)->SetRangeUser(m_min[j], m_max[j] - 0.001);
      hInvMassUSRot->GetAxis(2)->SetRangeUser(m_min[j], m_max[j] - 0.001);
      // std::cout << m_min[j] << std::endl;

      for (int k = 0; k < npt; k++) {
        hInvMassUS->GetAxis(1)->SetRangeUser(p_min[k], p_max[k] - 0.001);
        hInvMassLSpp->GetAxis(1)->SetRangeUser(p_min[k], p_max[k] - 0.001);
        hInvMassLSmm->GetAxis(1)->SetRangeUser(p_min[k], p_max[k] - 0.001);
        hInvMassUSRot->GetAxis(1)->SetRangeUser(p_min[k], p_max[k] - 0.001);

        hProjInvMassUS[j][k] = hInvMassUS->Projection(0, "e");
        hProjInvMassUS[j][k]->SetName(Form("hProjInvMassUS_%d_%d", j, k));
        hProjInvMassLSpp[j][k] = hInvMassLSpp->Projection(0, "e");
        hProjInvMassLSpp[j][k]->SetName(Form("hProjInvMassLSpp_%d_%d", j, k));
        hProjInvMassLSmm[j][k] = hInvMassLSmm->Projection(0, "e");
        hProjInvMassLSmm[j][k]->SetName(Form("hProjInvMassLSmm_%d_%d", j, k));
        hProjInvMassLS[j][k] = (TH1D *)hProjInvMassUS[j][k]->Clone();
        hProjInvMassLS[j][k]->SetName(Form("hProjInvMassLS_%d_%d", j, k));
        hProjInvMassLS[j][k]->Reset();
        hProjInvMassUSRot[j][k] = hInvMassUSRot->Projection(0, "e");
        hProjInvMassUSRot[j][k]->SetName(Form("hProjInvMassUSRot_%d_%d", j, k));
        
        // like sign method
        for (int p = 0; p < hProjInvMassUS[j][k]->GetNbinsX(); p++) {
          hProjInvMassLS[j][k]->SetBinContent(p + 1, 2.0 * sqrt(hProjInvMassLSpp[j][k]->GetBinContent(p + 1) * hProjInvMassLSmm[j][k]->GetBinContent(p + 1)));
          hProjInvMassLS[j][k]->SetBinError(p + 1, sqrt(hProjInvMassLSpp[j][k]->GetBinContent(p + 1) * hProjInvMassLSmm[j][k]->GetBinContent(p + 1))
                        * (pow(hProjInvMassLSpp[j][k]->GetBinError(p + 1) / hProjInvMassLSpp[j][k]->GetBinContent(p + 1), 2)
                        + pow(hProjInvMassLSmm[j][k]->GetBinError(p + 1) / hProjInvMassLSmm[j][k]->GetBinContent(p + 1), 2)));
        }

        // double USP_integral = hProjInvMassUS[j][k] -> Integral(hProjInvMassUS[j][k] -> FindBin(norm_min), hProjInvMassUS[j][k] -> FindBin(norm_max));
        // double LSP_integral = hProjInvMassLS[j][k] -> Integral(hProjInvMassLS[j][k] -> FindBin(norm_min), hProjInvMassLS[j][k] -> FindBin(norm_max));
        // double norm_factor_LS = (LSP_integral > 0) ? (USP_integral / LSP_integral) : 1.0;
        // hProjInvMassLS[j][k]->Scale(norm_factor_LS);
        // std::cout << "normalization factor (USLS): " << norm_factor_LS << std::endl;

        hProjInvMassSub[j][k] = (TH1D*)hProjInvMassUS[j][k]->Clone();
        hProjInvMassSub[j][k]->SetName(Form("hProjInvMassSub_%d_%d", j, k));
        hProjInvMassSub[j][k]->Add(hProjInvMassLS[j][k], -1.0);

        dir_US_EPA -> cd();
        hProjInvMassUS[j][k] -> Write();
        dir_LS_EPA -> cd();
        hProjInvMassLS[j][k] -> Write();
        dir_SUB_EPA -> cd();
        hProjInvMassSub[j][k] -> Write();

        // normalization for rotational method
        double US_integral = hProjInvMassUS[j][k] -> Integral(hProjInvMassUS[j][k] -> FindBin(norm_min), hProjInvMassUS[j][k] -> FindBin(norm_max));
        double USRot_integral = hProjInvMassUSRot[j][k] -> Integral(hProjInvMassUSRot[j][k] -> FindBin(norm_min), hProjInvMassUSRot[j][k] -> FindBin(norm_max));
        // double norm_factor_Rot = (USRot_integral > 0) ? (US_integral / USRot_integral) : 1.0;
        double US_integral2 = (norm2max > norm2min) ? hProjInvMassUS[j][k]->Integral(hProjInvMassUS[j][k]->FindBin(norm2min), hProjInvMassUS[j][k]->FindBin(norm2max)) : 0.0;
        double USRot_integral2 = (norm2max > norm2min) ? hProjInvMassUSRot[j][k]->Integral(hProjInvMassUSRot[j][k]->FindBin(norm2min), hProjInvMassUSRot[j][k]->FindBin(norm2max)) : 0.0;
        double US_total = US_integral + US_integral2;
        double USRot_total = USRot_integral + USRot_integral2;
        double norm_factor_Rot = (USRot_total > 0) ? (US_total / USRot_total) : 1.0;
        std::cout << ""<<runName<<" normalization factor ["<<j<<"],["<<k<<"] : " << norm_factor_Rot << std::endl;
        normOut_txt << "run name" << runName << "centrality " << j << ", pT " << k << "; normalization factor : " << norm_factor_Rot << std::endl;
        if (j == 0 && k == 0) {
          normOut_csv << "run name,centrality,pT,normalization factor" << std::endl; // CSV header
        }
        normOut_csv << runName << "," << j << "," << k << "," << norm_factor_Rot << std::endl;
        // double LS_integral = hProjInvMassLS[j][k] -> Integral(hProjInvMassLS[j][k] -> FindBin(norm_min), hProjInvMassLS[j][k] -> FindBin(norm_max));
        // double norm_factor_LS = (LS_integral > 0) ? (US_integral / LS_integral) : 1.0;

        // like sign method
        // hProjInvMassNLS[j][k] = (TH1D *)hProjInvMassLS[j][k]->Clone();
        // hProjInvMassNLS[j][k]->Scale(norm_factor_LS);
        // hProjInvMassNLS[j][k]->SetName(Form("hProjInvMassNLS_%d_%d", j, k));

        // hProjInvMassNSub[j][k] = (TH1D *)hProjInvMassUS[j][k]->Clone();
        // hProjInvMassNSub[j][k]->SetName(Form("hProjInvMassNSub_%d_%d", j, k));
        // hProjInvMassNSub[j][k]->Add(hProjInvMassNLS[j][k], -1.0);

        // dir_NLS_EPA -> cd();
        // hProjInvMassNLS[j][k] -> Write();
        // dir_NSub_EPA -> cd();
        // hProjInvMassNSub[j][k] -> Write();

        // //rotational method
        hProjInvMassUSRot[j][k]->Scale(norm_factor_Rot);

        hProjInvMassSubRot[j][k] = (TH1D *)hProjInvMassUS[j][k]->Clone();
        hProjInvMassSubRot[j][k]->SetName(Form("hProjInvMassSubRot_%d_%d", j, k));
        hProjInvMassSubRot[j][k]->Add(hProjInvMassUSRot[j][k], -1.0);

        dir_USRot_EPA -> cd();
        hProjInvMassUSRot[j][k] -> Write();
        dir_SUBRot_EPA -> cd();
        hProjInvMassSubRot[j][k] -> Write();


        // Draw - like sign method
        if (j == 5 && k == 18) {
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

        if (j == 6 && k == 3) {
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

        // Draw - rotational method
        if (j == 5 && k == 18) {
          c2->cd(1);
          gStyle->SetOptTitle(0);
          gStyle->SetOptStat(0);

          hProjInvMassSubRot_D[j][k] = (TH1D *)hProjInvMassUS[j][k]->Clone();

          hProjInvMassSubRot_D[j][k]->Draw("");
          hProjInvMassSubRot_D[j][k]->SetMarkerColor(kBlack);
          hProjInvMassSubRot_D[j][k]->SetMarkerStyle(4);
          hProjInvMassSubRot_D[j][k]->SetMarkerSize(0.5);
          hProjInvMassSubRot_D[j][k]->GetXaxis()->SetTitle("M_{#pi#pi} (GeV/c^{2})");
          // hProjInvMassSubRot_D[j][k]->GetXaxis()->SetRangeUser(mass_min, mass_max);
          hProjInvMassSubRot_D[j][k]->GetXaxis()->SetRangeUser(0.99, 1.08);
          hProjInvMassSubRot_D[j][k]->GetYaxis()->SetTitle("Counts");
          hProjInvMassUSRot[j][k]->Draw("same");
          hProjInvMassUSRot[j][k]->SetMarkerColor(kRed);
          hProjInvMassUSRot[j][k]->SetMarkerStyle(4);
          hProjInvMassUSRot[j][k]->SetMarkerSize(0.5);

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
          leg->AddEntry(hProjInvMassUSRot[j][k], "UnLike-sign rotation pairs", "p");
          leg->Draw();
        }

        if (j == 6 && k == 3) {
          c2->cd(2);
          hProjInvMassSubRot_S[j][k] = (TH1D *)hProjInvMassSubRot[j][k]->Clone();
          hProjInvMassSubRot_S[j][k]->Draw("l");

          hProjInvMassSubRot_S[j][k]->GetXaxis()->SetTitle("M_{#pi#pi} (GeV/c^{2})");
          // hProjInvMassSubRot_S[j][k]->GetXaxis()->SetRangeUser(mass_min, mass_max);
          hProjInvMassSubRot_S[j][k]->GetXaxis()->SetRangeUser(0.99, 1.08);
          hProjInvMassSubRot_S[j][k]->GetYaxis()->SetTitle("Counts");
          hProjInvMassSubRot_S[j][k]->SetMarkerColor(kBlue);
          hProjInvMassSubRot_S[j][k]->SetMarkerStyle(4);
          hProjInvMassSubRot_S[j][k]->SetMarkerSize(0.8);

        }
      }
    }
    // fout->Close();
    c1->SaveAs((DIRECTORY + runName + "/" + "plot_c/bg_subtraction_LSM.pdf").c_str());
    c2->SaveAs((DIRECTORY + runName + "/" + "plot_c/bg_subtraction_Rot.pdf").c_str());
    normOut_txt.close();
    normOut_csv.close();
  }
}