#ifndef converter_hpp
#define converter_hpp
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
#include <iomanip> 
#include "voro++.hh"
#include "structure.hpp"

using namespace std;
using namespace voro;

class Converter
{
private:
	//Inner data
	//raw data
	vector<Vertex> vertexListRaw; // also vertices outside the periodic box
	vector<Edge> edgeListRaw;
	vector<Surface> surfaceListRaw;
	vector<Volume> volumeListRaw;
	//mapping
	vector<int> vertexListMapping; //maps vertices from Raw to Periodic
	vector<int> edgeListMap;
	vector<int> surfaceListMap;
	vector<int> volumeListMap;
	//command files
	vector<string> cmdFiles;
	//box size
	double xmin, xmax, ymin, ymax, zmin, zmax, threshold;
	//offset
	int vert_offset=1;
	int edge_offset=1;
	int surf_offset=1;
	int vol_offset=1;
public:
	//Public data structures (recommendation:read only)
	vector<Volume> volumeList;
	vector<Surface> surfaceList;
	vector<Edge> edgeList;
	vector<Vertex> vertexListUnique; //  only unique vertices
	float scalex=1.0;
	bool periodic=true;
	Converter (float th,vector<float> box_size);
	// Main functions to control the program
	//Load or generate data
	bool LoadGeo (string file_name);
	bool LoadFe (string file_name);
	bool Generate (string gen, int ncell);
	bool LoadCmdFiles (string csvCmdfiles);
	//Save loaded data to output files
	bool SaveFe (string file_name);
	bool SaveCmd (string file_name);
	bool SaveGeo (string file_name);
	bool SaveGnuPlot (string file_name);
	//Analyze structure, shape analysis
	bool AnalyzeCells (string file_name);
	bool SaveAnalysis (string file_name, vector<vector<int>> celltypes, vector<vector<double>> vertex_angles,vector<double> edge_lengths, double avg_fperc, double avg_eperf, double avg_a, double vrc_a, double avg_e, double vrc_e);
	vector<vector<double>> EvalVertexAngles(double &avg_a,double &vrc_a);
	vector<double> EvalEdgeLength(double &avg_e,double &vrc_e);
	vector<vector<int>> EvalCellTypes();
	double AvgEdgesPerFacet (vector<vector<int>> celltypes);
	double AvgFacetsPerCell (vector<vector<int>> celltypes);
	// Manipulate with structure
	bool MergeStructure (int nvolpercell);
	bool MergeStructureRaw();
	//Structure operation functions
	bool MergeVolumes (int nvolpercell);
	bool MergeSurfaces();
	bool MergeEdges();
	bool RepairIds();
	//others
	static inline bool abscomp (int a, int b) {
		return abs (a) < abs (b);
	}
	static inline bool compComm (vector<int> &a, vector<int> &b) {
		if (a.size() > 0 && b.size() > 0)
			return a[0] < b[0];
		return false;
	}

private:
	//Updating of local data
	bool AddVertex (Vertex v);
	bool AddEdge (Edge e);
	bool AddSurface (Surface s);
	void AddVolume (Volume v);
	//Parsing
	vector<string> ParseLnGeo (string line);
	vector<string> ParseLnFe (string line);
	//generate structure
	void gen_random (vector<double> &centerx, vector<double> &centery, vector<double> &centerz, int ncell);
	void gen_cubic (vector<double> &centerx, vector<double> &centery, vector<double> &centerz, int ncell);
	void gen_hexab (vector<double> &centerx, vector<double> &centery, vector<double> &centerz, int ncell);
	void gen_anhexab (vector<double> &centerx, vector<double> &centery, vector<double> &centerz, int ncell);
	//Inner computing functions
	WrappingCont ComputeWrappingCont (int v0, int v1);
	Wrapping GetWrapping (double p0, double p1, double min, double max);
	int CompareLists (vector<int> &a, vector<int> &b);
	inline double Abs (double a) {
		return a > 0 ? a : -a;
	}
	inline int mod (int a, int b) { //mod for negative numbers
		return a >= 0 ? a % b : (b - (-a) % b);
	}
	//Structure operation functions
	vector<int> ListAbsUnion (vector<int> &a, vector<int> &b, bool considerDir);
	vector<vector<int>> FindCommonItems (vector<int> &a, vector<int> &b);
	vector<vector<int>> FindCommonItems (vector<int> &a, vector<int> &b, vector<int> &c);
	vector<int> MergeSurfacesFromList (vector<vector<int>> &comm, int &minId, vector<bool> &used);
	bool MergeEdgesFromList (vector<vector<int>> &comm, int s0, int s1, int s2);
	WrappingCont ComputeAproxWrapping (Vertex &v0, Vertex &v1);
	bool CheckSurface (vector<int> &surfL);
	int GetHeadVertexId (int eId);
	int GetTailVertexId (int eId);
	bool RepairOriVolume();
	//Raw structure operation functions
	bool RepairIdsRaw();
	bool MergeSurfacesRaw();
	void MergeSurfacesFromListRaw (vector<vector<int>> &comm, vector<int> &mergedFaceIds);
	vector<vector<int>> FindCommonSurfRaw (vector<vector<int>> &comm, vector<int> &surfL);
	vector<vector<int>> FindUniqueRaw (vector<vector<int>> &comm0, vector<vector<int>> &comm1);
	vector<vector<int>> FindCommonRaw (vector<vector<int>> &comm0, vector<vector<int>> &comm1);
	bool UpdateVolumeRaw (vector<vector<int>> &comm, vector<int> &mergedFaceIds, int i);
	//Linear Algebra functions
	vector<double> FindVolCenter (vector<int> &faces);
	vector<double> GetNormal (vector<int> &edges);
	vector<double> GetVector (int edgeId);
	vector<double> GetVector (int edgeId,int vId);
	vector<double> VectorDiff (vector<double> v0, vector<double> v1);
	vector<double> Cross (vector<double> v0, vector<double> v1);
	double Dot (vector<double> v0, vector<double> v1);
	vector<double> DivideByScalar (vector<double> v0, double s);
	double Norm (vector<double> v0);
	vector<double> MovePointByWrap (vector<double> p0, WrappingCont wcont);
	//debug functions
	void printV (vector<int> &a);
	void printV (vector<double> &a);
	void printV (vector<vector<int>> &a);
	// output internal functions
	void PrintHeaderTorus (ofstream &se_file);
	void PrintHeaderEmpty (ofstream &se_file);
};


#endif
