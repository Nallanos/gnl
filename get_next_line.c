/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abenameu <abenameu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 10:08:37 by abenameu          #+#    #+#             */
/*   Updated: 2026/11/06 12:53:22 by abenameu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	rotate_buff(char buff[])
{
	char	*end_of_line;
	int		i;

	i = 0;
	end_of_line = ft_strchr(buff, '\n') + 1;
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
	temp = NULL;
	byte = 1;
	while (byte > 0 && !ft_strchr(line, '\n'))
	{
		ft_strswap(&line, buff);
		byte = read(fd, buff, BUFFER_SIZE);
		buff[byte] = '\0';
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
