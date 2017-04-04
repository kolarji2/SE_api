#include "converter.hpp"
#include <math.h> 
#define PI 3.14159265
using namespace std;

//analyze foam shape

bool Converter::AnalyzeCells (string file_name)
{
	double avg_a,vrc_a,avg_e,vrc_e;
	cout << "Analysing foam... " << endl;
	vector<vector<int>> celltypes = EvalCellTypes();
	double avgCell = AvgFacetsPerCell (celltypes);
	double avgFacet = AvgEdgesPerFacet (celltypes);
	vector<vector<double>> vertex_angles=EvalVertexAngles(avg_a,vrc_a);
	vector<double> edge_lengths=EvalEdgeLength(avg_e,vrc_e);
	SaveAnalysis (file_name, celltypes, vertex_angles,edge_lengths,avgCell, avgFacet,avg_a,vrc_a,avg_e,vrc_e);
	return true;
}

vector<vector<int>> Converter::EvalCellTypes()
{
	//run after all facets and edges are merged
	int i, j, f, surfI, fmax;
	fmax = 12;
	vector<int> cellType;
	vector<vector<int>> cellTypeList;
	for (i = 0; i < volumeList.size(); i++) {
		cellType = vector<int> (fmax-2, 0);
		//cell Type is list with count of facets with 3,4,5,6,7 or 8 edges
		for (j = 0; j < volumeList[i].surfaceList.size(); j++) {
			surfI = abs (volumeList[i].surfaceList[j]) - 1;
			surfaceList[surfI].id = 0;
			f = surfaceList[surfI].edgeList.size();
			if (f >= 3 && f <= fmax) {
				cellType[f - 3] += 1;
			} else {
				cout << "WARNING: There was found a facet with more than: " << fmax << " edges" << endl;
			}
		}
		cellTypeList.push_back (cellType);
	}
	return cellTypeList;
}

vector<vector<double>> Converter::EvalVertexAngles(double &avg_a,double &vrc_a)
{
	//run after all facets and edges are merged
	int i, j,k, edge0,edge1, fmax;
	int v0,v1;
	double cosa,angle;
	int ntot=0;
	vector<int> edges;
	vector<double> vect0,vect1;
	vector<vector<double>> angleList(vertexListUnique.size(),vector<double>());
	vector<vector<int>> vertexEdgeList(vertexListUnique.size(),vector<int>());
	avg_a=0.0;	
	vrc_a=0.0;	
	for (i = 0; i < edgeList.size(); i++) {
		v0=edgeList[i].V0-1;
		v1=edgeList[i].V1-1;
		vertexEdgeList[v0].push_back(i+1);
		vertexEdgeList[v1].push_back(i+1);		
	}
	for (i=0;i<vertexEdgeList.size();i++) {
		edges=vertexEdgeList[i];
		for (j=0;j<edges.size();j++) {
		for (k=j+1;k<edges.size();k++){	
			vect0=GetVector(edges[j],i+1);
			vect1=GetVector(edges[k],i+1);
			cosa=Dot(vect0,vect1)/Norm(vect0)/Norm(vect1);
			angle=acos(cosa)/PI*180;
			avg_a+=angle;
			vrc_a+=angle*angle;
			ntot+=1;		
			angleList[i].push_back(angle);
		}					
		}			
	}
	avg_a=avg_a/ntot;
	vrc_a=vrc_a/ntot-avg_a*avg_a;
	vrc_a=sqrt(vrc_a);
	return angleList;
}
vector<double> Converter::EvalEdgeLength(double &avg_e,double &vrc_e) {
	int i;
	double length;
	avg_e=0.0;
	vrc_e=0.0;
	vector<double> edgeLengths(edgeList.size());
	for (i = 0; i < edgeList.size(); i++) {
		length=Norm(GetVector(i+1));
		avg_e+=length;
		vrc_e+=length*length;
		edgeLengths[i]=length;
	}
	avg_e/=edgeList.size();
	vrc_e=vrc_e/edgeList.size()-avg_e*avg_e;
	vrc_e=sqrt(vrc_e);
	return edgeLengths;
	}

double Converter::AvgEdgesPerFacet (vector<vector<int>> celltypes)
{
	//average number of edges per facet
	int i, j;
	double avg, count;
	avg = 0;
	count = 0;
	for (i = 0; i < celltypes.size(); i++) {
		vector<int> ct = celltypes[i];
		for (j = 0; j < ct.size(); j++) {
			avg += (j + 3) * ct[j];
			count += ct[j];
		}
	}
	return avg / count;
}

double Converter::AvgFacetsPerCell (vector<vector<int>> celltypes)
{
	//average number of facets per cell
	int i, j;
	double avg, count;
	avg = 0;
	count = celltypes.size();
	for (i = 0; i < celltypes.size(); i++) {
		vector<int> ct = celltypes[i];
		for (j = 0; j < ct.size(); j++) {
			avg += ct[j];
		}
	}
	return avg / count;
}


bool Converter::SaveAnalysis (string file_name, vector<vector<int>> celltypes, vector<vector<double>> vertex_angles,vector<double> edge_lengths, double avg_fperc, double avg_eperf, double avg_a, double vrc_a, double avg_e, double vrc_e)
{
	int i, j;
	
	//detorus the structure
	ofstream an_file(file_name);
	if (!an_file.is_open()) {
		cout << "Can not open output file to save analysis." << endl;
		return false;
	}
	cout << "Saving foam analysis to file: "<< file_name << endl;

	an_file << "{\"avg-facet-per-cell\": "<< avg_fperc;
	an_file << ", \"avg-edges-per-facet\": " << avg_eperf;
	an_file << ", \"vertex-angle\": {\"avg\": " << avg_a << ", \"sigma\" : " << vrc_a << "}";
	an_file << ", \"edge-length\": {\"avg\": " << avg_e << ", \"sigma\" : " << vrc_e << "}";
	//cell types
	an_file << ", \"cell-types\": [";
	for (i = 0; i < celltypes.size(); i++) {
		vector<int> ct = celltypes[i];
		if (i==0)
			an_file << "[";
		else
			an_file << ",[";
		for (j = 0; j < ct.size()-1; j++) {
			an_file << ct[j] << ',';
		}
		an_file << ct[ct.size()-1] << "]";
	}
	an_file << "]";
	//Angle distribution
	an_file << ", \"angles-by-vertex\": [";
	for (i = 0; i < vertex_angles.size(); i++) {
		vector<double> al = vertex_angles[i];
		if (i==0)
			an_file << "[";
		else
			an_file << ",[";
		for (j = 0; j < al.size()-1; j++) {
			an_file << al[j] << ',';
		}
		an_file << al[al.size()-1] << "]";
	}
	an_file << "]";
	//Edge size distribution
	an_file << ", \"length-by-edges\": [";
	for (i = 0; i < edge_lengths.size()-1; i++) {
		an_file << edge_lengths[i] <<',';
	}
	an_file << edge_lengths[edge_lengths.size()-1] << "]";
	an_file << "}";
	
	
	an_file.close();
	return true;
}
