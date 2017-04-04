#ifndef structure_hpp
#define structure_hpp
#include <vector>
#include <iostream>
#include <string>
using namespace std;

static int sgn (int a)
{
	return a >= 0 ? 1 : -1;
}

enum Wrapping {plus = 0, minus = 1, asterix = 2};
enum Coor {x, y, z};
//basic structures for storing of loaded data
class WrappingCont
{
public:
	Wrapping x;
	Wrapping y;
	Wrapping z;

	inline WrappingCont() {
		this->x = Wrapping::asterix;
		this->y = Wrapping::asterix;
		this->z = Wrapping::asterix;
	}
	inline WrappingCont (Wrapping x, Wrapping y, Wrapping z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	inline string ToString() {

		return GetStr (x) + GetStr (y) + GetStr (z);
	}
	inline string GetStr (Wrapping &w) {
		switch (w) {
		case Wrapping::plus:
			return " +";
		case Wrapping::minus:
			return " -";
		case Wrapping::asterix:
			return " *";
		default:
			return " _";
		}
	}
	inline int GetSign (Wrapping &w) {
		switch (w) {
		case Wrapping::plus:
			return 1;
		case Wrapping::minus:
			return -1;
		case Wrapping::asterix:
			return 0;
		default:
			return 0;
		}
	}
	inline bool IsWrapping() {
		if (this->x != Wrapping::asterix)
			return true;
		if (this->y != Wrapping::asterix)
			return true;
		if (this->z != Wrapping::asterix)
			return true;
		return false;
	}
	inline bool Equal (WrappingCont &wc) {
		if (wc.x == x && wc.y == y && wc.z == z) return true;
		return false;
	}
	inline bool EqualInv (WrappingCont &wc) {
		if (wc.x == GetInv (x) && wc.y == GetInv (y) && wc.z == GetInv (z)) return true;
		return false;
	}
	inline Wrapping GetInv (Wrapping &w) {
		switch (w) {
		case Wrapping::plus:
			return Wrapping::minus;
		case Wrapping::minus:
			return Wrapping::plus;
		default:
			return Wrapping::asterix;
		}
	}
	inline void UpdateCont (WrappingCont &wc) {
		this->x = MergeWrap (x, wc.x);
		this->y = MergeWrap (y, wc.y);
		this->z = MergeWrap (z, wc.z);
	}

	inline Wrapping MergeWrap (Wrapping &a, Wrapping &b) {
		int sa = GetSign (a);
		int sb = GetSign (b);
		if (sa == 0)
			return b;
		if (sb == 0)
			return a;
		if (sa * sb > 0)
			return a;
		return Wrapping::asterix;
	}	
};

class Vertex
{
public:
	double X;
	double Y;
	double Z;
	int id = 0;
	//vector<int> mapRaw;
	inline Vertex (double x, double y, double z) {
		this->X = x;
		this->Y = y;
		this->Z = z;
	}
	inline vector<double> GetPos() {
		vector<double> v (3, 0.0);
		v[0] = this->X;
		v[1] = this->Y;
		v[2] = this->Z;
		return v;
	}
	inline vector<double> MoveByWrappingCont(WrappingCont &wc,double max) {
		vector<double> mv(3);
		mv[0]=MoveByWrapping(wc.x,this->X,max);
		mv[1]=MoveByWrapping(wc.y,this->Y,max);
		mv[2]=MoveByWrapping(wc.z,this->Z,max);
		return mv;
	}
	
