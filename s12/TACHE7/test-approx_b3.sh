for fich in {"Asterix3","lettre-L-cursive","ColombesDeLaPaix"}; do
for dis in {"0","0.5","1","2","4","8","16"}; do 
echo Le nb contours pour $fich.pbm en d = $dis :
./bitmap-vectoriser -i image_test/$fich.pbm -o sortie_eps/$fich-d$dis-t$type.eps -d $dis -s fill -m bezier3
echo fichier de sortie est : sortie_eps/$fich-d$dis-t$type.eps
echo  " "
done
done