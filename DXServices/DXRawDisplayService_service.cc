// DXRawDisplayService_service.cc

// David Adams
// February 2016
//
// This service histograms of TPC raw data.
//
// Signal (i.e. channel-tick) histograms. These are recorded in eventE.
//   Notation:
//     E is the event ID
//     TTT denotes the track ID
//     AAA is an APA identifier, e.g. 0u or 3z2
//     hE_rawapaAAA -    Raw data for APA AAA
//       DoRawSignalHists - Make these histograms

#include "DXRawDisplayService.h"

#include <sstream>
#include <iomanip>

#include "lardata/RawData/RawDigit.h"
#include "lardata/RawData/raw.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalService.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalProvider.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"

// ROOT includes.
#include "TH2.h"
#include "TH1F.h"

// Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "cetlib/exception.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"

// Larsoft includes.
#include "larcore/Geometry/Geometry.h"

// Dune includes.
#include "dune/DuneInterface/ChannelMappingService.h"
#include "dune/DuneInterface/RawDigitPrepService.h"
#include "dune/DuneInterface/AdcSignalFindingService.h"

// Local includes.
#include "DXUtil/ChannelTickHistCreator.h"
#include "DXGeometry/GeoHelper.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::ostringstream;
using std::setw;
using lariov::ChannelStatusService;
using lariov::ChannelStatusProvider;
using raw::RawDigit;

//************************************************************************

namespace {

void fill2dhist(TH2* ph, double x, double y, double val, double err =-1.0) {
  int ibin = ph->FindBin(x, y);
  ph->SetBinContent(ibin, val);
  if ( err > 0.0 ) ph->SetBinError(ibin, err);
}

}  // end unnamed namespace

//************************************************************************

DXRawDisplayService::DXRawDisplayService(const fhicl::ParameterSet& pset)
: m_LogLevel(1), m_NEventsProcessed(0), m_NDigitsProcessed(0) {
  const string myname = "DXRawDisplayService::ctor: ";
  if ( m_LogLevel > 0 ) cout << myname << "Begin job." << endl;

  pset.get_if_present<int>("LogLevel", m_LogLevel);
  m_TdcTickMin             = pset.get<unsigned int>("TdcTickMin");
  m_TdcTickMax             = pset.get<unsigned int>("TdcTickMax");
  m_NTickPerBin            = pset.get<unsigned int>("NTickPerBin");
  m_NChanPerBin            = pset.get<unsigned int>("NChanPerBin");
  m_DoROPs                 = pset.get<bool>("DoROPs");
  m_DoAll                  = pset.get<bool>("DoAll");
  m_DoAllOnline            = pset.get<bool>("DoAllOnline");
  m_DoAllFlag              = pset.get<bool>("DoAllFlag");
  m_DoZSROPs               = pset.get<bool>("DoZSROPs");
  m_DoMean                 = pset.get<bool>("DoMean");
  m_NchanMeanRms           = pset.get<int>("NchanMeanRms");
  m_UseChannelMap          = pset.get<bool>("UseChannelMap");
  m_BadChannelFlag         = pset.get<int>("BadChannelFlag");
  m_SkipStuckBits          = pset.get<bool>("SkipStuckBits");
  m_MaxEventsLog           = pset.get<int>("MaxEventsLog");
  m_MaxDigitsLog           = pset.get<int>("MaxDigitsLog");
  art::ServiceHandle<geo::Geometry> geosvc;       // pointer to Geometry service
  m_pgh = new GeoHelper(&*geosvc, true, 0);
  if ( m_LogLevel > 0 ) cout << myname << "Fetched geometry helper." << endl;

  if ( m_LogLevel > 0 ) {
    cout << myname << "                LogLevel: " << m_LogLevel << endl;
    cout << myname << "              TdcTickMin: " << m_TdcTickMin << endl;
    cout << myname << "              TdcTickMax: " << m_TdcTickMax << endl;
    cout << myname << "             NTickPerBin: " << m_NTickPerBin << endl;
    cout << myname << "             NChanPerBin: " << m_NChanPerBin << endl;
    cout << myname << "                  DoROPs: " << m_DoROPs << endl;
    cout << myname << "                   DoAll: " << m_DoAll << endl;
    cout << myname << "             DoAllOnline: " << m_DoAllOnline << endl;
    cout << myname << "               DoAllFlag: " << m_DoAllFlag << endl;
    cout << myname << "                DoZSROPs: " << m_DoZSROPs << endl;
    cout << myname << "                  DoMean: " << m_DoMean << endl;
    cout << myname << "            NchanMeanRms: " << m_NchanMeanRms << endl;
    cout << myname << "           UseChannelMap: " << m_UseChannelMap << endl;
    cout << myname << "          BadChannelFlag: " << m_BadChannelFlag << endl;
    cout << myname << "           SkipStuckBits: " << m_SkipStuckBits << endl;
    cout << myname << "            MaxEventsLog: " << m_MaxEventsLog << endl;
    cout << myname << "            MaxDigitsLog: " << m_MaxDigitsLog << endl;
  }
}

