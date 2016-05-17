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
  m_DoMean                 = pset.get<bool>("DoMean");
  m_NchanMeanRms           = pset.get<int>("NchanMeanRms");
  m_PedestalOption         = pset.get<unsigned int>("PedestalOption");
  m_AdcOffset              = pset.get<float>("AdcOffset");
  m_DecompressWithPedestal = pset.get<bool>("DecompressWithPedestal");
  m_UseChannelMap          = pset.get<bool>("UseChannelMap");
  m_BadChannelFlag         = pset.get<int>("BadChannelFlag");
  m_MaxEventsLog           = pset.get<int>("MaxEventsLog");
  m_MaxDigitsLog           = pset.get<int>("MaxDigitsLog");
  art::ServiceHandle<geo::Geometry> geosvc;       // pointer to Geometry service
  m_pgh = new GeoHelper(&*geosvc, true, 0);
  if ( m_LogLevel > 0 ) cout << myname << "Fetched geometry helper." << endl;

  // Fetch pedestal provider.
  m_pPedProv = nullptr;
  if ( m_PedestalOption == 2 ) {
    m_pPedProv = &art::ServiceHandle<lariov::DetPedestalService>()->GetPedestalProvider();
    cout << myname << "Pedestal provider: @" <<  m_pPedProv << endl;
  }

  if ( m_LogLevel > 0 ) {
    cout << myname << "                LogLevel: " << m_LogLevel << endl;
    cout << myname << "              TdcTickMin: " << m_TdcTickMin << endl;
    cout << myname << "              TdcTickMax: " << m_TdcTickMax << endl;
    cout << myname << "             NTickPerBin: " << m_NTickPerBin << endl;
    cout << myname << "             NChanPerBin: " << m_NChanPerBin << endl;
    cout << myname << "                  DoROPs: " << m_DoROPs << endl;
    cout << myname << "                   DoAll: " << m_DoAll << endl;
    cout << myname << "             DoAllOnline: " << m_DoAllOnline << endl;
    cout << myname << "                  DoMean: " << m_DoMean << endl;
    cout << myname << "            NchanMeanRms: " << m_NchanMeanRms << endl;
    cout << myname << "          PedestalOption: " << m_PedestalOption << endl;
    cout << myname << "               AdcOffset: " << m_AdcOffset << endl;
    cout << myname << "  DecompressWithPedestal: " << m_DecompressWithPedestal << endl;
    cout << myname << "           UseChannelMap: " << m_UseChannelMap << endl;
    cout << myname << "          BadChannelFlag: " << m_BadChannelFlag << endl;
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

  // Check geometry helper.
  if ( m_pgh == nullptr ) {
    cout << myname << "ERROR: Geometry helper is absent." << endl;
    return 1;
  }
  const GeoHelper& geohelp = *m_pgh;
  int nchan = m_pgh->geometry()->Nchannels();

  // Create histograms.
  vector<TH2*> rophists;
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

  // Loop over digits.
  // Digit information is only logged for dbg >= 5.
  bool first = true;
  unsigned int maxdbgchan = 50;
  unsigned int maxdbgtick = 50;
  unsigned int idig = 0;
  // Flags that record how the digits are ordered.
  bool isOnlineOrdered = true;
  bool isOfflineOrdered = true;
  if ( dbg > 4 ) cout << myname << "Looping over digits." << endl;
  for ( const RawDigit& digit : digs ) {
    if ( dbg > 5 ) cout << myname << "Processed digit count: " << m_NDigitsProcessed << endl;
    if ( m_NDigitsProcessed > m_MaxDigitsLog && dbg > 4 ) {
      cout << myname << "MaxDigitsLog limit reached. Increase this parameter to log more digits." << endl;
      dbg = 4;
    }
    ++m_NDigitsProcessed;
    if ( dbg > 4 ) cout << myname << "----------" << endl;
    unsigned int ichan = digit.Channel();
    unsigned int ichanon = ichan;
    if ( pchanmap != nullptr ) {
      ichanon = pchanmap->online(ichan);
      isOnlineOrdered &= ichanon == idig;
      isOfflineOrdered &= ichan == idig;
    }
    if ( m_BadChannelFlag == 1 ) {
      if ( pcsp->IsBad(ichanon) ) {
        if ( dbg >=2 ) cout << myname << "Skipping bad channel on/off = " << ichanon << "/" << ichan << endl;
        continue;
      }
    }
    if ( dbg > 5 ) cout << myname << "          Channel: " << ichan << endl;
    unsigned int irop = geohelp.channelRop(ichan);
    if ( dbg > 5 ) cout << myname << "              ROP: " << irop << endl;
    TH2* ph = nullptr;
    TH2* ph_mean = nullptr;
    TH2* ph_rms = nullptr;
    if ( rophists.size() > irop ) ph = rophists[irop];
    if ( rophists_mean.size() > irop ) ph_mean = rophists_mean[irop];
    if ( rophists_rms.size() > irop ) ph_rms = rophists_rms[irop];
    if ( dbg >= 5 ) {
      cout << myname << "ROP hists for channel " << ichan << ":";
      if ( ph != nullptr ) cout << " " << ph->GetName();
      if ( ph_mean != nullptr ) cout << " " << ph_mean->GetName();
      if ( ph_rms != nullptr ) cout << " " << ph_rms->GetName();
      cout << endl;
    }
    unsigned int iropchan = ichan - geohelp.ropFirstChannel(irop);
    if ( dbg > 5 ) cout << myname << "      ROP channel: " << iropchan << endl;
    int nadc = digit.NADC();
    vector<short> adcs;
    if ( dbg > 5 ) cout << myname << "      Compression: " << digit.Compression() << endl;
    if ( dbg > 5 ) cout << myname << "  Compressed size: " << digit.ADCs().size() << endl;
    if ( dbg > 5 ) cout << myname << "Uncompressed size: " << digit.Samples() << endl;
    if ( dbg > 5 ) cout << myname << "   Digit pedestal: " << digit.GetPedestal() << endl;
    if ( digit.Compression() == raw::kNone ) {
      if ( dbg > 5 ) cout << myname << "Copying uncompressed..." << endl;
      adcs = digit.ADCs();
    } else {
      if ( dbg > 6 ) {
        cout << myname << "Uncompressed data:" << endl;
        for ( unsigned int tick=0; tick<digit.ADCs().size(); ++tick ) {
          cout << myname << "  Raw ADC entry " << tick << ": " << digit.ADCs()[tick] << endl;
          if ( dbg < 6 && tick >= maxdbgtick ) {
            cout << myname << "..." << endl;
            break;
          }
        }
      }
      if ( dbg > 5 ) cout << myname << "Uncompressing..." << endl;
      // Following is to avoid crash. See https://cdcvs.fnal.gov/redmine/issues/11572.
      adcs.resize(digit.Samples());
      if ( m_DecompressWithPedestal ) {
        float fped = digit.GetPedestal();
        raw::Uncompress(digit.ADCs(), adcs, fped, digit.Compression());
      } else {
        raw::Uncompress(digit.ADCs(), adcs, digit.Compression());
      }
      if ( dbg > 5 ) cout << myname << "Uncompressed." << endl;
    }
    if ( first ) {
      if ( dbg > 4 && dbg < 6 ) {
        cout << myname << " Compression level for first digit: " << digit.Compression() << endl;
        cout << myname << "      # TDC slices for first digit: " << adcs.size() << endl;
      }
      first = false;
    }
    unsigned int nzero = 0;
    for ( auto adc : adcs ) if ( adc == 0.0 ) ++nzero;
    if ( dbg > 4 ) cout << myname << "Digit channel " << ichan
                        << " (ROP-chan = " << irop << "-" << iropchan
                        << ") has " << nadc << " ADCs and "
                        << digit.Samples() << " samples. Uncompressed size is " << adcs.size()
                        << ". Number filled is " << adcs.size()-nzero << endl;
    float pedestal = 0.0;
    if ( m_PedestalOption == 1 ) {
      pedestal += digit.GetPedestal();
    } else if ( m_PedestalOption == 2 ) {
      float dbped = m_pPedProv->PedMean(ichan);
      if ( dbg > 5 ) cout << myname << "DB Pedestal: " << dbped << endl;
      pedestal += dbped;
    }
    // Loop over ticks.
    int icnt = 0;
    double tsum = 0.0;
    double tsumsq = 0.0;
    double tsum_bin = 0;
    double tsumsq_bin = 0.0;
    unsigned int ibin = 0;
    unsigned int ntick_bin = 0;
    for ( unsigned int tick=0; tick<adcs.size(); ++tick ) {
      double wt = adcs[tick] - pedestal + m_AdcOffset;;
      ++icnt;
      tsum += wt;
      tsumsq += wt*wt;
      if ( dbg > 6 || ( dbg > 5 && ichan<maxdbgchan && tick<maxdbgtick ) )
        cout << myname << "  Tick " << tick << " raw - ped: " << adcs[tick] << " - " << pedestal
                           << " = " << wt << endl;
      if ( wt == 0 ) continue;
      if ( ph != nullptr ) ph->Fill(tick, iropchan, wt);
      double allwt = wt;
      if ( fAbsAll ) allwt = fabs(allwt);
      if ( phallraw != nullptr ) phallraw->Fill(tick, ichan, allwt);
      if ( phallrawon != nullptr ) phallrawon->Fill(tick, ichanon, allwt);
      tsum_bin += wt;
      tsumsq_bin += wt*wt;
      ++ntick_bin;
      if ( ph_mean != nullptr && ph_rms != nullptr &&
           (ntick_bin == m_NchanMeanRms || tick==adcs.size()-1) ) {
        double mean = tsum_bin/ntick_bin;
        double rms = sqrt(tsumsq_bin/ntick_bin);
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
