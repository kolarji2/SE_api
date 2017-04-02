//count average angle between edges on original vertices
vangle_file:="vangle.data"
print "" >>> vangle_file
vangle := {
local vertex_angle,angle_variance;
local used_facet;
local xx,xx0;
local aa,bb;
vertex_orig_max:=0;
//count number of original edges
foreach vertex vv where id==original do{
	vertex_orig_max+=1;
};
maxvalence:=10;
define vertex_angle real[vertex_orig_max];
define angle_variance real[vertex_orig_max];
define used_facet integer[maxvalence];
define xx real[5][3];
vertex_angle:=0;
angle_variance:=0;
tangle:=0;
tvariance:=0;
inx:=1;
ideal:=109.47;
foreach vertex vv where id==original do {
	//for each facet compute a separate angle between two adjacent edges
	nna:=0;
	nnv:=0;
	//print "Vertex:";
	//print vv.id;
	currinx:=1;
	used_facet:=0;
	foreach vv.facet ff do {
	cff:=ff.original;
	used:=0;
	for (inf:=1;inf<=currinx;inf+=1) {
		if used_facet[inf]==cff then
		{
			used:=1;
		};
	};
	if used==0 then 
	{
		used_facet[currinx]:=cff;
		currinx+=1;
	} else {
		continue;
	};
	//kontrola zda na facet original
	iny:=1;
		//find adjacent edges
		foreach vv.edge ee where original>0 and valence>2 do {
			addedge:=0;
			foreach ee.facet ffe where original==ff.original do {
				addedge:=1;
			};
					
			if addedge==1 then
			{
			xx[iny]:=ee.edge_vector;
			iny:=iny+1;
			};
		};
		//compute vectors
		naa:=0;
		nbb:=0;
		vab:=0;
		for (inz:=1;inz<=3;inz+=1){
		naa+=xx[1][inz]^2;
		nbb+=xx[2][inz]^2;
		vab+=xx[1][inz]*xx[2][inz];
		};
		//compute angel
		naa:=sqrt(naa);
		nbb:=sqrt(nbb);
		angle0:=acos(vab/naa/nbb)*180.0/pi;
		dangle0:=abs(angle0-ideal);
		//print angle0;
		angle_variance[inx]+=angle0^2;
		vertex_angle[inx]+=angle0*dangle0;
		nna+=dangle0;
		nnv+=1;
	};
	//print "-----";
	//print nn;
	//print vertices[inx].valence;
	
	angle_variance[inx]:=angle_variance[inx]/nnv;
	vertex_angle[inx]:=vertex_angle[inx]/nna;
	set vv.angle vertex_angle[inx];
	angle_variance[inx]:=angle_variance[inx]-vertex_angle[inx]^2;
	tangle+=vertex_angle[inx];
	tvariance+=angle_variance[inx];
	inx+=1;
};
tangle:=tangle/vertex_orig_max;
tvariance:=tvariance/vertex_orig_max;
//print vertex_angle;

print tangle;
print sqrt(tvariance);
}; //end vangle
vangle_dump:={vangle >> vangle_file};

