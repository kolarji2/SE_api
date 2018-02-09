#include "converter.hpp"

using namespace std;

bool Converter::MergeStructure (int nvolpercell)
{	
	MergeVolumes (nvolpercell);	
	MergeSurfaces();
	RepairIds();
	MergeEdges();	
	RepairIds();
	return true;
}

bool Converter::RepairIds()
{
	cout << "Repairing numbering ..." << endl;
	//remove all unused entities and renumber
	int i, j, k, id, v0I, v1I, surfI, edgeI;
	//Unique
	vector<Vertex> newVertList;
	vector<Edge> newEdgeList;
	vector<Surface> newSurfaceList;
	//reserve

	newVertList.reserve (vertexListUnique.size());
	newEdgeList.reserve (edgeList.size());
	newSurfaceList.reserve (surfaceList.size());

	//set all ids on -1
	for (i = 0; i < surfaceList.size(); i++)
		surfaceList[i].id = -1;
	for (i = 0; i < edgeList.size(); i++)
		edgeList[i].id = -1;
	for (i = 0; i < vertexListUnique.size(); i++)
		vertexListUnique[i].id = -1;


	//find usable, assing 0
	for (i = 0; i < volumeList.size(); i++) {
		for (j = 0; j < volumeList[i].surfaceList.size(); j++) {
			surfI = abs (volumeList[i].surfaceList[j]) - 1;
			surfaceList[surfI].id = 0;
			for (k = 0; k < surfaceList[surfI].edgeList.size(); k++) {
				edgeI = abs (surfaceList[surfI].edgeList[k]) - 1;
				edgeList[edgeI].id = 0;
				v0I = edgeList[edgeI].V0 - 1;
				v1I = edgeList[edgeI].V1 - 1;
				vertexListUnique[v0I].id = 0;
				vertexListUnique[v1I].id = 0;
			}
		}
	}

	//renumber vertices
	k = 1;
	for (i = 0; i < vertexListUnique.size(); i++) {
		if (vertexListUnique[i].id == -1) continue;
		vertexListUnique[i].id = k;
		k += 1;
	}
	//renumber edges
	k = 1;
	for (i = 0; i < edgeList.size(); i++) {
		if (edgeList[i].id == -1) continue;
		edgeList[i].id = k;
		k += 1;
	}
	//renumber surfaces
	k = 1;
	for (i = 0; i < surfaceList.size(); i++) {
		if (surfaceList[i].id == -1) continue;
		surfaceList[i].id = k;
		k += 1;
	}

	//repair ids
	for (i = 0; i < edgeList.size(); i++) {
		if (edgeList[i].id == -1) continue;
		v0I = edgeList[i].V0 - 1;
		v1I = edgeList[i].V1 - 1;
		edgeList[i].V0 = vertexListUnique[v0I].id;
		edgeList[i].V1 = vertexListUnique[v1I].id;
	}
	for (i = 0; i < surfaceList.size(); i++) {
		if (surfaceList[i].id == -1) continue;
		surfaceList[i].RepairIds (edgeList);
	}
	for (i = 0; i < volumeList.size(); i++) {
		volumeList[i].RepairIds (surfaceList);
		sort (volumeList[i].surfaceList.begin(), volumeList[i].surfaceList.end(), abscomp);
	}

	//copy not removed vertices
	for (i = 0; i < vertexListUnique.size(); i++) {
		if (vertexListUnique[i].id == -1) continue;
		newVertList.push_back (vertexListUnique[i]);
	}
	//copy not removed edges
	for (i = 0; i < edgeList.size(); i++) {
		if (edgeList[i].id == -1) continue;
		newEdgeList.push_back (edgeList[i]);
	}
	//copy not removed surfaces
	for (i = 0; i < surfaceList.size(); i++) {
		if (surfaceList[i].id == -1) continue;
		newSurfaceList.push_back (surfaceList[i]);
	}
	vertexListUnique.swap (newVertList);
	edgeList.swap (newEdgeList);
	surfaceList.swap (newSurfaceList);
}


bool Converter::RepairOriVolume()
{
	// check if orientation of facets for each volume is correct
	int i, j, k, id;
	Volume v;
	vector<double> n;
	vector<double> c;

	for (int i = 0; i < volumeList.size(); i++) {
		v = volumeList[i];
		//find volume center

		//check each facet
		cout << "Volume: " << i + 1 << endl;
		for (int j = 0; j < v.surfaceList.size(); j++) {

			id = abs (v.surfaceList[j]) - 1;
			cout << "   Surf: " << id + 1 << endl;
			n = GetNormal (surfaceList[id].edgeList);
			printV (n);

			//find normal vector to plane defined by facet
		}


	}

	return true;
}

