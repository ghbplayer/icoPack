#include "pch.h"

namespace fs = std::filesystem;

void readFile(const char* path, std::string& out)
{
	struct stat fi;
	if (::stat(path, &fi))
		throw std::runtime_error("Failed to stat");
	AutoFD fd = open(path, O_RDONLY, 0);
	if (fd.invalid())
		throw std::runtime_error("Failed to open");
	out.resize(fi.st_size);
	if (out.size() != uint(::read(fd.fd, out.data(), out.size())))
		throw std::runtime_error("Failed to read");
}

struct ICOheader
{
	uint16_t zero = 0;
	uint16_t type = 1;
	uint16_t count;
};

struct ICOimageHeader
{
	uint8_t width;
	uint8_t height;
	uint8_t palette = 0;
	uint8_t rzero = 0;
	uint16_t planes = 1;
	uint16_t bpp;
	uint32_t bytes;
	uint32_t offset;
};

const u_char PNGheader[8] = {0x89, 'P', 'N', 'G', 0xD, 0xA, 0x1A, 0xA};

struct Target
{
	Target(std::string name) : name(name) {}
	std::string name;
	std::string data;
	uint16_t bpp = 32;
	bool png = false;
	bool validate()
	{
		const char* raw = data.data();
		if (!memcmp(raw, PNGheader, 8))
		{
			png = true;
			return true;
		}

		const ICOheader& h1 = *reinterpret_cast<const ICOheader*>(raw);
		if (h1.zero != 0 || h1.type != 1 || h1.count != 1)
		{
			std::cout << "Error: " << name << " header not accepted.\n";
			return false;
		}

		const ICOimageHeader& h2 = *reinterpret_cast<const ICOimageHeader*>(raw + sizeof(ICOheader));
		if (h2.rzero != 0 || h2.bytes != data.size() - 22 || h2.offset != 22)
		{
			std::cout << "Error: " << name << " header2 not accepted.\n";
			return false;
		}

		bpp = h2.bpp;
		return true;
	}

	uint32_t dataLength() const { return data.size() - (png ? 0 : 22); }
	const char* payload() const { return data.data() + (png ? 0 : 22); }
};

struct NameComp
{
	bool operator() (const Target& l, const Target& r) const
	{
		if (l.name.size() < r.name.size())
			return true;
		else if (l.name.size() > r.name.size())
			return false;
		else
			return l.name < r.name;
	}
};

struct Invalid
{
	bool operator() (Target& t) const
	{
		return !t.validate();
	}
};

struct IsPNG
{
	bool operator() (Target& t) const
	{
		return t.png;
	}
};

int main(int argc, char** argv)
{
	// parameter parsing
	if (argc != 2 && argc != 3)
	{
		std::cout << "Usage: " << argv[0] << " prefix [path]\n";
		std::cout << "Will find all files starting with prefix in path, and output a file named prefix.ico at that path.\n";
		return 1;
	}
	std::filesystem::path path = fs::current_path();
	if (argc == 3)
		path = argv[2];
	std::string prefix = argv[1];

	// find targets
	std::vector<Target> targets;

	for (const auto & entry : fs::directory_iterator(path))
	{
		if (!entry.is_regular_file())
			continue;
		std::string fileName = entry.path().filename();
		if (!fileName.compare(0, prefix.size(), prefix))
			targets.push_back(fileName);
	}

	// read
	std::sort(targets.begin(), targets.end(), NameComp());
	for (Target& t : targets)
		readFile((fs::path(path) /= t.name).c_str(), t.data);
	
	// remove invalid files
	targets.erase(std::remove_if(targets.begin(), targets.end(), Invalid()), targets.end());

	if (1 < std::count_if(targets.begin(), targets.end(), IsPNG()))
		throw std::runtime_error("Too many PNGs, this code can't read their sizes and assumes there's 1 large png");

	// write output

	{
		std::cout << "Writing " << ((fs::path(path) /= prefix) += ".ico") << std::endl;
		AutoFD out(creat(((fs::path(path) /= prefix) += ".ico").c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP));

		// write headers
		{
			std::string headers;
			headers.resize(6 + 16 * targets.size());

			{
				ICOheader& h1 = *reinterpret_cast<ICOheader*>(headers.data());
				h1.zero = 0;
				h1.type = 1;
				h1.count = targets.size();
			}

			uint32_t nextDataOffset = headers.size();
			for (size_t i = 0; i < targets.size(); ++i)
			{
				ICOimageHeader& h2 = *reinterpret_cast<ICOimageHeader*>(headers.data() + 6 + 16 * i);
				Target& target = targets[i];
				h2.bytes = target.dataLength();
				h2.offset = nextDataOffset;
				nextDataOffset += h2.bytes;
				h2.bpp = target.bpp;
				h2.palette = 0;
				h2.rzero = 0;
				h2.planes = 1;
				if (target.png)
				{	// only 256 width should be PNG for the moment. Later improvements will have to be made
					h2.width = 0;
					h2.height = 0;
					continue;
				}
				ICOimageHeader& hIn = *reinterpret_cast<ICOimageHeader*>(target.data.data() + 6);
				h2.width = hIn.width;
				h2.height = hIn.height;
			}

			if (headers.size() != size_t(write(out.fd, headers.data(), headers.size())))
				throw std::runtime_error("Failed to write headers");
		}

		// write data

		for (const Target& t : targets)
			if (t.dataLength() != write(out.fd, t.payload(), t.dataLength()))
				throw std::runtime_error("Failed to write payload");
	}

	std::cout << "done" << std::endl;
	return 0;
}
