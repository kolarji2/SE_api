#include "converter.hpp"

using namespace std;

//analyze foam shape

bool Converter::AnalyzeCells(ofstream &an_file) {
	cout << "Analysing foam... " << endl;
	vector<vector<int>> celltypes=EvalCellTypes();
	double avgCell=AvgFacetsPerCell(celltypes);
	double avgFacet=AvgEdgesPerFacet(celltypes);
	SaveAnalysis(an_file,celltypes,avgCell,avgFacet);
	return true;	
}

vector<vector<int>> Converter::EvalCellTypes() {
	int i,j,f,surfI,fmax;
	fmax=8;
	vector<int> cellType;
	vector<vector<int>> cellTypeList;
	for (i=0;i<volumeList.size();i++) {
		cellType=vector<int>(fmax,0);
		//cell Type is list with count of facets with 3,4,5,6,7 or 8 edges
		for(j=0;j<volumeList[i].surfaceList.size();j++) {
			surfI=abs(volumeList[i].surfaceList[j])-1;
			surfaceList[surfI].id=0;
			f=surfaceList[surfI].edgeList.size();
			if (f>=3 && f<fmax+3) {
				cellType[f-3]+=1;
			} else {
			cout << "WARNING: There was found a facet with more than: " << fmax+2 << " edges" << endl;
			}
		}
		cellTypeList.push_back(cellType);
	}
	return cellTypeList;
}

double Converter::AvgEdgesPerFacet(vector<vector<int>> celltypes) {
	//average number of edges per facet
	int i,j;
	double avg,count;
	avg=0;
	count=0;
	for (i = 0; i < celltypes.size(); i++) {
		vector<int> ct=celltypes[i];
		for (j=0;j<ct.size();j++) {
			avg+=(j+3)*ct[j];
			count+=ct[j];
		}
	}
	return avg/count;
}

double Converter::AvgFacetsPerCell(vector<vector<int>> celltypes) {
	//average number of facets per cell
	int i,j;
	double avg,count;
	avg=0;
	count=celltypes.size();
	for (i = 0; i < celltypes.size(); i++) {
		vector<int> ct=celltypes[i];
		for (j=0;j<ct.size();j++) {
			avg+=ct[j];
		}
	}
	return avg/count;
}


bool Converter::SaveAnalysis (ofstream &an_file,vector<vector<int>> celltypes,double avgCell,double avgFacet)
{
	int i,j;

	//detorus the structure
	
	if (!an_file.is_open()) {
		cout << "Can not open output file to save analysis." << endl;
		return false;
	}
	cout << "Generating file with cell types analysis..." << endl;
	
	an_file << "Number of faces with specified number of edges" << endl;
	an_file << "AvgFacetsPerCell\tAvgEdgesPerFacet" << endl;
	an_file << avgCell << '\t' << avgFacet << endl;
	an_file << "3\t4\t5\t6\t7\t8\t9\t10\t edge-facet" << endl;
	an_file << "------------------------------" << endl;
	for (i = 0; i < celltypes.size(); i++) {
		vector<int> ct=celltypes[i];
		for (j=0;j<ct.size();j++) {
			an_file << ct[j] << '\t';
		}
		an_file << endl;
	}
	return true;
}
