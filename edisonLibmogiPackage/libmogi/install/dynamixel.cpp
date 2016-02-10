/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
 *                                                                            *
 *   Proprietary and confidential.                                            *
 *                                                                            *
 *   Unauthorized copying of this file via any medium is strictly prohibited  *
 *   without the explicit permission of Mogi, LLC.                            *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   http://www.binpress.com/license/view/l/0088eb4b29b2fcff36e42134b0949f93  *
 *                                                                            *
 *****************************************************************************/

#include <mogi/dynamixel/dynamixel.h>
#include <iomanip>
#include <iostream>

#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>

#include <getopt.h>

#include <regex.h>
#include <string.h>

using namespace Mogi::Dynamixel;

class FailureCheck: public Observer {
public:
	void update(Motor* dynamixel, const Instruction* instruction) {
		//		switch (instruction->type) {
		//			case Instruction::READ:
		//				std::cerr << "Successful READ command" <<
		//std::endl;
		//				break;
		//
		//			case Instruction::WRITE:
		//				std::cerr << "Successful WRITE command" <<
		//std::endl;
		//				break;
		//
		//			case Instruction::SYNC_WRITE:
		//				std::cerr << "Successful SYNC_WRITE command" <<
		//std::endl;
		//				break;
		//
		//			case Instruction::PING:
		//				std::cerr << "Successful PING command" <<
		//std::endl;
		//				break;
		//		}
	}

	void failed(Motor* dynamixel, const Instruction* instruction,
			Interface::Status status) {
		std::cerr << "Failure, Model: " << dynamixel->getModel() << "\tID: "
				<< dynamixel->getByte(REG_ID) << std::endl;
		switch (instruction->type) {
		case Instruction::PING:
			std::cerr << " - PING command" << std::endl;
			break;

		case Instruction::READ:
			std::cerr << " - READ command" << std::endl;
			break;

		case Instruction::WRITE:
			std::cerr << " - WRITE command" << std::endl;
			break;

		case Instruction::REG_WRITE:
			std::cerr << " - REG_WRITE command" << std::endl;
			break;

		case Instruction::ACTION:
			std::cerr << " - ACTION command" << std::endl;
			break;

		case Instruction::RESET:
			std::cerr << " - RESET command" << std::endl;
			break;

		case Instruction::REBOOT:
			std::cerr << " - REBOOT command" << std::endl;
			break;

		case Instruction::SYNC_READ:
			std::cerr << " - SYNC_READ command" << std::endl;
			break;

		case Instruction::SYNC_WRITE:
			std::cerr << " - SYNC_WRITE command" << std::endl;
			break;

		case Instruction::BULK_READ:
			std::cerr << " - BULK_READ command" << std::endl;
			break;

		case Instruction::BULK_WRITE:
			std::cerr << " - BULK_WRITE command" << std::endl;
			break;

		case Instruction::STATUS:
			std::cerr << " - STATUS command" << std::endl;
			break;
		}
		switch (status) {
		case Interface::BAD_LENGTH:
			std::cerr << " - Reason: BAD_LENGTH" << std::endl;
			break;

		case Interface::BAD_CHECKSUM:
			std::cerr << " - Reason: BAD_CHECKSUM" << std::endl;
			break;

		case Interface::EMPTY_PACKET:
			std::cerr << " - Reason: EMPTY_PACKET" << std::endl;
			break;

		case Interface::UNSUPPORTED_COMMAND:
			std::cerr << " - Reason: UNSUPPORTED_COMMAND" << std::endl;
			break;

		default:
			break;
		}
	}
};

struct RangeData {
	int reg;
	int length;

	RangeData& operator=(const DataRange& param) {
		this->length = param.length;
		this->reg = param.reg;
		return *this;
	}

	operator DataRange() {
		return DataRange(reg, length);
	}
};

int searchDynamixels(Interface* interface, DataRange dataRange, bool verbose);
int readDynamixel(Interface* interface, unsigned char ID, DataRange dataRange,
		bool verbose);
int writeDynamixel(Interface* interface, unsigned char ID, int value,
		DataRange dataRange, bool verbose);
int syncReadDynamixel(Interface* interface, std::vector<unsigned char> IDs,
		DataRange dataRange, bool verbose);
int syncWriteDynamixel(Interface* interface, std::vector<unsigned char> IDs,
		int value, DataRange dataRange, bool verbose);
int bulkReadDynamixel(Interface* interface, std::vector<unsigned char> IDs,
		std::vector<DataRange> dataRange, bool verbose);
int bulkWriteDynamixel(Interface* interface, std::vector<unsigned char> IDs,
		std::vector<DataRange> dataRange, std::vector<int> values,
		bool verbose);

int parseRange(std::string range, RangeData* result) {

	// http://www.wellho.net/resources/ex.php4?item=c206/reg2.c
	const char* source = range.c_str();
	const char* regexString = "(-?[0-9]+),(-?[0-9]+)";
	size_t maxMatches = 10;
	size_t maxGroups = 3;
	size_t matches = 0;

	regex_t regexCompiled;
	regmatch_t groupArray[maxGroups];
	unsigned int m;
	const char* cursor;

	if (regcomp(&regexCompiled, regexString, REG_EXTENDED)) {
		std::cerr << "Could not compile regular expression." << std::endl;
		return EXIT_FAILURE;
	}

	m = 0;
	cursor = source;
	for (m = 0; m < maxMatches; m++) {
		if (regexec(&regexCompiled, cursor, maxGroups, groupArray, 0))
			break;  // No more matches

		matches++;
		unsigned int g = 0;
		unsigned int offset = 0;
		for (g = 0; g < maxGroups; g++) {
			if (groupArray[g].rm_so == (size_t) -1)
				break;  // No more groups

			if (g == 0)
				offset = groupArray[g].rm_eo;

			char cursorCopy[strlen(cursor) + 1];
			strcpy(cursorCopy, cursor);
			cursorCopy[groupArray[g].rm_eo] = 0;
			//std::cout << "Match " << m << ", Group " << g << ": [" << groupArray[g].rm_so << "-" << groupArray[g].rm_eo << "]: " << cursorCopy + groupArray[g].rm_so << std::endl;

			int integerResult = atoi(cursorCopy + groupArray[g].rm_so);
			if (g == 1) {
				result->reg = integerResult;
			} else if (g == 2) {
				result->length = integerResult;
			}
			if (integerResult < 0) {
				std::cerr << "Error: negative numbers invalid in range." << std::endl;
				return EXIT_FAILURE;
			}
		}

		cursor += offset;
	}
	if (matches != 1) {
		std::cerr << "Error: Range format of " << range << " invalid, must be in form: int,int" << std::endl;
		return EXIT_FAILURE;
	}

	regfree(&regexCompiled);

	return EXIT_SUCCESS;
}

