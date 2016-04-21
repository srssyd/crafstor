source /opt/intel/vtune_amplifier_xe/amplxe-vars.sh
amplxe-cl -collect advanced-hotspots -knob collection-detail=stack-call-and-tripcount ./main
amplxe-cl -report callstacks `ls | grep 'ah' | tail -n 1` > report.txt
