#!/bin/bash

# cd to the required folder and execute ./cleanup_junk.sh

cleanup_junk()
{
	#echo $PWD
	rm -rf .svn
	rm -rf _svn
	rm -rf .directory
	
	for i in *
	do
	if [ -d "$i" ]
	then
		cd "$i"
		#echo $PWD
		rm -rf .svn
		rm -rf _svn
		rm -rf .directory
		rm -rf *~
		rm -rf *.bak
		cleanup_junk
		cd ..
	fi
	done
}

cleanup_junk
