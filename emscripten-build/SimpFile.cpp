#include "SimpFile.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <algorithm>

SimpFile::SimpFile(const std::string& fileName) : currentStream(fileName)
{
	if (currentStream)
	{
		std::string line;
		while (std::getline(currentStream, line))
		{
			parseAndAddLine(line);
		}
	}
	else
	{
		std::cout << "Cannot open file " << fileName << std::endl;
		throw std::invalid_argument("Cannot open file");
	}
}

std::vector<Command> SimpFile::commands() const
{
	return _commands;
}

std::vector<std::string> SimpFile::getTokens(const std::string& line)
{
	std::vector<std::string> result;
	static auto findDelimLambda = [](const char c)
	{
		return (c == ' ' || c == '\t' || c == ',' || c == '\n' || c == '(' || c == ')');
	};

	auto currentIter = line.cbegin();

	auto nextDelim = currentIter;
	while ((nextDelim = std::find_if(currentIter, line.cend(), findDelimLambda)) != line.cend())
	{
		if (std::distance(currentIter, nextDelim) > 0)
		{
			auto token = std::string(currentIter, nextDelim);
			result.push_back(token);
		}
		// If same as current
		else
		{
			// Move forward once
			nextDelim = std::next(nextDelim);
		}
		// Move the search forward
		currentIter = nextDelim;
	}

	if (std::distance(currentIter, nextDelim) > 0)
	{
		auto token = std::string(currentIter, nextDelim);
		result.push_back(token);
	}

	return result;
}

void SimpFile::parseAndAddLine(const std::string& line)
{
	auto tokens = getTokens(line);
	if (tokens.size() > 0 && (OperationTokens.find(tokens[0]) != OperationTokens.end()))
	{
		Command command(tokens);
		if (command.operation() == Command::Operation::File ||
			command.operation() == Command::Operation::ObjectFile)
		{
			auto fileName = mpark::get<std::string>(command.parameters());
			std::cout << "Opening file " << fileName << std::endl;
			if (command.operation() == Command::Operation::ObjectFile)
			{
				_commands.push_back(command);
			}
			
			SimpFile childFile(fileName);
			auto childCommands = childFile.commands();
			_commands.insert(_commands.end(), childCommands.begin(), childCommands.end());
			
			if (command.operation() == Command::Operation::ObjectFile)
			{
				_commands.push_back(Command{ Command::Operation::ObjectFile, "ENDOFOBJECTFILE"s });
			}
		}
		else
		{
			_commands.push_back(command);
		}

	}
}