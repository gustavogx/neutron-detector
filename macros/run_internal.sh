
for pos in 1 2 6 7 9 10 12 14 15 16 17
do
    echo 
    echo ==== Distance ${pos} ==========================
    time ./arapuca -b ../macros/photons.batch -v 0 -p0 ${pos} -tag ${pos} -j 8 -o ../analysis/angle_study/data/internal_17M_vikuiti.dat  -a
done