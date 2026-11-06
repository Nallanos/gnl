/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abenameu <abenameu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 10:08:37 by abenameu          #+#    #+#             */
/*   Updated: 2026/11/06 13:00:53 by abenameu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	*free_all(char **lin, char **temp, char **buff)
{
	if (*lin)
	{
		free(*lin);
		*lin = NULL;
	}
	if (*temp)
	{
		free(*temp);
		*temp = NULL;
	}
	if (*buff)
	{
		free(*buff);
		*buff = NULL;
	}
	return (NULL);
}

void	rotate_buff(char buff[])
{
	char	*end_of_line;
	int		i;

	i = 0;
	end_of_line = ft_strchr(buff, '\n');
	if (!end_of_line)
	{
		buff[0] = '\0';
		return ;
	}
	end_of_line++;
	while (end_of_line[i])
	{
		buff[i] = end_of_line[i];
		i++;
	}
	buff[i] = '\0';
}

char	*create_line(int fd, char buff[])
{
	int		byte;
	char	*line;
	char	*temp;

	line = NULL;
	byte = 1;
	temp = ft_strdup(buff);
	while (byte > 0 && !ft_strchr(line, '\n'))
	{
		byte = read(fd, buff, BUFFER_SIZE);
		if (byte < 0)
			return (free_all(&line, &temp, NULL));
		buff[byte] = '\0';
		ft_strswap(&line, buff);
	}
	if (byte < 0)
	{
		if (line)
			free(line);
		if (temp)
			free(temp);
		return (NULL);
	}
	rotate_buff(buff);
	return (line);
}
char	*get_next_line(int fd)
{
	static char	buff[BUFFER_SIZE + 1];
	char		*line;

	if (fd < 0)
		return (NULL);
	if (BUFFER_SIZE == 10000)
		buff[BUFFER_SIZE] = '\0';
	line = create_line(fd, buff);
	return (line);
}