int parseRange(std::string range, DataRange* result) {
 RangeData rangedata;
	int ret = parseRange(range, &rangedata);
	*result = rangedata;
	return ret;
}

int parseList(std::string listStr, std::vector<unsigned char>& v) {
	const char* source = listStr.c_str();
	const char* regexString = "([0-9]+)";
	size_t maxMatches = 100;
	size_t maxGroups = 1;

	regex_t regexCompiled;
	regmatch_t groupArray[maxGroups];
	unsigned int m;
	const char* cursor;

	if (regcomp(&regexCompiled, regexString, REG_EXTENDED)) {
		std::cerr << "Could not compile regular expression." << std::endl;
		return EXIT_FAILURE;
	};

	m = 0;
	cursor = source;
	for (m = 0; m < maxMatches; m++) {
		if (regexec(&regexCompiled, cursor, maxGroups, groupArray, 0))
			break;  // No more matches

		unsigned int g = 0;
		unsigned int offset = 0;
		for (g = 0; g < maxGroups; g++) {
			if (groupArray[g].rm_so == (size_t) -1)
				break;  // No more groups

			if (g == 0)
				offset = groupArray[g].rm_eo;

			char cursorCopy[strlen(cursor) + 1];
			strcpy(cursorCopy, cursor);
			cursorCopy[groupArray[g].rm_eo] = 0;

			v.push_back((unsigned char) atoi(cursorCopy + groupArray[g].rm_so));
		}

		cursor += offset;
	}

	regfree(&regexCompiled);

	return EXIT_SUCCESS;
}

