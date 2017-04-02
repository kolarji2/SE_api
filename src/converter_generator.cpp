#include "converter.hpp"
#include "voro++.hh"

using namespace std;
using namespace voro;
double rnd()
{
	return double (rand()) / RAND_MAX;
}

bool Converter::Generate (string gen, int ncell)
{
	/*
	*Generate structure according to settings
	* Place seeds to the container and call voronoi tesselaion
	* Output of voro++ makes periodic and transform to local data structure
	*/

	//Structure alloc
	bool reservedVRaw = false;
	bool reservedVUnique = false;
	bool reservedE = false;
	bool reservedSurf = false;
	bool reservedEMap = false;
	bool reservedSMap = false;
	//Cache mem size
	int cacheMem = 50;
	//other
	double px, py, pz;
	double vx0, vy0, vz0, vx1, vy1, vz1;
	int pid;
	int neigh_pid;
	int edgeMapShift = 0;
	int i, j, k;
	int n_x = 6, n_y = 6, n_z = 6;
	int vertex_index;
	//voro++ data
	voronoicell_neighbor c;
	vector<int> neigh, f_vert;
	vector<double> vert;
	vector<double> xcenter;
	vector<double> ycenter;
	vector<double> zcenter;
	xcenter.reserve (8);
	ycenter.reserve (8);
	zcenter.reserve (8);
	volumeList.reserve (8);

	if (gen == "random") {
		gen_random (xcenter, ycenter, zcenter, ncell);
	} else if (gen == "cubic") {
		gen_cubic (xcenter, ycenter, zcenter, ncell);
	} else if (gen == "hexab") {
		gen_hexab (xcenter, ycenter, zcenter, ncell);
	} else if (gen == "anhexab") {
		gen_anhexab (xcenter, ycenter, zcenter, ncell);
	} else {
		throw invalid_argument ("Unknown generate type: " + gen);
	}

	container con (xmin, xmax, ymin, ymax, zmin, zmax, n_x, n_y, n_z, true, true, true, 8);
	cout << "Generating 3D structure: " << gen << endl;
	ncell = xcenter.size();
	for (i = 0; i < ncell; i++) { //put cell centers in container
		con.put (i, xcenter[i], ycenter[i], zcenter[i]);
		volumeList.push_back (Volume());
	}

	//generate Gnu PLot file *NOT IMPLEMENTED*
	/*if (p) {
		cout << "Generating output for GnuPlot..." << endl;
		const char* cgnu_file = gnu_file.c_str();
		con.draw_cells_gnuplot (cgnu_file);
	}*/

	c_loop_all cl (con);

	if (cl.start()) do if (con.compute_cell (c, cl)) {
				cl.pos (px, py, pz);
				pid = cl.pid();
				c.neighbors (neigh);
				c.face_vertices (f_vert);
				c.vertices (px, py, pz, vert);
				edgeMapShift = edgeListMap.size();

//allocation

				if (!reservedVRaw && vertexListRaw.size() % cacheMem == 0) {
					vertexListRaw.reserve (vertexListRaw.size() + cacheMem);
					vertexListMapping.reserve (vertexListRaw.size() + cacheMem);
					reservedVRaw = true;
				}
				if (!reservedVUnique && vertexListUnique.size() % cacheMem == 0) {
					vertexListUnique.reserve (vertexListUnique.size() + cacheMem);
					reservedVUnique = true;
				}
				if (!reservedE && edgeList.size() % cacheMem == 0) {
					edgeList.reserve (edgeList.size() + cacheMem);
					reservedE = true;
				}
				if (!reservedEMap && edgeListMap.size()  % cacheMem == 0) {
					edgeListMap.reserve (edgeListMap.size() + cacheMem);
					reservedEMap = true;
				}

				if (!reservedSurf && surfaceList.size() % cacheMem == 0) {
					surfaceList.reserve (surfaceList.size() + cacheMem);
					reservedSurf = true;
				}

				if (!reservedSMap && surfaceListMap.size()  % cacheMem == 0) {
					surfaceListMap.reserve (surfaceListMap.size() + cacheMem);
					reservedSMap = true;
				}
				for (j = 0, k = 0; j < neigh.size(); j++) {

					//if { // each face is counted once
					if ( (neigh[j] >= pid)) {
						neigh_pid = neigh[j];
						Surface srf (f_vert[k]);
						//for current face add all vertices to the se_vert
						//for each added vertex add also edge
						//add faces for both neighbors
						for (int i0 = k + 1; i0 < k + f_vert[k] + 1; i0++) {

							vertex_index = 3 * f_vert[i0];
							//find vertex
							vx0 = vert[vertex_index];
							vy0 = vert[vertex_index + 1];
							vz0 = vert[vertex_index + 2];
							if (i0 == k + f_vert[k]) {
								vertex_index = 3 * f_vert[k + 1];
							} else {
								vertex_index = 3 * f_vert[i0 + 1];
							}
							vx1 = vert[vertex_index];
							vy1 = vert[vertex_index + 1];
							vz1 = vert[vertex_index + 2];

							//compute wraping
							reservedVRaw = false;
							reservedEMap = false;

							Vertex v0 = Vertex (vx0, vy0, vz0);
							reservedVUnique = AddVertex (v0);
							Vertex v1 = Vertex (vx1, vy1, vz1);
							reservedVUnique = AddVertex (v1);
							int vi0 = vertexListRaw.size() - 2;
							int vi1 = vertexListRaw.size() - 1;
							WrappingCont wrappingCont = ComputeWrappingCont (vi0, vi1);
							vi0 = vertexListMapping[vi0] + 1;
							vi1 = vertexListMapping[vi1] + 1;
							Edge e (vi0, vi1, wrappingCont);
							reservedE = AddEdge (e);
							srf.edgeList.push_back (edgeListMap[edgeListMap.size() - 1]);
						}
						reservedSMap = false;
						reservedSurf = AddSurface (srf);
						volumeList[pid].surfaceList.push_back (-surfaceListMap[surfaceListMap.size() - 1]);
						if (pid != neigh_pid) volumeList[neigh_pid].surfaceList.push_back (surfaceListMap[surfaceListMap.size() - 1]);
					}
					k += f_vert[k] + 1;
				}
			}
		while (cl.inc());
	return true;
}

