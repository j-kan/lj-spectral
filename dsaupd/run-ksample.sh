DIR=sample-297987-Lrw-Nev-8
echo $DIR

mkdir $DIR

echo "======> Started ", `date` > $DIR/output.txt
./kmsample297987-8.exe -g usample.pairs -d Lrw-8-eigenvectors.txt -o $DIR --normalize RW -r 10 >> $DIR/output.txt
echo "======> Finished ", `date` >> $DIR/output.txt

email -s $DIR jkan@iconstructs.com < `tail -12 $DIR/output.txt`DIR=sample-297987-Lrw-Nev-8


DIR=sample-297987-Lrw-Nev-16
echo $DIR

mkdir $DIR

echo "======> Started ", `date` > $DIR/output.txt
./kmsample297987-16.exe -g usample.pairs -d Lrw-16-eigenvectors.txt -o $DIR --normalize RW -r 10 >> $DIR/output.txt
echo "======> Finished ", `date` >> $DIR/output.txt

email -s $DIR jkan@iconstructs.com < `tail -12 $DIR/output.txt`
