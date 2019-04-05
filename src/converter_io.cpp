#include "converter.hpp"

using namespace std;
//
// INPUT
//
bool Converter::LoadGeo (string file_name)
{
	/*
	* Load .geo file to converter
	* Makes it periodic and transform to local data structure
	*/

	string line;
	string val;
	vector<string> sl;

	int i = 0;
	int id = 0;
	float num;
	ifstream fin (file_name);
				
	if (!fin.is_open()) {
		cout << "Can not open input file." << endl;
		return false;
	}
	cout << "Loading input geo file: "<< file_name << endl;
	int nverts = 0;
	int nedges = 0;
	int nsurfs = 0;
	int nvols = 0;
	int ntot = 0;
	//Get number of entities
	auto const start_pos = fin.tellg();
	while (getline (fin, line)) {
		sl = ParseLnGeo (line);
		if (sl.size() > 0) {
			if (sl[0] == "Point") {
				nverts++;
			} else if (sl[0] == "Line") {
				nedges++;
			} else if (sl[0] == "Line Loop") {
				nsurfs++;
			} else if (sl[0] == "Surface Loop") {
				nvols++;
			}
			ntot++;
		}
	}
	fin.clear();
	fin.seekg (start_pos);

	//allocate memory
	//points
	vertexListUnique.reserve (nverts);
	vertexListRaw.reserve (nverts);
	vertexListMapping.reserve (nverts);
	//edges
	edgeList.reserve (nedges);
	edgeListRaw.reserve (nedges);
	edgeListMap.reserve (nedges);
	//surfaces
	surfaceList.reserve (nsurfs);
	surfaceListRaw.reserve (nsurfs);
	surfaceListMap.reserve (nsurfs);
	//volumes
	volumeList.reserve (nvols);
	volumeListRaw.reserve (nvols);
	//loading structure
	int percStep = 10;
	int perc = 10;
	int iline = 0;
	while (getline (fin, line)) {
		iline++;
		if (ntot > 500000 && iline > perc * (ntot / 100)) {
			cout << "\tCompleted: " << perc << " %" << endl;
			perc += percStep;
		}
		//parsing line
		sl = ParseLnGeo (line);
		if (sl.size() > 0) {
			if (sl[0] == "Point") {
				if (sl.size() < 5) throw length_error ("Error in input file: section Point");
				if (stoi(sl[1]) ==0)
					vert_offset=0;
				if (stoi (sl[1]) != vertexListRaw.size() + vert_offset) throw length_error ("Inconsistent order of in section Point");
				Vertex v (stod (sl[2]), stod (sl[3]), stod (sl[4]));
				AddVertex (v);
			} else if (sl[0] == "Line") {
				if (sl.size() < 4) throw length_error ("Error in input file: section Line");
				if (stoi(sl[1]) ==0)
					edge_offset=0;
				int v0 = stoi (sl[2]) - vert_offset;
				int v1 = stoi (sl[3]) - vert_offset;
				if (v0 < 0 || v1 < 0 || v0 >= vertexListRaw.size() || v1 >= vertexListRaw.size())
					throw out_of_range ("Index of vertex out of range");
				
				WrappingCont wrappingCont= WrappingCont();
				if (periodic)
					wrappingCont = ComputeWrappingCont (v0, v1);
									
				Edge eRaw (v0 + vert_offset, v1 + vert_offset, WrappingCont());
				edgeListRaw.push_back (eRaw);
				v0 = vertexListMapping[v0] + vert_offset;
				v1 = vertexListMapping[v1] + vert_offset;
				Edge e (v0, v1, wrappingCont);
				AddEdge (e);
			} else if (sl[0] == "Line Loop") {
				if (sl.size() - 2 <= 0) throw length_error ("Error in Line Loop section");
				if (stoi(sl[1]) ==0)
					surf_offset=0;
				if (stoi (sl[1]) != surfaceListMap.size() + surf_offset) throw length_error ("Inconsistent order of in section Line Loop");
				Surface srfRaw (sl);
				surfaceListRaw.push_back (srfRaw);
				Surface srf (sl, edgeListMap);
				AddSurface (srf);
			} else if (sl[0] == "Surface Loop") {
				if (sl.size() - 2 <= 0) throw length_error ("Error in Surface Loop section");
				if (stoi(sl[1]) ==0)
					vol_offset=0;
				if (stoi (sl[1]) != volumeListMap.size() + vol_offset) throw length_error ("Inconsistent order of in section Surface Loop");
				Volume volRaw (sl);
				volumeListRaw.push_back (volRaw);
				Volume vol (sl, surfaceListMap);
				AddVolume (vol);
			}
		}
	}
	for (int i = 0; i < surfaceListMap.size(); i++) {
		surfaceList[abs(surfaceListMap[i]) - 1].mapRaw.push_back (i + 1);
	}
	fin.close();
	return true;
}

