//Clear histograms
ehist_file:="ehist.data"
fhist_file:="fhist.data"
vhist_file:="vhist.data"
print "" >>> ehist_file
print "" >>> fhist_file
print "" >>> vhist_file
ehist_counter:=0
fhist_counter:=0
vhist_counter:=0
//edge length
ehist := {
	local edges_length,minl,maxl,bins;
	local range,step,val,iny;
	local left,right, mean;
	local hist;
	bins:=10;
	edge_orig_max:=0;
	//count number of original edges
	foreach edge ee do{
		if edge_orig_max<ee.original then
		{
			edge_orig_max:=ee.original;
		};
	};
	define edges_length real[edge_orig_max];
	define hist integer[bins];
	hist:=0;
	edges_length:=0;
	//count the length of each original edge
	foreach edge ee where original>0 do{
			edges_length[ee.original]+=ee.length;	
	};
	minl:=1e30;
	maxl:=0;
	//count max and min length
	for (inx:=1;inx<=edge_orig_max;inx+=1) {		
		if maxl<edges_length[inx] then
		{
			maxl:=edges_length[inx];
		};
		if minl>edges_length[inx] then
		{
			minl:=edges_length[inx];
		};
	};
	range:=(maxl-minl);
	step:=range/bins;
	//fill the bins
	for (inx:=1;inx<=edge_orig_max;inx+=1) {
		for(iny:=1;iny<bins;iny+=1) {
			val:=(edges_length[inx]-minl)/step;
			if val<iny then
				break;
		};
		hist[iny]+=1;
	};
	//write histogram to output 
	printf "%d \t left \t right \t mean \t count \n",ehist_counter;
	for (inx:=1;inx<=bins;inx+=1) {
		right:=inx*step+minl;
		left:=(inx-1)*step+minl;
		mean:=(left+right)/2;
		printf "%f \t %f \t %f \t %d \n",left, right, mean, hist[inx];
	};
}; //end edge length
ehist_dump:={ehist >> ehist_file}


//face area
fhist := {
	local faces_area,mina,maxa,bins;
	local range,step,val,iny;
	local left,right, mean;
	local hist;
	bins:=10;
	facet_orig_max:=0;
	//count number of original facet
	foreach facet ff do{
		if facet_orig_max<ff.original then
		{
			facet_orig_max:=ff.original;
		};
	};
	define faces_area real[facet_orig_max];
	define hist integer[bins];
	hist:=0;
	edges_length:=0;
	//count area for each facet
	foreach facet ff where original>0 do{
			faces_area[ff.original]+=ff.area;	
	};
	mina:=1e30;
	maxa:=0;
	//count min and max area
	for (inx:=1;inx<=facet_orig_max;inx+=1) {		
		if maxa<faces_area[inx] then
		{
			maxa:=faces_area[inx];
		};
		if mina>faces_area[inx] then
		{
			mina:=faces_area[inx];
		};
	};
	//fill the bins
	range:=(maxa-mina);
	step:=range/bins;
	for (inx:=1;inx<=facet_orig_max;inx+=1) {
		for(iny:=1;iny<bins;iny+=1) {
			val:=(faces_area[inx]-mina)/step;
			if val<iny then
				break;
		};
		hist[iny]+=1;
	};
	//write histogram to output 
	printf "%d \t left \t right \t mean \t count \n", fhist_counter;
	for (inx:=1;inx<=bins;inx+=1) {
		right:=inx*step+mina;
		left:=(inx-1)*step+mina;
		mean:=(left+right)/2;
		printf "%f \t %f \t %f \t %d \n",left, right, mean, hist[inx];
	};
};

vhist:={
	local range,step,val,iny,inx,body_max;
	local left,right, mean;
	local bins,minv,maxv;
	local hist;
	local total_vol;
	bins:=10;
	minv:=1e30;
	maxv:=0;
	body_max:=0;
	total_vol:=0;
	define hist integer[bins];
	foreach body bb do{
		if maxv<bb.volume then
		{
			maxv:=bb.volume;
		};
		if minv>bb.volume then
		{
			minv:=bb.volume;
		};
		total_vol+=bb.volume;
		body_max+=1;
	};
	//fill the bins
	range:=(maxv-minv);
	step:=range/bins;
	foreach body bb do {
		for(iny:=1;iny<bins;iny+=1) {
			val:=(bb.volume-minv)/step;
			if val<iny then
				break;
		};
		hist[iny]+=1;
	};
	//write histogram to output
	printf "%d \t left \t right \t mean \t count \t total volume: %f \n",vhist_counter, total_vol;
	for (inx:=1;inx<=bins;inx+=1) {
		right:=inx*step+minv;
		left:=(inx-1)*step+minv;
		mean:=(left+right)/2;
		printf "%f \t %f \t %f \t %d \n",left, right, mean, hist[inx];
	};
};
ehist_dump:={ehist >> ehist_file;print "\n" >> ehist_file;ehist_counter+=1};
fhist_dump:={fhist >> fhist_file;print "\n" >> fhist_file;fhist_counter+=1};
vhist_dump:={vhist >> vhist_file;print "\n" >> vhist_file;vhist_counter+=1};
logg:={ehist_dump;fhist_dump;vhist_dump};
