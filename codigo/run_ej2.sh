#!bin/bash
make
./simusched lote_ej2.tsk 1 0 0 SchedFCFS | python graphsched.py > ej2_1.png
./simusched lote_ej2.tsk 2 0 0 SchedFCFS | python graphsched.py > ej2_2.png
./simusched lote_ej2.tsk 3 0 0 SchedFCFS | python graphsched.py > ej2_3.png
