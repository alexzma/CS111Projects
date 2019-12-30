#NAME: Alexander Ma
#EMAIL: alexanderzma@hotmail.com
#ID: 105093055
#!/bin/bash

total=0
passed=0

#test incorrect argument
total=$((total+1))
./lab0 sldkfnsldkfnlsknf 2>error
if [ $? -ne 1 ]
then
	cat error
	echo "Invalid argument test ... FAILED"
else
cat error
touch empty
cmp error empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Invalid argument test ... PASSED"
	passed=$((passed+1))
else
	echo "Invalid argument test ... FAILED"
fi
fi

#test invalid argument with single dash
total=$((total+1))
./lab0 -djfkdjfkj 2>error
if [ $? -ne 1 ]
then
	cat error
	echo "Invalid one dash argument test ... FAILED"
else
cat error
cmp error empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "Invalid one dash argument test ... FAILED"
else
	echo "Invalid one dash argument test ... PASSED"
	passed=$((passed+1))
fi
fi

#test invalid argument with two dashes
total=$((total+1))
./lab0 --sdfdfdfd 2>error
if [ $? -ne 1 ]
then
	cat error
	echo "Invalid two dash argument test ... FAILED"
else
cat error
cmp error empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "Invalid two dash argument test ... FAILED"
else
	echo "Invalid two dash argument test ... PASSED"
	passed=$((passed+1))
fi
fi

#test invalid argument after valid argument
total=$((total+1))
./lab0 --segfault sdlfkndlk 2>error
if [ $? -ne 1 ]
then
	cat error
	echo "Invalid following argument test ... FAILED"
else
cat error
cmp error empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Invalid following argument test ... PASSED"
	passed=$((passed+1))
else
	echo "Invalid following argument test ... FAILED"
fi
fi

#test invalid argument before valid argument
total=$((total+1))
./lab0 --segfault sdfdfdf 2>error
if [ $? -ne 1 ]
then
	cat error
	echo "Invalid preceding argument test ... FAILED"
else
cat error
cmp error empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Invalid preceding argument test ... PASSED"
	passed=$((passed+1))
else
	echo "Invalid preceding argument test ... FAILED"
fi
fi

#test no input file
total=$((total+1))
./lab0 --input 2>error
if [ $? -ne 1 ]
then
	cat error
	echo "No input file test ... FAILED"
else
cat error
cmp error empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "No input file test ... FAILED"
else
	echo "No input file test ... PASSED"
	passed=$((passed+1))
fi
fi

#test nonexistent input file
total=$((total+1))
./lab0 --input=nonexistent 2>error
if [ $? -ne 2 ]
then
	cat error
	echo "Nonexistent input file test ... FAILED"
else
cat error
cmp error empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "Nonexistent input file test ... FAILED"
else
	echo "Nonexistent input file test ... PASSED"
	passed=$((passed+1))
fi
fi

#test unreadable input file
total=$((total+1))
echo test >cannotread
chmod ugo-r cannotread
./lab0 --input=cannotread >output 2>error
if [ $? -ne 2 ]
then
	cat error
	cat output
	echo "Unreadable input file test ... FAILED"
else
cat error
cat output
cmp output empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Unreadable input file test ... FAILED"
else
cmp error empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "Unreadable input file test ... FAILED"
else
	echo "Unreadable input file test ... PASSED"
	passed=$((passed+1))
fi
fi
fi

#test no output file
total=$((total+1))
./lab0 --output 2>error
if [ $? -ne 1 ]
then
	cat error
	echo "No output file test ... FAILED"
else
cat error
cmp error empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "No output file test ... FAILED"
else
	echo "No output file test ... PASSED"
	passed=$((passed+1))
fi
fi

#test nonexistent output file
total=$((total+1))
echo test >input
./lab0 --output=output <input 2>error
if [ $? -ne 0 ]
then
	cat error
	echo "Nonexistent output file test ... FAILED"
else
cat error
cmp output input &>/dev/null
if [ $? -ne 0 ]
then
	echo "Nonexistent output file test ... FAILED"
else
cmp error empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Nonexistent output file test ... FAILED"
else
	echo "Nonexistent output file test ... PASSED"
	passed=$((passed+1))
fi
fi
fi


#test unwritable output file
total=$((total+1))
touch cannotwrite
chmod ugo-w cannotwrite
echo test >input
./lab0 --output=cannotwrite <input 2>error
if [ $? -ne 3 ]
then
	cat error
	echo "Unwritable output file test ... FAILED"
else
cat error
cmp error empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "Unwritable output file test ... FAILED"
else
	echo "Unwritable output file test ... PASSED"
	passed=$((passed+1))
fi
fi

#test standard input to standard output
total=$((total+1))
echo test >input
./lab0 <input >output 2>error
if [ $? -ne 0 ]
then
	cat error
	cat output
	echo "Standard input to standard output test ... FAILED"
