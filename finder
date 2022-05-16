#!/bin/bash

DIR=./out_files

for var in "$@"     # for every argument you take
do
    tld=$var
    counter=0
    IFS='.  '       # setting . as delimiter  
    cut=$tld
    myarray=(0)
    while read line; do     # read the file
    	for word in $line; do   # for every word in the file
    	  keep=$word
    	  res=${word%%${cut}*}  # separate the word
	    if [ $tld = $keep ]; then # if the word is the input
	  	    myarray=($keep)
    	elif [ $myarray = $tld ]; then  # if the last "word" you read is the tld
    	    let counter=$counter+$word  # counter++
    	    myarray=(0)         # set the array back to 0
	    fi
    	done
    done < <(cat "$DIR"/*.out)
    echo "$tld $counter"
done