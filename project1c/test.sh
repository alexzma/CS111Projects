#NAME: Alexander Ma
#EMAIL: alexanderzma@hotmail.com
#ID: 105093055

#!/usr/local/cs/bin/bash
total=0
passed=0

touch input
touch output
touch error
touch empty
touch shinput
touch shoutput
touch sherror

#test no options
total=$((total+1))
./simpsh >shoutput 2>sherror
if [ $? -ne 0 ]
then
	cat sherror
	echo "No option test ... FAILED"
else
cat sherror
cmp sherror empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "No option test ... FAILED"
else
cmp shoutput empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "No option test ... FAILED"
else
	echo "No option test ... PASSED"
	passed=$((passed+1))
fi
fi
fi

#test invalid no-dash option
total=$((total+1))
./simpsh sdfdfdd >shoutput 2>sherror
if [ $? -ne 1 ]
then
	cat sherror
	echo "Invalid no dash test ... FAILED"
else
cat sherror
cmp sherror empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "Invalid no dash test ... FAILED"
else
cmp shoutput empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Invalid no dash test ... FAILED"
else
	echo "Invalid no dash test ... PASSED"
	passed=$((passed+1))
fi
fi
fi

#test invalid single-dash option
total=$((total+1))
./simpsh -dfdfdf >shoutput 2>sherror
if [ $? -ne 1 ]
then
    cat sherror
    echo "Invalid single dash test ... FAILED"
else
cat sherror
cmp sherror empty &>/dev/null
if [ $? -eq 0 ]
then
    echo "Invalid single dash test ... FAILED"
else
cmp shoutput empty &>/dev/null
if [ $? -ne 0 ]
then
    echo "Invalid single dash test ... FAILED"
else
    echo "Invalid single dash test ... PASSED"
    passed=$((passed+1))
fi
fi
fi

#test invalid double-dash option
total=$((total+1))
./simpsh --dfdfdf >shoutput 2>sherror
if [ $? -ne 1 ]
then
    cat sherror
    echo "Invalid double dash test ... FAILED"
else
cat sherror
cmp sherror empty &>/dev/null
if [ $? -eq 0 ]
then
    echo "Invalid double dash test ... FAILED"
else
cmp shoutput empty &>/dev/null
if [ $? -ne 0 ]
then
    echo "Invalid double dash test ... FAILED"
else
    echo "Invalid double dash test ... PASSED"
    passed=$((passed+1))
fi
fi
fi

#test rdonly with no arg
total=$((total+1))
./simpsh --rdonly 2>sherror
if [ $? -ne 1 ]
then
	cat sherror
	echo "Read only no args test ... FAILED"
else
cat sherror
cmp sherror empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "Read only no args test ... FAILED"
else
	echo "Read only no args test ... PASSED"
	passed=$((passed+1))
fi
fi

#test rdonly with unreadable arg
total=$((total+1))
chmod u-r input
./simpsh --rdonly input 2>sherror
if [ $? -ne 1 ]
then
	cat sherror
	chmod u+r input
	echo "Read only unreadable arg test ... FAILED"
else
cat sherror
chmod u+r input
cmp sherror empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "Read only unreadable arg test ... FAILED"
else
	echo "Read only unreadable arg test ... PASSED"
	passed=$((passed+1))
fi
fi

#test wronly with no arg
total=$((total+1))
./simpsh --wronly 2>sherror
if [ $? -ne 1 ]
then
	cat sherror
	echo "Write only no args test ... FAILED"
else
cat sherror
cmp sherror empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "Write only no args test ... FAILED"
else
	echo "Write only no args test ... PASSED"
	passed=$((passed+1))
fi
fi

#test wronly with unwritable arg
total=$((total+1))
chmod u-w output
./simpsh --wronly output 2>sherror
if [ $? -ne 1 ]
then
	cat sherror
	chmod u+w output
	echo "Write only unwritable arg test ... FAILED"
else
cat sherror
chmod u+w output
cmp sherror empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "Write only no args test ... FAILED"
else
	echo "Write only no args test ... PASSED"
	passed=$((passed+1))
fi
fi

#test basic use
total=$((total+1))
echo input >input
rm output
touch output
./simpsh --verbose --rdonly input --wronly output --wronly error --command 0 1 2 sort >shoutput 2>sherror
if [ $? -ne 0 ]
then
	cat sherror
	cat shoutput
	echo "Basic test ... FAILED"
else
cat sherror
cat shoutput
cmp error empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Basic test ... FAILED"
else
cmp shoutput empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "Basic test ... FAILED"
else
cmp sherror empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Basic test ... FAILED"
else
sleep 1
cmp input output &>/dev/null
if [ $? -ne 0 ]
then
	echo "Basic test ... FAILED"
