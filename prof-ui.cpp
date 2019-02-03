
#include <cstdio>
#include <cstring>
#include <linux/types.h>
#include <vector>
#include <type_traits>
#include "DataLink/HostPacket/HostPacket.hpp"
#include "DataLink/LinkLayer/TcpLink.hpp"
#include "DataLink/ProtocolLayer/DefaultProtocol.hpp"

struct prof_response
{
	__u64 cycleCount;
	__u64 retInstrCount;
	__u64 ctxSwitches;
	__u64 cpuFrequency;
};

void printResults(prof_response& result)
{
	printf("\nProfiling result:\n");
	printf("%20llu cycles\n", result.cycleCount);
	printf("%20llu retired instructions\n", result.retInstrCount);
	printf("%20.2f cycles per instruction\n", ((float) result.cycleCount / result.retInstrCount));
	printf("%20llu context switches\n", result.ctxSwitches);

	double timeElapsed = (result.cycleCount / 1.2);
	int unitIndex = 0;
	const char* units[] =
	{ "ns", "us", "ms", "s" };
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

int main(int argc, char const *argv[])
{
	try
	{
		HostPacket packet(100);
		const char* data = "Please work ...";
		auto len = strlen(data) + 1;
		packet.addPayload(data, len);
		TcpLink link;
		link.initialize();
		link.open("127.0.0.1", 8080);

		link.sendPacket(packet);
		return 0;
	}
	catch(const char* err)
	{
		printf("Error: %s\n", err);
		return -1;
	}

	return 0;
}
