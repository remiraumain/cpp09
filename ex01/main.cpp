/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraumain <rraumain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 12:35:28 by rraumain          #+#    #+#             */
/*   Updated: 2025/10/22 10:49:13 by rraumain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"
#include <iostream>

int main(int ac, char* av[])
{
	if (ac != 2)
	{
		std::cerr << "Error" << std::endl;
		return (1);
	}

	RPN rpn;
	try
	{
		std::cout << rpn.calc(av[1]) << "\n";
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	return (0);
}