else
	echo "Basic test ... PASSED"
	passed=$((passed+1))
fi
fi
fi
fi
fi

#test wrong input in middle
total=$((total+1))
rm output
touch output
./simpsh --rdonly input --wronly non --wronly output --wronly error --command 0 2 3 cat >shoutput 2>sherror
if [ $? -eq 0 ]
then
	cat sherror
	cat shoutput
	echo "Wrong in middle test ... FAILED"
else
cat sherror
cat shoutput
cmp shoutput empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Wrong in middle test ... FAILED"
else
cmp sherror empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "Wrong in middle test ... FAILED"
else
sleep 1
cmp input output &>/dev/null
if [ $? -ne 0 ]
then
	echo "Wrong in middle test ... FAILED"
else
cmp error empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Wrong in middle test ... FAILED"
else
	echo "Wrong in middle test ... PASSED"
	passed=$((passed+1))
fi
fi
fi
fi
fi

#test multiple single dash options concatenated
total=$((total+1))
echo /dev/null >output
./simpsh --rdonly input --wronly output --wronly error --command 0 1 2 ls -la >shoutput 2>sherror
if [ $? -ne 0 ]
then
	cat sherror
	cat shoutput
	echo "Single dash cat test ... FAILED"
else
cat sherror
cat shoutput
sleep 1
cmp output empty &>/dev/null
if [ $? -eq 0 ]
then
	echo "Single dash cat test ... FAILED"
else
cmp error empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Single dash cat test ... FAILED"
else
cmp shoutput empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Single dash cat test ... FAILED"
else
cmp sherror empty &>/dev/null
if [ $? -ne 0 ]
then
	echo "Single dash cat test ... FAILED"
else
	echo "Single dash cat test ... PASSED"
	passed=$((passed+1))
fi
fi
fi
fi
fi

echo "Passed $passed tests out of $total tests"

cat error

rm input output error empty shinput shoutput sherror

#echo ""
#echo "Timed test for bash, dash, and simpsh using find on directory /usr/local/cs:"
#echo "bash"
#bash1=`$(time --format="Total: %e s\nSystem: %S s\nUser: %U s" bash -c "find /usr/local/cs </dev/urandom &>/dev/null" exit)`
#echo $bash1

#time --format="Total: %e s\nSystem: %S s\nUser: %U s" bash -c "find /usr/local/cs </dev/urandom &>/dev/null" exit
#grep -E -o "l[0-9]\.[0-9][0-9][0-9]" bash1times >bash1total
#bash1total=`$(sed s/l//g bash1total)`
#grep -E -o "m[0-9]\.[0-9][0-9][0-9]" bash1times >bash1system
#bash1system=`$(sed s/m//g bash1system)`
#grep -E -o "r[0-9]\.[0-9][0-9][0-9]" bash1times >bash1user
#bash1user=`$(sed s/r//g bash1user)`

#rm bash1times bash1total bash1system bash1user

#time --format="Total%e\nSystem%S\nUser%U" bash -c "find /usr/local/cs </dev/urandom &>/dev/null" &>bash2times exit
#grep -E -o "l[0-9]\.[0-9][0-9][0-9]" bash2times >bash2total
#bash2total=`$(sed s/l//g bash2total)`
#grep -E -o "m[0-9]\.[0-9][0-9][0-9]" bash2times >bash2system
#bash2system=`$(sed s/m//g bash2system)`
#grep -E -o "r[0-9]\.[0-9][0-9][0-9]" bash2times >bash2user
#bash2user=`$(sed s/r//g bash2user)`

#rm bash2times bash2total bash2system bash2user

#time --format="Total%e\nSystem%S\nUser%U" bash -c "find /usr/local/cs </dev/urandom &>/dev/null" &>bash3times exit
#grep -E -o "l[0-9]\.[0-9][0-9][0-9]" bash3times >bash3total
#bash3total=`$(sed s/l//g bash3total)`
#grep -E -o "m[0-9]\.[0-9][0-9][0-9]" bash3times >bash3system
#bash3system=`$(sed s/m//g bash3system)`
#grep -E -o "r[0-9]\.[0-9][0-9][0-9]" bash3times >bash3user
#bash3user=`$(sed s/r//g bash3user)`

#rm bash3times bash3total bash3system bash3user

#echo "Total: $bash1total s"
#echo "System: $bash1system s"
#echo "User: $bash1user s"

#echo ""
#echo "dash"
#time --format="Total: %e s\nSystem: %S s\nUser: %U s" dash -c "find /usr/local/cs </dev/urandom >/dev/null" exit

#echo ""
#echo "simpsh"
#time --format="Total: %e s\nSystem: %S s\nUser: %U s" ./simpsh --rdonly /dev/urandom --wronly /dev/null --wronly /dev/null --command 0 1 2 find /usr/local/cs
