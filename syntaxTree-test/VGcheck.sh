#!/usr/bin/bash
for i in ./*
do
	if [[ $i == *"linux"* ]] || [[ $i == *"win"* ]]; then
		RESULT=$(valgrind ./../IFJ20 < $i 2>&1 >/dev/null)
		RESULT_MEM=$(grep "All heap blocks were freed" <<< "$RESULT" | wc -l)
		RESULT_ERR=$(grep "ERROR SUMMARY: 0" <<< "$RESULT" | wc -l)
		if [ "$RESULT_MEM" -eq "0" ] ; then
			echo $i : "MEMORY LEAK"
		else
			echo $i : "MEM: OK"
		fi
		if [ "$RESULT_ERR" -eq "0" ] ; then
			echo $i : "ERROR from context"
		else
			echo $i : "ERR: OK"
		fi
		echo "-----------------------------------------------"
	fi
done