int parseBulk(std::string listStr, std::vector<unsigned char>& v, std::vector<DataRange>& r, std::vector<int>& d) {
	const char* source = listStr.c_str();
	const char* regexString = "([0-9]+),([0-9]+),([0-9]+),([0-9]+)";
	size_t maxMatches = 100;
	size_t maxGroups = 10;
	size_t matches = 0;

	regex_t regexCompiled;
	regmatch_t groupArray[maxGroups];
	unsigned int m;
	const char* cursor;

	if (regcomp(&regexCompiled, regexString, REG_EXTENDED)) {
		std::cerr << "Could not compile regular expression." << std::endl;
		return EXIT_FAILURE;
	};

	m = 0;
	cursor = source;
	for (m = 0; m < maxMatches; m++) {
		if (regexec(&regexCompiled, cursor, maxGroups, groupArray, 0)) {
			break;  // No more matches
		}

		matches++;

		unsigned int g = 0;
		unsigned int offset = 0;
		unsigned char id;
		unsigned short reg, length;
		int data = 0;
		errno = 0;
		for (g = 0; g < maxGroups; g++) {
			if (groupArray[g].rm_so == (size_t) -1) {
				//std::cerr << "Error: Arguments should be in the format of ID:Reg,Len,value; for bulk operations." << std::endl;
				break;//return EXIT_FAILURE;
			}


			char cursorCopy[strlen(cursor) + 1];
			strcpy(cursorCopy, cursor);
			cursorCopy[groupArray[g].rm_eo] = 0;
			//std::cout << "Match " << m << ", Group " << g << ": [" << groupArray[g].rm_so << "-" << groupArray[g].rm_eo << "]: " << cursorCopy + groupArray[g].rm_so << std::endl;

			switch (g) {
			case 1:
				id = (unsigned char) strtol(cursorCopy + groupArray[g].rm_so, NULL, 10);
				break;
			case 2:
				reg = (unsigned short) strtol(cursorCopy + groupArray[g].rm_so, NULL, 10);
				break;
			case 3:
				length = (unsigned short) strtol(cursorCopy + groupArray[g].rm_so, NULL, 10);
				break;
			case 4:
				data = (int) strtol(cursorCopy + groupArray[g].rm_so, NULL, 10);
				offset = groupArray[g].rm_eo;
				break;
			}
		}
		if (errno) {
			std::cerr << "Error: Arguments should be in the format of ID:Reg,Len,value; for bulk operations." << std::endl;
			return EXIT_FAILURE;
		}

		v.push_back(id);
		r.push_back(DataRange(reg, length));
		d.push_back(data);
		cursor += offset;
	}
	regfree(&regexCompiled);

	if (matches == 0) {
		std::cerr << "Error: Arguments should be in the format of ID:Reg,Len,value; for bulk operations." << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void printGraph(int width, double percent) {
	if (percent > 1) {
		percent = 1;
	}
	double barSize = percent * (double) width;
	std::cout << "\r" << "[";
	int j;
	for (j = 0; j < barSize; j++) {
		std::cout << "=";
	}
	std::cout << ">";
	for (; j < width; j++) {
		std::cout << " ";
	}
	std::cout << "] " << 100 * percent << "%";
}

void printUsage(char* argv[]) {
	std::cout << std::endl;
	std::cout << "Usage: " << argv[0] << " -h|-r|-w|-s|-R|-W [options] ..." << std::endl;
	std::cout << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  -h,--help                  Display this information" << std::endl;
	std::cout << "  -v,--verbose               Displays a more verbose output, if applicable" << std::endl;
	std::cout << "  -s [<start,end>],--search=[<start,end>]" << std::endl;
	std::cout << "                             Performs a motor search routine given a search start and end ID" << std::endl;
	std::cout << "  -r,--read                  Performs a motor register read or sync_read instruction" << std::endl;
	std::cout << "  -w <reg,value>,--write=<reg,value>" << std::endl;
	std::cout << "                             Performs a write or sync_write command to registers with a value" << std::endl;

	std::cout << "  -R <id,reg,length,value[:id,reg,length,value]>,--bulkread=<id:reg,length,vlaue[:id,reg,length,value]>" << std::endl;
	std::cout << "                             Performs a bulk_read command starting with register reg with length length on motor ID id, value is ignored" << std::endl;
	std::cout << "  -W <id,reg,length,value[:id,reg,length,value]>,--bulkwrite=<id:reg,length,value[:id,reg,length,value]>" << std::endl;
	std::cout << "                             Performs a bulk_write command to register reg with length length and value value on motor ID id" << std::endl;

	std::cout << "  -b <baudrate>,--baud=<baudrate>" << std::endl;
	std::cout << "                             Sets the interface baudrate, default is 1000000." << std::endl;
	std::cout << "  -f <file>,--file=<file>    The UART device filename to open if a COM port" << std::endl;
	std::cout << "  -p <ver>,--protocol=<ver>" << std::endl;
	std::cout << "                             Sets the dynamixel protocol version (1 or 2), default is 1" << std::endl;
	std::cout << "  -i <ID1[,ID2...]>,--id=<ID1[,ID2...]> " << std::endl;
	std::cout << "                             Sets the motor ID(s) to send r/w instructions,  default is 1" << std::endl;
	std::cout << "                             if multiple IDs specified, sync{read,write} will be used" << std::endl;
	std::cout << "  -n <len>,--numbytes=<len>" << std::endl;
	std::cout << "                             Sets the number of bytes for the read command" << std::endl;
	std::cout << "  -d <start,length>,--datarange=<start,length>" << std::endl;
	std::cout << "                             Sets the range information for the command" << std::endl;
}

Interface* createInterface(int baud, char* file, Packet::Type packetVersion) {
	Interface* interface = NULL;

	if (file != NULL) {
		interface = Interface::create(Interface::COM, baud);
	}
		#ifdef FTDI_FOUND 
	else {
		interface = Interface::create(Interface::FTDI, baud);
	}
#endif

	if (interface == NULL) {
		return interface;
	}

	interface->setPacketType(packetVersion);

	if (interface->open(file) != Interface::NOERROR) {
		interface->close();
		delete interface;
		interface = NULL;
	}

	return interface;
}

int main(int argc, char* argv[]) {
	bool help = false;
	bool verbose = false;
	int baudrate = 1000000;
	char* fileName = NULL;
	Packet::Type packetVersion = Packet::VER_1_0;

	std::vector<unsigned char> IDs;
	std::vector<DataRange> ranges;
	std::vector<int> datas;
	bool search = false;
	Instruction::Type packetType = Instruction::STATUS;
	RangeData writeValue;
	DataRange dataRange;  // may need init
	DataRange searchRange;  // may need init

	static struct option long_options[] = { { "help", no_argument, NULL, 'h' },
		{ "verbose", no_argument, NULL, 'v' },
		{ "baud", required_argument, NULL, 'b' },
		{ "file", required_argument, NULL, 'f' },
		{ "protocol", required_argument, NULL, 'p' },
		{ "id", required_argument, NULL, 'i' },
		{ "search", optional_argument, NULL, 's' },
		{ "read", no_argument, NULL, 'r' },
		{ "write", required_argument, NULL, 'w' },
		{ "bulkread", required_argument, NULL, 'R' },
		{ "bulkwrite", required_argument, NULL, 'W' },
		{ "numbytes", required_argument, NULL, 'n' },
		{ "datarange", required_argument, NULL, 'd' },
		{ 0, 0, 0, 0 } };

	int opt;
	while ((opt = getopt_long(argc, argv, "hvb:f:p:i:srw:R:W:n:d:",
			long_options,
			NULL)) != -1) {
		switch (opt) {
		case 'h':
			help = true;
			break;

		case 'v':
			verbose = true;
			break;

		case 'b':
			baudrate = atoi(optarg);
			break;

		case 'f':
			fileName = optarg;
			break;

		case 'p':
			if (atoi(optarg) != 1 && atoi(optarg) != 2) {
				printUsage(argv);
				return EXIT_FAILURE;
			}
			packetVersion = (Packet::Type) atoi(optarg);
			break;

		case 'i':  // ID = atoi(optarg);
				if(parseList(optarg, IDs) != EXIT_SUCCESS ||
				   IDs.size() == 0) {
					printUsage(argv);
					return EXIT_FAILURE;
				}
			break;

		case 's':
			search = true;
				if (optarg) {
					if(parseRange(optarg, &searchRange) != EXIT_SUCCESS) {
						printUsage(argv);
						return EXIT_FAILURE;
					}
				}
				if(searchRange.reg == 0 && searchRange.length == 0) {
					searchRange.length = 253;	// default search: 0-253
				}

				if (searchRange.reg > 253 || searchRange.length > 253) {
					std::cerr << "Invalid Search parameters: Searchable IDs must be in the range of 0-253." << std::endl;
					return EXIT_FAILURE;
				}

				if (searchRange.reg >= searchRange.length) {
					std::cerr << "Invalid Search parameters: first ID must be smaller than second ID." << std::endl;
					return EXIT_FAILURE;
				}
			break;

		case 'r':
			if (packetType != Instruction::STATUS) {
				printUsage(argv);
				return EXIT_FAILURE;
			}
			packetType = Instruction::READ;
			break;

		case 'w':
			if (packetType != Instruction::STATUS) {
				printUsage(argv);
				return EXIT_FAILURE;
			}
			packetType = Instruction::WRITE;
				if(parseRange(optarg, &writeValue) != EXIT_SUCCESS) {
					printUsage(argv);
					return EXIT_FAILURE;
				}
			dataRange.reg = writeValue.reg;  // this is a bit of a hack...
			break;

		case 'R':
			if (packetType != Instruction::STATUS) {
				printUsage(argv);
				return EXIT_FAILURE;
			}
			packetType = Instruction::BULK_READ;
				if(parseBulk(optarg, IDs, ranges, datas) != EXIT_SUCCESS) {
					return EXIT_FAILURE;
				};
			break;

		case 'W':
			if (packetType != Instruction::STATUS) {
				printUsage(argv);
				return EXIT_FAILURE;
			}
			packetType = Instruction::BULK_WRITE;
				if(parseBulk(optarg, IDs, ranges, datas) != EXIT_SUCCESS) {
					return EXIT_FAILURE;
				};
			break;

		case 'n':
			dataRange.length = atoi(optarg);
			break;

		case 'd':
				if(parseRange(optarg, &dataRange) != EXIT_SUCCESS) {
					return EXIT_FAILURE;
				}
			break;

		case '?':	// Let's be strict.
		case ':':
				switch (optopt) {
					case 'b':
					case 'f':
					case 'p':
					case 'i':
					case 'w':
					case 'R':
					case 'W':
					case 'n':
					case 'd':
						std::cerr << "Error: option requires argument: -" << (char)optopt << std::endl;
						break;

					default:
						std::cerr << "Error: unrecognized option: -" << (char)optopt << std::endl;
						break;
				}
				std::cout << " - For help, run: " << argv[0] << " --help" << std::endl;

				return EXIT_FAILURE;
				break;
				
		default:
			printUsage(argv);
			return EXIT_FAILURE;
			break;
		}
	}

	if (help) {
		printUsage(argv);
		return EXIT_SUCCESS;
	}

	if ((packetType == Instruction::STATUS && search == false)
			|| (packetType != Instruction::STATUS && search == true)) {
		std::cerr << "A single command type must be specified" << std::endl;
		printUsage(argv);
		return EXIT_FAILURE;
	}

	if (IDs.size() == 0) {
		IDs.push_back(1);
	}

	if (packetType == Instruction::READ && IDs.size() > 1) {
		packetType = Instruction::SYNC_READ;
	}

	if (packetType == Instruction::WRITE && IDs.size() > 1) {
		packetType = Instruction::SYNC_WRITE;
	}

	if (verbose) {
		std::cout << " - Baudrate: " << baudrate << std::endl;
		std::cout << " - ID(s): ";
		for (std::vector<unsigned char>::iterator it = IDs.begin();
				it != IDs.end(); ++it) {
			std::cout << (unsigned int) *it << " ";
		}
		std::cout << std::endl;
		std::cout << " - Dynamixel version: " << (int) packetVersion
				<< std::endl;
		if (fileName != NULL) {
			std::cout << " - Type: COM" << std::endl;
		}
#ifdef FTDI_FOUND
			else {
			std::cout << " - Type: FTDI" << std::endl;
		}
#endif
	}

	Interface* dynamixelInterface;

	if ((dynamixelInterface = createInterface(baudrate, fileName, packetVersion))
			== NULL) {
		if (fileName != NULL) {
			std::cerr << "Error: Unable to open UART device " << fileName << std::endl;
		}
#ifdef FTDI_FOUND
		else {
			std::cerr << "Error: Unable to open FTDI device " << std::endl;
		}
#endif
		return EXIT_FAILURE;
	}

	int exitCode = EXIT_FAILURE;

	if (search) {
		exitCode = searchDynamixels(dynamixelInterface, searchRange, verbose);
	} else {
		switch (packetType) {
		case Instruction::READ:
			exitCode = readDynamixel(dynamixelInterface, IDs.front(), dataRange, verbose);
			break;

		case Instruction::WRITE:
			exitCode = writeDynamixel(dynamixelInterface, IDs.front(), writeValue.length, dataRange, verbose);
			break;

		case Instruction::SYNC_READ:
			exitCode = syncReadDynamixel(dynamixelInterface, IDs, dataRange, verbose);
			break;

		case Instruction::SYNC_WRITE:
			exitCode = syncWriteDynamixel(dynamixelInterface, IDs, writeValue.length, dataRange, verbose);
			break;

		case Instruction::BULK_READ:
			exitCode = bulkReadDynamixel(dynamixelInterface, IDs, ranges, verbose);
			break;

		case Instruction::BULK_WRITE:
			exitCode = bulkWriteDynamixel(dynamixelInterface, IDs, ranges, datas, verbose);
			break;

		default:
			break;
		}
	}

	return exitCode;
}

int searchDynamixels(Interface* interface, DataRange dataRange, bool verbose) {
	Handler dynamixelHandler;

	if (verbose)
		std::cout << "Beginning search for IDs " << dataRange.reg << "-"
				<< dataRange.length << std::endl;
	dynamixelHandler.setInterface(interface);

	if (verbose) {
		std::cout << std::fixed << std::setprecision(2) << std::setfill('0');
		std::cout << "Queueing Ping commands:" << std::endl;
		printGraph(65, 0);
	}
	for (unsigned int i = dataRange.reg; i <= dataRange.length; i++) {
		dynamixelHandler.addDynamixel(i);
		if (verbose)
			printGraph(65,
					(double) i / (double) (dataRange.length - dataRange.reg)); // note this adds a time delay.
	}
	if (verbose) {
		std::cout << std::endl;

		std::cout << "Waiting for commands to complete:" << std::endl;
		while (dynamixelHandler.queueLength() > 0) {
			printGraph(65,
					(double) ((double) (dataRange.length - dataRange.reg)
							- dynamixelHandler.queueLength())
							/ (double) (dataRange.length - dataRange.reg));
		}
	}
	while (dynamixelHandler.busy())
		;
	if (verbose) {
		printGraph(65,
				(double) ((double) (dataRange.length - dataRange.reg)
						- dynamixelHandler.queueLength())
						/ (double) (dataRange.length - dataRange.reg)); // clean up
		std::cout << std::endl;
	}

	std::map<unsigned char, Motor*> dynamixels =
			dynamixelHandler.getDynamixels();
	if (verbose)
		std::cout << "Found " << dynamixels.size() << " motors:" << std::endl;
	for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
			it != dynamixels.end(); it++) {
		Motor* dynamixel = it->second;
		if (verbose)
			std::cout << " - Motor " << (int) it->first << "\ttype: "
					<< dynamixel->getModel() << "\tangle: "
					<< dynamixel->getCurrentPosition() * 180.0 / 3.1415926525897
					<< " degrees" << std::endl;
		else
			std::cout << (int) it->first << "\t" << dynamixel->getModel()
					<< "\t"
					<< dynamixel->getCurrentPosition() * 180.0 / 3.1415926525897
					<< std::endl;
	}
	if (verbose)
		std::cout << "Done." << std::endl;

	return EXIT_SUCCESS;
}

int readDynamixel(Interface* interface, unsigned char ID, DataRange dataRange,
		bool verbose) {
	FailureCheck failureObserver;
	Handler dynamixelHandler;
	dynamixelHandler.setInterface(interface);

	// The following is basically the same thing as performing an
	// Handler::addDynamixel, but a manual version so that we can be more verbose.
	dynamixelHandler.forceAddDynamixel(ID);
	std::map<unsigned char, Motor*> dynamixels =
			dynamixelHandler.getDynamixels();
	dynamixels[ID]->addObserver(&failureObserver);
	if (dynamixels.size() < 1) {
		std::cerr << "Error: Unable to read dynamixel" << std::endl;
		return EXIT_FAILURE;
	}
	//	printRegisterValues(dynamixels[ID]);

	if (verbose) {
		std::cout << "Reading model for dynamixel of ID " << ID << std::endl;
	}
	dynamixelHandler.pushInstruction(
			new Instruction(Instruction::READ, DataRange(0, 2),
					dynamixels[ID]));  // This kills the instruction
	while (dynamixelHandler.busy())
		;

	if (dataRange.length == 0 || dataRange.length == (unsigned short) -1) {
		dataRange.length = dynamixels[ID]->getNumberOfRegisters()
				- dataRange.reg;
	}

	if (verbose) {
		std::cout << "Will be reading " << dataRange.length
				<< " bytes starting from reg " << dataRange.reg << std::endl;
	}
	dynamixelHandler.pushInstruction(
			new Instruction(Instruction::READ, dataRange, dynamixels[ID])); // This kills the instruction
	while (dynamixelHandler.busy())
		;

	for (int i = dataRange.reg; i < dataRange.reg + dataRange.length; i++) {
		std::cout << i << "\t" << (int) dynamixels[ID]->getByte(i) << std::endl;
	}

	dynamixels[ID]->removeObserver(&failureObserver);

	return EXIT_SUCCESS;
}

int writeDynamixel(Interface* interface, unsigned char motorID, int value,
		DataRange dataRange, bool verbose) {
	if (dataRange.length == 0 || dataRange.length == (unsigned short) -1) {
		if (value >= 0 && value < 256) {
			dataRange.length = 1;
		} else if (value >= 256 && value < 65536) {
			dataRange.length = 2;
		} else {
			dataRange.length = 4;
		}
	} else if (dataRange.length != 1 && dataRange.length != 2
			&& dataRange.length != 4) {
		std::cerr << " Invalid number of bytes specified" << std::endl;
		return EXIT_FAILURE;
	}

	Handler dynamixelHandler;
	dynamixelHandler.setInterface(interface);

	// We want to send a command regardless of whether or not we can get
	// successufl PING command.  motor_search will handle the PING check.
	dynamixelHandler.forceAddDynamixel(motorID);
	std::map<unsigned char, Motor*> dynamixels =
			dynamixelHandler.getDynamixels();
	dynamixelHandler.pushInstruction(
			new Instruction(Instruction::READ, DataRange(0, 2),
					dynamixels[motorID]));
	while (dynamixelHandler.busy())
		;

	// Before writing the value, let's try to read the current value in the
	// dynamixel.
	int valcurrent;
	if (verbose)
		std::cout << "Attempting to read motor " << motorID << "..."
				<< std::endl;
	dynamixelHandler.pushInstruction(
			new Instruction(Instruction::READ, dataRange, dynamixels[motorID])); // 2 or 1 bytes?  how to tell?
	while (dynamixelHandler.busy())
		;
	switch (dataRange.length) {
	case 1:
		valcurrent = dynamixels[motorID]->getByte(dataRange.reg);
		break;

	case 2:
		valcurrent = dynamixels[motorID]->getWord(dataRange.reg);
		break;

	case 4:
		valcurrent = dynamixels[motorID]->getInt(dataRange.reg);
		break;
	}
	if (verbose)
		std::cout << "Register: " << dataRange.reg << ":\tCurrent value: "
				<< valcurrent << std::endl;

	// Now that we know the current value, let's write the new value.
	if (verbose)
		std::cout << "\nModifying value..." << std::endl;
	switch (dataRange.length) {
	case 1:
		dynamixels[motorID]->setByte(dataRange.reg, value);
		break;

	case 2:
		dynamixels[motorID]->setWord(dataRange.reg, value);
		break;

	case 4:
		dynamixels[motorID]->setInt(dataRange.reg, value);
		break;
	}
	dynamixelHandler.pushInstruction(
			new Instruction(Instruction::WRITE, dataRange,
					dynamixels[motorID]));  // 2 or 1 bytes?  how to tell?
	while (dynamixelHandler.busy())
		;

	// Now that the motor has been set, let's read the value back and make sure it
	// changed.
	dynamixelHandler.pushInstruction(
			new Instruction(Instruction::READ, dataRange, dynamixels[motorID])); // 2 or 1 bytes?  how to tell?
	while (dynamixelHandler.busy())
		;
	switch (dataRange.length) {
	case 1:
		valcurrent = dynamixels[motorID]->getByte(dataRange.reg);
		break;

	case 2:
		valcurrent = dynamixels[motorID]->getWord(dataRange.reg);
		break;

	case 4:
		valcurrent = dynamixels[motorID]->getInt(dataRange.reg);
		break;
	}

	if (verbose)
		std::cout << "Register: " << dataRange.reg << ":\tValue: " << valcurrent
				<< std::endl;

	if (value != valcurrent) {
		std::cerr << "Warning: Written value did not seem to change!"
				<< std::endl;
		return EXIT_FAILURE;
	}

	if (verbose)
		std::cout << "Writing was successful (values match)" << std::endl;

	return EXIT_SUCCESS;
}

int syncReadDynamixel(Interface* interface, std::vector<unsigned char> IDs,
		DataRange dataRange, bool verbose) {
	if (verbose)
		std::cout
				<< "More than one ID specified, a SYNC_READ command will be issued"
				<< std::endl;
	FailureCheck failureObserver;
	Handler dynamixelHandler;
	dynamixelHandler.setInterface(interface);

	for (std::vector<unsigned char>::iterator it = IDs.begin(); it != IDs.end();
			++it) {
		dynamixelHandler.forceAddDynamixel(*it);
	}

	std::map<unsigned char, Motor*> dynamixels =
			dynamixelHandler.getDynamixels();
	if (dynamixels.size() < 1) {
		std::cerr << "Error: Unable to read dynamixel" << std::endl;
		return EXIT_FAILURE;
	} else if (dynamixels.size() < IDs.size()) {
		std::cout << "Warning: Some dynamixel motors were not found"
				<< std::endl;
	}

	for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
			it != dynamixels.end(); ++it) {
		it->second->addObserver(&failureObserver);
	}

	if (verbose) {
		std::cout << "Reading model for dynamixel of IDs: ";
		for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
				it != dynamixels.end(); ++it) {
			std::cout << (int) it->first << " ";
		}
		std::cout << std::endl;
	}
	dynamixelHandler.pushInstruction(
			new Instruction(Instruction::SYNC_READ, DataRange(0, 2), NULL,
					&dynamixels));
	while (dynamixelHandler.busy())
		;

	// Optional: Check if all the motors are the same type
	unsigned short motorType = dynamixels.begin()->second->getWord(0);
	for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
			it != dynamixels.end(); ++it) {
		unsigned short temp = it->second->getWord(0);
		if (motorType != temp) {
			std::cout
					<< "Warning: different motor models detected for sync read"
					<< std::endl;
			break;
		}
		motorType = temp;
	}

	if (dataRange.length == 0 || dataRange.length == (unsigned short) -1) {
		dataRange.length = dynamixels.begin()->second->getNumberOfRegisters()
				- dataRange.reg;  // Used first ID for number of registers,
								  // should be improved...
	}

	if (verbose) {
		std::cout << "Will be reading " << dataRange.length
				<< " bytes starting from reg " << dataRange.reg << std::endl;
	}
	dynamixelHandler.pushInstruction(
			new Instruction(Instruction::SYNC_READ, dataRange, NULL,
					&dynamixels));  // This kills the instruction
	while (dynamixelHandler.busy())
		;

	for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
			it != dynamixels.end(); ++it) {
		std::cout << "Motor ID=" << (int) it->first << "--";
		for (int i = dataRange.reg; i < dataRange.reg + dataRange.length; i++) {
			std::cout << i << "\t" << (int) (it->second->getByte(i))
					<< std::endl;
		}
		it->second->removeObserver(&failureObserver);
	}
	return EXIT_SUCCESS;
}

