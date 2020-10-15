# Pos 0
for cut in 100
do
    echo 
    echo ==== Cut ${cut} ==========================
    time ./arapuca -b ../macros/photons.batch -v 0 -p0 0 -p9 ${cut} -tag ${cut} -j 8 -o ../results/cut_study/sipm_0_17M.dat  -a
done
