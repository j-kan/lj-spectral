DIR=sample-297987-Lrw-Nev-32 
echo $DIR

mkdir $DIR

echo "======> Started ", `date` > $DIR/output.txt
./sample297987.exe -g usample.pairs -o $DIR --normalize RW -r 10 >> $DIR/output.txt
echo "======> Finished ", `date` >> $DIR/output.txt

email -s $DIR jkan@iconstructs.com < `tail -12 $DIR/output.txt`
