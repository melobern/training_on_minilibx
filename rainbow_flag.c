/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rainbow_flag.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbernard <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 14:10:35 by mbernard          #+#    #+#             */
/*   Updated: 2024/02/16 13:26:32 by mbernard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Convertir image en xpm pour que ca s affiche correctement
 *
 */
#include "mlx.h"
#include "rainbow_flag.h"

void	my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

void	square(t_data *img, int start, int side, int color)
{
	int	x;
	int	y;

	x = 0;
	while (x < side)
	{
		y = 0;
		while (y < side)
		{
			my_mlx_pixel_put(img, start + x, y, color);
			y++;
		}
		x++;
	}
}

void	rectangle_100(t_data *img, int start, int height, int color)
{
	int	width;
	int	x;
	int	y;

	x = 0;
	width = 600;
	while (x < width)
	{
		y = 0;
		while (y < height)
		{
			my_mlx_pixel_put(img, x, start + y, color);
			y++;
		}
		x++;
	}
}

void	rainbow(t_data *img)
{
	rectangle_100(img, 0, 100, 0xde0000);
	rectangle_100(img, 100, 100, 0xf78900);
	rectangle_100(img, 200, 100, 0xf7e700);
	rectangle_100(img, 300, 100, 0x027d20);
	rectangle_100(img, 400, 100, 0x004af7);
	rectangle_100(img, 500, 100, 0x730084);
}

int	main(void)
{
	void	*mlx;
	void	*mlx_window;
	t_data	img;

	mlx = mlx_init();
	img.img = mlx_new_image(mlx, 600, 600);
	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length,
			&img.endian);
	mlx_window = mlx_new_window(mlx, 600, 600, "My First Rainbow");
	rainbow(&img);
	mlx_put_image_to_window(mlx, mlx_window, img.img, 0, 0);
	mlx_loop(mlx);
	return (0);
}