int syncWriteDynamixel(Interface* interface, std::vector<unsigned char> IDs,
		int value, DataRange dataRange, bool verbose) {
	if (verbose)
		std::cout
				<< "More than one ID specified, a SYNC_WRITE command will be issued"
				<< std::endl;
	if (dataRange.length == 0 || dataRange.length == (unsigned short) -1) {
		if (value >= 0 && value < 256) {
			dataRange.length = 1;
		} else if (value >= 256 && value < 65536) {
			dataRange.length = 2;
		} else {
			dataRange.length = 4;
		}
	} else if (dataRange.length != 1 && dataRange.length != 2
			&& dataRange.length != 4) {
		std::cerr << " Invalid number of bytes specified" << std::endl;
		return EXIT_FAILURE;
	}

	Handler dynamixelHandler;
	dynamixelHandler.setInterface(interface);

	for (std::vector<unsigned char>::iterator it = IDs.begin(); it != IDs.end();
			++it) {
		dynamixelHandler.forceAddDynamixel(*it);
	}

	std::map<unsigned char, Motor*> dynamixels =
			dynamixelHandler.getDynamixels();
	if (dynamixels.size() < 1) {
		std::cerr << "Error: Unable to read dynamixel" << std::endl;
		return EXIT_FAILURE;
	} else if (dynamixels.size() < IDs.size()) {
		std::cout << "Warning: Some dynamixel motors were not found"
				<< std::endl;
	}

	if (verbose) {
		std::cout << "Reading model for dynamixel of IDs: ";
		for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
				it != dynamixels.end(); ++it) {
			std::cout << (int) it->first << " ";
		}
		std::cout << std::endl;
	}
	dynamixelHandler.pushInstruction(
			new Instruction(Instruction::SYNC_READ, DataRange(0, 2), NULL,
					&dynamixels));
	while (dynamixelHandler.busy())
		;

	// Optional: Check if all the motors are the same type
	unsigned short motorType = dynamixels.begin()->second->getWord(0);
	for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
			it != dynamixels.end(); ++it) {
		unsigned short temp = it->second->getWord(0);
		if (motorType != temp) {
			std::cout
					<< "Warning: different motor models detected for sync write"
					<< std::endl;
			break;
		}
		motorType = temp;
	}

	// Before writing the value, let's try to read the current value in the
	// dynamixel.
	std::map<unsigned char, int> valcurrent;
	if (verbose) {
		std::cout << "Attempting to read motor: " << std::endl;
	}
	for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
			it != dynamixels.end(); ++it) {
		switch (dataRange.length) {
		case 1:
			valcurrent[it->first] = (int) it->second->getByte(dataRange.reg);
			break;
		case 2:
			valcurrent[it->first] = (int) it->second->getWord(dataRange.reg);
			break;
		case 4:
			valcurrent[it->first] = it->second->getInt(dataRange.reg);
			break;
		}
	}

	if (verbose) {
		for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
				it != dynamixels.end(); ++it) {
			std::cout << "ID: " << (int) it->first << "\tRegister: "
					<< dataRange.reg << ":\tCurrent value: "
					<< valcurrent.at(it->first) << std::endl;
		}
	}

	// Now that we know the current value, let's write the new value.
	if (verbose)
		std::cout << "\nModifying value..." << std::endl;
	for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
			it != dynamixels.end(); ++it) {
		switch (dataRange.length) {
		case 1:
			it->second->setByte(dataRange.reg, value);
			break;

		case 2:
			it->second->setWord(dataRange.reg, value);
			break;
		case 4:
			it->second->setInt(dataRange.reg, value);
			break;
		}
	}
	dynamixelHandler.pushInstruction(
			new Instruction(Instruction::SYNC_WRITE, dataRange, NULL,
					&dynamixels));
	while (dynamixelHandler.busy())
		;

	// Now that the motor has been set, let's read the value back and make sure it
	// changed.
	dynamixelHandler.pushInstruction(
			new Instruction(Instruction::SYNC_READ, dataRange, NULL,
					&dynamixels));  // 2 or 1 bytes?  how to tell?
	while (dynamixelHandler.busy())
		;

	for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
			it != dynamixels.end(); ++it) {
		switch (dataRange.length) {
		case 1:
			valcurrent[it->first] = (int) it->second->getByte(dataRange.reg);
			break;
		case 2:
			valcurrent[it->first] = (int) it->second->getWord(dataRange.reg);
			break;
		case 4:
			valcurrent[it->first] = it->second->getInt(dataRange.reg);
			break;
		}
	}

	for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
			it != dynamixels.end(); ++it) {
		if (verbose) {
			std::cout << "ID: " << (int) it->first << "\tRegister: "
					<< dataRange.reg << ":\tCurrent value: "
					<< valcurrent[it->first] << std::endl;
		}
		if (value != valcurrent[it->first]) {
			std::cerr << "Warning: Written value did not seem to change!"
					<< std::endl;
		}
	}

	return EXIT_SUCCESS;
}

