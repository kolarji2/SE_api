#include "converter.hpp"

using namespace std;

bool Converter::MergeStructureRaw()
{
	MergeSurfacesRaw();	
	RepairIdsRaw();
	// TODO
	//dodelat merge edges
	//cout << "Merging edges..." << endl;
	//MergeEdges();
	//cout << "Repairing numbering of edges..." << endl;
	//RepairIds();
	return true;
}

bool Converter::RepairIdsRaw()
{
	cout << "Repairing numbering of raw surfaces..." << endl;
	//remove all unused entities and renumber
	int i, j, k, id, v0I, v1I, surfI, edgeI;
	//Raw
	vector<Vertex> newVertListRaw;
	vector<Edge> newEdgeListRaw;
	vector<Surface> newSurfaceListRaw;
	//reserve
	newVertListRaw.reserve (vertexListRaw.size());
	newEdgeListRaw.reserve (edgeListRaw.size());
	newSurfaceListRaw.reserve (surfaceListRaw.size());

	//set all ids on -1
	for (i = 0; i < surfaceListRaw.size(); i++)
		surfaceListRaw[i].id = -1;
	for (i = 0; i < edgeListRaw.size(); i++)
		edgeListRaw[i].id = -1;
	for (i = 0; i < vertexListRaw.size(); i++)
		vertexListRaw[i].id = -1;


	//find usable, assing 0
	for (i = 0; i < volumeListRaw.size(); i++) {
		for (j = 0; j < volumeListRaw[i].surfaceList.size(); j++) {
			surfI = abs (volumeListRaw[i].surfaceList[j]) - 1;
			surfaceListRaw[surfI].id = 0;
			for (k = 0; k < surfaceListRaw[surfI].edgeList.size(); k++) {
				edgeI = abs (surfaceListRaw[surfI].edgeList[k]) - 1;
				edgeListRaw[edgeI].id = 0;
				v0I = edgeListRaw[edgeI].V0 - 1;
				v1I = edgeListRaw[edgeI].V1 - 1;
				vertexListRaw[v0I].id = 0;
				vertexListRaw[v1I].id = 0;
			}
		}
	}

	//renumber vertices
	k = 1;
	for (i = 0; i < vertexListRaw.size(); i++) {
		if (vertexListRaw[i].id == -1) continue;
		vertexListRaw[i].id = k;
		k += 1;
	}
	//renumber edges
	k = 1;
	for (i = 0; i < edgeListRaw.size(); i++) {
		if (edgeListRaw[i].id == -1) continue;
		edgeListRaw[i].id = k;
		k += 1;
	}
	//renumber surfaces
	k = 1;
	for (i = 0; i < surfaceListRaw.size(); i++) {
		if (surfaceListRaw[i].id == -1) continue;
		surfaceListRaw[i].id = k;
		k += 1;
	}

	//repair ids
	for (i = 0; i < edgeListRaw.size(); i++) {
		if (edgeListRaw[i].id == -1) continue;
		v0I = edgeListRaw[i].V0 - 1;
		v1I = edgeListRaw[i].V1 - 1;
		edgeListRaw[i].V0 = vertexListRaw[v0I].id;
		edgeListRaw[i].V1 = vertexListRaw[v1I].id;
	}
	for (i = 0; i < surfaceListRaw.size(); i++) {
		if (surfaceListRaw[i].id == -1) continue;
		surfaceListRaw[i].RepairIds (edgeListRaw);
	}
	for (i = 0; i < volumeListRaw.size(); i++) {
		volumeListRaw[i].RepairIds (surfaceListRaw);
		sort (volumeListRaw[i].surfaceList.begin(), volumeListRaw[i].surfaceList.end(), abscomp);
	}

	//copy not removed vertices
	for (i = 0; i < vertexListRaw.size(); i++) {
		if (vertexListRaw[i].id == -1) continue;
		newVertListRaw.push_back (vertexListRaw[i]);
	}
	//copy not removed edges
	for (i = 0; i < edgeListRaw.size(); i++) {
		if (edgeListRaw[i].id == -1) continue;
		newEdgeListRaw.push_back (edgeListRaw[i]);
	}
	//copy not removed surfaces
	for (i = 0; i < surfaceListRaw.size(); i++) {
		if (surfaceListRaw[i].id == -1) continue;
		newSurfaceListRaw.push_back (surfaceListRaw[i]);
	}
	vertexListRaw.swap (newVertListRaw);
	edgeListRaw.swap (newEdgeListRaw);
	surfaceListRaw.swap (newSurfaceListRaw);
	return true;
}



