#include "converter.hpp"

using namespace std;

Converter::Converter (float th,vector<float> box_size)
{
	//init const
	xmin = 0.0;
	ymin = xmin;
	zmin = xmin;
	xmax = box_size[0];
	ymax = box_size[1];
	zmax = box_size[2];
	threshold = th;
}

bool Converter::AddVertex (Vertex v)
{
	vertexListRaw.push_back (v);
	bool vertExist = false;
	int i = 0;
	if (periodic) {
	//check if added vertex is already in list if is outside the box

		//correction, to place vertex inside the box
		if (v.X < xmin) v.X = v.X + xmax;
		if (v.X > xmax) v.X = v.X - xmax;
		if (v.Y < ymin) v.Y = v.Y + ymax;
		if (v.Y > ymax) v.Y = v.Y - ymax;
		if (v.Z < zmin) v.Z = v.Z + zmax;
		if (v.Z > zmax) v.Z = v.Z - zmax;
	}
	//find if there already exists same vertex;
	vertExist = false;
	for (i = 0; i < vertexListUnique.size(); i++) {
		Vertex *vOld = &vertexListUnique[i];
		if ( (Abs (vOld->X - v.X) < threshold) &&
		     (Abs (vOld->Y - v.Y) < threshold) &&
		     (Abs (vOld->Z - v.Z) < threshold)) {
			vertExist = true;
			break;
		}
	}
	vertexListMapping.push_back (i);

	
	if (!vertExist) {
		vertexListUnique.push_back (v);
		return true;
	}
	return false;
}
bool Converter::AddEdge (Edge e)
{
	int id = 1;
	bool edgeExist = false;

	for (int i = 0; i < edgeList.size(); i++) {
		Edge *eOld = &edgeList[i];
		if (eOld->Equal (e)) {
			edgeExist = true;
			break;
		}
		if (eOld->EqualInv (e)) {
			edgeExist = true;
			id = -id;
			break;
		}
		id++;
	}

	edgeListMap.push_back (id);
	if (!edgeExist) {
		edgeList.push_back (e);
		return true;
	}
	return false;
}

bool Converter::AddSurface (Surface s)
{
	int id = 1;
	bool surfExist = false;

		for (int i = 0; i < surfaceList.size(); i++) {
			int result = CompareLists (s.edgeList, surfaceList[i].edgeList);
			if (result == 1 || result == -1) {
				id = id * result;
				surfExist = true;
				break;
			}
			id++;
		}

	surfaceListMap.push_back (id);
	if (!surfExist) {
		surfaceList.push_back (s);
		return true;
	}
	return false;
}

void Converter::AddVolume (Volume v)
{
	bool volExist = false;
	sort (v.surfaceList.begin(), v.surfaceList.end(), abscomp);
	int  i = 0;
	for (i = 0; i < v.surfaceList.size(); i++) {
		int result = CompareLists (v.surfaceList, volumeList[i].surfaceList);
		if (result == 1 || result == -1) {
			volExist = true;
			break;
		}
	}
	volumeListMap.push_back (i);
	if (!volExist)	volumeList.push_back (v);
}

int Converter::CompareLists (vector<int> &a, vector<int> &b)
{
	if (a.size() != b.size()) return 0;
	int match = 0;
	bool inv = false;
	for (int i = 0; i < a.size(); i++) {
		//find if all lines matches
		for (int j = 0; j < a.size(); j++) {
			if (a[i] == b[j]) {
				match++;
				break;
			}
			if (a[i] == -b[j]) {
				match++;
				inv = true;
				break;
			}
		}
	}
	if (match == a.size()) {
		if (inv) return -1;
		return 1;
	}
	return 0;
}

vector<double> Converter::FindVolCenter (vector<int> &faces)
{
	vector<double> center;
	//needs to know all vertices for one volume


	return center;
}

WrappingCont Converter::ComputeWrappingCont (int v0, int v1)
{
	//compute if an edge goes outside the box
	Wrapping x = GetWrapping (vertexListRaw[v0].X, vertexListRaw[v1].X, xmin, xmax);
	Wrapping y = GetWrapping (vertexListRaw[v0].Y, vertexListRaw[v1].Y, ymin, ymax);
	Wrapping z = GetWrapping (vertexListRaw[v0].Z, vertexListRaw[v1].Z, zmin, zmax);
	//cout << vertexListRaw[v0].X << " " << vertexListRaw[v1].X << " " << xmin << " " << xmax << endl;
	//cout << x << y << z << endl;
	return WrappingCont (x, y, z);
}
Wrapping Converter::GetWrapping (double p0, double p1, double min, double max)
{
	//compute if an edge of specific direction p0-->p1
	// goes outside the box
	if (p0 < min && p1 > min && p1 < max) return Wrapping::plus;
	if (p1 > max && p0 > min && p0 < max) return Wrapping::plus;
	if (p0 > max && p1 > min && p1 < max) return Wrapping::minus;
	if (p1 < min && p0 > min && p0 < max) return Wrapping::minus;
	return Wrapping::asterix;
}

WrappingCont Converter::ComputeAproxWrapping (Vertex &v0, Vertex &v1)
{
	double dx, dy, dz;
	double th = 0.6;
	WrappingCont  wcont;

	dx = v1.X - v0.X;
	dy = v1.Y - v0.Y;
	dz = v1.Z - v0.Z;
	if (dx < -th*xmax)
		wcont.x = Wrapping::plus;
	else if (dx > th*xmax)
		wcont.x = Wrapping::minus;
	if (dy < -th*ymax)
		wcont.y = Wrapping::plus;
	else if (dy > th*ymax)
		wcont.y = Wrapping::minus;
	if (dz < -th*zmax)
		wcont.z = Wrapping::plus;
	else if (dz > th*zmax)
		wcont.z = Wrapping::minus;
	return wcont;
}



