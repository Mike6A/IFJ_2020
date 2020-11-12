#!/usr/bin/bash
count_OK_mem=0
count_ERR_mem=0
count_OK_err=0
count_ERR_err=0
for i in ./*
do
	if [[ $i == *"linux"* ]] || [[ $i == *"win"* ]]; then
		RESULT=$(valgrind ./../IFJ20 < $i 2>&1 >/dev/null)
		RESULT_MEM=$(grep "All heap blocks were freed" <<< "$RESULT" | wc -l)
		RESULT_ERR=$(grep "ERROR SUMMARY: 0" <<< "$RESULT" | wc -l)
		if [ "$RESULT_MEM" -eq "0" ] ; then
			echo -e "$i : \e[31mMEMORY LEAK\e[0m"
			count_ERR_mem=$((count_ERR_mem+1))
		else
			echo -e "$i : \e[32mMEM: OK\e[0m"
			count_OK_mem=$((count_OK_mem+1))
		fi
		if [ "$RESULT_ERR" -eq "0" ] ; then
			echo -e "$i : \e[31mERROR from context\e[0m"
			count_ERR_err=$((count_ERR_err+1))
		else
			echo -e "$i : \e[32mERR: OK\e[0m"
			count_OK_err=$((count_OK_err+1))
		fi
		echo "-----------------------------------------------"
	fi
done
echo "-----------------------------------------------"
echo "-----------------------------------------------"
echo -e "MEM:{\e[32mOK: $count_OK_mem\e[0m ,\e[31mERR: $count_ERR_mem\e[0m} | CON:{\e[32mOK: $count_OK_err\e[0m ,\e[31mERR: $count_ERR_err\e[0m}"
echo "-----------------------------------------------"
echo "-----------------------------------------------"
