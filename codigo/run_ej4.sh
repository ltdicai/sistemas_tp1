#!bin/bash
make
./simusched lote_ej4_1.tsk 1 1 2 SchedRR | python graphsched.py > ej4_1_1.png
#./simusched lote_ej4_1.tsk 2 1 2 SchedRR | python graphsched.py > ej4_2.png
#./simusched lote_ej4_1.tsk 3 1 2 SchedRR | python graphsched.py > ej4_3.png
