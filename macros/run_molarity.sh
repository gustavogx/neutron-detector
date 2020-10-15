
for mol in {0..100}
do
    echo 
    echo ==== Distance ${pos} ==========================
    time ./arapuca -b ../macros/photons.batch -v 0 -j 8 -o ../analysis/angle_study/data/molarity_${mol}.dat -p1 ${mol}
done