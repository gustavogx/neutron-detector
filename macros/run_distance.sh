
for dist in 0 2 4 5 6 8 10 12 15 18 20
do
    echo 
    echo ==== Distance ${dist} ==========================
    time ./arapuca -b ../macros/photons.batch -v 0 -p8 ${dist} -p9 1 -j 8 -o CircCut_20M_Dist_${dist}.dat
done