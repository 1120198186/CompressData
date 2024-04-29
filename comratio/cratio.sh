set -e

# spack env activate compress
# ./cratio.sh

cd /home/xulongshan/CompressData/comratio
mkdir -p cratios

numQubits=20

python main.py Grover $numQubits
python main.py QFT $numQubits
python main.py RandomRegular $numQubits
python main.py RandomMedium $numQubits
python main.py RandomRandom $numQubits
