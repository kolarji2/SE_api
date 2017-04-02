geo_checks := { 

  if torus then
  { errprintf "Cannot run 'geo' command in torus mode. Do 'detorus' first.\n";
    abort;
  };
}

geo := {
	geo_checks;
	RENUMBER_ALL;
	foreach vertex vv do {
		printf "Point (%d) = {%10.9f,%10.9f,%10.9f};\n",vv.id,vv.X,vv.Y,vv.Z;
	};
	
	foreach edge ee do {
			printf "Line (%d) = {%d,%d};\n",ee.id,ee.vertices[1].id,ee.vertices[2].id;
	};
	
	foreach facet ff do {
		printf "Line Loop (%d) = {",ff.id;
		inx:=0;
		foreach ff.edges ee do {
			if inx>0 then {
				printf ",";
			};
			printf "%d",ee.oid;
			inx++;
		};
		printf "};\n";
		printf "Plane Surface (%d) = {%d}; Physical Surface (%d) = {%d};\n",ff.id,ff.id,ff.id,ff.id;
	};
	
	foreach body bb do {
		printf "Surface Loop (%d) = {",bb.id;
		inx:=0;
		foreach bb.facets ff do {
			if inx>0 then {
				printf ",";
			};
			printf "%d",ff.oid;
			inx++;
		};
		printf "};\n";
		printf "Volume (%d) = {%d};\n",bb.id,bb.id;
	};
}
dmpgeo := {
	detorus;
	geo >>> "foam_dmp.geo";
};
