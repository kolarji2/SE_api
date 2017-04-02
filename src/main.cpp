#include "converter.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>
#include <boost/program_options.hpp>
#include <iterator>
// Author: Jiri Kolar
// May 2016
//namespace po = boost::program_options;
using namespace std;
namespace po = boost::program_options;

int main (int argc, char* argv[])
{
	try {
		po::options_description desc ("Allowed options");
		desc.add_options()
		("help,h", "produce help message")
		("input-file,i", po::value<string>(), "input .geo file where information about foam structure is stored.")
		("output-file,o", po::value<string>()->default_value ("foam"), "Output .fe file where output for Surface Evolver is stored (optional).")
		("output-geo-file", po::value<string>()->implicit_value ("foam.geo"), "Generate also an output .geo file, after union for example (optional).")
		("commands,c", po::value<string>(), "List of cmd files for evolver, separated by comma. Link to all files is added into resulting cmd file. (optional).")
		("volume-union,u", po::value<int>(), "Union of specified number of volumes, they have to share exactly one facet (optional).")
		("generate,g", po::value<string>()->implicit_value ("random"), "Generate foam structure as specified [cubic|random|hexab](optional).")
		("num-cell,n", po::value<int>()->default_value (-1), "Number of generated cells, -1=auto-choose optimal value. (optional).")
		("analyze,a", po::value<string>()->implicit_value ("foam.an"), "Analyze foam shape (optional).")
		("threshold,t", po::value<float>()->default_value (1e-9), "Threshold of accuracy for mathematical operation (optional).")
		;
		// -p  // plot gnu plot graph NOT implemented

		po::variables_map vm;
		po::store (po::parse_command_line (argc, argv, desc), vm);
		po::notify (vm);

		if (vm.count ("help")) {
			cout << desc << "\n";
			return 0;
		}
		if (!vm.count ("input-file") && !vm.count ("generate")) {
			cout << ("You have to to specify at least input file by -i foam.geo or call generator etc. -g random") << endl;
			cout << ("\t Use --help for more information about usage") << endl;
			return 0;
		}
		if (vm.count ("input-file") && vm.count ("generate")) {
			cout << ("Can not read input-file and generate structure at same time. Choose one!") << endl;
			return 0;
		}

		//call functions according to loaded cmds
		Converter converter (vm["threshold"].as<float>());
		bool dataLoaded = false;
		string finName;
		string foutFeName;
		string foutCmdName;
		string foutGeoName;

		if (vm.count ("input-file")) {
			ifstream finGeo (vm["input-file"].as<string>());
			dataLoaded = converter.LoadGeo (finGeo);
			finGeo.close();
			//if (p) { // TO DO: gnuplot support not implemented
			//	ofstream foutGnuPlot (foutGnuPlotName);
			//	bool saveGnu = converter.SaveGnuPlot (foutGnuPlot);
			//}
		} else if (vm.count ("generate")) {
			dataLoaded = converter.Generate (vm["generate"].as<string>(), vm["num-cell"].as<int>());
		}
		string foutName = vm["output-file"].as<string>();
		if (foutName.substr (foutName.length() - 3, 3) == ".fe")
			foutName = foutName.substr (0, foutName.length() - 3);
		foutFeName = foutName + ".fe";
		foutCmdName = foutName + ".cmd";

		if (dataLoaded) {
			if (vm.count ("commands")) {
				//to do implement like a vector<string>
				converter.LoadCmdFiles (vm["commands"].as<string>());
			}
			if (vm.count ("volume-union")) {
				converter.MergeStructure (vm["volume-union"].as<int>());
			}
			if (vm.count ("analyze")) {
				ofstream foutAn (vm["analyze"].as<string>());
				converter.AnalyzeCells (foutAn);
			}
			if (vm.count ("output-geo-file")) {
				foutGeoName = vm["output-geo-file"].as<string>();
				ofstream foutGeo (foutGeoName);
				converter.SaveGeo (foutGeo);
				foutGeo.close();
			}

			ofstream foutFe (foutFeName);
			ofstream foutCmd (foutCmdName);
			converter.SaveFe (foutFe);
			converter.SaveCmd (foutCmd);
			foutFe.close();
			foutCmd.close();
		}
	} catch (exception& e) {
		cout << "An exception occurred: " << endl << e.what() << endl;
	}
	return 0;
}
