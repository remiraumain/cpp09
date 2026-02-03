/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraumain <rraumain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/03 13:00:34 by rraumain          #+#    #+#             */
/*   Updated: 2026/02/03 13:48:26 by rraumain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector>
#include <deque>
#include <string>

class PmergeMe
{
	public:
		PmergeMe();
		PmergeMe(const PmergeMe &other);
		PmergeMe &operator=(const PmergeMe &other);
		~PmergeMe();

		void parseInput(int ac, char **av);
		void sort();
		void displayResults() const;

	private:
		std::vector<int> _vec;
		std::deque<int>  _deq;
		std::vector<int> _sortedVec;
		std::deque<int>  _sortedDeq;
		double           _timeVec;
		double           _timeDeq;

		void fordJohnsonVector(std::vector<int> &arr);
		void fordJohnsonDeque(std::deque<int> &arr);

		static std::vector<size_t> getJacobsthalOrder(size_t n);
		static double getTime();

		template <typename Container>
		static size_t binarySearch(const Container &arr, int value, size_t left, size_t right);
};

#endif