//************************************************************************

DXRawDisplayService::DXRawDisplayService(const fhicl::ParameterSet& pset, art::ActivityRegistry&)
: DXRawDisplayService(pset) { }

//************************************************************************

DXRawDisplayService::~DXRawDisplayService() {
  const string myname = "DXRawDisplayService::dtor: ";
  if ( m_LogLevel > 0 ) {
    cout << myname << "Processed event count: " << m_NEventsProcessed << endl;
    cout << myname << "Processed digit count: " << m_NDigitsProcessed << endl;
  }
}
   
//************************************************************************

int DXRawDisplayService::process(const vector<RawDigit>& digs, const art::Event* pevt) const {
  const string myname = "DXRawDisplayService::process: ";
  int dbg = m_LogLevel;
  if ( dbg > 1 ) cout << myname << "Processed event count: " << m_NEventsProcessed << endl;
  if ( m_NEventsProcessed > m_MaxEventsLog && dbg > 1 ) {
    cout << myname << "MaxEventsLog limit reached. Increase this parameter to log more events." << endl;
    dbg = 1;
  }
  ++m_NEventsProcessed;

  // Fetch TFileService.
  art::ServiceHandle<art::TFileService> tfsHandle;
  art::TFileService* ptfs = &*tfsHandle;

  // Fetch the offline/online channel map.
  const ChannelMappingService* pchanmap = nullptr;
  if ( m_UseChannelMap ) {
    art::ServiceHandle<ChannelMappingService> hchanmap;
    pchanmap = &*hchanmap;
  }

  // Fetch the channel status service.
  const ChannelStatusProvider* pcsp = nullptr;
  if ( m_BadChannelFlag > 0 ) {
    art::ServiceHandle<ChannelStatusService> cssHandle;
    pcsp = cssHandle->GetProviderPtr();
    if ( pcsp == nullptr ) {
      cout << myname << "ERROR: Channel status service not found." << endl;
      abort();
    }
  }

  // Fetch the raw data prep service.
  const art::ServiceHandle<RawDigitPrepService> hrdp;

  // Build event string.
  string sevt;
  int wnam = 12;       // Base width for a histogram name.
  if ( pevt != nullptr ) {
    unsigned int event  = pevt->id().event(); 
    unsigned int run    = pevt->run();
    unsigned int subrun = pevt->subRun();
    if ( dbg > 1 ) cout << myname << "Processing run " << run << "-" << subrun
                         << ", event " << event << endl;
    ostringstream ssevt;
    ssevt << event;
    sevt = ssevt.str();
    wnam += 9;
  } else {
    if ( dbg > 1 ) cout << myname << "Processing without event ID." << endl;
  }

  // Create directory for event-level histograms.
  art::TFileDirectory tfsdir = ptfs->mkdir("event" + sevt);

  // Channel-tick histogram creators for the reconstructed data products.
  string ztitle = "ADC counts";
  double zmax = 100;
  int ncontour = 20;
  ChannelTickHistCreator hcreateRop(tfsdir, sevt, m_TdcTickMin, m_TdcTickMax, ztitle, -zmax, zmax, 2*ncontour, m_NTickPerBin, m_NChanPerBin);
  ChannelTickHistCreator hcreateRopMr(tfsdir, sevt, m_TdcTickMin, m_TdcTickMax, ztitle, -zmax, zmax, 2*ncontour, m_NchanMeanRms, 1);
  bool fAbsAll = false;
  string allname = "ADC counts";
  double allzmin = -zmax;
  if ( fAbsAll ) {
    allname = "|" + allname + "|";
    allzmin = 0.0;
  }
  ChannelTickHistCreator hcreateAll(tfsdir, sevt, m_TdcTickMin, m_TdcTickMax, allname, allzmin, zmax, 2*ncontour, m_NTickPerBin, m_NChanPerBin);
  ChannelTickHistCreator hcreateFlag(tfsdir, sevt, m_TdcTickMin, m_TdcTickMax, allname, 0.0, 8.0, 8, m_NTickPerBin, 1);

  // Check geometry helper.
  if ( m_pgh == nullptr ) {
    cout << myname << "ERROR: Geometry helper is absent." << endl;
    return 1;
  }
  const GeoHelper& geohelp = *m_pgh;
  int nchan = m_pgh->geometry()->Nchannels();

  // Create histograms.
  vector<TH2*> rophists;
  vector<TH2*> rophists_zs;
  if ( dbg > 2 ) cout << myname << "Creating raw data histograms." << endl;
  if ( m_DoROPs ) {
    for ( unsigned int irop=0; irop<geohelp.nrop(); ++irop ) {
      TH2* ph = hcreateRop.create("raw" + geohelp.ropName(irop), 0, geohelp.ropNChannel(irop),
                                      "Raw signals for " + geohelp.ropName(irop));
      if ( dbg > 3 ) cout << myname << "  " << ph->GetName() << endl;
      rophists.push_back(ph);
      m_eventhists.push_back(ph);
    }
  }
  if ( m_DoZSROPs ) {
    for ( unsigned int irop=0; irop<geohelp.nrop(); ++irop ) {
      TH2* ph = hcreateRop.create("rzs" + geohelp.ropName(irop), 0, geohelp.ropNChannel(irop),
                                      "Zero-suppressed raw signals for " + geohelp.ropName(irop));
      if ( dbg > 3 ) cout << myname << "  " << ph->GetName() << endl;
      rophists_zs.push_back(ph);
      m_eventhists.push_back(ph);
    }
  }
  TH2* phallraw = nullptr;
  if ( m_DoAll ) {
    phallraw = hcreateAll.create("rawall", 0, geohelp.geometry()->Nchannels(),
                                 "Raw signals for full detector");
    m_eventhists.push_back(phallraw);
  }
  TH2* phallrawon = nullptr;
  if ( m_DoAllOnline ) {
    phallrawon = hcreateAll.create("rawallon", 0, geohelp.geometry()->Nchannels(),
                                  "Online-ordered raw signals for full detector");
    m_eventhists.push_back(phallrawon);
  }
  TH2* phallflag = nullptr;
  if ( m_DoAllFlag ) {
    phallflag = hcreateFlag.create("rawflag", 0, geohelp.geometry()->Nchannels(),
                                   "Raw signal flags for full detector");
    m_eventhists.push_back(phallflag);
    phallflag->GetZaxis()->Set(8, 0.0, 8.0);
    phallflag->GetZaxis()->SetBinLabel(1,"OK");
    phallflag->GetZaxis()->SetBinLabel(2,"Under");
    phallflag->GetZaxis()->SetBinLabel(3,"Over");
    phallflag->GetZaxis()->SetBinLabel(4,"Bits0");
    phallflag->GetZaxis()->SetBinLabel(5,"Bits1");
    phallflag->GetZaxis()->SetBinLabel(6,"Fixed");
    phallflag->GetZaxis()->SetBinLabel(7,"Inter");
    phallflag->GetZaxis()->SetBinLabel(8,"Extra");
  }
  TH1* phmean = nullptr;
  if ( m_DoMean ) {
    string hname = "h" + sevt + "_rawmean";
    string htitle = "Mean ADC event " + sevt + ";Channel;ADC counts";
    phmean = tfsdir.make<TH1F>(hname.c_str(), htitle.c_str(), nchan, 0, nchan);
    m_eventhists.push_back(phmean);
  }
  vector<TH2*> rophists_mean;
  vector<TH2*> rophists_rms;
  if ( m_NchanMeanRms ) {
    for ( unsigned int irop=0; irop<geohelp.nrop(); ++irop ) {
      TH2* ph = hcreateRopMr.create("rawmean" + geohelp.ropName(irop), 0, geohelp.ropNChannel(irop),
                                    "Raw signal mean for " + geohelp.ropName(irop));
      rophists_mean.push_back(ph);
      m_eventhists.push_back(ph);
      if ( dbg > 3 ) cout << myname << "  " << ph->GetName() << endl;
      ph = hcreateRopMr.create("rawrms" + geohelp.ropName(irop), 0, geohelp.ropNChannel(irop),
                                    "Raw signal RMS for " + geohelp.ropName(irop));
      rophists_rms.push_back(ph);
      m_eventhists.push_back(ph);
      if ( dbg > 3 ) cout << myname << "  " << ph->GetName() << endl;
    }
  }

  // Prepare the raw data.
  AdcChannelDataMap prepdigs;

  if ( hrdp->prepare(digs, prepdigs) != 0 ) {
    cout << myname << "ERROR: Data prep failed!" << endl;
    return 1;
  }
  if ( dbg >=2 ) cout << myname << "Prepared digit count: " << prepdigs.size() << endl;

  // Fetch zero-supression service.
  const AdcSignalFindingService* psfs = nullptr;
  if ( m_DoZSROPs ) {
    psfs = &*art::ServiceHandle<AdcSignalFindingService>();
  }

  // Loop over digits.
  // Digit information is only logged for dbg >= 5.
  unsigned int idig = 0;
  // Flags that record how the digits are ordered.
  bool isOnlineOrdered = true;
  bool isOfflineOrdered = true;
  if ( dbg > 4 ) cout << myname << "Looping over digits." << endl;
  for ( AdcChannelDataMap::value_type chprepdig : prepdigs ) {
    ++m_NDigitsProcessed;
    // Extract and check prep data.
    AdcChannel ichan        =  chprepdig.first;
    AdcChannelData& prepdig = chprepdig.second;
    const AdcSignalVector& sigs =  prepdig.samples;
    const AdcFlagVector& flags  =  prepdig.flags;
    const raw::RawDigit& digit  = *prepdig.digit;
    unsigned int nsig = digit.Samples();
    if ( sigs.size() != nsig || flags.size() != nsig ) {
      cout << myname << "ERROR: Inconsistent array sizes in prep data." << endl;
    }
    // Determine if data is online or offline ordered.
    unsigned int ichanon = ichan;
    if ( pchanmap != nullptr ) {
      ichanon = pchanmap->online(ichan);
      isOnlineOrdered &= ichanon == idig;
      isOfflineOrdered &= ichan == idig;
    }
    // Skip bad channels.
    if ( m_BadChannelFlag == 1 ) {
      if ( pcsp->IsBad(ichanon) ) {
        if ( dbg >=2 ) cout << myname << "Skipping bad channel on/off = " << ichanon << "/" << ichan << endl;
        continue;
      }
    }
    if ( dbg > 5 ) cout << myname << "          Channel: " << ichan << endl;
    // Find the ROP.
    unsigned int irop = geohelp.channelRop(ichan);
    if ( dbg > 5 ) cout << myname << "              ROP: " << irop << endl;
    unsigned int iropchan = ichan - geohelp.ropFirstChannel(irop);
    if ( dbg > 5 ) cout << myname << "      ROP channel: " << iropchan << endl;
    // Set the ROP histogram pointers.
    TH2* ph = nullptr;
    TH2* phzs = nullptr;
    TH2* ph_mean = nullptr;
    TH2* ph_rms = nullptr;
    if ( rophists.size() > irop ) ph = rophists[irop];
    if ( rophists_zs.size() > irop ) phzs = rophists_zs[irop];
    if ( rophists_mean.size() > irop ) ph_mean = rophists_mean[irop];
    if ( rophists_rms.size() > irop ) ph_rms = rophists_rms[irop];
    if ( dbg >= 5 ) {
      cout << myname << "ROP hists for channel " << ichan << ":";
      if ( ph != nullptr ) cout << " " << ph->GetName();
      if ( ph_mean != nullptr ) cout << " " << ph_mean->GetName();
      if ( ph_rms != nullptr ) cout << " " << ph_rms->GetName();
      cout << endl;
    }
    // Apply zero suppression.
    if ( psfs != nullptr ) psfs->find(prepdig);
    // Loop over ticks.
    int icnt = 0;
    double tsum = 0.0;
    double tsumsq = 0.0;
    double tsum_bin = 0;
    double tsumsq_bin = 0.0;
    unsigned int ibin = 0;
    unsigned int ntick_bin = 0;
    unsigned int ntick_bin_nominal = 0;
    for ( unsigned int tick=0; tick<nsig; ++tick ) {
      double wt = sigs[tick];
      double wtzs = m_DoZSROPs ? prepdig.signal[tick]*wt : 0.0;
      ++icnt;
      tsum += wt;
      tsumsq += wt*wt;
      ++ntick_bin_nominal;
      if ( wt != 0 ) {
        if ( phallflag != nullptr )  fill2dhist(phallflag, tick, ichan, flags[tick]);
        bool isSticky = flags[tick] == AdcStuckOn || flags[tick] == AdcStuckOff;
        if ( !isSticky || !m_SkipStuckBits )  {
          bool isFixed = flags[tick] == AdcSetFixed;
          bool isInterpolated = flags[tick] == AdcInterpolated;
          bool isExtrapolated = flags[tick] == AdcExtrapolated;
          double err = 0.5;
          if ( isSticky ) err = 32.0;
          if ( isFixed ) err = 100.0;
          if ( isInterpolated ) err = 2.0;
          if ( isExtrapolated ) err = 5.0;
          if ( ph != nullptr ) fill2dhist(ph, tick, iropchan, wt, err);
          if ( phzs != nullptr ) fill2dhist(phzs, tick, iropchan, wtzs, err);
          double allwt = wt;
          if ( fAbsAll ) allwt = fabs(allwt);
          if ( phallraw != nullptr )   fill2dhist(phallraw, tick, ichan, allwt, err);
          if ( phallrawon != nullptr ) fill2dhist(phallrawon, tick, ichanon, allwt, err);
          tsum_bin += wt;
          tsumsq_bin += wt*wt;
          ++ntick_bin;
        }
      }
      if ( ph_mean != nullptr && ph_rms != nullptr &&
           (ntick_bin_nominal == m_NchanMeanRms || tick==nsig-1) ) {
        double mean = 0.0;
        double rms = 0.0;
        if ( ntick_bin > 0 ) {
          mean = tsum_bin/ntick_bin;
          rms = sqrt(tsumsq_bin/ntick_bin);
        }
        ph_mean->SetBinContent(ibin+1, iropchan+1, mean);
        ph_rms->SetBinContent(ibin+1, iropchan+1, rms);
        if ( dbg >= 5 ) {
          cout << myname << "Mean[" << ibin << ", " << iropchan << "] = " << mean << endl;
          cout << myname << " Rms[" << ibin << ", " << iropchan << "] = " << rms << endl;
        }
        tsum_bin = 0.0;
        tsumsq_bin = 0.0;
        ++ibin;
        ntick_bin = 0;
        ntick_bin_nominal = 0;
      }
    }  // end loop over ticks
    // Channel stats.
    double tcnt = icnt;
    double mean = -1.e6;
    double rms = 0.0;
    if ( tcnt > 0.0 ) {
      mean = tsum/tcnt;
      double rmssq = tsumsq/tcnt - mean*mean;
      if ( rmssq > 0.0 ) rms = sqrt(rmssq);
      if ( phmean != nullptr ) {
        phmean->SetBinContent(ichan+1, mean);
        phmean->SetBinError(ichan+1, rms);
      }
      if ( dbg > 4 ) cout << myname << "Digit channel " << ichan << " Mean, RMS: " << mean << " +/- " << rms << endl;
    }
    ++idig;
  }  // end loop over digits.

  if ( dbg > 4 ) cout << myname << "----------" << endl;
  if ( dbg > 2 && pchanmap != nullptr ) {
    if ( isOnlineOrdered )   cout << myname << "Digit order is online." << endl;
    if ( isOfflineOrdered )  cout << myname << "Digit order is offline." << endl;
    if ( !isOnlineOrdered &&
         !isOfflineOrdered ) cout << myname << "Digit order is neither online nor offline." << endl;
  }
  // Display the contents of each raw data histogram.
  if ( dbg >= 2 ) {
    cout << myname << "Summary of raw data histograms:" << endl;
    for ( TH2* ph : rophists ) {
      summarize2dHist(ph, myname, wnam, 4, 7);
    }
    for ( TH2* ph : rophists_zs ) {
      summarize2dHist(ph, myname, wnam, 4, 7);
    }
    for ( TH2* ph : rophists_mean ) {
      summarize2dHist(ph, myname, wnam, 4, 7);
    }
    for ( TH2* ph : rophists_rms ) {
      summarize2dHist(ph, myname, wnam, 4, 7);
    }
    if ( phallraw != nullptr ) summarize2dHist(phallraw, myname, wnam, 4, 7);
    if ( phallrawon != nullptr ) summarize2dHist(phallrawon, myname, wnam, 4, 7);
  }
  removeEventHists();
  return 0;
}

