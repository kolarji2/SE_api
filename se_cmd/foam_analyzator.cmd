
//for each volume analyze number of edges per facet

an_simple := {
	local nedges;
	define ffperbody integer[max(body,id)];
	define eperbody integer[max(body,id)];
	define fftype integer[max(body,id)][10];
	//count number of original edges
	fftype:=0;

	foreach body bb do{
	for (ffId:=1;ffId<=max(facet,original);ffId++){
		//set body[bb.id].facet where original=ffId color ffId;
		nedges:=0;
		foreach bb.facet ff where original=ffId do {		
		foreach ff.edges ee where original>0 do {
			foreach ee.vertices[1] vv where original>0 do {
				nedges++;
			};
		};
		};			
		if nedges>=3 then {
			fftype[bb.id][nedges-2]++;
			
		};		
	}; //end for ffId original
	}; //end for bbId original
	print(fftype);
};

an := {
	RENUMBER_ALL;
	foreach body bb where original=1 do{
		for (bbId:=1;bbId<=max(body,id);bbId++){
			if bbId=bb.id then {
				continue;
			};
			bbId:=6;
			//printf "%d --- %d",bb.id, bbId;
			//list bodies[1].facet ff where ff.Backbody=bbId;
			//set bodies[1].facet color red where Backbody=bbId;
			break;
		};
	
	};

}

