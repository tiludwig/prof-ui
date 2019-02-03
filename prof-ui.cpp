
#include <cstdio>
#include <cstring>
#include <linux/types.h>
#include <vector>
#include <type_traits>
#include "Components/HostClient/HostPacket.hpp"
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
		auto protocol = std::unique_ptr<DefaultProtocol>(new DefaultProtocol());
		protocol->processData(100);
		protocol->processData(0);
		protocol->processData(0);
		protocol->processData(0);

		protocol->processData(6);
		protocol->processData(0);
		protocol->processData(0);
		protocol->processData(0);
		protocol->processData('H');
		protocol->processData('e');
		protocol->processData('l');
		protocol->processData('l');
		protocol->processData('o');
		protocol->processData('\0');
		protocol->processData(-94);
		if(protocol->isPacketComplete())
		{
			auto packet = protocol->getPacket();
			printf("received: id=%d, plsize=%d\n", packet->id, packet->payloadSize);
			printf("payload:  %s\n", packet->payload);
		}

		return 0;
	}
	catch(const char* err)
	{
		printf("Error: %s\n", err);
		return -1;
	}

	return 0;
}
