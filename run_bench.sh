BENCHS=~/python/pyperformance/pyperformance/benchmarks

if [ "$1" = "" ]; then
    echo "Usage: $0 JSON"
    exit 1
fi
JSON=$1

set -e -x

make distclean ||:

./configure --with-lto
make clean
make

rm -rf env
./python -m venv env
PYTHON=env/bin/python
$PYTHON -m pip install pyperf

for name in deltablue go telco; do
    $PYTHON $BENCHS/bm_$name.py --append=$JSON -v
done

echo "JSON: $JSON"
