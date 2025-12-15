/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gde-cast <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 12:18:19 by gde-cast          #+#    #+#             */
/*   Updated: 2025/12/15 12:18:23 by gde-cast         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*get_next_line(int fd)
{
	static char	*text;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	text = read_file(fd, text);
	if (!text)
		return (NULL);
	line = copy_line(text);
	text = get_remainder(text);
	return (line);
}

char	*read_file(int fd, char *text)
{
	char	*buff;
	int		bytes;

	buff = malloc(BUFFER_SIZE + 1);
	if (!buff)
		return (NULL);
	bytes = 1;
	while (bytes > 0 && (!text || !ft_strchr(text, '\n')))
	{
		bytes = read(fd, buff, BUFFER_SIZE);
		if (bytes == -1)
			return (free(buff), free(text), NULL);
		buff[bytes] = '\0';
		text = ft_strjoin(text, buff);
		if (!text)
			return (free(buff), NULL);
	}
	free(buff);
	return (text);
}

char	*copy_line(char *text)
{
	int		i;
	char	*line;

	if (!text || !*text)
		return (NULL);
	i = 0;
	while (text[i] && text[i] != '\n')
		i++;
	line = malloc(i + 2);
	if (!line)
		return (NULL);
	i = 0;
	while (text[i] && text[i] != '\n')
	{
		line[i] = text[i];
		i++;
	}
	if (text[i] == '\n')
		line[i++] = '\n';
	line[i] = '\0';
	return (line);
}

char	*get_remainder(char *text)
{
	int		i;
	int		j;
	char	*rest;

	i = 0;
	while (text[i] && text[i] != '\n')
		i++;
	if (!text[i])
		return (free(text), NULL);
	rest = malloc(ft_strlen(text) - i);
	if (!rest)
		return (free(text), NULL);
	i++;
	j = 0;
	while (text[i])
		rest[j++] = text[i++];
	rest[j] = '\0';
	free(text);
	return (rest);
}
