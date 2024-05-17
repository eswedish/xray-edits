//combines different threads together into a new combined file

void mt_combiner()
  {
    gROOT->Reset();

    int i, n, fEvent, fParentID, fTrackID, fProcess;
    double fX, fY, fZ, time, dedep, fEdep;
    double totEdep_vac, totEdep_air1, totEdep_fl, totEdep_air2, totEdep_di, totEdep_air3, totEdep_ff, totEdep_samp, totEdep_bp, totEdep_ld, totEdep, totEdep_cr, totEdep_cu; //totEdep_ic
    double vac_start, vac_air1, air1_cu, cu_di, di_cr, cr_air2, air2_fl, fl_air3, air3_samp, samp_bp, bp_ld, ld_end; //air1_ic ic_cr ic_air2 di_air2
    //double x[300000], y[300000], z[300000];
    double dedp[1000000], dedpc[300000], dedpff[300000], dedps[300000], dedpbp[300000], dedpld[300000];
    double geom[20], geom_conv[20];
    string geom_inpt;
    //double xc[300000], yc[300000], zc[300000], xd[300000], yd[300000], zd[300000];

    // initiate a TChain w/ name of TTree inside we want to proccess
    TChain photon_chain("Photons");
    photon_chain.Add("output_diode*.root"); // wildcard treatment -- ie doing all the files w this beginning (all the threads)
    
    //doing same as ^^^^ but for fEdep
    TChain scoring("Scoring");
    scoring.Add("output_diode*.root");

    // define variables from tree and assign to respective branches
    photon_chain.SetBranchAddress("fEvent", &fEvent);
    photon_chain.SetBranchAddress("fX", &fX);
    photon_chain.SetBranchAddress("fY", &fY);
    photon_chain.SetBranchAddress("fZ", &fZ);
    photon_chain.SetBranchAddress("dedep", &dedep);
    photon_chain.SetBranchAddress("fParentID", &fParentID);
    photon_chain.SetBranchAddress("fTrackID", &fTrackID);
    photon_chain.SetBranchAddress("fProcess", &fProcess);
    
    scoring.SetBranchAddress("fEdep", &fEdep);


    photon_chain.Merge("outputsum.root");

    // for each one ie a. vacuum (0,30) b. air1 (35,99.75) c. copper () d. diode (99.75,100.25) e. ceramic ()
    // f. air 2 (100.25,270) g. filter (270, 272) h. air3 (272, 289.05) i. finalfilt (294, 295) - this is air :)
    // j. sample (295, 300) k. base plate (300, 320) l is tot
    totEdep_vac=0.0;
    totEdep_air1=0.0;
    totEdep_fl=0.0; 
    totEdep_cu=0.0; //copper filter
    totEdep_cr=0.0; //ceramic
    totEdep_air2=0.0;
    totEdep_di=0.0; //diode
    totEdep_air3=0.0;
    totEdep_ff=0.0;
    totEdep_samp=0.0;
    totEdep_bp=0.0;
    totEdep_ld=0.0;
    totEdep=0.0;

    n = 0;
    FILE * file = fopen("geometry.tsv", "r");
    for (n=0; n<12; n++)
    {
      fscanf(file, "%*s %lf\n", &geom[n]);
      geom_conv[n] = -geom[n]*1000;
      printf("geom[%i]=%lf\n",n,geom_conv[n]);
    }
    fclose(file);

    vac_start = geom_conv[0];
    vac_air1  = geom_conv[1];
    air1_cu   = geom_conv[2];
    cu_di     = geom_conv[3];
    di_cr     = geom_conv[4];
    cr_air2   = geom_conv[5];
    air2_fl   = geom_conv[6];
    fl_air3   = geom_conv[7];
    air3_samp = geom_conv[8]; 
    samp_bp   = geom_conv[9]; 
    bp_ld     = geom_conv[10]; 
    ld_end    = geom_conv[11]; 
    
    //Histogram setup:
    // for each of above, do dEdep, Z dist w dEdep weight, XY weighted energy, now also fEdep

    // a) Vacuum 
    TH1F *h1 = new TH1F("h1","dEdep in Vac", 250, 0.0, 0.025);
    TH1F *h2 = new TH1F("h2","Z Dist w/ dEdep Weighting in Vac", 150, vac_start, vac_air1);
    TH2D *h3 = new TH2D("h3","XY Gamma weighted energy in Vac", 300, -30.0, 30.0, 300, -30.0, 30.0);
    TH2D *h4 = new TH2D("h4","XY e- weighted energy in Vac", 300, -30.0, 30.0, 300, -30.0, 30.0);

    // b) Air Gap 1
    TH1F *h11 = new TH1F("h11","dEdep in Air Gap 1", 250, 0.0, 0.025);
    TH1F *h12 = new TH1F("h12","Z Dist w/ dEdep Weighting in Air Gap 1", 118, vac_air1, air1_cu);
    TH2D *h13 = new TH2D("h13","XY weighted energy in Air Gap 1", 300, -30.0, 30.0, 300, -30.0, 30.0);
    TH2D *h14 = new TH2D("h14","XY e- weighted energy in Air Gap 1", 300, -30.0, 30.0, 300, -30.0, 30.0);

    // c) Copper 1mm
    TH1F *h21 = new TH1F("h21","dEdep in Copper Filter", 250, 0.0, 0.025);
    TH1F *h22 = new TH1F("h22","Z Dist w/ dEdep Weighting in Copper Filter", 120, air1_cu, cu_di); 
    TH2D *h23 = new TH2D("h23","XY weighted energy in Copper Filter", 300, -30.0, 30.0, 300, -304.0, 30.0);
    TH2D *h24 = new TH2D("h24","XY e- weighted energy in Copper Filter", 300, -30.0, 30.0, 300, -30.0, 30.0);

    // d) Diode
    TH1F *h31 = new TH1F("h31","dEdep in Diode", 250, 0.0, 0.025);
    TH1F *h32 = new TH1F("h32","Z Dist w/ dEdep Weighting in Diode", 120, cu_di, di_cr); 
    TH2D *h33 = new TH2D("h33","XY weighted energy in Diode", 300, -30.0, 30.0, 300, -304.0, 30.0);
    TH2D *h34 = new TH2D("h34","XY e- weighted energy in Diode", 300, -30.0, 30.0, 300, -30.0, 30.0);
    TH1F *h35 = new TH1F("h35", "fEdep in Diode", 250, 0.0, 0.025);

    //e) Ceramic
    TH1F *h41 = new TH1F("h41","dEdep in Ceramic", 250, 0.0, 0.025);
    TH1F *h42 = new TH1F("h42","Z Dist w/ dEdep Weighting in Ceramic", 120 , di_cr, cr_air2);
    TH2D *h43 = new TH2D("h43","XY weighted energy in Ceramic", 300, -30.0, 30.0, 300, -30.0, 30.0);
    TH2D *h44 = new TH2D("h44","XY e- weighted energy in Ceramic", 300, -30.0, 30.0, 300, -30.0, 30.0);

    // f) Air Gap 2
    TH1F *h51 = new TH1F("h51","dEdep in Air Gap 2", 250, 0.0, 0.025);
    TH1F *h52 = new TH1F("h52","Z Dist w/ dEdep Weighting in Air Gap 2", 164, cr_air2, air2_fl); 
    TH2D *h53 = new TH2D("h53","XY weighted energy in Air Gap 2", 300, -30.0, 30.0, 300, -30.0, 30.0);
    TH2D *h54 = new TH2D("h54","XY e- weighted energy in Air Gap 2", 300, -30.0, 30.0, 300, -30.0, 30.0);

    // g) Filter
    TH1F *h61 = new TH1F("h61","dEdep in Filter", 250, 0.0, 0.025);
    TH1F *h62 = new TH1F("h62","Z Dist w/ dEdep Weighting in Filter", 200, air2_fl, fl_air3);
    TH2D *h63 = new TH2D("h63","XY weighted energy in Filter", 300, -30.0, 30.0, 300, -30.0, 30.0);
    TH2D *h64 = new TH2D("h64","XY e- weighted energy in Filter", 300, -30.0, 30.0, 300, -30.0, 30.0);

    // h) Air Gap 3
    TH1F *h71 = new TH1F("h71","dEdep in Air Gap 3", 250, 0.0, 0.025);
    TH1F *h72 = new TH1F("h72","Z Dist w/ dEdep Weighting in Air Gap 3", 170, fl_air3, air3_samp);
    TH2D *h73 = new TH2D("h73","XY weighted energy in Air Gap 3", 300, -30.0, 30.0, 300, -30.0, 30.0);
    TH2D *h74 = new TH2D("h74","XY e- weighted energy in Air Gap 3", 300, -30.0, 30.0, 300, -30.0, 30.0);

    // i) Sample
    TH1F *h81 = new TH1F("h81","dEdep in Sample", 300, 0.0, 0.03);
    TH1F *h82 = new TH1F("h82","Z Dist w/ dEdep Weighting in Sample", 200, air3_samp, samp_bp);
    TH2D *h83 = new TH2D("h83","XY weighted energy in Sample", 300, -30.0, 30.0, 300, -30.0, 30.0);
    TH2D *h84 = new TH2D("h84","XY weighted energy in Sample", 300, -30.0, 30.0, 300, -30.0, 30.0);

    // j) Base Plate
    TH1F *h91 = new TH1F("h91","dEdep in BP", 300, 0.0, 0.03);
    TH1F *h92 = new TH1F("h92","Z Dist w/ dEdep Weighting in BP", 115, samp_bp, bp_ld);
    TH2D *h93 = new TH2D("h93","XY weighted energy in BP", 300, -30.0, 30.0, 300, -30.0, 30.0);
    TH2D *h94 = new TH2D("h94","XY weighted energy in BP", 300, -30.0, 30.0, 300, -30.0, 30.0);

    // k) Lead
    TH1F *h101 = new TH1F("h101","dEdep in Lead", 300, 0.0, 0.03);
    TH1F *h102 = new TH1F("h102","Z Dist w/ dEdep Weighting in Lead", 130, bp_ld, ld_end);
    TH2D *h103 = new TH2D("h103","XY weighted energy in Lead", 300, -30.0, 30.0, 300, -30.0, 30.0);
    TH2D *h104 = new TH2D("h104","XY weighted energy in Lead", 300, -30.0, 30.0, 300, -30.0, 30.0);

    // l) Total
    TH1F *h111 = new TH1F("h111","dEdep Tot", 300, 0.0, 0.03);
    TH1F *h112 = new TH1F("h112","Z Dist w/ dEdep Weighting Tot", 170, vac_start, ld_end);
    TH2D *h113 = new TH2D("h113","XY gamma weighted energy Tot", 300, -30.0, 30.0, 300, -30.0, 30.0);
    TH2D *h114 = new TH2D("h114","XY e- weighted energy Tot", 300, -30.0, 30.0, 300, -30.0, 30.0);

    //dividing it up into in chamber v Sampleor
    for (i=0; i<photon_chain.GetEntries(); i++)
    {
      photon_chain.GetEntry(i);
      if (dedep !=0)
      {
        // a) Vacuum
        if ((fZ >= vac_start) && (fZ< vac_air1))
        {
          if (((-250< fX) && (fX< 250)) && ((-250 < fY) && (fY < 250)))
            {
              h1->Fill(dedep);
              h2->Fill(fZ, dedep);
              h3->Fill(fX, fY, dedep);
              totEdep_vac = totEdep_vac + dedep;
            }
        }

        // b) Air Gap 1
        if ((fZ >= vac_air1) && (fZ< air1_cu))
        {
          if (((-250< fX) && (fX< 250)) && ((-250 < fY) && (fY < 250)))
            {
              h11->Fill(dedep);
              h12->Fill(fZ, dedep);
              h13->Fill(fX, fY, dedep);
              totEdep_air1 = totEdep_air1 + dedep;
            }
        }

        // c) Copper Filter
        if ((fZ >= air1_cu) && (fZ< cu_di))
        {
          if (((-20< fX) && (fX< 20)) && ((-20 < fY) && (fY < 20)))
            {
              h21->Fill(dedep);
              h22->Fill(fZ, dedep);
              h23->Fill(fX, fY, dedep);
              totEdep_cu = totEdep_cu + dedep;
            }
        }

        // c) -- now Diode :)
        if ((fZ >= cu_di) && (fZ< di_cr)) 
        {
          if (((-20< fX) && (fX< 20)) && ((-20 < fY) && (fY < 20)))
            {
              h31->Fill(dedep);
              h32->Fill(fZ, dedep);
              h33->Fill(fX, fY, dedep);
              totEdep_di = totEdep_di + dedep;
            }
        }

        // d) Ceramic
        if ((fZ >= di_cr) && (fZ< cr_air2))
        {
          if (((-20< fX) && (fX< 20)) && ((-20 < fY) && (fY < 20)))
            {
              h41->Fill(dedep);
              h42->Fill(fZ, dedep);
              h43->Fill(fX,fY, dedep);
              totEdep_cr = totEdep_cr + dedep;
            }
        }

        // e) Air Gap 2 
        if ((fZ >= cr_air2) && (fZ< air2_fl))
        {
          if (((-25< fX) && (fX< 25)) && ((-25 < fY) && (fY < 25)))
            {
              h51->Fill(dedep);
              h52->Fill(fZ, dedep);
              h53->Fill(fX, fY, dedep);
              totEdep_air2 = totEdep_air2 + dedep;
            }
        }

        // f) Filter 
        if ((fZ >= air2_fl) && (fZ< fl_air3))
        {
          if (sqrt((fX*fX)+(fY*fY)) <= (12.26))
            {
              h61->Fill(dedep);
              h62->Fill(fZ, dedep);
              h63->Fill(fX, fY, dedep);
              totEdep_fl = totEdep_fl + dedep;
            }
        }

        // g) Air Gap 3
        if ((fZ >= fl_air3) && (fZ< air3_samp))
        {
          if (((-25< fX) && (fX< 25)) && ((-25 < fY) && (fY < 25)))
            {
              h71->Fill(dedep);
              h72->Fill(fZ, dedep);
              h73->Fill(fX, fY, dedep);
              totEdep_air3 = totEdep_air3 + dedep;
            }
        }

        // h) Sample 
        if ((fZ >= air3_samp) && (fZ< samp_bp))
        {
          // if (((-25< fX) && (fX< 25)) && ((-25 < fY) && (fY < 25))) // used to be limit 4.63 both
          if (sqrt((fX*fX)+(fY*fY)) <= (13.12)) // x^2+y^2<=r^2 with r^2 z^2tan^2(half ang)
            {
              h81->Fill(dedep);
              h82->Fill(fZ, dedep);
              if (fParentID == 0)
              {
                h83->Fill(fX, fY, dedep);
              }
              if (fParentID != 0) // 1 obv e-, but also 2 3 4 ... all come frm 1
              {
                h84->Fill(fX, fY, dedep);
              }
              totEdep_samp = totEdep_samp + dedep;
            }
        }

        // i) Base Plate
        if ((fZ >= samp_bp) && (fZ< bp_ld))
        {
          if (((-100< fX) && (fX< 100)) && ((-100 < fY) && (fY < 100))) // used to be limit 4.63 both
            {
              h91->Fill(dedep);
              h92->Fill(fZ, dedep);
              h93->Fill(fX, fY, dedep);
              totEdep_bp = totEdep_bp + dedep;
            }
        }

        // j) NEW LEAD 
        if ((fZ >= bp_ld) && (fZ< ld_end))
        {
          if (((-200< fX) && (fX< 200)) && ((-200 < fY) && (fY < 200))) // used 20 cm -- current world xy
            {
              h101->Fill(dedep);
              h102->Fill(fZ, dedep);
              h103->Fill(fX, fY, dedep);
              totEdep_ld = totEdep_ld + dedep;
            }
        }

        // k) Total 
        if ((fZ >= vac_start) && (fZ< ld_end))
        {
          if (((-200< fX) && (fX< 200)) && ((-200 < fY) && (fY < 200))) // used 20 cm -- current world xy
            {
              h111->Fill(dedep);
              h112->Fill(fZ, dedep);
              h113->Fill(fX, fY, dedep);
              totEdep = totEdep + dedep;
            }
        }


      }
    }
    
    //same process as above but for fEdep
    for (i=0; i<scoring.GetEntries(); i++)
    {
      scoring.GetEntry(i);
      if(fEdep != 0)
      {
        //c) diode
        if ((fZ >= cu_di) && (fZ< di_cr))
        {
          if (((-20< fX) && (fX< 20)) && ((-20 < fY) && (fY < 20)))
            {
              h35->Fill(fEdep);
            }
        }
       }
     }


    cout << "e vac:" << totEdep_vac << endl;
    cout << "e air 1: " << totEdep_air1 << endl;
    cout << "e cu: " << totEdep_cu << endl;
    cout << "e diode: " << totEdep_di << endl;
    cout << "e ceramic: " << totEdep_cr << endl;
    cout << "e air 2:" << totEdep_air2 << endl;
    cout << "e filter:" << totEdep_fl << endl;
    cout << "e air 3:" << totEdep_air3 << endl;
    cout << "e sample:" << totEdep_samp << endl;
    cout << "e bp:"<< totEdep_bp << endl;
    cout << "e ld:" << totEdep_ld << endl;
    cout << "tot e:" << totEdep << endl;


    TFile *fout = new TFile("xray_diode.root", "RECREATE");

    TCanvas *c0 = new TCanvas("Vac", "Vac");
    TCanvas *c1 = new TCanvas("Air Gap 1", "Air Gap 1");
    TCanvas *c2 = new TCanvas("Copper Filter", "Copper Filter");
    TCanvas *c3 = new TCanvas("Diode", "Diode");
    TCanvas *c4 = new TCanvas("Ceramic", "Ceramic");
    TCanvas *c5 = new TCanvas("Air Gap 2", "Air Gap 2");
    TCanvas *c6 = new TCanvas("Filter", "Filter");
    TCanvas *c7 = new TCanvas("Air Gap 3", "Air Gap 3");
    TCanvas *c8 = new TCanvas("Sample", "Sample");
    TCanvas *c9 = new TCanvas("Base Plate", "Base Plate");
    TCanvas *c10 = new TCanvas("Lead", "Lead");
    TCanvas *c11 = new TCanvas("Total", "Total");

    fout->cd();

    //ROOT CANVASES 
    // a)
    c0->Divide(2,2);
    c0->cd(1);
    gPad->SetLogy(1);
    h1->Draw("HIST");
      h1->GetXaxis()->SetTitle("Deposited Energy over Step (MeV)");
      h1->GetYaxis()->SetTitle("Freq");
      h1->GetXaxis()->CenterTitle();
      h1->GetYaxis()->CenterTitle();
    c0->cd(2);
    h2->Draw("HIST");
      h2->GetXaxis()->SetTitle("-Z Dist (mm)");
      h2->GetYaxis()->SetTitle("Energy deposited at position (MeV)");
      h2->GetXaxis()->CenterTitle();
      h2->GetYaxis()->CenterTitle();
    c0->cd(3);
    h3->Draw("HIST");
    c0->Write();

    // b)
    c1->Divide(2,2);
    c1->cd(1);
    gPad->SetLogy(1);
    h11->Draw("HIST");
      h11->GetXaxis()->SetTitle("Deposited Energy over Step (MeV)");
      h11->GetYaxis()->SetTitle("Freq");
      h11->GetXaxis()->CenterTitle();
      h11->GetYaxis()->CenterTitle();
    c1->cd(2);
    h12->Draw("HIST");
      h12->GetXaxis()->SetTitle("-Z Dist (mm)");
      h12->GetYaxis()->SetTitle("Energy deposited at position (MeV)");
      h12->GetXaxis()->CenterTitle();
      h12->GetYaxis()->CenterTitle();
    c1->cd(3);
    h13->Draw("HIST");
    c1->Write();

    // c)
    c2->Divide(2,2);
    c2->cd(1);
    gPad->SetLogy(1);
    h21->Draw("HIST");
      h21->GetXaxis()->SetTitle("Deposited Energy over Step (MeV)");
      h21->GetYaxis()->SetTitle("Freq");
      h21->GetXaxis()->CenterTitle();
      h21->GetYaxis()->CenterTitle();
    c2->cd(2);
    h22->Draw("HIST");
      h22->GetXaxis()->SetTitle("-Z Dist (mm)");
      h22->GetYaxis()->SetTitle("Energy deposited at position (MeV)");
      h22->GetXaxis()->CenterTitle();
      h22->GetYaxis()->CenterTitle();
    c2->cd(3);
    h23->Draw("HIST");
    c2->Write();

    // c)
    c3->Divide(2,2);
    c3->cd(1);
    gPad->SetLogy(1);
    h31->Draw("HIST");
      h31->GetXaxis()->SetTitle("Deposited Energy over Step (MeV)");
      h31->GetYaxis()->SetTitle("Freq");
      h31->GetXaxis()->CenterTitle();
      h31->GetYaxis()->CenterTitle();
    c3->cd(2);
    h32->Draw("HIST");
      h32->GetXaxis()->SetTitle("-Z Dist (mm)");
      h32->GetYaxis()->SetTitle("Energy deposited at position (MeV)");
      h32->GetXaxis()->CenterTitle();
      h32->GetYaxis()->CenterTitle();
    c3->cd(4);
    h33->Draw("HIST");
    c3->cd(3);
    h35->Draw("HIST");
      h35->GetXaxis()->SetTitle("Deposited Energy by Event (MeV)");
      h35->GetYaxis()->SetTitle("Freq");
      h35->GetXaxis()->CenterTitle();
      h35->GetYaxis()->CenterTitle();
    c3->Write();

    //d) ceramic
    c4->Divide(2,2);
    c4->cd(1);
    gPad->SetLogy(1);
    h41->Draw("HIST");
      h41->GetXaxis()->SetTitle("Deposited Energy over Step (MeV)");
      h41->GetYaxis()->SetTitle("Freq");
      h41->GetXaxis()->CenterTitle();
      h41->GetYaxis()->CenterTitle();
    c4->cd(2);
    h42->Draw("HIST");
      h42->GetXaxis()->SetTitle("-Z Dist (mm)");
      h42->GetYaxis()->SetTitle("Energy deposited at position (MeV)");
      h42->GetXaxis()->CenterTitle();
      h42->GetYaxis()->CenterTitle();
    c4->cd(3);
    h43->Draw("HIST");
    c4->Write();

    // e) 
    c5->Divide(2,2);
    c5->cd(1);
    gPad->SetLogy(1);
    h51->Draw("HIST");
      h51->GetXaxis()->SetTitle("Deposited Energy over Step (MeV)");
      h51->GetYaxis()->SetTitle("Freq");
      h51->GetXaxis()->CenterTitle();
      h51->GetYaxis()->CenterTitle();
    c5->cd(2);
    h52->Draw("HIST");
      h52->GetXaxis()->SetTitle("-Z Dist (mm)");
      h52->GetYaxis()->SetTitle("Energy deposited at position (MeV)");
      h52->GetXaxis()->CenterTitle();
      h52->GetYaxis()->CenterTitle();
    c5->cd(3);
    h53->Draw("HIST");
    c5->Write();

    // f)
    c6->Divide(2,2);
    c6->cd(1);
    gPad->SetLogy(1);
    h61->Draw("HIST");
      h61->GetXaxis()->SetTitle("Deposited Energy over Step (MeV)");
      h61->GetYaxis()->SetTitle("Freq");
      h61->GetXaxis()->CenterTitle();
      h61->GetYaxis()->CenterTitle();
    c6->cd(2);
    h62->Draw("HIST");
      h62->GetXaxis()->SetTitle("-Z Dist (mm)");
      h62->GetYaxis()->SetTitle("Energy deposited at position (MeV)");
      h62->GetXaxis()->CenterTitle();
      h62->GetYaxis()->CenterTitle();
    c6->cd(3);
    h63->Draw("HIST");
    c6->Write();

    // g)
    c7->Divide(2,2);
    c7->cd(1);
    gPad->SetLogy(1);
    h71->Draw("HIST");
      h71->GetXaxis()->SetTitle("Deposited Energy over Step (MeV)");
      h71->GetYaxis()->SetTitle("Freq");
      h71->GetXaxis()->CenterTitle();
      h71->GetYaxis()->CenterTitle();
    c7->cd(2);
    h72->Draw("HIST");
      h72->GetXaxis()->SetTitle("-Z Dist (mm)");
      h72->GetYaxis()->SetTitle("Energy deposited at position (MeV)");
      h72->GetXaxis()->CenterTitle();
      h72->GetYaxis()->CenterTitle();
    c7->cd(3);
    h73->Draw("HIST"); 
    c7->Write();

    // h)
    c8->Divide(2,2);
    c8->cd(1);
    gPad->SetLogy(1);
    h81->Draw("HIST");
      h81->GetXaxis()->SetTitle("Deposited Energy over Step (MeV)");
      h81->GetYaxis()->SetTitle("Freq");
      h81->GetXaxis()->CenterTitle();
      h81->GetYaxis()->CenterTitle();
    c8->cd(2);
    h82->Draw("HIST");
      h82->GetXaxis()->SetTitle("-Z Dist (mm)");
      h82->GetYaxis()->SetTitle("Energy deposited at position (MeV)");
      h82->GetXaxis()->CenterTitle();
      h82->GetYaxis()->CenterTitle();
    c8->cd(3);
    h83->Draw("HIST");
    c8->Write();

    // i)
    c9->Divide(2,2);
    c9->cd(1);
    gPad->SetLogy(1);
    h91->Draw("HIST");
      h91->GetXaxis()->SetTitle("Deposited Energy over Step (MeV)");
      h91->GetYaxis()->SetTitle("Freq");
      h91->GetXaxis()->CenterTitle();
      h91->GetYaxis()->CenterTitle();
    c9->cd(2);
    h92->Draw("HIST");
      h92->GetXaxis()->SetTitle("-Z Dist (mm)");
      h92->GetYaxis()->SetTitle("Energy deposited at position (MeV)");
      h92->GetXaxis()->CenterTitle();
      h92->GetYaxis()->CenterTitle();
    c9->cd(3);
    h93->SetMarkerColor(kRed);
    h93->Draw("HIST");
    h94->SetMarkerColor(kBlue);
    h94->Draw("HIST same");
    c9->Write();

    // i) lead
    c10->Divide(2,2);
    c10->cd(1);
    gPad->SetLogy(1);
    h101->Draw("HIST");
      h101->GetXaxis()->SetTitle("Deposited Energy over Step (MeV)");
      h101->GetYaxis()->SetTitle("Freq");
      h101->GetXaxis()->CenterTitle();
      h101->GetYaxis()->CenterTitle();
    c10->cd(2);
    h102->Draw("HIST");
      h102->GetXaxis()->SetTitle("-Z Dist (mm)");
      h102->GetYaxis()->SetTitle("Energy deposited at position (MeV)");
      h102->GetXaxis()->CenterTitle();
      h102->GetYaxis()->CenterTitle();
    c10->cd(3);
    h103->Draw("HIST");
    c10->Write();

    // j) total
    c11->Divide(2,2);
    c11->cd(1);
    gPad->SetLogy(1);
    h111->Draw("HIST");
      h111->GetXaxis()->SetTitle("Deposited Energy over Step (MeV)");
      h111->GetYaxis()->SetTitle("Freq");
      h111->GetXaxis()->CenterTitle();
      h111->GetYaxis()->CenterTitle();
    c11->cd(2);
    h112->Draw("HIST");
      h112->GetXaxis()->SetTitle("-Z Dist (mm)");
      h112->GetYaxis()->SetTitle("Energy deposited at position (MeV)");
      h112->GetXaxis()->CenterTitle();
      h112->GetYaxis()->CenterTitle();
    c11->cd(3);
    h113->Draw("HIST");
    c11->Write();

    fout->Close();

    }


