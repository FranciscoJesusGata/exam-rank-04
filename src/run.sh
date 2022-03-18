gcc -Wall -Werror -Wextra -I. -g microshell.c -o microshell
if [ $? -eq 0 ]; then
	exec ./microshell $@
fi
