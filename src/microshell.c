/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgata-va <fgata-va@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 15:21:19 by fgata-va          #+#    #+#             */
/*   Updated: 2022/03/18 16:06:52 by fgata-va         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

void	ft_puterr(char *s, char *name) {
	write(2, "error: ", 7);
	while (*s) {
		write(2,s++,1);
	}
	write(2, " ", 1);
	while (*name) {
		write(2,name++,1);
	}
	write(2, "\n", 1);
	exit (1);
}

pid_t	execute(int first, int second, int io, char **args, char *const envp[]) {
	pid_t	pid;
	pid = fork();
	if (!pid) {
		close(first);
		if (second >= 0) {
			if ((dup2(second, io)) < 0)
				ft_puterr("fatal", NULL);
		}
		if (!args[0]) 
			exit(0);
		execve(args[0], args, envp);
		ft_puterr("cannot execute", *args);
	}
	return (pid);
}

int main(int argc, char *argv[], char *const envp[]) {
	int		i = 1;
	int		j = 1;
	char	**args;
	pid_t	pid;
	int		fds[2];
	int		exit_code = 0;
	int		tmp = -1;

	fds[0] = -1;
	fds[1] = -1;
	while (i < argc) {
		while (i < argc - 1 && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		if (!strcmp(argv[j], "cd")) {
			args = argv + j;
			if ((i - j) > 2)
				ft_puterr("cd:", "bad arguments");
			else if ((chdir(args[1])) < 0)
				ft_puterr("cd: cannot change directory to", args[1]);
			j = i + 1;
		}
		else if (!strcmp(argv[i], "|")) {
			argv[i] = NULL;
			args = argv + j;
			j = i + 1;
			if (fds[0] >= 0) {
				tmp = dup(STDIN_FILENO);
				dup2(fds[0], STDIN_FILENO);
				close(fds[0]);
			}
			pipe(fds);
			pid = execute(fds[0], fds[1], STDOUT_FILENO, args, envp);
			if (tmp >= 0) {
				dup2(tmp, STDIN_FILENO);
				close(tmp);
				tmp = -1;
			}
			close(fds[1]);
			waitpid(pid, &exit_code, 0);
		}
		else {
			if (!strcmp(argv[i], ";"))
				argv[i] = NULL;
			args = argv + j;
			j = i + 1;
			pid = execute(fds[1], fds[0], STDIN_FILENO, args, envp);
			close(fds[0]);
			close(fds[1]);
			if (!argv[i]) {
				fds[0] = -1;
				fds[1] = -1;
			}
			waitpid(pid, &exit_code, 0);
		}
		i++;
	}
	return (WEXITSTATUS(exit_code));
}
