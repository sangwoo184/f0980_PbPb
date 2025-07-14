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
// const std::string results = "/InvMassOut_c.root";
const std::string results = "/InvMassOutAll.root";
// const std::string results = "/InvMassOutLSM.root";

// TLegend* MakeLegend(const std::string& runName, int m_min, int m_max, double pmin, double pmax) {
//     TLegend *leg = new TLegend(0.43, 0.66, 0.85, 0.88);
//     leg->SetTextSize(0.045);
//     leg->SetLineWidth(0.0);
//     leg->SetFillStyle(0);
//     leg->AddEntry((TObject *)0, "LHC23zzh_pass4_small", "");
//     leg->AddEntry((TObject *)0, Form("train number : %s", runName.c_str()), "");
//     leg->AddEntry((TObject *)0, Form("FT0C %d#font[122]{-}%d %%", m_min, m_max), "");
//     leg->AddEntry((TObject *)0, "PbPb 5.36 TeV, |#it{y}| < 0.5", "");
//     leg->AddEntry((TObject *)0, Form("%.1lf < #it{p}_{T} < %.1lf GeV/#it{c}", pmin, pmax), "");
//     return leg;
// }

void drawInvMassLSM();

int main() {
  drawInvMassLSM();
}

void drawInvMassLSM(){
  // std::ifstream file("runlist.json");
  // json inputJson = json::parse(file);
  // // auto Runs = inputJson["runs"];
  // // auto Runs = inputJson["run"];
  // // auto Runs = inputJson["runWoInitial"];
  // // auto Runs = inputJson["singlerun"];
  // auto Runs = inputJson["local"];
  // // std::vector<std::string> runs;

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
    // //	centralitys
    // std::vector<int> m_min = runInfos["m_min"];
    // std::vector<int> m_max = runInfos["m_max"];
    // //	pt
    // std::vector<double> p_min = runInfos["p_min"];
    // std::vector<double> p_max = runInfos["p_max"]

    std::vector<int> m_min;
    for (auto& v : runInfos["m_min"].GetArray()) m_min.push_back(v.GetInt());

    std::vector<int> m_max;
    for (auto& v : runInfos["m_max"].GetArray()) m_max.push_back(v.GetInt());
    std::cout << "m_max: ";
    for (const auto& val : m_max) std::cout << val << " ";
    std::cout << std::endl;

    std::vector<double> p_min;
    for (auto& v : runInfos["p_min"].GetArray()) p_min.push_back(v.GetDouble());

    std::vector<double> p_max;
    for (auto& v : runInfos["p_max"].GetArray()) p_max.push_back(v.GetDouble());
    std::cout << "p_max: ";
    for (const auto& val : p_max) std::cout << val << " ";
    std::cout << std::endl;


    // TFile *fin = new TFile((DIRECTORY + runName + results).c_str(), "READ");
    TFile *fin = TFile::Open((DIRECTORY + runName + results).c_str(), "READ");
    if (!fin || fin->IsZombie()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    TH1D* hProjInvMassSub[nmult][npt];
    // TH1D* hProjInvMassNSub[nmult][npt];
    

    for (int j = 0; j < nmult; j++) {
      // TCanvas *c = new TCanvas(Form("c_%d_%d_%d", i, r, j), Form("Canvas
      // %d_%d_%d", i, r, j), 1000, 1000);
      // TCanvas *c = new TCanvas(Form("c_%d", j), Form("Canvas %d", j), 2400, 1200);
      TCanvas *c = new TCanvas(Form("c_%d", j), Form("Canvas %d", j), 6*200, (npt+5)/6*160);
      c->Divide(6, (npt+5)/6, 0.001, 0.001);
      c->SetGrid();

      // TCanvas *nc = new TCanvas(Form("nc_%d", j), Form("NCanvas %d", j), 6*200, (npt+5)/6*160);
      // nc->Divide(6, (npt+5)/6, 0.001, 0.001);
      // nc->SetGrid();

      for (int k = 0; k < npt; k++) {
        const int cIdx = k + 1;
        c->cd(cIdx);
        gStyle->SetOptTitle(0);
        gStyle->SetOptStat(0);

        hProjInvMassSub[j][k] = (TH1D*)fin->Get(Form("f0980_SUB_EPA/hProjInvMassSub_%d_%d", j, k));
        // hProjInvMassNSub[j][k] = (TH1D*)fin->Get(Form("f0980_NSub_EPA/hProjInvMassNSub_%d_%d", j, k));
        //hProjInvMassSub[j][k] = (TH1D*)fin->Get("f0980_SUB_EPA")->Get(Form("hProjInvMassSub_%d_%d", j, k));
        if (!hProjInvMassSub[j][k]) {
          std::cerr << "Error opening histogram." << std::endl;
          return;
        }
        // if (!hProjInvMassNSub[j][k]) {
        //   std::cerr << "Error opening N histogram." << std::endl;
        //   return;
        // }

        if (colName == "pp") {
            hProjInvMassSub[j][k]->Rebin(2); // 2 : 5 MeV
        }
        else if (colName == "pbpb_kaon") {
            hProjInvMassSub[j][k]->Rebin(2); // 2 : 2 MeV
        }

        hProjInvMassSub[j][k]->SetLineWidth(1);
        hProjInvMassSub[j][k]->Draw("");
        hProjInvMassSub[j][k]->GetXaxis()->SetTitle("M_{#pi#pi} (GeV/c^{2})");
        hProjInvMassSub[j][k]->GetXaxis()->SetRangeUser(0.99, 1.08); // mass range

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
        // TLegend* leg = MakeLegend(runName, m_min[j], m_max[j], p_min[k], p_max[k]);
        // leg->Draw();


        // nc->cd(cIdx);
        // gStyle->SetOptTitle(0);
        // gStyle->SetOptStat(0);

        // hProjInvMassNSub[j][k]->SetLineWidth(1);
        // hProjInvMassNSub[j][k]->Draw("");
        // hProjInvMassNSub[j][k]->GetXaxis()->SetTitle("M_{#pi#pi} (GeV/c^{2})");
        // hProjInvMassNSub[j][k]->GetXaxis()->SetRangeUser(0.99, mass_max);

        // TLegend* leg_nc = MakeLegend(runName, m_min[j], m_max[j], p_min[k], p_max[k]);
        // leg_nc->Draw();
      }
      // TString fileName = Form("plot/Invmass_mult_%d_%d.pdf", m_min[j], m_max[j]); 
      TString fileName = Form((DIRECTORY + runName + "/" + "plot_c/InvmassLSM_mult_%d_%d.pdf").c_str(), m_min[j], m_max[j]);
      // TString fileName = Form((DIRECTORY + runName + "/" + "plot_c/rebin_Invmass_mult_%d_%d.pdf").c_str(), m_min[j], m_max[j]);
      std::cout << (DIRECTORY + runName + "/" + "plot_c").c_str() << std::endl;
      c->SaveAs(fileName);

      // TString fileName_nc = Form((DIRECTORY + runName + "/" + "plot_c/InvmassNLSM_mult_%d_%d.pdf").c_str(), m_min[j], m_max[j]);
      // // TString fileName = Form((DIRECTORY + runName + "/" + "plot_c/rebin_Invmass_mult_%d_%d.pdf").c_str(), m_min[j], m_max[j]);
      // std::cout << (DIRECTORY + runName + "/" + "plot_c").c_str() << std::endl;
      // nc->SaveAs(fileName_nc);

      delete c;
      // delete nc;
    }
    fin->Close();
    delete fin;
  }
}