bool Converter::MergeVolumes (int nvolpercell)
{
	cout << "Merging volumes..." << endl;
	if (nvolpercell <= 1) {
		cout << "\tNothing to merge, skipping MergeVolumes!" << endl;
		return false;
	}
	int i, j, k;
	int nTot = volumeList.size() / nvolpercell;
	vector<Volume> volumeListUn;
	vector<int> nlist0;
	vector<int> nlist1;
	volumeListUn.reserve (nTot);

	if (volumeList.size() % nvolpercell != 0)
		throw out_of_range ("UnionVolumes: Index out of range, volumeList.size!=nvolpercell*nTot");

	for (i = 0; i < volumeList.size(); i += nvolpercell) {
		for (j = 0; j < nvolpercell; j++) {
			// folowing cell-vol order 2n..4-2-0-1-3..2n-1 means nextVol is always adjacent
			nlist1 = volumeList[i + j].surfaceList;
			nlist0 = ListAbsUnion (nlist0, nlist1, false);
		}
		Volume vol;
		vol.surfaceList.swap (nlist0);
		sort (vol.surfaceList.begin(), vol.surfaceList.end(), abscomp);
		nlist0 = vector<int>();
		volumeListUn.push_back (vol);
	}
	volumeList.swap (volumeListUn);
	return true;
}

bool Converter::MergeSurfaces()
{
	cout << "Merging surfaces..." << endl;
	int i, j, k, l, minSurfID;
	vector<vector<int>> comm;
	vector<int> mergedFace;
	vector<int> mergedFaceIds;
	vector<bool> used;
	bool del, del0, del1;
	//for each volume test his surrounding
	for (i = 0; i < volumeList.size(); i++) {
		for (j = i + 1; j < volumeList.size(); j++) {
			comm = FindCommonItems (volumeList[i].surfaceList, volumeList[j].surfaceList);
			if (comm.size() > 1) { //process volumes with more than one common surface
				used = vector<bool> (comm.size(), false);
				mergedFaceIds = vector<int>();
				for (k = 0; k < comm.size(); k++) {
					minSurfID = -1;
					mergedFace = MergeSurfacesFromList (comm, minSurfID, used);
					if (mergedFace.size() == 0)
						break;
					mergedFaceIds.push_back (minSurfID);
					//update surfaceList
					surfaceList[minSurfID - 1].edgeList.swap (mergedFace);
					mergedFace = vector<int> ();
				}
				for (k = 0; k < comm.size(); k++) {
					del = true;
					for (l = 0; l < mergedFaceIds.size(); l++) {
						if (mergedFaceIds[l] == comm[k][0])
							del = false;
					}
					if (!del)
						continue;
					//delete surfaces in volumes
					del0 = volumeList[i].DeleteItem (comm[k][0]);
					del1 = volumeList[j].DeleteItem (comm[k][0]);
					if (!del0 || !del1)
						throw invalid_argument ("MergeSurfaces: Can not delete item.");
				}
				for (k = 0; k < used.size(); k++) {
					if (!used[k]) {
						throw domain_error ("MergeSurfacesFromList: Not all surfaces from the list were merged.");
					}
				}
			}
		}
	}
	return true;
}

//private methods
vector<int> Converter::MergeSurfacesFromList (vector<vector<int>> &comm, int &minId, vector<bool> &used)
{
	//merge all facets from the comm list
	//raise an error if all facets could not be merged
	vector<int> u;
	vector<vector<int>> commE;
	bool first = true;
	int i, j, id0, prevsize;
	for (i = 0; i < comm.size(); i++) {
		for (j = 0; j < comm.size(); j++) {
			if (!used[j]) {
				id0 = comm[j][0] - 1;
				commE = FindCommonItems (u, surfaceList[id0].edgeList);
				if (commE.size() > 0 || u.size() == 0) {
					u = ListAbsUnion (u, surfaceList[id0].edgeList, true);
					used[j] = true;
					if (first)
						minId = comm[j][0];
					first = false;
				}
			}
		}
	}
	return u;
}