//************************************************************************

void DXRawDisplayService::
summarize2dHist(TH2* ph, string prefix,
                unsigned int wnam, unsigned int wbin, unsigned int went) const {
  cout << prefix << "  " << setw(wnam) << std::left << ph->GetName()
       << std::right << " bins=" << setw(wbin) << ph->GetNbinsY() << "x" << ph->GetNbinsX()
       << ", entries=" << setw(went) << ph->GetEntries()
       << ", integral=" << setw(went) << ph->Integral()
       << endl;
}

//************************************************************************

void DXRawDisplayService::removeEventHists() const {
  const string myname = "DXRawDisplayService::removeEventHists: ";
  bool dbg = m_LogLevel;
  if ( dbg > 1 ) cout << "Deleting events hists, count = " << m_eventhists.size() << endl;
  for ( TH1* ph : m_eventhists ) {
    if ( dbg > 2 ) cout << myname << "Removing " << ph->GetName() << endl;
    ph->Write();
    ph->SetDirectory(0);
    delete ph;
  }
  m_eventhists.clear();
  if ( dbg > 1 ) cout << "After delete event hist count: " << m_eventhists.size() << endl;
}

//************************************************************************

DEFINE_ART_SERVICE_INTERFACE_IMPL(DXRawDisplayService, RawDigitAnalysisService)

//**********************************************************************
