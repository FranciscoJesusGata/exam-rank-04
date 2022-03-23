/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgata-va <fgata-va@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/23 16:55:21 by fgata-va          #+#    #+#             */
/*   Updated: 2022/03/23 19:00:40 by fgata-va         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void	ft_puterr(char *s, char *name) {
	write(1, "error: ", 7);
	while (*s)
		write(2, s++, 1);
	write(2, " ", 1);
	while (*name)
		write(2, name++, 1);
	write(2, "\n", 1);
	exit(1);
}

pid_t	execute(int first, int second, int io, char *argv[], char *const envp[]) {
	pid_t	pid = fork();
	if (!pid) {
		close (first);
		if (second >= 0 && (dup2(second, io)) < 0)
			ft_puterr("fatal", NULL);
		if (!argv[0])
			exit(0);
		if ((execve(*argv, argv, envp)) < 0)
			ft_puterr("cannot execute", *argv);
	}
	return (pid);
}

int	main(int argc, char *argv[], char *envp[]) {
	int		i;
	pid_t	pid;
	int		exit_code = 0;
	int		fds[2] = {-1, -1};
	int		tmp = dup(STDIN_FILENO);

	(void)argc;
	argv++;
	while (*argv) {
		i = 0;
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		if (!(strcmp(*argv, "cd"))) {
			if (i != 2) {
				close(tmp);
				ft_puterr("cd:", "bad arguments");
			} else if ((chdir(argv[1])) < 0) {
				close(tmp);
				ft_puterr("cd: cannot change directory to", argv[1]);
			}
		}
		else if (argv[i] && !(strcmp(argv[i], "|"))) {
			argv[i++] = NULL;
			if (fds[0] >= 0) {
				if ((dup2(fds[0], STDIN_FILENO)) < 0) {
					close(tmp);
					close(fds[0]);
					ft_puterr("fatal", NULL);
				}
				close(fds[0]);
			}
			pipe(fds);
			pid = execute(fds[0], fds[1], STDOUT_FILENO, argv, envp);
			dup2(tmp, STDIN_FILENO);
			waitpid(pid, &exit_code, 0);
			close(fds[1]);
		} else {
			if (argv[i] && !(strcmp(argv[i], ";")))
				argv[i++] = NULL;
			pid = execute(fds[1], fds[0], STDIN_FILENO, argv, envp);
			close(fds[0]);
			fds[0] = -1;
			close(fds[1]);
			fds[1] = -1;
			waitpid(pid, &exit_code, 0);
		}
		argv += i;
	}
	close(tmp);
	return (WEXITSTATUS(exit_code));
}
