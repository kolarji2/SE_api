#include "converter.hpp"

using namespace std;

vector<double> Converter::GetNormal (vector<int> &edges)
{

	vector<double> n;
	double normn;
	vector<double> v0;
	vector<double> v1;
	if (edges.size() < 3)
		throw length_error ("GetNormal: Face has less than 3 edges.");
	v0 = GetVector (edges[0]);
	v1 = GetVector (edges[1]);
	n = Cross (v0, v1);
	normn = Norm (n);
	n = DivideByScalar (n, normn);
	return n;

}

vector<double> Converter::GetVector (int edgeId)
{
	int vId0;
	int vId1;
	vector<double> p0;
	vector<double> p1;
	vector<double> res;
	vId0 = edgeList[abs (edgeId) - 1].V0;
	vId1 = edgeList[abs (edgeId) - 1].V1;
	p0 = vertexListUnique[vId0 - 1].GetPos();
	p1 = vertexListUnique[vId1 - 1].GetPos();
	p1 = MovePointByWrap (p1, edgeList[abs (edgeId) - 1].wrappingCont);
	if (edgeId > 0)
		res = VectorDiff (p0, p1);
	else
		res = VectorDiff (p1, p0);
	return res;
}

vector<double> Converter::GetVector (int edgeId,int vId) {
	if (edgeList[abs (edgeId) - 1].V0==vId)
		return GetVector (abs(edgeId));
	return GetVector (-abs(edgeId));	
}

vector<double> Converter::MovePointByWrap (vector<double> p0, WrappingCont wcont)
{
	vector<double> res (3, 0.0);
	double dx = xmax - xmin;
	double dy = ymax - ymin;
	double dz = zmax - zmin;
	res[0] = p0[0] + dx * wcont.GetSign (wcont.x);
	res[1] = p0[1] + dy * wcont.GetSign (wcont.y);
	res[2] = p0[2] + dz * wcont.GetSign (wcont.z);
	return res;
}

vector<double> Converter::VectorDiff (vector<double> v0, vector<double> v1)
{
	vector<double> res (3, 0.0);
	res[0] = v1[0] - v0[0];
	res[1] = v1[1] - v0[1];
	res[2] = v1[2] - v0[2];
	return res;
}
vector<double> Converter::Cross (vector<double> v0, vector<double> v1)
{
	vector<double> res (3, 0.0);
	res[0] = v0[1] * v1[2] - v0[2] * v1[1];
	res[1] = v0[2] * v1[0] - v0[0] * v1[2];
	res[2] = v0[0] * v1[1] - v0[1] * v1[0];
	return res;
}

double Converter::Dot (vector<double> v0, vector<double> v1)
{
	return v0[0]*v1[0]+v0[1]*v1[1]+v0[2]*v1[2];
}

vector<double> Converter::DivideByScalar (vector<double> v0, double s)
{
	vector<double> res (3, 0.0);
	res[0] = v0[0] / s;
	res[1] = v0[1] / s;
	res[2] = v0[2] / s;
	return res;
}

double Converter::Norm (vector<double> v0)
{
	double res = 0;
	res = v0[0] * v0[0] + v0[1] * v0[1] + v0[2] * v0[2];
	res = sqrt (res);
	return res;
}