void Converter::gen_random (vector<double> &centerx, vector<double> &centery, vector<double> &centerz, int ncell)
{
	if (ncell == -1) ncell = 9;
	for (int i = 0; i < ncell; i++) {
		centerx.push_back (xmin + rnd());
		centery.push_back (ymin + rnd());
		centerz.push_back (zmin + rnd());
	}
}
void Converter::gen_cubic (vector<double> &centerx, vector<double> &centery, vector<double> &centerz, int ncell)
{
	if (ncell == -1) ncell = 2;
	double space = 1.0 / ncell;
	for (int i = 0; i < ncell; i++)
		for (int j = 0; j < ncell; j++)
			for (int k = 0; k < ncell; k++) {
				centerx.push_back (space / 2 + space * i);
				centery.push_back (space / 2 + space * j);
				centerz.push_back (space / 2 + space * k);
			}
}
void Converter::gen_hexab (vector<double> &centerx, vector<double> &centery, vector<double> &centerz, int ncell)
{
	if (ncell == -1) ncell = 2;
	if (ncell % 2 == 1) ncell++;
	double space = 1.0 / (ncell);
	for (int i = 0; i < ncell; i++)
		for (int j = 0; j < ncell; j++)
			for (int k = 0; k < ncell; k++) {
				if (k % 2 == 0) {
					if (j % 2 == 0) {
						centerx.push_back (space / 4 + space * i);
					} else {
						centerx.push_back (space * 3 / 4 + space * i);
					}

					centery.push_back (space / 4 + space * j);
				} else {
					if (j % 2 == 0) {
						centerx.push_back (space * 3 / 4 + space * i);
					} else {
						centerx.push_back (space / 4 + space * i);
					}

					centery.push_back (space * 3 / 4 + space * j);
				}
				centerz.push_back (space / 4 + space * k);
			}

}

void Converter::gen_anhexab (vector<double> &centerx, vector<double> &centery, vector<double> &centerz, int ncell)
{
	if (ncell == -1) ncell = 2;
	if (ncell % 2 == 1) ncell++;
	double space = 1.0 / (ncell);
	for (int i = 0; i < ncell; i++)
		for (int j = 0; j < ncell; j++)
			for (int k = 0; k < ncell; k++) {
				if (k % 2 == 0) {
					if (j % 2 == 0) {
						centerx.push_back (space / 4 + space * i);
					} else {
						centerx.push_back (space * 3 / 4 + space * i);
					}

					centery.push_back (space / 4 + space * j);
				} else {
					if (j % 2 == 0) {
						centerx.push_back (space * 3 / 4 + space * i);
					} else {
						centerx.push_back (space / 4 + space * i);
					}

					centery.push_back (space * 3 / 4 + space * j);
				}
				centerz.push_back (space / 4 + space * k);
			}

	for (int i = 0; i < ncell; i++) {
		centerx[i] = centerx[i] * 0.5;
	}
}
