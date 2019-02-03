/*
 * LinkStream.cpp
 *
 *  Created on: Feb 3, 2019
 *      Author: tim
 */

#include "LinkStream.hpp"
#include <string.h>

LinkStream::LinkStream()
{
	streamReadPosition = 0;
}

LinkStream::LinkStream(unsigned int initialSize)
{
	dataBuffer.reserve(initialSize);
	streamReadPosition = 0;
}

LinkStream::~LinkStream()
{
}

void LinkStream::appendValue(char value)
{
	dataBuffer.push_back(value);
}

void LinkStream::reserve(unsigned int size)
{
	dataBuffer.reserve(size);
}

void LinkStream::reset()
{
	dataBuffer.clear();
	streamReadPosition = 0;
}

void LinkStream::seekg(unsigned int position, unsigned int direction)
{
	auto currentPosition = streamReadPosition;
	if (direction == pos_beg)
	{
		currentPosition = position;
	}
	else if (direction == pos_end)
	{
		currentPosition = dataBuffer.size() - position;
	}
	else
	{
		currentPosition += position;
	}

	if (currentPosition > dataBuffer.size())
		currentPosition = dataBuffer.size();

	streamReadPosition = currentPosition;
}

unsigned int LinkStream::bytesAvailable()
{
	int result = (dataBuffer.size() - streamReadPosition);
	if (result < 0)
		result = 0;

	return result;
}

unsigned int LinkStream::streamSize()
{
	return dataBuffer.size();
}

char* LinkStream::begin()
{
	return &dataBuffer[0];
}

char* LinkStream::end()
{
	if (dataBuffer.size() == 0)
		return nullptr;
	return &dataBuffer[dataBuffer.size()];
}

LinkStream& operator<<(LinkStream& stream, char value)
{
	stream.appendValue(value);
	return stream;
}

LinkStream& operator<<(LinkStream& stream, unsigned char value)
{
	stream.appendValue(value);
	return stream;
}

LinkStream& operator<<(LinkStream& stream, const char* value)
{
	while (*value != '\0')
	{
		stream.appendValue(*value++);
	}
	stream.appendValue('\0');
	return stream;
}

LinkStream& operator<<(LinkStream& stream, LinkStream& value)
{
	for (auto& v : value)
	{
		stream << v;
	}

	return stream;
}

LinkStream& operator<<(LinkStream& stream, std::string& value)
{
	stream << static_cast<unsigned int>(value.size()) + 1;
	for (auto& character : value)
		stream << character;

	stream << '\0';
	return stream;
}

LinkStream& operator>>(LinkStream& stream, char& value)
{
	if (stream.bytesAvailable() < sizeof(char))
		throw "Failed to extract from CheckedStream. Not enough bytes available";

	value = stream.dataBuffer[stream.streamReadPosition++];
	return stream;
}

LinkStream& operator>>(LinkStream& stream, std::string& value)
{
	if (stream.bytesAvailable() < sizeof(unsigned int))
		throw "Failed to extract from CheckedStream. Not enough bytes available";

	unsigned int strLength;
	stream >> strLength;

	if (stream.bytesAvailable() < strLength)
			throw "Failed to extract from CheckedStream. Not enough bytes available";


	value.reserve(strLength - 1 + value.size());
	char temp;
	for(unsigned int i = 0; i < strLength; i++)
	{
		stream >> temp;
		value += temp;
	}

	return stream;
}