bool Converter::MergeEdges()
{
	cout << "Merging edges..." << endl;
	// hledani trojic neni efektivni!
	int i, j, k, edgeI;
	vector<vector<int>> comm0;
	vector<vector<int>> comm1;
	bool commFound;

	// check if there are always 3 facets per edge
	for (i = 0; i < surfaceList.size(); i++) {
		for (j = 0; j < surfaceList[i].edgeList.size(); j++) {
			edgeI = abs (surfaceList[i].edgeList[j]) - 1;
			edgeList[edgeI].surfaceList.push_back (i + 1);
		}

	}
	for (i = 0; i < edgeList.size(); i++) {
		if (edgeList[i].surfaceList.size() != 3)
			cout << "MergeEdges: " << edgeList[i].surfaceList.size() << " surfaces" << endl;
		//throw length_error("MergeEdges: An edge has more than 3 common surfaces!");
	}
	for (i = 0;  i < surfaceList.size(); i++) {
		for (j = i + 1; j < surfaceList.size(); j++) {
			comm0 = FindCommonItems (surfaceList[i].edgeList, surfaceList[j].edgeList);
			if (comm0.size() > 1) {
				for (k = j + 1; k < surfaceList.size(); k++) {
					comm1 = FindCommonItems (surfaceList[i].edgeList, surfaceList[j].edgeList, surfaceList[k].edgeList);
					if (comm1.size() > 1) {
						MergeEdgesFromList (comm1, i, j, k);
					}
				}
			}
		}
	}
	return true;
}

bool Converter::MergeEdgesFromList (vector<vector<int>> &comm, int s0, int s1, int s2)
{
	int i, j, k, startE, endE;
	int minEdgeId = comm[0][0];
	WrappingCont wcont0;
	WrappingCont wcont;
	vector<int> e0list;
	int v0Id, v1Id, dir0, wrapEdgeId, dir1, dir2;
	bool match;
	//identify side vertices
	startE = 0;
	endE = 0;
	k = 0;
	e0list = surfaceList[s0].edgeList;
	//jump to first uncommon edge
	for (i = 0; i < e0list.size(); i++) {
		match = false;
		for (j = 0; j < comm.size(); j++) {
			if (comm[j][0] == abs (e0list[i])) {
				match = true;
			}
		}
		k = i;
		if (!match)
			break;
	}
	for (i = 0; i < e0list.size(); i++) {
		for (j = 0; j < comm.size(); j++) {
			if (comm[j][0] == abs (e0list[k])) {
				//if (edgeList[comm[j][0]-1].wrappingCont.IsWrapping()) {
				//wcont0=edgeList[comm[j][0]-1].wrappingCont;
				//if (e0list[k]<0) { //inverse wrapping
				//wcont0.x=wcont0.GetInv(wcont0.x);
				//wcont0.y=wcont0.GetInv(wcont0.y);
				//wcont0.z=wcont0.GetInv(wcont0.z);
				//}
				//wcont.UpdateCont(wcont0);
				//}
				if (startE == 0) startE = e0list[k];
				endE = e0list[k];
			}
		}
		k = (k + 1) % e0list.size();
	}

	if (startE > 0)
		v0Id = edgeList[startE - 1].V0;
	else
		v0Id = edgeList[abs (startE) - 1].V1;
	if (endE > 0)
		v1Id = edgeList[endE - 1].V1;
	else
		v1Id = edgeList[abs (endE) - 1].V0;
	//update edge with minimal index
	edgeList[minEdgeId - 1].V0 = v0Id;
	edgeList[minEdgeId - 1].V1 = v1Id;
	wcont = ComputeAproxWrapping (vertexListUnique[v0Id - 1], vertexListUnique[v1Id - 1]);
	edgeList[minEdgeId - 1].wrappingCont = wcont;

	//set right direction to edges
	dir0 = surfaceList[s0].GetSign (minEdgeId);
	dir1 = 1;
	dir2 = 1;
	if (dir0 != surfaceList[s1].GetSign (minEdgeId))
		dir1 = -1;
	if (dir0 != surfaceList[s2].GetSign (minEdgeId))
		dir2 = -1;
	surfaceList[s0].UpdateDir (minEdgeId, 1);
	surfaceList[s1].UpdateDir (minEdgeId, dir1);
	surfaceList[s2].UpdateDir (minEdgeId, dir2);

	//remove other edges
	for (i = 1; i < comm.size(); i++) {
		surfaceList[s0].DeleteItem (comm[i][0]);
		surfaceList[s1].DeleteItem (comm[i][0]);
		surfaceList[s2].DeleteItem (comm[i][0]);
	}
	return true;
}

