
GCCPATH=~/Letöltések/gcc-arm-none-eabi-5_4-2016q3/bin/
GCCPREFIX=arm-none-eabi-
FLAGS="-I../../algorithm/ -I../../meta/ -I../../container/ -I../../ubiquitous/ -std=c++11 -c -fno-exceptions -fno-rtti -mthumb -mcpu=cortex-m4 -O3"
CXX=g++
CXX=$GCCPATH$GCCPREFIX$CXX
OBJD=objdump
OBJD=$GCCPATH$GCCPREFIX$OBJD

SYMS=$($CXX -c Inline.cpp $FLAGS)

SYMS=$($OBJD -t Inline.o | gawk '/\*UND\*/{print $4}')
NUMSYMS=$(echo $SYMS | sort | uniq | wc -w)

if [ $NUMSYMS -eq 1 ]; then 
	echo "OK!"
else 
	echo "Failed: external symbol references: "
	echo $SYMS
fi