bool Converter::MergeSurfacesRaw()
{
	cout << "Merging Raw surfaces..." << endl;
	int i, j, k, l, minSurfID, id0, id1;
	vector<vector<int>> comm;
	vector<vector<int>> raw0;
	vector<vector<int>> raw1;
	vector<vector<int>> commRaw;
	vector<vector<int>> unqRaw0;
	vector<vector<int>> unqRaw1;
	vector<int> mergedFaceIds0;
	vector<int> mergedFaceIds1;
	vector<bool> used;
	bool del, del0, del1;
	//for each volume test his surrounding
	for (i = 0; i < volumeList.size(); i++) {
		for (j = i + 1; j < volumeList.size(); j++) {
			comm = FindCommonItems (volumeList[i].surfaceList, volumeList[j].surfaceList);
			if (comm.size() > 1) { //process volumes with more than one common surface
				mergedFaceIds0 = vector<int>();
				mergedFaceIds1 = vector<int>();
				//find comm surfaces in raw volume list and create commRaw
				raw0 = FindCommonSurfRaw (comm, volumeListRaw[i].surfaceList);
				raw1 = FindCommonSurfRaw (comm, volumeListRaw[j].surfaceList);
				commRaw = FindCommonRaw (raw0, raw1);
				unqRaw0 = FindUniqueRaw (raw0, raw1);
				unqRaw1 = FindUniqueRaw (raw1, raw0);
				//merge surfaces
				MergeSurfacesFromListRaw (commRaw, mergedFaceIds0);
				for (k = 0; k < mergedFaceIds0.size(); k++) {
					mergedFaceIds1.push_back (mergedFaceIds0[k]);
				}
				MergeSurfacesFromListRaw (unqRaw0, mergedFaceIds0);
				MergeSurfacesFromListRaw (unqRaw1, mergedFaceIds1);
				UpdateVolumeRaw (raw0, mergedFaceIds0, i);
				UpdateVolumeRaw (raw1, mergedFaceIds1, j);
			}
		}
	}
	return true;
}

bool Converter::UpdateVolumeRaw (vector<vector<int>> &comm, vector<int> &mergedFaceIds, int i)
{
	// update volume's surfaceList
	int l, k;
	bool del;
	for (k = 0; k < comm.size(); k++) {
		del = true;
		for (l = 0; l < mergedFaceIds.size(); l++) {
			if (mergedFaceIds[l] == comm[k][0])
				del = false;
		}
		if (!del)
			continue;
		//delete surfaces in volumes
		if (!volumeListRaw[i].DeleteItem (comm[k][0]))
			throw invalid_argument ("MergeSurfacesRaw: Can not delete item.");
	}
	return del;
}


vector<vector<int>> Converter::FindUniqueRaw (vector<vector<int>> &comm0, vector<vector<int>>& comm1)
{
	//find items in first list that distinguish from the second
	int i, j;
	bool unique;
	vector<vector<int>> commUn;
	for (i = 0; i < comm0.size(); i++) {
		unique = true;
		for (j = 0; j < comm1.size(); j++) {
			if (comm0[i][0] == comm1[j][0]) {
				unique = false;
				break;
			}
		}
		if (unique)
			commUn.push_back (comm0[i]);
	}
	return commUn;
}
vector<vector<int>> Converter::FindCommonRaw (vector<vector<int>> &comm0, vector<vector<int>>& comm1)
{
	//find items common for both lists
	int i, j;
	bool unique;
	vector<vector<int>> commUn;
	for (i = 0; i < comm0.size(); i++) {
		unique = true;
		for (j = 0; j < comm1.size(); j++) {
			if (comm0[i][0] == comm1[j][0]) {
				unique = false;
				break;
			}
		}
		if (!unique)
			commUn.push_back (comm0[i]);
	}
	return commUn;
}

vector<vector<int>> Converter::FindCommonSurfRaw (vector<vector<int>> &comm, vector<int> &surfL)
{
	//find all surface Id that matches common list according to mapping between unique and raw entities
	vector<vector<int>> commRaw;
	int k, l, id0, id1, idR2, idR1;
	for (k = 0; k < surfL.size(); k++) {
		id0 = abs (surfL[k]);
		for (l = 0; l < comm.size(); l++) {
			id1 = comm[l][0];
			idR1 = surfaceList[id1 - 1].mapRaw[0];
			idR2 = -1;
			if (surfaceList[id1 - 1].mapRaw.size() == 2)
				idR2 = surfaceList[id1 - 1].mapRaw[1];
			if (id0 == idR1 || id0 == idR2) {
				vector<int> item {id0, k, 0};
				commRaw.push_back (item);
				break;
			}
		}
	}
	sort (commRaw.begin(), commRaw.end(), compComm);
	return commRaw;
}



//private methods

void Converter::MergeSurfacesFromListRaw (vector<vector<int>> &comm, vector<int> &mergedFaceIds)
{
	//merge all facets from the comm list
	//raise an error if all facets could not be merged
	int i, j, k, id0, prevsize, minId;
	vector<vector<int>> commE;
	vector<int> u;
	vector<bool> used = vector<bool> (comm.size(), false);
	for (k = 0; k < comm.size(); k++) {
		minId = -1;
		bool first = true;
		for (i = 0; i < comm.size(); i++) {
			for (j = 0; j < comm.size(); j++) {
				if (!used[j]) {
					id0 = comm[j][0] - 1;
					commE = FindCommonItems (u, surfaceListRaw[id0].edgeList);
					if (commE.size() > 0 || u.size() == 0) {
						u = ListAbsUnion (u, surfaceListRaw[id0].edgeList, true);
						used[j] = true;
						if (first)
							minId = comm[j][0];
						first = false;
					}
				}
			}
		}
		if (u.size() == 0)
			break;
		mergedFaceIds.push_back (minId);
		//update surfaceList
		surfaceListRaw[minId - 1].edgeList.swap (u);
		u = vector<int> ();
	}
	for (k = 0; k < used.size(); k++) {
		if (!used[k]) {
			throw domain_error ("MergeSurfacesFromListRaw: Not all surfaces from the list were merged.");
		}
	}
}