bool Converter::CheckSurface (vector<int> &surfL)
{
	int i, j, k;
	int edgeId, v0Id, v1Id;
	if (surfL.size() < 3)
		return false;
	v0Id = GetTailVertexId (surfL[0]);
	for (i = 1; i < surfL.size(); i++) {
		v1Id = GetHeadVertexId (surfL[i]);
		if (v0Id != v1Id)
			return false;
		v0Id = GetTailVertexId (surfL[i]);
	}
	v1Id = GetHeadVertexId (surfL[0]);
	if (v0Id != v1Id)
		return false;
	return true;
}

int Converter::GetHeadVertexId (int eId)
{
	if (eId == 0) return 0;
	if (eId > 0)
		return edgeList[eId - 1].V0;
	return edgeList[abs (eId) - 1].V1;
}

int Converter::GetTailVertexId (int eId)
{
	return GetHeadVertexId (-eId);
}

vector<int> Converter::ListAbsUnion (vector<int> &a, vector<int> &b, bool considerDir)
{
	//remove common items and makes a union of the rest
	//return union in vector a
	if (a.size() == 0)
		return vector<int> (b);
	vector<int> u;
	vector<vector<int>> comm;
	u.reserve (a.size() + b.size());
	int i, j, k, l, dir;
	bool iscomm;
	comm = FindCommonItems (a, b);
	if (comm.size() == 0)
		return vector<int> (a);
	if (comm.size() == a.size())
		throw domain_error ("ListAbsUnion: Both sets are identical!");

	for (i = 0; i < a.size(); i++) {
		iscomm = false;
		for (l = 1; l < comm.size(); l++) {
			if (comm[l][1] == i) iscomm = true;
		}
		if (iscomm) continue; //preskoc nechtene strany
		if (i != comm[0][1]) {
			u.push_back (a[i]);
		} else {
			//decide orientation
			dir = 1;
			k = comm[0][2];
			if (a[i]*b[k] > 0 && considerDir)
				dir = -1;
			for (j = 0; j < b.size() - 1; j++) {
				iscomm = false;
				k = mod ( (k + dir), b.size());
				for (l = 0; l < comm.size(); l++) {
					if (comm[l][2] == k) iscomm = true;
				}
				if (!iscomm)
					u.push_back (dir * b[k]);
			}
		}
	}
	return u;
}

vector<vector<int>> Converter::FindCommonItems (vector<int> &a, vector<int> &b)
{
//find common items in list and their position
//minimal first
	vector<vector<int>> comm;
	comm.reserve((a.size()+b.size())/2);
	int i, j;
	for (i = 0; i < a.size(); i++) {
		for (j = 0; j < b.size(); j++) {
			if (abs (a[i]) == abs (b[j])) {
				vector<int> item {abs (a[i]), i, j};
				comm.push_back (item);
				continue;
			}
		}
	}
	sort (comm.begin(), comm.end(), compComm);
	return comm;
}

vector<vector<int>> Converter::FindCommonItems (vector<int> &a, vector<int> &b, vector<int> &c)
{
//find common items in list and their position
//minimal first
	int i, j;
	vector<vector<int>> comm0;
	vector<vector<int>> comm1;
	vector<vector<int>> comm2;
	comm2.reserve((a.size()+b.size()+c.size())/3);
	comm0 = FindCommonItems (a, b);
	comm1 = FindCommonItems (a, c);
	for (i = 0; i < comm0.size(); i++) {
		for (j = 0; j < comm1.size(); j++) {
			if (comm0[i][0] == comm1[j][0]) {
				vector<int> item {comm0[i][0], comm0[i][1], comm0[i][2], comm1[i][2]};
				comm2.push_back (item);
				break;
			}
		}
	}
	return comm2;
}

void Converter::printV (vector<int> &a)
{
	for (int i = 0; i < a.size(); i++) {
		cout << a[i] << ' ';
	}
	cout << endl;
}

void Converter::printV (vector<double> &a)
{
	for (int i = 0; i < a.size(); i++) {
		cout << a[i] << ' ';
	}
	cout << endl;
}

void Converter::printV (vector<vector<int>> &a)
{
	for (int i = 0; i < a.size(); i++) {
		cout << a[i][0] << ' ';
	}
	cout << endl;
}
