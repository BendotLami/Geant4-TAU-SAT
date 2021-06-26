#!/bin/bash
for i in {0..10..1}
    do
        ./microelectronics test_$i.in > test_$i.out
        echo "Done test $i"
    done