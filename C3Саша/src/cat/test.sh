#!/bin/bash

test_number=1

compare() {
    if [ $# == 3 ]
    then
        opts="$1 $2 $3"
    elif [ $# == 2 ]
    then
        opts="$1 $2"
    else
        opts=$1
    fi
    echo -e "Test number: $test_number\n\tFlags: $opts"
    cat $opts test_file > cat_output
    ./s21_cat $opts test_file > s21_cat_output
    if [[ $(diff -q cat_output s21_cat_output) ]]
    then
        echo -e "\t$(diff -q cat_output s21_cat_output)"
        echo -e "\tResult: Output for \"$opts\" is different\n"
        exit
    else
        echo -e "\tResult: Success\n"
    fi
    test_number=$((test_number + 1))
}

options=(-b -e -n -s -t -v)

gcc genfile.c -o genfile
./genfile

echo -e "\nOne short flag tests:"

for flag1 in ${options[@]}
do
    compare $flag1
done

echo -e "\nCombination of flags tests:"

for flag1 in ${options[@]}
do
    for flag2 in ${options[@]}
    do
        if [ $flag1 != $flag2 ]
        then
        
        compare $flag1 $flag2
        fi
    done
done

for flag1 in ${options[@]}
do
    for flag2 in ${options[@]}
    do
        for flag3 in ${options[@]}
        do
            if [ $flag1 != $flag2 ] && [ $flag1 != $flag3 ] && [ $flag2 != $flag3 ]
            then
            compare $flag1 $flag2 $flag3
            fi
        done
    done
done

rm test_file cat_output s21_cat_output genfile