bool Converter::LoadFe (string file_name) {
	/*
	* Load .fe file to converter
	* Makes it periodic and transform to local data structure
	*/

	string line;
	string val;
	vector<string> sl;

	int i = 0;
	int id = 0;
	float num;
	ifstream fin (file_name);
				
	if (!fin.is_open()) {
		cout << "Can not open input file." << endl;
		return false;
	}
	cout << "Loading input fe file: "<< file_name << endl;
	int nverts = 0;
	int nedges = 0;
	int nsurfs = 0;
	int nvols = 0;
	int ntot = 0;
	//Get number of entities
	auto const start_pos = fin.tellg();
	while (getline (fin, line)) {
		sl = ParseLnFe (line);
		if (sl.size() > 0) {
			if (sl[0] == "Point") {
				nverts++;
			} else if (sl[0] == "Line") {
				nedges++;
			} else if (sl[0] == "Line Loop") {
				nsurfs++;
			} else if (sl[0] == "Surface Loop") {
				nvols++;
			}
			ntot++;
		}
	}
	fin.clear();
	fin.seekg (start_pos);

	//allocate memory
	//points
	vertexListUnique.reserve (nverts);
	vertexListRaw.reserve (nverts);
	vertexListMapping.reserve (nverts);
	//edges
	edgeList.reserve (nedges);
	edgeListRaw.reserve (nedges);
	edgeListMap.reserve (nedges);
	//surfaces
	surfaceList.reserve (nsurfs);
	surfaceListRaw.reserve (nsurfs);
	surfaceListMap.reserve (nsurfs);
	//volumes
	volumeList.reserve (nvols);
	volumeListRaw.reserve (nvols);
	//loading structure
	int percStep = 10;
	int perc = 10;
	int iline = 0;
	while (getline (fin, line)) {
		iline++;
		if (ntot > 500000 && iline > perc * (ntot / 100)) {
			cout << "\tCompleted: " << perc << " %" << endl;
			perc += percStep;
		}
		//parsing line
		sl = ParseLnFe (line);
	}
	fin.close();
	return false;
	}

vector<string> Converter::ParseLnFe (string line)
{
	vector<string> strVec;
	return strVec;
}

vector<string> Converter::ParseLnGeo (string line)
{
	//Parse line from .geo file
	//Dodelat get_token() a parsovani, at nemuze spadnout
	bool id = false;
	bool order = false;
	bool value = false;
	int i = 0;
	int previ = 0;
	string val;
	vector<string> strVec;
	while (previ < line.size() && line[previ] == ' ') previ++;
	for (i = previ; i < line.size(); i++) {
		if (!id && line[i] == '(') {
			val = line.substr (previ, i - previ - 1);
			if (val == "Volume") {
				while (i < line.size() && line[i] != ';') i++;
				i++;
				if (i >= line.size()) {
					strVec.push_back (val);
					break;
				}
				while (i < line.size() && line[i] == ' ') i++;
				i--;
			} else {
				id = true;
				strVec.push_back (val);
			}
			previ = i + 1;
		} else if (!order && id && line[i] == ')') {
			order = true;
			strVec.push_back (line.substr (previ, i - previ));
		} else if (order && id && line[i] == '{') {
			previ = i + 1;
		} else if (line[i] == ';') {
			stringstream valStream (line.substr (previ, i - previ - 1));
			while (getline (valStream, val, ',')) {
				strVec.push_back (val);
			}
		}
		if (previ >= line.size())	break;

	}
	return strVec;
}

bool Converter::LoadCmdFiles (string csvCmdfiles)
{
	//Load cmd files, only names.
	int iprev = 0;
	for (int i = 0; i <= csvCmdfiles.length(); i++) {
		if (i == csvCmdfiles.length() || csvCmdfiles[i] == ',') {
			cmdFiles.push_back (csvCmdfiles.substr (iprev, i - iprev));
			iprev = i + 1;
		}
	}
	return true;
}

//
//	OUTPUT
//
void Converter::PrintHeaderTorus (ofstream &se_file) {
	double f=scalex;
	se_file << "TORUS_FILLED" << endl << endl;
	se_file << " parameter period_x = "<< xmax*f <<  endl;
	se_file << " parameter period_y = "<< ymax <<  endl;
	se_file << " parameter period_z = "<< zmax <<  endl;
	//constraint
	//se_file << "quantity body_vol energy method facet_torus_volume" << endl;
	//se_file <<"SYMMETRIC_CONTENT" <<  endl;
	//se_file << "PHASEFILE \"foam.phase\"" << endl;
	se_file << "periods" << endl;
	se_file << "period_x 0.000000 0.000000" << endl;
	se_file << "0.000000 period_y 0.000000" << endl;
	se_file << "0.000000 0.000000 period_z" << endl;
	
}

void Converter::PrintHeaderEmpty (ofstream &se_file) {
	
}

