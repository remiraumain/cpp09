/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraumain <rraumain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 19:49:20 by rraumain          #+#    #+#             */
/*   Updated: 2025/10/22 10:56:50 by rraumain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"
#include <iostream>
#include <sstream>
#include <exception>

RPN::RPN()
{
	std::cout << "RPN constructed\n";
}

RPN::RPN(const RPN &other) : _stack(other._stack)
{
	std::cout << "RPN copy-constructed\n";
}

RPN &RPN::operator=(const RPN &other)
{
	if (this != &other)
	{
		_stack = other._stack;
		std::cout << "RPN copy-assigned\n";
	}
	return (*this);
}

RPN::~RPN()
{
	std::cout << "RPN destructed\n";
}

int RPN::calc(const std::string &args)
{
	std::istringstream iss(args);
	std::string arg;
	while (iss >> arg)
	{
		if (arg.size() ==  1 && std::isdigit(arg[0]))
			_stack.push(arg[0]-'0');
		else if (arg == "+" || arg == "-" || arg == "*" || arg == "/")
		{
			if (_stack.size() < 2)
				throw std::runtime_error("Error");
			int b = _stack.top();
			_stack.pop();
			int a = _stack.top();
			_stack.pop();

			if (arg == "+")
				_stack.push(a + b);
			else if (arg == "-")
				_stack.push(a - b);
			else if (arg == "*")
				_stack.push(a * b);
			else
			{
				if (b == 0)
					throw std::runtime_error("Error");
				_stack.push(a / b);
			}
		}
		else
			throw std::runtime_error("Error");
	}
	if (_stack.size() != 1)
		throw std::runtime_error("Error");
	return (_stack.top());
}
