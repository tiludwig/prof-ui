#include <cstdio>
#include <cstring>
#include <linux/types.h>
#include <vector>
#include <type_traits>
#include "DataLink/HostPacket/HostPacket.hpp"
#include "DataLink/LinkLayer/TcpLink.hpp"
#include "DataLink/ProtocolLayer/DefaultProtocol.hpp"

class ProfilingResponse
{

public:
	__u64 cycleCount;
	__u64 retInstrCount;
	__u64 ctxSwitches;
	__u64 cpuFrequency;

	ProfilingResponse()
	{
		cycleCount = 0;
		retInstrCount = 0;
		ctxSwitches = 0;
		cpuFrequency = 0;
	}

	void deserialize(LinkStream& payload)
	{
		payload >> cycleCount;
		payload >> retInstrCount;
		payload >> ctxSwitches;
		payload >> cpuFrequency;
	}
};

void printResults(ProfilingResponse& result)
{
	printf("\nProfiling result:\n");
	printf("%20llu cycles\n", result.cycleCount);
	printf("%20llu retired instructions\n", result.retInstrCount);
	printf("%20.2f cycles per instruction\n", ((float) result.cycleCount / result.retInstrCount));
	printf("%20llu context switches\n", result.ctxSwitches);

	double timeElapsed = (result.cycleCount / 1.2);
	int unitIndex = 0;
	const char* units[] = { "ns", "us", "ms", "s" };
	if (timeElapsed > 1000.0)
	{
		timeElapsed = timeElapsed / 1000.0;
		unitIndex++;
	}

	if (timeElapsed > 1000.0)
	{
		timeElapsed = timeElapsed / 1000.0;
		unitIndex++;
	}

	if (timeElapsed > 1000.0)
	{
		timeElapsed = timeElapsed / 1000.0;
		unitIndex++;
	}

	printf("%20.2f %s task time\n", timeElapsed, units[unitIndex]);
}

class InfoPacket
{
public:
	std::string info;
	unsigned int receivedid;

public:
	void deserialize(LinkStream& stream)
	{
		stream >> info;
		stream >> receivedid;
	}
};

int main(int argc, char const *argv[])
{
	try
	{
		auto protocol = std::unique_ptr<DefaultProtocol>(new DefaultProtocol());
		TcpLink link;
		link.setProtocol(std::move(protocol));
		link.initialize();
		link.open(argv[1], 8080);

		if (std::string(argv[2]) == "exit")
		{
			printf("Sending exit\n");
			{
				HostPacket packet(100);
				packet.addPayload(true);

				link.sendPacket(packet);
				return 0;
			}
		}

		{
			HostPacket packet(static_cast<uint32_t>(300));
			packet.addPayload(argv[2]);
			packet.addPayload(static_cast<uint32_t>(1));
			packet.addPayload(argv[3]);

			link.sendPacket(packet);
		}
		{
			auto response = link.waitForPacket();
			auto payload = response->getPayload();

			bool ok;
			payload >> ok;
			if (ok)
				printf("Configuration successfully sent.\n");
			else
				printf("There was an error.\n");
		}

		printf("Starting profiler");
		{
			HostPacket packet(static_cast<uint32_t>(200));
			packet.addPayload(true);
			packet.addPayload("bubblesort");

			link.sendPacket(packet);
		}
		{
			auto response = link.waitForPacket();
			auto result = response->createType<ProfilingResponse>();
			printResults(result);
		}
		return 0;
	} catch (const char* err)
	{
		printf("Error: %s\n", err);
		return -1;
	}

	return 0;
}