	inline double MoveByWrapping(Wrapping &w,double p,double max) {
		switch (w) {
		case Wrapping::plus:
			return p+max;
		case Wrapping::minus:
			return p-max;
		default:
			return p;
		}		
	}
};

class Edge
{
public:
	int V0;
	int V1;
	int id = 0;
	vector<int> surfaceList;
	//vector<int> mapRaw;
	WrappingCont wrappingCont;
	inline Edge (int v0, int v1, WrappingCont wc) {
		this->V0 = v0;
		this->V1 = v1;
		this->wrappingCont = wc;
		this->surfaceList.reserve(3);
	}
	inline bool Equal (Edge &e) {

		if (this->V0 == e.V0 && this->V1 == e.V1) {
			if (wrappingCont.Equal (e.wrappingCont)) return true;
		}
		return false;
	}
	inline bool EqualInv (Edge &e) {
		if (this->V0 == e.V1 && this->V1 == e.V0) {
			if (wrappingCont.EqualInv (e.wrappingCont)) return true;
		}
		return false;
	}		
};
class Surface
{
public:
	int id = 0;
	vector<int> edgeList;
	vector<int> mapRaw;
	inline Surface (vector<string> &data, vector<int> &edgeListMap) {
		edgeList.reserve (data.size() - 2);
		for (int i = 2; i < data.size(); i++) {
			int edge = stoi (data[i]);
			if (abs (edge) > edgeListMap.size())
				throw out_of_range ("Index out of range in Line loop section");
			edgeList.push_back (sgn (edge) *edgeListMap[abs (edge) - 1]);
		}
	}
	inline Surface (vector<string> &data) {
		edgeList.reserve (data.size() - 2);
		for (int i = 2; i < data.size(); i++) {
			int edge = stoi (data[i]);
			edgeList.push_back (edge);
		}
	}
	inline Surface (int size) {
		edgeList.reserve (size);
	}
	inline bool RepairIds (vector<Edge> &data) {
		int oldId;
		for (int i = 0; i < edgeList.size(); i++) {
			oldId = edgeList[i];
			edgeList[i] = sgn (oldId) * data[abs (oldId) - 1].id;
		}
		return true;
	}
	inline bool UpdateDir (int itemId, int dir) {
		int i = FindItem (itemId);
		if (i == -1)
			return false;
		edgeList[i] = abs (edgeList[i]) * dir;
		return true;
	}
	inline int GetSign (int itemId) {
		int i = FindItem (itemId);
		if (i == -1)
			return 0;
		return sgn (edgeList[i]);
	}
	inline bool DeleteItem (int itemId) {
		int i = FindItem (itemId);
		if (i == -1)
			return false;
		edgeList.erase (edgeList.begin() + i);
		return true;
	}
	inline int FindItem (int itemId) {
		for (int i = 0; i < edgeList.size(); i++) {
			if (abs (edgeList[i]) == abs (itemId)) {
				return i;
			}
		}
		return -1;
	}
};

class Volume
{
public:
	int id = 0;
	vector<int> surfaceList;
	vector<double> center; //position of center of the volume
	inline Volume (vector<string> &data, vector<int> &surfaceListMap) {
		surfaceList.reserve (data.size() - 2);
		for (int i = 2; i < data.size(); i++) {
			int srf = stoi (data[i]);
			if (abs (srf) > surfaceListMap.size())
				throw out_of_range ("Index out of range in Surface Loop section");
			surfaceList.push_back (sgn (srf) * surfaceListMap[abs (srf) - 1]);
		}
		this->center = vector<double> (3, 0);
	}
	inline Volume (vector<string> &data) {
		surfaceList.reserve (data.size() - 2);
		for (int i = 2; i < data.size(); i++) {
			int srf = stoi (data[i]);
			surfaceList.push_back (srf);
		}
		this->center = vector<double> (3, 0);
	}
	inline Volume () {};
	inline bool DeleteItem (int itemId) {
		bool del = false;
		for (int i = 0; i < surfaceList.size(); i++) {
			if (abs (surfaceList[i]) == abs (itemId)) {
				surfaceList.erase (surfaceList.begin() + i);
				del = true;
				break;
			}
		}
		return del;
	}

	inline bool RepairIds (vector<Surface> &data) {
		int oldId;
		for (int i = 0; i < surfaceList.size(); i++) {
			oldId = surfaceList[i];
			surfaceList[i] = sgn (oldId) * data[abs (oldId) - 1].id;
		}
		return true;
	}


};

#endif