else
cat error
cat output
cmp input output &>/dev/null
if [ $? -ne 0 ]
then
	echo "Standard input to standard output test ... FAILED"
else
cmp error empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "Standard input to standard output test ... PASSED"
	passed=$((passed+1))
else
	echo "Standard input to standard output test ... FAILED"
fi
fi
fi

#test standard input to output file
total=$((total+1))
echo test >input
./lab0 <input --output=output 2>error
if [ $? -ne 0 ]
then
	cat error
	echo "Standard input to output file test ... FAILED"
else
cat error
cmp input output &>/dev/null
if [ $? -ne 0 ]
then
	echo "Standard input to output file test ... FAILED"
else
cmp error empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Standard input to output file test ... FAILED"
else
	echo "Standard input to output file test ... PASSED"
	passed=$((passed+1))
fi
fi
fi

#test input file to standard output
total=$((total+1))
echo test >input
./lab0 --input=input >output 2>error
if [ $? -ne 0 ]
then
	cat error
	cat output
	echo "Input file to standard output test ... FAILED"
else
cat error
cat output
cmp input output &>/dev/null
if [ $? -ne 0 ]
then
	echo "Input file to standard output test ... FAILED"
else
cmp error empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Input file to standard output test ... FAILED"
else
	echo "Input file to standard output test ... PASSED"
	passed=$((passed+1))
fi
fi
fi

#test input file to output file
total=$((total+1))
echo 5 >input
./lab0 --input=input --output=output 2>error
if [ $? -ne 0 ]
then
	cat error
	echo "Input file to output file test ... FAILED"
else
cat error
cmp input output &>/dev/null
if [ $? -ne 0 ]
then
	echo "Input file to output file test ... FAILED"
else
cmp error empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Input file to output file test ... FAILED"
else
	echo "Input file to output file test ... PASSED"
	passed=$((passed+1))
fi
fi
fi

#test input files to output file
total=$((total+1))
echo 5 >input
echo 6 >input2
./lab0 --input=input --input=input2 --output=output 2>error
if [ $? -ne 0 ]
then
	cat error
	echo "Input files to output file test ... FAILED"
else
cat error
cmp input2 output &>/dev/null
if [ $? -ne 0 ]
then
	echo "Input files to output file test ... FAILED"
else
cmp error empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Input files to output file test ... FAILED"
else
	echo "Input files to output file test ... PASSED"
	passed=$((passed+1))
fi
fi
fi

#test input file to output files
total=$((total+1))
echo 5 >input
rm output
touch output
./lab0 --input=input --output=output --output=output2 2>error
if [ $? -ne 0 ]
then
	cat error
	echo "Input file to output files test ... FAILED"
else
cat error
cmp input output2 &>/dev/null
if [ $? -ne 0 ]
then
	echo "Input file to output files test ... FAILED"
else
cmp output empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Input file to output files test ... FAILED"
else
cmp error empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Input file to output files test ... FAILED"
else
	echo "Input file to output files test ... PASSED"
	passed=$((passed+1))
fi
fi
fi
fi

#test segfault
total=$((total+1))
./lab0 --segfault
if [ $? -ne 139 ]
then
	echo "Segfault test ... FAILED"
else
	echo "Segfault test ... PASSED"
	passed=$((passed+1))
fi

#test segfault catch
total=$((total+1))
./lab0 --segfault --catch 2>error
if [ $? -ne 4 ]
then
	cat error
	echo "Segfault catch test ... FAILED"
else
cat error
cmp error empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "Segfault catch test ... FAILED"
else
	echo "Segfault catch test ... PASSED"
	passed=$((passed+1))
fi
fi

#test segfault catch dump
total=$((total+1))
./lab0 --segfault --catch --dump
if [ $? -ne 139 ]
then
	echo "Segfault catch dump test ... FAILED"
else
	echo "Segfault catch dump test ... PASSED"
	passed=$((passed+1))
fi

#test segfault before input and output
total=$((total+1))
echo 5 >input
rm output
touch output
./lab0 --segfault --input=input --output=output
if [ $? -ne 139 ]
then
	echo "Segfault before I/O test ... FAILED"
else
cmp input output &>/dev/null
if [ $? -eq 0 ]
then
	echo "Segfault before I/O test ... FAILED"
else
	echo "Segfault before I/O test ... PASSED"
	passed=$((passed+1))
fi
fi

#test segfault after input and output
total=$((total+1))
echo 5 >input
rm output
touch output
./lab0 --input=input --output=output --segfault
if [ $? -ne 139 ]
then
	echo "Segfault after I/O test ... FAILED"
else
cmp input output &>/dev/null
if [ $? -eq 0 ]
then
	echo "Segfault after I/O test ... FAILED"
else
	echo "Segfault after I/O test ... PASSED"
	passed=$((passed+1))
fi
fi

chmod ugo+w cannotwrite
rm empty input output error cannotread cannotwrite input2 output2
echo "$passed tests passed out of $total tests"
