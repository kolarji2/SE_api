test := {r;{g 10;u 10} 10;}
opt10:={nn := 1;while nn < 10 do { g 50;u 10;g 50;u 10;j 0.05;;nn:=nn+1}}
opt5:={nn := 1;while nn < 5 do { g 50;u 10;g 50;u 10;j 0.01;;nn:=nn+1}}
opt20:={r;nn := 1;while nn < 20 do { g 50;u 10;g 50;u 10;j 0.01;;nn:=nn+1}};

read "stl.cmd";
CONNECTED;
foreach body bb do {set bb.facet color white};
do_stl:={detorus;stl >>> "PeriodicRVE.stl"};
