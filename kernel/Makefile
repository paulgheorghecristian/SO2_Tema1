KDIR = /home/paul/Desktop/Facultate/Anul4/SO2/linux-4.9.11

kbuild:
	make -C $(KDIR) M=`pwd`

clean:
	make -C $(KDIR) M=`pwd` clean
	rm -f *~ Module.symvers Module.markers modules.order
