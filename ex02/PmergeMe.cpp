/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraumain <rraumain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 13:00:37 by rraumain          #+#    #+#             */
/*   Updated: 2026/02/03 14:05:35 by rraumain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <climits>
#include <ctime>
#include <algorithm>

PmergeMe::PmergeMe() : _timeVec(0), _timeDeq(0) {}

PmergeMe::PmergeMe(const PmergeMe &other)
	: _vec(other._vec), _deq(other._deq),
	  _sortedVec(other._sortedVec), _sortedDeq(other._sortedDeq),
	  _timeVec(other._timeVec), _timeDeq(other._timeDeq) {}

PmergeMe &PmergeMe::operator=(const PmergeMe &other)
{
	if (this != &other)
	{
		_vec = other._vec;
		_deq = other._deq;
		_sortedVec = other._sortedVec;
		_sortedDeq = other._sortedDeq;
		_timeVec = other._timeVec;
		_timeDeq = other._timeDeq;
	}
	return *this;
}

PmergeMe::~PmergeMe() {}

double PmergeMe::getTime()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000000.0 + ts.tv_nsec / 1000.0;
}

void PmergeMe::parseInput(int ac, char **av)
{
	for (int i = 1; i < ac; ++i)
	{
		std::string arg(av[i]);
		if (arg.empty())
			throw std::runtime_error("Error");

		size_t j = 0;
		if (arg[0] == '+')
			j = 1;
		if (j >= arg.length())
			throw std::runtime_error("Error");

		for (; j < arg.length(); ++j)
		{
			if (!std::isdigit(arg[j]))
				throw std::runtime_error("Error");
		}

		long value = std::strtol(arg.c_str(), NULL, 10);
		if (value < 0 || value > INT_MAX)
			throw std::runtime_error("Error");

		_vec.push_back(static_cast<int>(value));
		_deq.push_back(static_cast<int>(value));
	}

	if (_vec.empty())
		throw std::runtime_error("Error");
}

std::vector<size_t> PmergeMe::getJacobsthalOrder(size_t n)
{
	std::vector<size_t> order;
	if (n == 0)
		return order;

	std::vector<size_t> jacobsthal;
	jacobsthal.push_back(0);
	jacobsthal.push_back(1);

	while (jacobsthal.back() < n)
	{
		size_t next = jacobsthal[jacobsthal.size() - 1] + 2 * jacobsthal[jacobsthal.size() - 2];
		jacobsthal.push_back(next);
	}

	std::vector<bool> inserted(n, false);

	for (size_t i = 1; i < jacobsthal.size(); ++i)
	{
		size_t jVal = jacobsthal[i];
		if (jVal > n)
			jVal = n;

		size_t prevJ = jacobsthal[i - 1];

		for (size_t k = jVal; k > prevJ; --k)
		{
			size_t idx = k - 1;
			if (idx < n && !inserted[idx])
			{
				order.push_back(idx);
				inserted[idx] = true;
			}
		}
	}

	for (size_t i = 0; i < n; ++i)
	{
		if (!inserted[i])
			order.push_back(i);
	}

	return order;
}

template <typename Container>
size_t PmergeMe::binarySearch(const Container &arr, int value, size_t left, size_t right)
{
	while (left < right)
	{
		size_t mid = left + (right - left) / 2;
		if (arr[mid] < value)
			left = mid + 1;
		else
			right = mid;
	}
	return left;
}

void PmergeMe::fordJohnsonVector(std::vector<int> &arr)
{
	size_t n = arr.size();

	if (n <= 1)
		return;

	if (n == 2)
	{
		if (arr[0] > arr[1])
			std::swap(arr[0], arr[1]);
		return;
	}

	bool hasStraggler = (n % 2 == 1);
	int straggler = hasStraggler ? arr[n - 1] : 0;

	std::vector<std::pair<int, int> > pairs;
	for (size_t i = 0; i + 1 < n; i += 2)
	{
		int a = arr[i];
		int b = arr[i + 1];
		if (a > b)
			pairs.push_back(std::make_pair(a, b));
		else
			pairs.push_back(std::make_pair(b, a));
	}

	std::vector<int> winners;
	for (size_t i = 0; i < pairs.size(); ++i)
		winners.push_back(pairs[i].first);

	fordJohnsonVector(winners);

	std::vector<int> sortedLosers(winners.size());
	for (size_t i = 0; i < winners.size(); ++i)
	{
		for (size_t j = 0; j < pairs.size(); ++j)
		{
			if (pairs[j].first == winners[i])
			{
				sortedLosers[i] = pairs[j].second;
				pairs[j].first = -1;
				break;
			}
		}
	}

	std::vector<int> mainChain;
	mainChain.push_back(sortedLosers[0]);
	for (size_t i = 0; i < winners.size(); ++i)
		mainChain.push_back(winners[i]);

	if (sortedLosers.size() > 1)
	{
		std::vector<int> pendingLosers(sortedLosers.begin() + 1, sortedLosers.end());
		std::vector<size_t> insertOrder = getJacobsthalOrder(pendingLosers.size());

		for (size_t i = 0; i < insertOrder.size(); ++i)
		{
			size_t idx = insertOrder[i];
			int loser = pendingLosers[idx];

			size_t limit = mainChain.size();
			int targetWinner = winners[idx + 1];
			for (size_t j = 0; j < mainChain.size(); ++j)
			{
				if (mainChain[j] == targetWinner)
				{
					limit = j;
					break;
				}
			}

			size_t pos = binarySearch(mainChain, loser, 0, limit);
			mainChain.insert(mainChain.begin() + pos, loser);
		}
	}

	if (hasStraggler)
	{
		size_t pos = binarySearch(mainChain, straggler, 0, mainChain.size());
		mainChain.insert(mainChain.begin() + pos, straggler);
	}

	arr = mainChain;
}

void PmergeMe::fordJohnsonDeque(std::deque<int> &arr)
{
	size_t n = arr.size();

	if (n <= 1)
		return;

	if (n == 2)
	{
		if (arr[0] > arr[1])
			std::swap(arr[0], arr[1]);
		return;
	}

	bool hasStraggler = (n % 2 == 1);
	int straggler = hasStraggler ? arr[n - 1] : 0;

	std::deque<std::pair<int, int> > pairs;
	for (size_t i = 0; i + 1 < n; i += 2)
	{
		int a = arr[i];
		int b = arr[i + 1];
		if (a > b)
			pairs.push_back(std::make_pair(a, b));
		else
			pairs.push_back(std::make_pair(b, a));
	}

	std::deque<int> winners;
	for (size_t i = 0; i < pairs.size(); ++i)
		winners.push_back(pairs[i].first);

	fordJohnsonDeque(winners);

	std::deque<int> sortedLosers(winners.size());
	for (size_t i = 0; i < winners.size(); ++i)
	{
		for (size_t j = 0; j < pairs.size(); ++j)
		{
			if (pairs[j].first == winners[i])
			{
				sortedLosers[i] = pairs[j].second;
				pairs[j].first = -1;
				break;
			}
		}
	}

	std::deque<int> mainChain;
	mainChain.push_back(sortedLosers[0]);
	for (size_t i = 0; i < winners.size(); ++i)
		mainChain.push_back(winners[i]);

	if (sortedLosers.size() > 1)
	{
		std::deque<int> pendingLosers(sortedLosers.begin() + 1, sortedLosers.end());
		std::vector<size_t> insertOrder = getJacobsthalOrder(pendingLosers.size());

		for (size_t i = 0; i < insertOrder.size(); ++i)
		{
			size_t idx = insertOrder[i];
			int loser = pendingLosers[idx];

			size_t limit = mainChain.size();
			int targetWinner = winners[idx + 1];
			for (size_t j = 0; j < mainChain.size(); ++j)
			{
				if (mainChain[j] == targetWinner)
				{
					limit = j;
					break;
				}
			}

			size_t pos = binarySearch(mainChain, loser, 0, limit);
			mainChain.insert(mainChain.begin() + pos, loser);
		}
	}

	if (hasStraggler)
	{
		size_t pos = binarySearch(mainChain, straggler, 0, mainChain.size());
		mainChain.insert(mainChain.begin() + pos, straggler);
	}

	arr = mainChain;
}

void PmergeMe::sort()
{
	_sortedVec = _vec;
	_sortedDeq = _deq;

	double startVec = getTime();
	fordJohnsonVector(_sortedVec);
	_timeVec = getTime() - startVec;

	double startDeq = getTime();
	fordJohnsonDeque(_sortedDeq);
	_timeDeq = getTime() - startDeq;
}

void PmergeMe::displayResults() const
{
	std::cout << "Before:";
	for (size_t i = 0; i < _vec.size(); ++i)
		std::cout << " " << _vec[i];
	std::cout << std::endl;

	std::cout << "After:";
	for (size_t i = 0; i < _sortedVec.size(); ++i)
		std::cout << " " << _sortedVec[i];
	std::cout << std::endl;

	std::cout << std::fixed << std::setprecision(3);
	std::cout << "Time to process a range of " << _vec.size()
			  << " elements with std::vector : " << _timeVec << " us" << std::endl;
	std::cout << "Time to process a range of " << _deq.size()
			  << " elements with std::deque : " << _timeDeq << " us" << std::endl;
}
