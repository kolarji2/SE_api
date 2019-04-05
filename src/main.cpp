#include "converter.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip> 
#include <exception>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
//#include <boost/program_options/value_semantic.hpp>
#include <iterator>
// Author: Jiri Kolar
// May 2016
//namespace po = boost::program_options;
using namespace std;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main (int argc, char* argv[])
{
	try {
		po::options_description desc ("Allowed options");
		desc.add_options()
		("help,h", "produce help message")
		("input-file,i", po::value<string>(), "input .geo or .fe file where information about foam structure is stored.")
		("output-file,o", po::value<string>()->default_value ("foam"), "Output .fe file where output for Surface Evolver is stored.")
		("output-geo-file", po::value<string>(), "Generate also an output .geo file, after union for example.")
		("commands,c", po::value<string>(), "List of cmd files for evolver, separated by comma. Link to all files is added into resulting cmd file.")
		("all-union", po::value<int>(), "Union of specified number of volumes, they have to share exactly one facet, also unify surfaces and edges.")
		("volume-union,v", po::value<int>(), "Union of specified number of volumes, they have to share exactly one facet.")
		("surface-union,s", po::bool_switch(), "Union of surfaces that share at least one side,in way that each volume will have only one common surface. Unless volumes share more separated surfaces.")
		("edge-union,e", po::bool_switch(), "Union of edges, that each surface will have only one common edge.")
		("generate,g", po::value<string>(), "Generate foam structure as specified [cubic|random|hexab].")
		("num-cell,n", po::value<int>()->default_value (-1), "Number of generated cells, -1=auto-choose optimal value.")
		("analyze,a", po::value<string>(), "Analyze foam shape.")
		("threshold,t", po::value<float>()->default_value (1e-6), "Threshold of accuracy for mathematical operation.")
		("scalex,x", po::value<float>()->default_value (1.0), "Scale parameter for x axis deformation.")
		("box-size", po::value<std::vector<float> >()->multitoken(), "Box size in format: [0.8 1.0 1.0]")
		("noperiodicity", po::bool_switch(), "No periodicity of input file.")
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
		
		vector<float> box_size;		
		if (vm["box-size"].empty()) {
			//default box-size
			box_size=vector<float>(3,1);
		} else {
			box_size=vm["box-size"].as<vector<float>>();
			if (box_size.size()!=3) {
				cout << "Input box-size need exactly 3 arguments e.g. --box-size 1 1 1" <<endl;
				return 0;
				}
		}			
		//call functions according to loaded cmds
		Converter converter (vm["threshold"].as<float>(),box_size);
		converter.scalex=vm["scalex"].as<float>();
		if (vm["noperiodicity"].as<bool>())
			converter.periodic=false;
		bool dataLoaded = false;
		string foutFeName;
		string foutCmdName;
		string inputfile;
		string inputFileExt;

		if (vm.count ("input-file")) {
			inputfile=vm["input-file"].as<string>();
			inputFileExt=fs::path(inputfile).extension().string();
			if (inputFileExt==".geo")
				dataLoaded = converter.LoadGeo (inputfile);
			else if (inputFileExt==".fe")
				dataLoaded = converter.LoadFe (inputfile);
			else
				cout << "Unsupported input file format!" << endl;
						
			//if (p) { // TO DO: gnuplot support not implemented
			//	ofstream foutGnuPlot (foutGnuPlotName);
			//	bool saveGnu = converter.SaveGnuPlot (foutGnuPlot);
			//}
		} else if (vm.count ("generate")) {
			dataLoaded = converter.Generate (vm["generate"].as<string>(), vm["num-cell"].as<int>());
		}
		if (dataLoaded) {
			if (vm.count ("commands")) {
				//to do implement like a vector<string>
				converter.LoadCmdFiles (vm["commands"].as<string>());
			}
			if (vm.count ("all-union") && vm["all-union"].as<int>()>1) {
				if (vm.count ("output-geo-file"))
					converter.MergeStructureRaw();
				converter.MergeStructure (vm["all-union"].as<int>());
			} else {
				if (vm.count ("volume-union")) {
					converter.MergeVolumes (vm["volume-union"].as<int>());
					converter.RepairIds();
				}
				if (vm["surface-union"].as<bool>()) {
					if (vm.count ("output-geo-file"))
						converter.MergeStructureRaw();
					converter.MergeSurfaces();
					converter.RepairIds();
				}
				if (vm["edge-union"].as<bool>()) {			
					converter.MergeEdges();
					converter.RepairIds();
				}
			}			
			if (vm.count ("analyze")) {
				converter.AnalyzeCells (vm["analyze"].as<string>());
			}
			if (vm.count ("output-geo-file")) {
				converter.SaveGeo (vm["output-geo-file"].as<string>());
			}
			string foutName = vm["output-file"].as<string>();
			if (foutName.substr (foutName.length() - 3, 3) == ".fe")
				foutName = foutName.substr (0, foutName.length() - 3);
			converter.SaveFe (foutName + ".fe");
			converter.SaveCmd (foutName + ".cmd");
		}
	} catch (exception& e) {
		cout << "An exception occurred: " << endl << e.what() << endl;
	}
	return 0;
}
