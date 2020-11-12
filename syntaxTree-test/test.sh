valgrind ../IFJ20 < test01_linux
echo $?
sleep 2
for file in *_linux 
do
   echo "------------------------------";
   echo "$file";
   echo "==============================";
   valgrind ../IFJ20 < "$file" 2> "res/res_$file.valgrind";
   errCode="$?"
   if [ "$errCode" == "0" ]	
   then
	   echo -e "\e[32mOK $errCode\e[0m"
   else
	echo -e "\e[31mVALGRIND ERROR $errCode\e[0m"
   fi
   #sleep 2;
done
for file in *.txt 
do
   echo "------------------------------";
   echo "$file";
   echo "==============================";
   valgrind ../IFJ20 < "$file" 2> "res/res_$file.valgrind";
   errCode="$?"
   if [ "$errCode" == "0" ]	
   then
	   echo -e "\e[32mOK $errCode\e[0m"
   else
	echo -e "\e[31mVALGRIND ERROR $errCode\e[0m"
   fi
  # sleep 2;
 #  valgrind ../IFJ20 < "$file";
   echo "$file";
   sleep 2;
done
