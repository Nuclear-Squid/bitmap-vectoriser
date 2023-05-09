echo "-----------Extration de contours----------"
read -p "Appuyer pour continuer..." 
echo "-Le nb contours pour image-groupe1.pbm en type = nosimp (.contours)" :
time ./bitmap-vectoriser -i image_test/image-groupe1.pbm -o sortie_contours/image-groupe1.contours -s fill -m nosimp
echo le fichier de sortie est sortie_contours/image-groupe1.contours
echo  " "
echo "------------------------------------------"
echo "--------------Simplification -------------"
for type in {"segments","bezier2","bezier3"}; do 
for dis in {"2","8"}; do 
read -p "Appuyer pour continuer..." 
echo Le nb contours pour JoaquimHock-LesArbres.pbm en d = $dis et type = $type :
time ./bitmap-vectoriser -i image_test/JoaquimHock-LesArbres.pbm -o sortie_eps/JoaquimHock-LesArbres-d$dis-t$type.eps -d $dis -s fill -m $type
echo le fichier de sortie est sortie_eps/JoaquimHock-LesArbres-d$dis-t$type.eps
echo  " "
done
done
echo "------------------------------------------"

