#pragma once
#include "pch.h"

struct AutoFD
{
	AutoFD() : fd(-1) {}
	AutoFD(int fd) : fd(fd) {}
	~AutoFD() { if(!invalid()){  hangup();  }}
	bool invalid() const {return fd == -1;}
	void hangup()
	{
		close(fd);
		fd = -1;
	}
	
	AutoFD(AutoFD&& rhs)
	: fd(rhs.fd)
	{
		rhs.fd = -1;
	}

	AutoFD(const AutoFD&) = delete;
	AutoFD& operator=(const AutoFD&) = delete;

	int fd;
};