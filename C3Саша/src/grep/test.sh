#! /bin/bash

test_number=1
file=test_file

compare() {
    echo -e "Test number: $test_number\n\tFlags: $*"
    grep $* $file > grep_output
    ./s21_grep $* $file > s21_grep_output
    if [[ $(diff -q grep_output s21_grep_output) ]]
    then
        echo -e "\t$(diff -q grep_output s21_grep_output)"
        echo -e "\tResult: Output for \"$*\" is different"
        exit
    else
        echo -e "\tResult: Success"
    fi
    test_number=$((test_number + 1))
}

options=(-i -v -c -l -n -h -s -o)
patterns=("pattern" "-epattern" "-fpattern_file")

gcc genfiles.c -o genfiles
./genfiles

echo -e "\nOne flag tests:"

for flag1 in ${options[@]}
do
    for pattern in ${patterns[@]}
    do
        compare $flag1 $pattern
    done
done

echo -e "\nCombination of flags tests:"

for flag1 in ${options[@]}
do
    for flag2 in ${options[@]}
    do
        if [ $flag1 != $flag2 ]
        then
            for pattern in ${patterns[@]}
            do
                compare $flag1 $flag2 $pattern
            done
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
                for pattern in ${patterns[@]}
                do
                    compare $flag1 $flag2 $flag3 $pattern
                done
            fi
        done
    done
done

rm -rf test_file pattern_file genfiles grep_output s21_grep_output