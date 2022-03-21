/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgata-va <fgata-va@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 20:18:25 by fgata-va          #+#    #+#             */
/*   Updated: 2022/03/21 21:20:12 by fgata-va         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

void	ft_puterr(char *s, char *name) {
	write(2, "error: ", 7);
	while (*s)
		write(2,s++,1);
	write(2, " ", 1);
	while (*name)
		write(2,name++,1);
	write(2, "\n",1);
	exit(1);
}

pid_t	execute(int first, int second, int io, char *argv[], char *const envp[]) {
	pid_t	pid;

	pid = fork();
	if (!pid) {
		close(first);
		if (second >= 0) {
			if ((dup2(second, io)) < 0)
				ft_puterr("fatal", NULL);
		}
		if (!(*argv))
			exit(0);
		execve(*argv, argv, envp);
		ft_puterr("cannot execute", *argv);
	}
	return (pid);
}

int main(int argc, char *argv[], char *const envp[]) {
	int		i;
	int		exit_code = 0;
	pid_t	pid;
	int		fds[2] = {-1, -1};
	int		tmp = -1;

	argv++;
	while (*argv) {
		i = 0;
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		if (!strcmp(*argv, "cd")) {
			if (i > 2)
				ft_puterr("cd:", "bad arguments");
			else if ((chdir(argv[1])) < 0)
				ft_puterr("cd: cannot change directory to", argv[1]);
			argv += i;
		} else if (argv[i] && !strcmp(argv[i], "|")) {
			argv[i++] = 0;
			if (fds[0] >= 0) {
				tmp = dup(STDIN_FILENO);
				dup2(fds[0], STDIN_FILENO);
				close(fds[0]);
			}
			pipe(fds);
			pid = execute(fds[0], fds[1], STDOUT_FILENO, argv, envp);
			close(fds[1]);
			if (tmp >= 0) {
				dup2(tmp, STDIN_FILENO);
				close(tmp);
				tmp = -1;
			}
			waitpid(pid, &exit_code, 0);
			argv += i;
		} else {
			if (argv[i] && !strcmp(argv[i], ";"))
				argv[i++] = 0;
			pid = execute(fds[1], fds[0], STDIN_FILENO, argv, envp);
			close(fds[0]);
			fds[0] = -1;
			close(fds[1]);
			fds[1] = -1;
			waitpid(pid, &exit_code, 0);
			argv += i;
		}
	}
	return (WEXITSTATUS(exit_code));
}
