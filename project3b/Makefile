#NAME: Alexander Ma
#EMAIL: alexanderzma@hotmail.com
#ID: 105093055

default: lab3b.c
	gcc -Wall -Wextra -g -O3 -o lab3b.out lab3b.c
	echo "#!/usr/local/cs/bin/bash" >lab3b
	echo 'numargs=$$#' >>lab3b
	echo 'if [ $$# -ne 1 ]' >>lab3b
	echo "then" >>lab3b
	echo 'echo "Invalid number of parameters: $$numargs" 1>&2' >>lab3b
	echo "exit 1" >>lab3b
	echo "fi" >>lab3b
	echo 'if [ ! -e $$1 ]' >>lab3b
	echo "then" >>lab3b
	echo 'echo "File $$1 does not exist" 1>&2' >>lab3b
	echo "exit 1" >>lab3b
	echo "fi" >>lab3b
	echo '#if [[ $$1 =~ "/-.*/" ]]' >>lab3b
	echo "#then" >>lab3b
	echo '#echo "Program does not take arguments such as: $$1" 1>&2' >>lab3b
	echo "#exit 1" >>lab3b
	echo "#fi" >>lab3b
	echo "if [ ! -e lab3b.out ]" >>lab3b
	echo "then" >>lab3b
	echo "gcc -Wall -Wextra -O3 -g -o lab3b.out lab3b.c" >>lab3b
	echo "fi" >>lab3b
	echo 'FILE=$$RANDOM' >>lab3b
	echo 'while [ -e $$FILE ]' >>lab3b
	echo "do" >>lab3b
	echo 'FILE=$$RANDOM' >>lab3b
	echo "done" >>lab3b
	echo 'grep -E "^SUPERBLOCK" $$1 >$$FILE' >>lab3b
	echo 'grep -E "^GROUP" $$1 >>$$FILE' >>lab3b
	echo 'grep -E "^BFREE" $$1 >>$$FILE' >>lab3b
	echo 'grep -E "^IFREE" $$1 >>$$FILE' >>lab3b
	echo 'grep -E "^INODE" $$1 >>$$FILE' >>lab3b
	echo 'grep -E "^INDIRECT" $$1 >>$$FILE' >>lab3b
	echo 'grep -E "^DIRENT" $$1 >>$$FILE' >>lab3b
	echo 'if [ ! -s $$FILE ]' >>lab3b
	echo 'then' >>lab3b
	echo 'echo "Invalid file format in $$1" 1>&2' >>lab3b
	echo 'rm -rf $$FILE' >>lab3b
	echo 'exit 1' >>lab3b
	echo 'fi' >>lab3b
	echo './lab3b.out $$FILE' >>lab3b
	echo 'output=$$?' >>lab3b
	echo 'rm -rf $$FILE' >>lab3b
	echo 'exit $$output' >>lab3b
	chmod ugo+x lab3b

FILES=Makefile README lab3b.c

dist: $(FILES)
	tar -czf lab3b-105093055.tar.gz $(FILES)

clean:
	rm -rf lab3b-105093055.tar.gz lab3b lab3b.out
	find . -name "[0-9]*" -delete
