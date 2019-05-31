echo "atpg for c$1 with ndet = $2 and the anum number is $3 backtrace $4"

Tfolder="../simruslt/$4"
Tfile="$Tfolder/c$1_anum$3_bt$4.log"
if [ ! -d  "$Tfolder" ]; then
	  mkdir "$Tfolder"
fi

(time ./atpg -ndet $2 -anum $3 -bt $4 -tdfatpg -compression ../sample_circuits/c$1.ckt ) >"$Tfile" 2>&1
../bin/golden_tdfsim -ndet $2 -tdfsim patterns ../sample_circuits/c$1.ckt >> "$Tfile"
python util.py "$Tfile"  >> "$Tfile"
#cat ../tdf_patterns/c$1.pat
#../bin/golden_tdfsim -ndet $2 -tdfsim ../tdf_patterns/c$1.pat ../sample_circuits/c$1.ckt
# ./atpg -ndet $2 -tdfsim ../tdf_patterns/c$1.pat ../sample_circuits/c$1.ckt

