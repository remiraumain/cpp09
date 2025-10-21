/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rraumain <rraumain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 17:47:19 by rraumain          #+#    #+#             */
/*   Updated: 2025/10/21 15:10:18 by rraumain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <sstream>

void ltrim(std::string &s)
{
	std::string::size_type i = s.find_first_not_of(" \t\n\r\f\v");
	s.erase(0, (i == std::string::npos) ? s.size() : i);
}

void rtrim(std::string &s)
{
    std::string::size_type i = s.find_last_not_of(" \t\n\r\f\v");
    if (i == std::string::npos)
		s.clear(); else s.erase(i + 1);
}
void trim(std::string &s)
{
	ltrim(s);
	rtrim(s);
}


bool isLeapYear(int year)
{
		if (year % 400 == 0)
			return (true);
		if (year % 100 == 0)
			return (false);
		return (year % 4 == 0);
}
int getRefDay(int year, int month)
{
	const int days[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if (month == 2)
		return (isLeapYear(year) ? 29 : 28);
    if (month < 1 || month > 12)
		return 0;
    return (days[month - 1]);
}

bool checkSeparator(const std::string &line, const char sep, const size_t amount, std::string& err)
{
	size_t count = std::count(line.begin(), line.end(), sep);

	if (count != amount)
	{
		err = "bad input => " + line;
		return (false);
	}
	return (true);
}

bool splitLine(const std::string &line, const char sep, std::string& err, std::string &date, double &value)
{
	if (!checkSeparator(line, sep, 1, err))
		return (false);
	
	size_t sep_pos = line.find(sep);
	
	date = line.substr(0, sep_pos);
	std::string valueStr = line.substr(sep_pos + 1);

    trim(date);
    trim(valueStr);

	if (date.empty() || valueStr.empty())
	{
		err = "bad input => " + line;
		return (false);
    }

	std::stringstream ss(valueStr);
	ss >> value;
	if (!ss || ss.peek() != std::char_traits<char>::eof())
	{
		err = "bad input => " + line;
		return false;
    }
	return (true);
}

bool checkHeader(const std::string &line, const std::string &ref, std::string& err)
{
	int cmp = line.compare(ref);
	
	if (cmp != 0)
	{
		err = "invalid header";
		return (false);
	}
	return (true);
}

bool checkDate(const std::string &date, std::string& err, const std::string &line)
{
	if (date.size() != 10 || date[4] != '-' || date[7] != '-')
	{
		err = "bad input => " + line;
		return (false);
	}

	const std::string yearStr = date.substr(0, 4);
	const std::string monthStr = date.substr(5, 2);
	const std::string dayStr = date.substr(8, 2);

	int year, month, day;
    {
        std::istringstream sy(yearStr), sm(monthStr), sd(dayStr);
        if (!(sy >> year) || sy.peek() != std::char_traits<char>::eof()
			|| !(sm >> month) || sm.peek() != std::char_traits<char>::eof()
			|| !(sd >> day) || sd.peek() != std::char_traits<char>::eof())
		{
			err = "bad input => " + line;
			return (false);
		}
    }

	if (month < 1 || month > 12)
	{
		err = "bad input => " + line;
		return (false);
	}

    int refDay = getRefDay(year, month);
    if (refDay < 1 || day > refDay || day < 1)
	{
		err = "bad input => " + line;
		return (false);
	}
	return (true);
}

bool checkValue(const double &value, std::string& err)
{
	if (value < 0.0)
		err = "not a positive number.";
	else if (value > 1000.0)
		err = "too large a number.";
	else
		return (true);
	return (false);
}

void print(const std::string &date, double &value, double rate)
{
	std::cout << date << " => " << value << " = " << value * rate << "\n";
}

BitcoinExchange::BitcoinExchange()
{
	std::cout << "BitcoinExchange constructed\n";
}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &other) : _db(other._db)
{
	std::cout << "BitcoinExchange copy-constructed\n";
}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &other)
{
	if (this != &other)
	{
		_db = other._db;
		std::cout << "BitcoinExchange copy-assigned\n";
	}
	return (*this);
}

BitcoinExchange::~BitcoinExchange()
{
	std::cout << "BitcoinExchange destructed\n";
}

double BitcoinExchange::getRate(const std::string &date) const
{
	std::map<std::string, double>::const_iterator it;

	if (_db.empty())
		throw std::logic_error("database empty");
	it = _db.find(date);
	if (it == _db.end())
	{
		it = _db.lower_bound(date);
		if (it == _db.begin())
			throw std::out_of_range("bad input");
		if (it == _db.end())
		{
			std::map<std::string, double>::const_iterator last = _db.end();
			it = --last;
		}
		else
			--it;
	}
	return (it->second);
}

void BitcoinExchange::loadCSV(const std::string &path)
{
	std::ifstream file(path.c_str());
	
	if (!file)
		throw std::logic_error("Error: could not open file");
	
	bool first_line = true;
	std::string line;
	std::string err;
	std::string date;
	double rate;
	while (std::getline(file, line))
	{
		if (first_line)
		{
			if (!checkHeader(line, "date,exchange_rate", err))
				throw std::logic_error("Error: " + err);
			first_line = false;
			continue;
		}
		if (!splitLine(line, ',', err, date, rate))
			throw std::logic_error("Error: " + err);
		if (!checkDate(date, err, line))
			throw std::logic_error("Error: " + err);
		fillDB(date, rate);
	}
}

void BitcoinExchange::exec(const std::string &path)
{
	std::ifstream file(path.c_str());
	
	if (!file)
		throw std::logic_error("Error: could not open file");
	
	bool first_line = true;
	std::string line;
	std::string err;
	std::string date;
	double value;
	while (std::getline(file, line))
	{
		if (first_line)
		{
			if (!checkHeader(line, "date | value", err))
				std::cout << "Error: " + err << "\n";
			first_line = false;
			continue;
		}
		if (!splitLine(line, '|', err, date, value))
			std::cout << "Error: " + err << "\n";
		else if (!checkDate(date, err, line))
			std::cout << "Error: " + err << "\n";
		else if (!checkValue(value, err))
			std::cout << "Error: " + err << "\n";
		else			
		{
			double rate;
			try
			{
				rate = getRate(date);
			}
			catch(const std::exception& e)
			{
				std::cerr << "Error: " << e.what() << " => " << line << '\n';
				continue;
			}
			print(date, value, rate);
		}
			
	}
}

void BitcoinExchange::fillDB(const std::string &date, const double &rate)
{
	_db[date] = rate;
}