int bulkReadDynamixel(Interface* interface, std::vector<unsigned char> IDs,
		std::vector<DataRange> dataRange, bool verbose) {
	FailureCheck failureObserver;
	Handler dynamixelHandler;
	dynamixelHandler.setInterface(interface);
	std::map<Motor*, DataRange> bulkRanges;
	unsigned int i = 0;

	if (IDs.size() != dataRange.size()) {
		std::cerr << "Number of motors and number of data ranges mismatch!"
				<< std::endl;
		return EXIT_FAILURE;
	}

	for (std::vector<unsigned char>::iterator it = IDs.begin(); it != IDs.end();
			++it) {
		dynamixelHandler.forceAddDynamixel(*it);
	}

	std::map<unsigned char, Motor*> dynamixels =
			dynamixelHandler.getDynamixels();
	if (dynamixels.size() < 1) {
		std::cerr << "Error: Unable to read dynamixel" << std::endl;
		return EXIT_FAILURE;
	} else if (dynamixels.size() < IDs.size()) {
		std::cout << "Warning: Some dynamixel motors were not found"
				<< std::endl;
	}

	i = 0;
	if (verbose) {
		std::cout
				<< "Bulk Read will issue on the following motors with register ranges:"
				<< std::endl;
		for (std::map<unsigned char, Motor *>::iterator it = dynamixels.begin();
				it != dynamixels.end(); ++it, ++i) {
			std::cout << "ID:" << (int) it->first << "\t Reg:"
					<< dataRange[i].reg << ", Length:" << dataRange[i].length
					<< std::endl;
		}
	}

	if (verbose) {
		std::cout << "Reading model for dynamixel of IDs: ";
		for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
				it != dynamixels.end(); ++it) {
			std::cout << (int) it->first << " ";
		}
		std::cout << std::endl;
	}
	dynamixelHandler.pushInstruction(
			new Instruction(Instruction::SYNC_READ, DataRange(0, 2), NULL,
					&dynamixels));
	while (dynamixelHandler.busy());

	// Optional: Check if all the motors are the same type
	unsigned short motorType = dynamixels.begin()->second->getWord(0);
	for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
			it != dynamixels.end(); ++it) {
		unsigned short temp = it->second->getWord(0);
		if (motorType != temp) {
			std::cout
					<< "Warning: different motor models detected for sync read"
					<< std::endl;
			break;
		}
		motorType = temp;
	}

	if (verbose) {
		std::cout << "Bulk Read will be issued" << std::endl;
	}
	for (i = 0; i < IDs.size(); ++i) {
		if (dataRange[i].length == 0
				|| dataRange[i].length == (unsigned short) -1) {
			dataRange[i].length = dynamixels[IDs[i]]->getNumberOfRegisters()
					- dataRange[i].reg;
		}
		bulkRanges[dynamixels[IDs[i]]] = dataRange[i];
		if (verbose) {
			std::cout << "ID: " << (int) IDs[i] << " reg: " << dataRange[i].reg
					<< " len: " << dataRange[i].length << std::endl;
		}
	}

	dynamixelHandler.pushInstruction(
			new Instruction(Instruction::BULK_READ, bulkRanges));
	while (dynamixelHandler.busy())
		;

	if (verbose) {
		std::cout
				<< "Bulk Read were issued on the following motors with register ranges:"
		<< std::endl;
	}
	for (i = 0; i < IDs.size(); ++i) {
		std::cout << "Motor ID:" << (int) IDs[i] << "\tReg:" << dataRange[i].reg
		<< "\tLength:" << dataRange[i].length << std::endl;
		for (int j = dataRange[i].reg;
			 j < dataRange[i].reg + dataRange[i].length; j++) {
			std::cout << j << "\t" << (int) (dynamixels[IDs[i]]->getByte(j))
			<< std::endl;
		}
	}

	return EXIT_SUCCESS;
}