bool Converter::SaveFe (string file_name)
{
	//Generates input file for SurfaceEvolver
	int i;
	int j;
	double f=scalex;
	ofstream se_file (file_name);
	
	if (!se_file.is_open()) {
		cout << "Can not open output file." << endl;
		return false;
	}
	cout << "Saving structure in Surface Evolver format to: " << file_name << endl;
	if (periodic)
		PrintHeaderTorus(se_file);
	else
		PrintHeaderEmpty(se_file);

	//se_file << "define vertex attribute angle real" << endl;
	//se_file << "define vertex attribute uid integer" << endl;
	//se_file << "constraint 1" << endl;
	//se_file << "formula: angle=109.47" << endl;
	se_file << endl << "vertices" << endl;
	for (i = 0; i < vertexListUnique.size(); i++) {
		se_file << i + 1 << setprecision(17) << " " << vertexListUnique[i].X*f << " " << vertexListUnique[i].Y << " " << vertexListUnique[i].Z << endl;
	}

	se_file << endl << "edges" << endl;
	for (i = 0; i < edgeList.size(); i++) {
		se_file << i + 1 << " " << edgeList[i].V0 + (1-vert_offset) << " " << edgeList[i].V1+(1-vert_offset);
		if (periodic)
			se_file << " " << edgeList[i].wrappingCont.ToString();
		se_file <<  endl;
	}

	se_file << endl << "faces" << endl;
	for (i = 0; i < surfaceList.size(); i++) {
		se_file << i + 1 << " ";
		for (j = 0; j < surfaceList[i].edgeList.size(); j++) {
			se_file << surfaceList[i].edgeList[j] << " ";
		}
		se_file << endl;
	}
	se_file << endl << "bodies" << endl;
	for (i = 0; i < volumeList.size(); i++) {
		se_file << i + 1 << " ";
		for (j = 0; j < volumeList[i].surfaceList.size(); j++) {
			se_file << volumeList[i].surfaceList[j] << " ";
		}
		se_file << endl;
	}
	se_file.close();
	return true;
}

bool Converter::SaveCmd (string file_name)
{
	ofstream cmd_file (file_name);
	if (!cmd_file.is_open()) {
		cout << "Can not open output cmd file." << endl;
		return false;
	}
	cout << "Saving commands for Surface Evolver to: "<< file_name << endl;

	//Other cmd files
	for (int i = 0; i < cmdFiles.size(); i++) {
		ifstream f (cmdFiles[i].c_str());
		if (!f.good()) {
			cout << "\tCan not open file: " << cmdFiles[i] << " (file skipped)" << endl;
			continue;
		}
		f.close();
		cout << "\t" << cmdFiles[i] << " ...loaded" << endl;
		cmd_file << "read \"" << cmdFiles[i] << "\"" << endl;
	}
	cmd_file.close();
	return true;
}

bool Converter::SaveGeo (string file_name)
{
	int i, j;
	double f = scalex;
	ofstream geo_file (file_name);
	if (!geo_file.is_open()) {
		cout << "Can not open output geo file." << endl;
		return false;
	}
	cout << "Generating geo file for GMSH..." << endl;

	for (i = 0; i < vertexListRaw.size(); i++) {
		geo_file << "Point (" << i + 1 << ") = {";
		geo_file << vertexListRaw[i].X*f << "," << vertexListRaw[i].Y << "," << vertexListRaw[i].Z << "};" << endl;
	}

	for (i = 0; i < edgeListRaw.size(); i++) {
		geo_file << "Line (" << i + 1 << ") = {";
		geo_file << edgeListRaw[i].V0 << "," << edgeListRaw[i].V1 << "};" << endl;
	}

	for (i = 0; i < surfaceListRaw.size(); i++) {
		geo_file << "Line Loop (" << i + 1 << ") = {";
		for (j = 0; j < surfaceListRaw[i].edgeList.size() - 1; j++) {
			geo_file << surfaceListRaw[i].edgeList[j] << ",";
		}
		j = surfaceListRaw[i].edgeList.size() - 1;
		geo_file << surfaceListRaw[i].edgeList[j] << "};" << endl;
		geo_file << "Plane Surface (" << i + 1 << ") = {" << i + 1 << "}; Physical Surface (" << i + 1 << ") = {" << i + 1 << "};" << endl;
	}

	for (i = 0; i < volumeListRaw.size(); i++) {
		geo_file << "Surface Loop (" << i + 1 << ") = {";
		for (j = 0; j < volumeListRaw[i].surfaceList.size() - 1; j++) {
			geo_file << volumeListRaw[i].surfaceList[j] << ",";
		}
		j = volumeListRaw[i].surfaceList.size() - 1;
		geo_file << volumeListRaw[i].surfaceList[j] << "};" << endl;
		geo_file << "Volume (" << i + 1 << ") = {" << i + 1 << "};" << endl;
	}
	geo_file.close();
	return true;
}



bool Converter::SaveGnuPlot (string file_name)
{
	//cout << "Generating output for GnuPlot...";
	cout << "GnuPlot Not implemented for input files... Skipped" << endl;
	return false;
}
