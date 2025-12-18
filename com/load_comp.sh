#!/bin/bash  

echo Here are all the components in the sdk
echo "  "
ls -d */
echo "  "

############################### component list start ##############################
comp1="lvgl"
comp2="resv01"
comp3="resv02"
comp4="resv03"
comp5="resv04"
comp6="resv05"
comp7="resv06"

compBuffer=("lvgl" "resv01" "resv02" "resv03" "resv04" "resv05" "resv06")
############################### component list end ###############################

# Prompts the user for a folder name 
read -p "Please enter component names(supports up to 3):" compName_01 compName_02 compName_03
# Determine if input parameter 01 is empty
if [ -z "$compName_01" ]; then  
	exit
else
	# Use grep to find a string equal to a string from an array
	result=$(echo "${compBuffer[@]}" | grep -w "$compName_01")  
	# Checks if a matching string is found 
	if [ -n "$result" ]; then  
		echo find "$compName_01" and copying...
		{
			if [ "$compName_01" = "$comp1" ]; then
				cp -r "${compName_01}"/* ../src/tlklib/gui
			# elif [ "$compName_01" = "$comp2" ]; then
			# 	cp -r "${compName_01}" ../src/tlklib
			# elif [ "$compName_01" = "$comp3" ]; then
			# 	cp -r "${compName_01}" ../src/tlkdrv/ext
			# elif [ "$compName_01" = "$comp4" ]; then
			# 	cp -r "${compName_01}" ../src/tlkdrv/ext
			# elif [ "$compName_01" = "$comp5" ]; then
			# 	cp -r "${compName_01}" ../src/tlkdrv/ext
			# elif [ "$compName_01" = "$comp6" ]; then
			# 	cp -r "${compName_01}" ../src/tlkdrv/ext
			# elif [ "$compName_01" = "$comp7" ]; then
			# 	cp -r "${compName_01}" ../src/tlkdrv/ext	
			else
				echo "Third level if judgment error 01"
			fi
		}
	else  
		echo not find "$compName_01"
	fi
fi

# Determine if input parameter 02 is empty
if [ -z "$compName_02" ]; then  
	exit
else
	# Use grep to find a string equal to a string from an array
	result=$(echo "${compBuffer[@]}" | grep -w "$compName_02")  
	# Checks if a matching string is found 
	if [ -n "$result" ]; then  
		echo find "$compName_02" and copying...
		{
			if [ "$compName_02" = "$comp1" ]; then
				cp -r "${compName_02}"/* ../src/tlklib/gui
			# elif [ "$compName_02" = "$comp2" ]; then
			# 	cp -r "${compName_02}" ../src/tlklib
			# elif [ "$compName_02" = "$comp3" ]; then
			# 	cp -r "${compName_02}" ../src/tlkdrv/ext
			# elif [ "$compName_02" = "$comp4" ]; then
			# 	cp -r "${compName_02}" ../src/tlkdrv/ext
			# elif [ "$compName_02" = "$comp5" ]; then
			# 	cp -r "${compName_02}" ../src/tlkdrv/ext
			# elif [ "$compName_02" = "$comp6" ]; then
			# 	cp -r "${compName_02}" ../src/tlkdrv/ext
			# elif [ "$compName_02" = "$comp7" ]; then
			# 	cp -r "${compName_02}" ../src/tlkdrv/ext	
			else
				echo "Third level if judgment error 02"
			fi
		}
	else  
		echo not find "$compName_02"
	fi
fi

# Determine if input parameter 03 is empty
if [ -z "$compName_03" ]; then  
	exit
else
	# Use grep to find a string equal to a string from an array
	result=$(echo "${compBuffer[@]}" | grep -w "$compName_03")  
	# Checks if a matching string is found 
	if [ -n "$result" ]; then  
		echo find "$compName_03" and copying...
		{
			if [ "$compName_03" = "$comp1" ]; then
				cp -r "${compName_03}"/* ../src/tlklib/gui
			# elif [ "$compName_03" = "$comp2" ]; then
			# 	cp -r "${compName_03}" ../src/tlklib
			# elif [ "$compName_03" = "$comp3" ]; then
			# 	cp -r "${compName_03}" ../src/tlkdrv/ext
			# elif [ "$compName_03" = "$comp4" ]; then
			# 	cp -r "${compName_03}" ../src/tlkdrv/ext
			# elif [ "$compName_03" = "$comp5" ]; then
			# 	cp -r "${compName_03}" ../src/tlkdrv/ext
			# elif [ "$compName_03" = "$comp6" ]; then
			# 	cp -r "${compName_03}" ../src/tlkdrv/ext
			# elif [ "$compName_03" = "$comp7" ]; then
			# 	cp -r "${compName_03}" ../src/tlkdrv/ext	
			else
				echo "Third level if judgment error 03"
			fi
		}
	else  
		echo not find "$compName_03"
	fi
fi

sleep 1