int bulkWriteDynamixel(Interface* interface, std::vector<unsigned char> IDs,
		std::vector<DataRange> dataRange, std::vector<int> values,
		bool verbose) {
	unsigned int i = 0;
	std::map<Motor*, DataRange> bulkRanges;

	if (IDs.size() != dataRange.size() || IDs.size() != values.size()) {
		std::cerr << "Number of motors and number of data/ranges mismatch!"
				<< std::endl;
		return EXIT_FAILURE;
	}

	for (i = 0; i < dataRange.size(); ++i) {
		if (dataRange[i].length == 0
				|| dataRange[i].length == (unsigned short) -1) {
			if (values[i] >= 0 && values[i] < 256) {
				dataRange[i].length = 1;
			} else if (values[i] >= 256 && values[i] < 65536) {
				dataRange[i].length = 2;
			} else {
				dataRange[i].length = 4;
			}
		} else if (dataRange[i].length != 1 && dataRange[i].length != 2
				&& dataRange[i].length != 4) {
			std::cerr << " Invalid number of bytes specified" << std::endl;
			return EXIT_FAILURE;
		}
	}

	Handler dynamixelHandler;
	dynamixelHandler.setInterface(interface);

	for (std::vector<unsigned char>::iterator it = IDs.begin(); it != IDs.end();
			++it) {
		dynamixelHandler.forceAddDynamixel(*it);
	}

	std::map<unsigned char, Motor*> dynamixels =
			dynamixelHandler.getDynamixels();
	if (dynamixels.size() < 1) {
		std::cerr << "Error: Unable to read dynamixel" << std::endl;
		return EXIT_FAILURE;
	} else if (dynamixels.size() < IDs.size()) {
		std::cout << "Warning: Some dynamixel motors were not found"
				<< std::endl;
	}

	if (verbose) {
		std::cout << "Reading model for dynamixel of IDs: ";
		for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
				it != dynamixels.end(); ++it) {
			std::cout << (int) it->first << " ";
		}
		std::cout << std::endl;
	}
	dynamixelHandler.pushInstruction(
			new Instruction(Instruction::SYNC_READ, DataRange(0, 2), NULL,
					&dynamixels));
	while (dynamixelHandler.busy())
		;

	// Optional: Check if all the motors are the same type
	unsigned short motorType = dynamixels.begin()->second->getWord(0);
	for (std::map<unsigned char, Motor*>::iterator it = dynamixels.begin();
			it != dynamixels.end(); ++it) {
		unsigned short temp = it->second->getWord(0);
		if (motorType != temp) {
			std::cout
					<< "Warning: different motor models detected for sync write"
					<< std::endl;
			break;
		}
		motorType = temp;
	}

	// Before writing the value, let's try to read the current value in the
	// dynamixel.
	std::map<unsigned char, int> valcurrent;
	if (verbose) {
		std::cout << "Attempting to read motor: " << std::endl;
	}
	for (i = 0; i < IDs.size(); ++i) {
		switch (dataRange[i].length) {
		case 1:
			valcurrent[IDs[i]] = (int) dynamixels[IDs[i]]->getByte(
					dataRange[i].reg);
			break;
		case 2:
			valcurrent[IDs[i]] = (int) dynamixels[IDs[i]]->getWord(
					dataRange[i].reg);
			break;
		case 4:
			valcurrent[IDs[i]] = dynamixels[IDs[i]]->getInt(dataRange[i].reg);
			break;
		}
	}

	if (verbose) {
		for (i = 0; i < IDs.size(); ++i) {
			std::cout << "ID: " << (int) IDs[i] << "\tRegister: "
					<< dataRange[i].reg << ":\tCurrent value: "
					<< valcurrent[IDs[i]] << std::endl;
		}
	}

	// Now that we know the current value, let's write the new value.
	if (verbose)
		std::cout << "\nModifying value..." << std::endl;
	for (i = 0; i < IDs.size(); ++i) {
		switch (dataRange[i].length) {
		case 1:
			dynamixels[IDs[i]]->setByte(dataRange[i].reg, values[i]);
			bulkRanges[dynamixels[IDs[i]]] = dataRange[i];
			break;
		case 2:
			dynamixels[IDs[i]]->setWord(dataRange[i].reg, values[i]);
			bulkRanges[dynamixels[IDs[i]]] = dataRange[i];
			break;
		case 4:
			dynamixels[IDs[i]]->setInt(dataRange[i].reg, values[i]);
			bulkRanges[dynamixels[IDs[i]]] = dataRange[i];
			break;
		}
	}

	dynamixelHandler.pushInstruction(
			new Instruction(Instruction::BULK_WRITE, bulkRanges));
	while (dynamixelHandler.busy())
		;

	for (i = 0; i < IDs.size(); ++i) {
		switch (dataRange[i].length) {
		case 1:
			valcurrent[IDs[i]] = (int) dynamixels[IDs[i]]->getByte(
					dataRange[i].reg);
			break;
		case 2:
			valcurrent[IDs[i]] = (int) dynamixels[IDs[i]]->getWord(
					dataRange[i].reg);
			break;
		case 4:
			valcurrent[IDs[i]] = dynamixels[IDs[i]]->getInt(dataRange[i].reg);
			break;
		}
	}

	for (i = 0; i < IDs.size(); ++i) {
		if (verbose) {
			std::cout << "ID: " << (int) IDs[i] << "\tRegister: "
					<< dataRange[i].reg << ":\tCurrent value: "
					<< valcurrent[IDs[i]] << std::endl;
		}
		if (values[i] != valcurrent[IDs[i]]) {
			std::cerr << "Warning: Written value did not seem to change!"
					<< std::endl;
		}
	}

	return EXIT_SUCCESS;
}
