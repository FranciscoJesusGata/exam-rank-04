gcc -Wall -Werror -Wextra -I. -g microshell.c -o microshell
if [ $? -eq 0 ]; then
	./microshell $@
	status=$?
	rm -rf microshell microshell.dSYM
	exit $status
fi
